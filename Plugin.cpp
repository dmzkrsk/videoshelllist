#include "StdAfx.h"
#include "Plugin.h"

#include "filesys.h"

void ::FreePluginInstance(CPluginInstance* pI)
{
	delete pI;
}

CPlugins::CPlugins()
{
	pArray.RemoveAll();
}

CPlugins::~CPlugins()
{
	DeleteAll();
}

CPlugin* CPlugins::GetAt(int nElement)
{
	if(nElement<pArray.GetSize() && nElement>=0) return pArray[nElement];
	else return NULL;
}

CPlugin* CPlugins::CreateElement()
{
	pArray.Add(new CPlugin);

	TRACE("Plugin entry created at %x. New size is %d\n", pArray[pArray.GetSize()-1], pArray.GetSize()-1);
	return pArray[pArray.GetSize()-1];
}

void CPlugins::DeleteElement()
{
	CPlugin* pPlToDelete=pArray[pArray.GetSize()-1];
	TRACE("Plugin entry deleted at %x. Size is %d\n", pPlToDelete, pArray.GetSize());
	delete pPlToDelete;
	pArray.RemoveAt(pArray.GetSize()-1);
}

void CPlugins::DeleteAll()
{
	while(pArray.GetSize()) DeleteElement();
}

CPlugin* CPlugins::FindPlugin(int nPluginID)
{
	for(int i=0;i<pArray.GetSize();i++)
		if(pArray[i]->GetID()==nPluginID) return pArray[i];

	return NULL;
}

CPlugin::CPlugin()
{
	szPluginName[0]='\0';
	szPluginPath[0]='\0';
	szPluginTitle[0]='\0';
	szPluginFileExt[0]='\0';
	szPluginMessage[0]='\0';
	szPluginStatusString[0]='\0';

	nPluginID=-1;

	hPluginBmp=NULL;
}

CPlugin::~CPlugin()
{
	DeleteObject(hPluginBmp);
}

int CPlugin::LoadPlugin(const char* szFilename)
{
	HINSTANCE hPlugin=LoadLibrary(szFilename);

	if(!hPlugin) return PLER_LOADLIBRARY;
	int nRet=PLER_OK;

	SPLITINFO si=CFileSystem::SplitFileName((char*)szFilename);
	lstrcpy(szPluginName, si.FilenameWithoutExtension);

	lstrcpy(szPluginPath, szFilename);
	if(!LoadString(hPlugin, IDS_PLUGINFULLNAME, szPluginTitle, MAX_PTITLE)) nRet=PLER_LOADSTRING;
	if(!LoadString(hPlugin, IDS_PLUGINFILEEXTENSION, szPluginFileExt, MAX_EXT)) nRet=PLER_LOADSTRING;
	if(!LoadString(hPlugin, IDS_PLUGINSTATUSSTRING, szPluginStatusString, MAX_PSTATUS)) nRet=PLER_LOADSTRING;
	if(!LoadString(hPlugin, IDS_PLUGINOKMESSAGE, szPluginMessage, MAX_PMESSAGE)) nRet=PLER_LOADSTRING;

	hPluginBmp=LoadBitmap(hPlugin, (LPCTSTR)IDB_PLUGINBITMAP);

	PLUGINGETINFO GetPluginInfo=(PLUGINGETINFO)GetProcAddress(hPlugin, "GetPluginInfo");
	GetPluginInfo(&PlInfo);

	FreeLibrary(hPlugin);
	return nRet;
}

CPluginInstance* CPlugin::InitPlugin(PLUGINMODE mode, SAVER* saver)
{
	HINSTANCE hPlugin=LoadLibrary(szPluginPath);
	if(hPlugin==NULL) return NULL;
	
	CPluginInstance* pInst=new CPluginInstance(hPlugin, this, saver, mode);

	return pInst;
}

CPluginInstance::CPluginInstance(HINSTANCE hPlugin, CPlugin* pPlugin, SAVER* pSaver, PLUGINMODE PluginMode)
{
	hInst=hPlugin;
	this->pPlugin=pPlugin;
	mode=PluginMode;
	bInitialized=TRUE;

	if(hInst!=NULL && this->pPlugin!=NULL)
	{
		InitPlugin=(PLUGININITPLUGIN)GetProcAddress(hInst, "InitPlugin");
		if(!InitPlugin)
		{
			bInitialized=FALSE;
			return;
		}

		INITPLUGIN ip;
		ip.pSaver=pSaver;
		ip.mode=mode;
		ip.nPluginID=pPlugin->GetID();
		ip.SetupDlgProc=NULL;

		if(InitPlugin(&ip))
		{
			bInitialized=FALSE;
			return;
		}

		if(pPlugin->IsSelfEx())
		{
			GetString=NULL;
			//Загрузка функций для автоматической рабоиты
		}
		else
		{
			GetString=(PLUGINGETSTRING)GetProcAddress(hInst, "GetString");
			if(GetString==NULL) bInitialized=FALSE;
		}

		if(pPlugin->IsSetup())
		{
			DialogProc=ip.SetupDlgProc;
			if(DialogProc==NULL)
			{
				bInitialized=FALSE;
				return;
			}
		}


	}
	else
	{
		bInitialized=FALSE;
		return;
	}
}


CPluginInstance::~CPluginInstance()
{
	ExitInstance();
}

void CPluginInstance::ExitInstance()
{
	FreeLibrary(hInst);
}


BOOL CPluginInstance::OpenFile(const char* szFilename)
{
	if(bInitialized==FALSE) return FALSE;
	if(!pPlugin->IsSelfEx())
		return m_File.Open(szFilename, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive, NULL);
	else
		return FALSE;
}

BOOL CPluginInstance::CloseFile()
{
	if(bInitialized==FALSE) return FALSE;
	if(!pPlugin->IsSelfEx())
	{
		m_File.Close();
		return TRUE;
	}
	else return FALSE;
}

BOOL CPluginInstance::WriteFormatHeader()
{
	if(bInitialized==FALSE) return FALSE;

	const char* szString=GetString(TYPE_FORMAT, DIV_HEADER);
	m_File.Write(szString, lstrlen(szString));

	return TRUE;
}

BOOL CPluginInstance::WriteOutLine(const char* szTemplate)
{
	if(bInitialized==FALSE) return FALSE;
	m_File.Write(szTemplate, lstrlen(szTemplate));
	return TRUE;
}

BOOL CPluginInstance::WriteFormatEnd()
{
	if(bInitialized==FALSE) return FALSE;

	const char* szString=GetString(TYPE_FORMAT, DIV_END);
	m_File.Write(szString, lstrlen(szString));

	return TRUE;
}

void CTemplateArray::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	CString sTmp;
	for(int i=0;i<GetSize();i++)
	{
		sTmp=GetAt(i);
		sTmp.Replace(lpszOld, lpszNew);
		SetAt(i, sTmp);
	}
}


BOOL CTemplateArray::SupportsMedia()
{
	for(int nEl=0;nEl<this->GetSize();nEl++)
	{
		if(this->GetAt(nEl).Find("%%VWIDTH%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%VHEIGHT%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%VFRAMERATE%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%VBITRATE%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%VCODEC%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%VFOURCC%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%ACODEC%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%ACHANNELS%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%ASAMPLING%%")!=-1) return TRUE;
		if(this->GetAt(nEl).Find("%%ABITRATE%%")!=-1) return TRUE;
	}

	return FALSE;
}

BOOL CPluginInstance::WriteTblHeader()
{
	if(bInitialized==FALSE) return FALSE;

	const char* szString=GetString(TYPE_TABLE, DIV_HEADER);
	m_File.Write(szString, lstrlen(szString));

	return TRUE;
}
/*
BOOL CPluginInstance::WriteTblLine(CStringArray &csaList, BOOL bHeader)
{
	if(bInitialized==FALSE) return FALSE;
	const char* szString;

	szString=bHeader?"":GetString(TYPE_LINE, DIV_HEADER);
	m_File.Write(szString, lstrlen(szString));

	szString=GetString(bHeader?TYPE_TABLE:TYPE_LINE, DIV_MIDDLE);
	for(int n=0;n<csaList.GetSize();n++)
	{
		if(n!=0) m_File.Write(szString, lstrlen(szString));

		m_File.Write(csaList[n], csaList[n].GetLength());
	}

	szString=bHeader?"":GetString(TYPE_LINE, DIV_END);
	m_File.Write(szString, lstrlen(szString));

	return TRUE;
}
*/
BOOL CPluginInstance::WriteTblEnd()
{
	if(bInitialized==FALSE) return FALSE;

	const char* szString=GetString(TYPE_TABLE, DIV_END);
	m_File.Write(szString, lstrlen(szString));

	return TRUE;
}
