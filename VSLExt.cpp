#include "stdafx.h"
#include "VideoShellList.h"

#include "Plugin.h"
#include "Settings.h"

#include "VSLExt.h"
#include "resource.h"
#include <atlconv.h>

#include "filesys.h"
#include "registry.h"

#include "MInfo.h"

#define RED_START	0
#define RED_END		160
#define GREEN_START	80
#define GREEN_END	240
#define BLUE_START	90
#define BLUE_END	120

#define PLUGINS_DIRECTORY "Plugins"
#define MENU_ITEM "Список Видео"
#define MENU_DESC "Создание плей-листов и таблиц описаний медиа-файлов"
#define DEFAULT_SAVEFILE "vsl.set"

#define POPUP_TITLE "Video List"
#define InfoBox(Message) MessageBox(NULL, Message, POPUP_TITLE, MB_OK|MB_ICONINFORMATION);
#define ErrorBox(Message) MessageBox(NULL, Message, POPUP_TITLE, MB_OK|MB_ICONERROR);

/////////////////////////////
// Savers
int SetIntParam(int nID, char* szParamName, DWORD dwValue);
int SetStrParam(int nID, char* szParamName, char* szValue);
int SetIntParamDump(int nID, char* szParamName, DWORD dwValue);
int SetStrParamDump(int nID, char* szParamName, char* szValue);
DWORD GetIntParam(int nID, char* szParamName, DWORD dwDefault);
void GetStrParam(int nID, char* szParamName, char** pszValue, int* pnSize, char* szDefaultValue);

/////////////////////////////
// Dialogs
BOOL CALLBACK ProgressDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK PropDlg(HWND hwndDlg, UINT uMsg, LPARAM lParam);
BOOL CALLBACK InfoDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK FileDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ExtDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void SwapListBoxValues(HWND hWnd, int nIDListBox1, int nIDListBox2);

//////////////////////////////
// GLOBAL
HWND g_prhWnd, g_exphWnd;
CSettings* pSettings;
HINSTANCE g_hInst;
CPlugins m_Plugins;
char g_szSettings[MAX_PATH];

/////////////////////////////////////////////////////////
// Other
BOOL ProcessFile(CString& szFilename, CTemplateArray* ctaLineTemplate, int nLineNumber);
BOOL SupportsMedia(const char* sTemplate);
void SplitFileName(CString sFilename, CString *psArtist, CString *psTitle, CString *psType);

void GetOutFileName(CString& sBaseFilename, char* m_szFile);

void InitProgressBar(HWND hProgress, char* szText, int nRange, int nStep);
void StepProgressBar(HWND hProgress, int n, int max);

CString GetTextField(int nField);
CString GetNameField(int nField);

CString GenerateTemplateString(CStringArray &csaList, char* szHead, char* szDelim, char* szEnd);

void SplitExtensions(CString sExt, CStringArray& csaExt);

BOOL InsertExtension(HWND hWnd, int nListBox, char* szExt);
/////////////////////////////////////////////////////////////////////////////
// CVSLExt

HRESULT CVSLExt::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	FORMATETC fmt={CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	STGMEDIUM stg={TYMED_HGLOBAL };
	HDROP hDrop;

	g_prhWnd=NULL;
	g_exphWnd=NULL;
	g_hInst=_Module.m_hInst;

    if(FAILED(pDataObj->GetData(&fmt, &stg))) return E_INVALIDARG;

    hDrop=(HDROP)GlobalLock(stg.hGlobal);

    if(NULL==hDrop) return E_INVALIDARG;

	CString sInstallDir="c:\\";
	CAppRegistry *pReg=new CAppRegistry("Fiziki", "VSL", TRUE);
	if(pReg) sInstallDir=pReg->AppGetValue("InstallDir", "c:\\");
	delete pReg;

	if(sInstallDir[sInstallDir.GetLength()-1]!='\\') sInstallDir+='\\';

	sprintf(g_szSettings, "%s%s", sInstallDir.GetBuffer(sInstallDir.GetLength()), DEFAULT_SAVEFILE);

	UINT uNumFiles=DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    if(0==uNumFiles)
	{
        GlobalUnlock(stg.hGlobal);
        ReleaseStgMedium(&stg);
        return E_INVALIDARG;
	}

	HRESULT hr=S_OK;

	hDlgWnd=CreateDialog(_Module.m_hInst, MAKEINTRESOURCE(IDD_PROGRESS), NULL, ProgressDlg);
	if(!IsWindow(hDlgWnd)) hr=E_INVALIDARG;
	else
	{
		::InitProgressBar(hDlgWnd, "Прогресс...", 0, 0);
		SendDlgItemMessage(hDlgWnd, IDC_PROGRESS, PBM_SETBKCOLOR, 0, (LPARAM)RGB(255,255,255));
		SendDlgItemMessage(hDlgWnd, IDC_PROGRESS, PBM_SETBARCOLOR, 0, (LPARAM)RGB(RED_START, GREEN_START, BLUE_START));

	}

	if (0==DragQueryFile(hDrop, 0, m_szFile, MAX_PATH)) hr=E_INVALIDARG;
	else
	{
		m_Plugins.DeleteAll();
		TCHAR dir[MAX_PATH];
		GetModuleFileName(_Module.m_hInst, dir, MAX_PATH);
		SPLITINFO si;
		si=CFileSystem::SplitFileName(dir);
		sprintf(dir, "%s%s%s\\", si.DriveLetter, si.FullPath, PLUGINS_DIRECTORY);

		CStringArray ext, csaPlugList;
		ext.Add("vsp");

		GenerateFileList(dir, ext, csaPlugList, FALSE);

		CPlugin* newPl;
		for(int i=0;i<csaPlugList.GetSize();i++)
		{
			newPl=m_Plugins.CreateElement();
			if(newPl) newPl->LoadPlugin(csaPlugList[i]);
		}
		
		m_hMenuBmp=LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_MENU));
		m_hMenuTools=LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_TOOLS));
	}

    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);

    return hr;
}

HRESULT CVSLExt::QueryContextMenu(HMENU hmenu, UINT  uMenuIndex, UINT  uidFirstCmd, UINT  uidLastCmd, UINT  uFlags)
{
    if(uFlags&CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	if((uidLastCmd-uidFirstCmd)<(unsigned)(m_Plugins.GetSize()+4))
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	HMENU hSubMenu=CreateMenu();

	int i=1;

	FillMenu(TRUE, hSubMenu, &i, uidFirstCmd);
	FillMenu(FALSE, hSubMenu, &i, uidFirstCmd);

	nSetupID=i;

	InsertMenu(hSubMenu, nSetupID, MF_BYPOSITION, uidFirstCmd+nSetupID, _T("Настройка"));
	if(m_hMenuTools)
		SetMenuItemBitmaps(hSubMenu, uidFirstCmd+nSetupID, MF_BYCOMMAND, m_hMenuTools, NULL);

	MENUITEMINFO mii;

	mii.cbSize=sizeof(mii);
	mii.fMask=MIIM_TYPE|MIIM_SUBMENU|MIIM_ID;
	mii.fType=MFT_STRING;
	mii.hSubMenu=hSubMenu;
	
	mii.dwTypeData=_T(MENU_ITEM);
	mii.cch=sizeof(MENU_ITEM);

	mii.wID=uidFirstCmd;

	InsertMenuItem(hmenu, uMenuIndex, TRUE, &mii);
	if(m_hMenuBmp)
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_hMenuBmp, NULL);

    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, (unsigned)(m_Plugins.GetSize()+4));
}

HRESULT CVSLExt::GetCommandString(UINT  idCmd, UINT  uFlags, UINT* pwReserved, LPSTR pszName, UINT  cchMax)
{
    USES_CONVERSION;

    if((unsigned)(m_Plugins.GetSize()+4)<idCmd) return E_INVALIDARG;
    if(uFlags&GCS_HELPTEXT)
	{
        TCHAR szText[256]="";

		if(idCmd==0)
			lstrcpy(szText, _T(MENU_DESC));
		else
			if(idCmd==nSetupID)
				lstrcpy(szText, _T("Настройка"));
			else
			{
				CPlugin* pSelectedPlugin=m_Plugins.FindPlugin(idCmd);
				if(pSelectedPlugin)
					lstrcpy(szText, pSelectedPlugin->GetStatusString());
			}

        if(uFlags & GCS_UNICODE) lstrcpynW((LPWSTR) pszName, T2CW(szText), cchMax);
        else lstrcpynA(pszName, T2CA(szText), cchMax);

        return S_OK;
	}
    return E_INVALIDARG;
}


HRESULT CVSLExt::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	g_exphWnd=pCmdInfo->hwnd;
	if((unsigned)(m_Plugins.GetSize()+4)<HIWORD(pCmdInfo->lpVerb)) return E_INVALIDARG;

	if(LOWORD(pCmdInfo->lpVerb)==nSetupID)
	{
		Setup();
		return S_OK;
	}
	else
	{

	CPlugin* cpActivePlugin=m_Plugins.FindPlugin(LOWORD(pCmdInfo->lpVerb));
	SAVER g_Saver={SetIntParamDump, SetStrParamDump, GetIntParam, GetStrParam};
	CPluginInstance	*cpi=cpActivePlugin->InitPlugin(modeAction, &g_Saver);

	if(!cpi->PluginInitialized())
	{
		ErrorBox("Неудаётся проиницализировать плагин");
		ShowWindow(hDlgWnd, SW_HIDE);
		FreePluginInstance(cpi);
		return S_OK;
	}

		CStringArray csaExt, csaList;
		csaExt.RemoveAll();
		pSettings=new CSettings(g_szSettings);
		SplitExtensions(pSettings->szExtensions, csaExt);
		delete pSettings;

		ShowWindow(hDlgWnd, SW_SHOW);
		::InitProgressBar(hDlgWnd, "Составление списка файлов", MAKELPARAM(0,100), 1);

		GenerateFileList(m_szFile, csaExt, csaList, TRUE, hDlgWnd);

		if(csaList.GetSize()==0)
		{
			ErrorBox("Нет файлов для обработки");
			ShowWindow(hDlgWnd, SW_HIDE);
			FreePluginInstance(cpi);
			return S_OK;
		}
		else
		{
			CString sBaseFilename;
			::GetOutFileName(sBaseFilename, m_szFile);

			sBaseFilename+='.';
			sBaseFilename+=cpActivePlugin->GetFileExtension();
			
			::InitProgressBar(hDlgWnd, "Запись...", MAKELPARAM(0,csaList.GetSize()), 1);

			if(cpActivePlugin->IsTable())
			{
				pSettings=new CSettings(g_szSettings);
				BOOL nExp=pSettings->sizeExporting!=0;
				delete pSettings;

				if(!nExp)
				{
					ErrorBox("Нет полей для экспорта");
					ShowWindow(hDlgWnd, SW_HIDE);
					FreePluginInstance(cpi);
					return S_OK;
				}
			}
			
			if(cpi->OpenFile(sBaseFilename.GetBuffer(sBaseFilename.GetLength())))
			{
				cpi->WriteFormatHeader();
				CTemplateArray ctaLine;
				if(cpActivePlugin->IsPlaylist())
				{
					//Playlist
					for(int nFileN=0;nFileN<csaList.GetSize();nFileN++)
					{
						ctaLine.RemoveAll();
						ctaLine.Add(cpi->TransferString(TYPE_FORMAT, DIV_MIDDLE));

						::ProcessFile(csaList[nFileN], &ctaLine, nFileN);
						::StepProgressBar(hDlgWnd, nFileN, csaList.GetSize());
						
						cpi->WriteOutLine(ctaLine[0].GetBuffer(ctaLine[0].GetLength()));
					}

				}
				else
				{
					//Table;
					pSettings=new CSettings(g_szSettings);
					CStringArray csaNames, csaFields;

					for(int n=0;n<pSettings->sizeExporting;n++)
					{
						csaFields.Add(GetTextField(pSettings->pnExporting[n]));
						csaNames.Add(GetNameField(pSettings->pnExporting[n]));
					}
					delete pSettings;

					cpi->WriteTblHeader();
					cpi->WriteOutLine(GenerateTemplateString(csaNames, cpi->TransferString(TYPE_TABLE, DIV_HEADER), cpi->TransferString(TYPE_TABLE, DIV_MIDDLE), cpi->TransferString(TYPE_TABLE, DIV_END)));

					for(int nFileN=0;nFileN<csaList.GetSize();nFileN++)
					{
						ctaLine.RemoveAll();
						ctaLine.Add(GenerateTemplateString(csaFields, cpi->TransferString(TYPE_LINE, DIV_HEADER), cpi->TransferString(TYPE_LINE, DIV_MIDDLE), cpi->TransferString(TYPE_LINE, DIV_END)));

						::ProcessFile(csaList[nFileN], &ctaLine, nFileN);
						::StepProgressBar(hDlgWnd, nFileN, csaList.GetSize());
						
						cpi->WriteOutLine(ctaLine[0].GetBuffer(ctaLine[0].GetLength()));
					}

					cpi->WriteTblEnd();
				}

				cpi->WriteFormatEnd();
				cpi->CloseFile();
				InfoBox(cpActivePlugin->GetOKMessage());
				pSettings=new CSettings(g_szSettings);
				if(pSettings->bOpenAfterAll)
					ShellExecute(NULL, "open", sBaseFilename.GetBuffer(sBaseFilename.GetLength()), NULL, NULL, SW_SHOWMAXIMIZED);
				delete pSettings;
			}
			else
				ErrorBox("Ошибка записи");

			FreePluginInstance(cpi);
		}
	}

	ShowWindow(hDlgWnd, SW_HIDE);
	return S_OK;
}

void CVSLExt::GenerateFileList(CString sPath, CStringArray &csaExtensions, CStringArray &csaList, BOOL bSub, HWND hCallbackWnd)
{
	CFileFind finder;
	if(sPath.GetAt(sPath.GetLength()-1)!='\\') sPath+='\\';
	BOOL bWorking = finder.FindFile(sPath+"*.*");

	while (bWorking)
	{
		if(hCallbackWnd) SendDlgItemMessage(hCallbackWnd, IDC_PROGRESS, PBM_STEPIT, 0, 0);
		bWorking = finder.FindNextFile();
		CString ffile=finder.GetFilePath();
		if(finder.IsDots()) continue;
		if(finder.IsDirectory() && bSub)
			GenerateFileList(ffile+"\\", csaExtensions, csaList, bSub, hCallbackWnd);
		else
		{
			int extSize;
			for(int i=0;i<csaExtensions.GetSize();i++)
			{
				CString extension=csaExtensions.GetAt(i);
				extSize=extension.GetLength();
				if(!ffile.Right(extSize+1).CompareNoCase("."+extension))
				{
					csaList.Add(ffile);
					break;
				}
			}
		}
	}
}

void CVSLExt::FillMenu(BOOL bUseList, HMENU hMenu, int *i, int uidCmd)
{
	HBITMAP hItemBmp;
	for(int j=0;j<m_Plugins.GetSize();j++)
		if(m_Plugins[j]->IsPlaylist()==bUseList)
		{
			InsertMenu(hMenu, *i, MF_BYPOSITION, uidCmd+*i, m_Plugins[j]->GetMenuString());
			hItemBmp=m_Plugins[j]->GetBitmap();
			if(hItemBmp)
				SetMenuItemBitmaps(hMenu, uidCmd+*i, MF_BYCOMMAND, hItemBmp, NULL);
			m_Plugins[j]->SetID(*i);
			(*i)++;
		}

	InsertMenu(hMenu, (*i)++, MF_SEPARATOR, NULL, NULL);
}

BOOL CVSLExt::Setup()
{
	int nAddTabs=0;
	int defTabs=3;
	pSettings=new CSettings(g_szSettings);
	SAVER g_Saver={SetIntParam, SetStrParam, GetIntParam, GetStrParam};

	CPluginInstance* cpi;
	CArray<CPluginInstance*, CPluginInstance*> InitPlugins;

	int nPlug;
	for(nPlug=0;nPlug<m_Plugins.GetSize();nPlug++)
		if(m_Plugins[nPlug]->IsSetup())
		{
			cpi=m_Plugins[nPlug]->InitPlugin(modeSetup, &g_Saver);
			if(!cpi->PluginInitialized())
			{
				cpi->ExitInstance();
				FreePluginInstance(cpi);
			}
			else
				InitPlugins.Add(cpi);
		}

	nAddTabs=InitPlugins.GetSize();

    PROPSHEETPAGE *ppsp=new PROPSHEETPAGE[defTabs+nAddTabs+1];
    PROPSHEETHEADER psh;

    ppsp[0].dwSize = sizeof(PROPSHEETPAGE);
    ppsp[0].dwFlags = PSP_USETITLE;
    ppsp[0].hInstance = _Module.m_hInst;
    ppsp[0].pszTemplate = MAKEINTRESOURCE(IDD_INFO);
    ppsp[0].pfnDlgProc = InfoDlg;
    ppsp[0].pszTitle = "Поля списков";
    ppsp[0].lParam = 0;
    ppsp[0].pfnCallback = NULL;

    ppsp[1].dwSize = sizeof(PROPSHEETPAGE);
    ppsp[1].dwFlags = PSP_USETITLE;
    ppsp[1].hInstance = _Module.m_hInst;
    ppsp[1].pszTemplate = MAKEINTRESOURCE(IDD_LIST);
    ppsp[1].pfnDlgProc = FileDlg;
    ppsp[1].pszTitle = "Файл";
    ppsp[1].lParam = 0;
    ppsp[1].pfnCallback = NULL;

    ppsp[2].dwSize = sizeof(PROPSHEETPAGE);
    ppsp[2].dwFlags = PSP_USETITLE;
    ppsp[2].hInstance = _Module.m_hInst;
    ppsp[2].pszTemplate = MAKEINTRESOURCE(IDD_EXTENSIONS);
    ppsp[2].pfnDlgProc = ExtDlg;
    ppsp[2].pszTitle = "Типы файлов";
    ppsp[2].lParam = 0;
    ppsp[2].pfnCallback = NULL;

	for(nPlug=0;nPlug<nAddTabs;nPlug++)
	{
	    ppsp[nPlug+defTabs].dwSize = sizeof(PROPSHEETPAGE);
		ppsp[nPlug+defTabs].dwFlags = PSP_DEFAULT|PSP_USEICONID;
		ppsp[nPlug+defTabs].hInstance = InitPlugins[nPlug]->GetInstance();
		ppsp[nPlug+defTabs].pszTemplate = MAKEINTRESOURCE(IDD_PLUGINSETUP);
		ppsp[nPlug+defTabs].pfnDlgProc = InitPlugins[nPlug]->GetDialogProc();
		ppsp[nPlug+defTabs].pszTitle = NULL;
		ppsp[nPlug+defTabs].pszIcon = MAKEINTRESOURCE(IDI_PLUGINICON);
		ppsp[nPlug+defTabs].lParam = 0;
		ppsp[nPlug+defTabs].pfnCallback = NULL;
	}

    ppsp[nAddTabs+defTabs].dwSize = sizeof(PROPSHEETPAGE);
    ppsp[nAddTabs+defTabs].dwFlags = PSP_USETITLE;
    ppsp[nAddTabs+defTabs].hInstance = _Module.m_hInst;
    ppsp[nAddTabs+defTabs].pszTemplate = MAKEINTRESOURCE(IDD_ABOUT);
    ppsp[nAddTabs+defTabs].pfnDlgProc = AboutDlg;
    ppsp[nAddTabs+defTabs].pszTitle = "О программе...";
    ppsp[nAddTabs+defTabs].lParam = 0;
    ppsp[nAddTabs+defTabs].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE|PSH_USECALLBACK;
    psh.hwndParent = g_exphWnd;
    psh.hInstance = _Module.m_hInst;
    psh.pszCaption = (LPSTR) "Настройки VideoList";
    psh.nPages = nAddTabs+defTabs+1;
    psh.nStartPage = pSettings->nSheet;
    psh.ppsp = (LPCPROPSHEETPAGE) ppsp;
    psh.pfnCallback = PropDlg;

	PropertySheet(&psh);

	for(nPlug=0;nPlug<nAddTabs;nPlug++)
	{
		InitPlugins[nPlug]->ExitInstance();
		FreePluginInstance(InitPlugins[nPlug]);
	}

	pSettings->Dump();

	delete pSettings;
	delete ppsp;

	return TRUE;
}

//////////////////////////////
// Dialog Boxes
BOOL CALLBACK AboutDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_MAIL:
			char szMail[128], szMailCmd[135];
			GetDlgItemText(hWnd, IDC_MAIL, szMail, 128);
			sprintf(szMailCmd, "mailto:%s", szMail);
			ShellExecute(NULL, "open", szMailCmd,  NULL, NULL, NULL);
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		switch(((NMHDR FAR*)lParam)->code)
		{
		case PSN_KILLACTIVE:
			pSettings->nSheet=2;
			SetWindowLong(hWnd, DWL_MSGRESULT, FALSE);
			return TRUE;
		case PSN_APPLY:
			SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);
			return TRUE;
		}
	}
	return FALSE;
}

int CALLBACK PropDlg(HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
	switch(uMsg)
	{
	case PSCB_INITIALIZED:
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInst, (LPCTSTR)IDI_SETUP));
		SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(g_hInst, (LPCTSTR)IDI_SETUP));
		g_prhWnd=hwndDlg;
	}

	return 0;
}

BOOL CALLBACK InfoDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
	{
		int i;
		HICON hIcon;
		hIcon=LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_AUP));
		SendDlgItemMessage(hWnd, IDC_MOVE_UP, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		hIcon=LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_ADW));
		SendDlgItemMessage(hWnd, IDC_MOVE_DOWN, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		hIcon=LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_ALT));
		SendDlgItemMessage(hWnd, IDC_ADD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		hIcon=LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_ARH));
		SendDlgItemMessage(hWnd, IDC_REMOVE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

		for(i=0;i<PROPSIZE;i++)
		{
			if(!pSettings->IsExported(proplist[i].nID))
			{
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_INSERTSTRING, -1, (LPARAM)proplist[i].szTitle);
				int iItem=SendDlgItemMessage(hWnd, IDC_LIST2, LB_FINDSTRINGEXACT, 0, (LPARAM)proplist[i].szTitle);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_SETITEMDATA, iItem, proplist[i].nID);
			}
		}

		int j;
		for(i=0;i<pSettings->sizeExporting;i++)
		{
			j=0;
			while(proplist[j++].nID!=pSettings->pnExporting[i]);
			j--;
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_INSERTSTRING, -1, (LPARAM)proplist[j].szTitle);
			int iItem=SendDlgItemMessage(hWnd, IDC_LIST1, LB_FINDSTRINGEXACT, 0, (LPARAM)proplist[j].szTitle);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_SETITEMDATA, iItem, proplist[j].nID);
		}
	}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ADD:
			SwapListBoxValues(hWnd, IDC_LIST2, IDC_LIST1);
			PropSheet_Changed(g_prhWnd, hWnd);
			return TRUE;
		case IDC_REMOVE:
			SwapListBoxValues(hWnd, IDC_LIST1, IDC_LIST2);
			PropSheet_Changed(g_prhWnd, hWnd);
			return TRUE;
		case IDC_MOVE_UP:
		{
			int iSelected=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			if(iSelected!=LB_ERR && iSelected>0)
			{
				char szSelected[256];
				SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, iSelected, (LPARAM)szSelected);
				int nData=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETITEMDATA, iSelected, 0);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_DELETESTRING, iSelected, 0);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_INSERTSTRING, iSelected-1, (LPARAM)szSelected);
				SendDlgItemMessage(hWnd, IDC_LIST1, LB_SETITEMDATA, iSelected-1, nData);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_SETCURSEL, iSelected-1, 0);
			}
		}
			PropSheet_Changed(g_prhWnd, hWnd);
			return TRUE;
		case IDC_MOVE_DOWN:
		{
			int iSelected=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			int iCount=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCOUNT, 0, 0);
			if(iSelected!=LB_ERR && iSelected<iCount-1)
			{
				char szSelected[256];
				SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, iSelected, (LPARAM)szSelected);
				int nData=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETITEMDATA, iSelected, 0);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_DELETESTRING, iSelected, 0);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_INSERTSTRING, iSelected+1, (LPARAM)szSelected);
				SendDlgItemMessage(hWnd, IDC_LIST1, LB_SETITEMDATA, iSelected+1, nData);

				SendDlgItemMessage(hWnd, IDC_LIST1, LB_SETCURSEL, iSelected+1, 0);
			}
		}
			PropSheet_Changed(g_prhWnd, hWnd);
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		switch(((NMHDR FAR*)lParam)->code)
		{
		case PSN_KILLACTIVE:
			pSettings->nSheet=0;
			SetWindowLong(hWnd, DWL_MSGRESULT, FALSE);
			return TRUE;
		case PSN_APPLY:
		{
			int iCount=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCOUNT, 0, 0);
			pSettings->sizeExporting=iCount;
			if(pSettings->pnExporting) delete pSettings->pnExporting;
			pSettings->pnExporting=new int[pSettings->sizeExporting];
			TRACE ("pnExporting allocated at %x[%d]\n", pSettings->pnExporting, pSettings->sizeExporting);
			for(int i=0;i<iCount;i++)
				pSettings->pnExporting[i]=SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETITEMDATA, i, 0);
		}
			SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CALLBACK ExtDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
	{
		ListView_SetExtendedListViewStyleEx(GetDlgItem(hWnd, IDC_EXTLIST), LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

		RECT rect;
		GetWindowRect(GetDlgItem(hWnd, IDC_EXTLIST), &rect);
		LVCOLUMN column;

		column.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
		column.fmt=LVCFMT_CENTER;

		column.cx=rect.right-rect.left-105;
		column.pszText="Описание";
		column.cchTextMax=8;

		SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_INSERTCOLUMN , (WPARAM)0, (LPARAM)&column);

		column.cx=80;
		column.pszText="Расширение";
		column.cchTextMax=10;

		SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_INSERTCOLUMN , (WPARAM)0, (LPARAM)&column);

		CStringArray csaExt;
		SplitExtensions(pSettings->szExtensions, csaExt);
		for(int nExt=0;nExt<csaExt.GetSize();nExt++)
			InsertExtension(hWnd, IDC_EXTLIST, csaExt[nExt].GetBuffer(csaExt[nExt].GetLength()));
	}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_DELEXTALL:
			SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_DELETEALLITEMS , (WPARAM)0, (LPARAM)0);
			break;
		case IDC_DELEXT:
		{
			int nItem=SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_GETNEXTITEM, (WPARAM)(-1), MAKELPARAM(LVNI_SELECTED,0));
			if(nItem!=-1)
			{
				SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_DELETEITEM, (WPARAM)nItem, (LPARAM)0);
				PropSheet_Changed(g_prhWnd, hWnd);
			}
		}
			break;
		case IDC_ADDEXT:
			char szExt[_MAX_EXT];
			GetDlgItemText(hWnd, IDC_EXTENSION, szExt, _MAX_EXT);
			SetDlgItemText(hWnd, IDC_EXTENSION, "");

			if(InsertExtension(hWnd, IDC_EXTLIST, szExt))
				PropSheet_Changed(g_prhWnd, hWnd);

			break;
		}
		break;
	case WM_NOTIFY:
		switch(((NMHDR FAR*)lParam)->code)
		{
			case PSN_KILLACTIVE:
				pSettings->nSheet=2;
				SetWindowLong(hWnd, DWL_MSGRESULT, FALSE);
				return TRUE;
				break;
			case PSN_APPLY:
				int nItems=SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_GETITEMCOUNT, (WPARAM)0, (LPARAM)0);
			
				char szExt[_MAX_EXT];
				LVITEM item;

				item.mask=LVIF_TEXT;
				item.iSubItem=0;
				item.pszText=szExt;
				item.cchTextMax =_MAX_EXT;
				
				strcpy(pSettings->szExtensions, "");

				for(int nItem=0;nItem<nItems;nItem++)
				{
//					item.iItem=nItem;
					SendDlgItemMessage(hWnd, IDC_EXTLIST, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&item);
					strcat(pSettings->szExtensions, szExt);
					if(nItem!=nItems-1) strcat(pSettings->szExtensions, ",");
				}

				SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);
				return TRUE;
				break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK FileDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		CheckRadioButton(hWnd, IDC_RADDIR_1, IDC_RADDIR_4, IDC_RADDIR_1+pSettings->nListSaveAt);
		CheckRadioButton(hWnd, IDC_RADNAME_1, IDC_RADNAME_2, IDC_RADNAME_1+pSettings->nListName);
		if(pSettings->nListSaveAt==3)
			SetDlgItemText(hWnd, IDC_DIR, pSettings->szListSaveAt);
		else
			EnableWindow(GetDlgItem(hWnd, IDC_DIR), FALSE);

		SendDlgItemMessage(hWnd, IDC_OPENAFTERALL, BM_SETCHECK, (WPARAM)pSettings->bOpenAfterAll?BST_CHECKED:BST_UNCHECKED, (LPARAM)NULL);

		if(pSettings->nListName==1)
			SetDlgItemText(hWnd, IDC_NAME, pSettings->szListName);
		else
			EnableWindow(GetDlgItem(hWnd, IDC_NAME), FALSE);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_RADDIR_1:
		case IDC_RADDIR_2:
		case IDC_RADDIR_3:
			if(LOWORD(wParam)-IDC_RADDIR_1!=pSettings->nListSaveAt)
				PropSheet_Changed(g_prhWnd, hWnd);
			EnableWindow(GetDlgItem(hWnd, IDC_DIR), FALSE);
			break;
		case IDC_RADDIR_4:
			if(LOWORD(wParam)-IDC_RADDIR_1!=pSettings->nListSaveAt)
				PropSheet_Changed(g_prhWnd, hWnd);
			EnableWindow(GetDlgItem(hWnd, IDC_DIR), TRUE);
			break;
		case IDC_RADNAME_1:
			if(LOWORD(wParam)-IDC_RADNAME_1!=pSettings->nListName)
				PropSheet_Changed(g_prhWnd, hWnd);
			EnableWindow(GetDlgItem(hWnd, IDC_NAME), FALSE);
			break;
		case IDC_RADNAME_2:
			if(LOWORD(wParam)-IDC_RADNAME_1!=pSettings->nListName)
				PropSheet_Changed(g_prhWnd, hWnd);
			EnableWindow(GetDlgItem(hWnd, IDC_NAME), TRUE);
			break;
		}
		break;
	case WM_NOTIFY:
		switch(((NMHDR FAR*)lParam)->code)
		{
		case PSN_KILLACTIVE:
			pSettings->nSheet=1;
			if(IsDlgButtonChecked(hWnd, IDC_RADDIR_4))
			{
				char szDir[MAX_PATH];
				GetDlgItemText(hWnd, IDC_DIR, szDir, MAX_PATH);
				if(!lstrlen(szDir))
				{
					ErrorBox("Вы должны ввести имя папку");
					SetWindowLong(hWnd, DWL_MSGRESULT, TRUE);
					return TRUE;
				}
				if(!CFileSystem::IsDirectoryExists(szDir))
				{
					ErrorBox("Папка не существует");
					SetWindowLong(hWnd, DWL_MSGRESULT, TRUE);
					return TRUE;
				}

			}
			if(IsDlgButtonChecked(hWnd, IDC_RADNAME_2))
			{
				char szFileName[_MAX_FNAME];
				GetDlgItemText(hWnd, IDC_NAME, szFileName, _MAX_FNAME);
				if(!lstrlen(szFileName))
				{
					ErrorBox("Вы должны ввести имя файла");
					SetWindowLong(hWnd, DWL_MSGRESULT, TRUE);
					return TRUE;
				}
			}
			SetWindowLong(hWnd, DWL_MSGRESULT, FALSE);
			return TRUE;
			break;
		case PSN_APPLY:
			if(IsDlgButtonChecked(hWnd, IDC_OPENAFTERALL))
				pSettings->bOpenAfterAll=true;
			else
				pSettings->bOpenAfterAll=false;

			if(IsDlgButtonChecked(hWnd, IDC_RADDIR_1))
				pSettings->nListSaveAt=0;
			if(IsDlgButtonChecked(hWnd, IDC_RADDIR_2))
				pSettings->nListSaveAt=1;
			if(IsDlgButtonChecked(hWnd, IDC_RADDIR_3))
				pSettings->nListSaveAt=2;
			if(IsDlgButtonChecked(hWnd, IDC_RADDIR_4))
			{
				char szDir[MAX_PATH];
				GetDlgItemText(hWnd, IDC_DIR, szDir, MAX_PATH);
				pSettings->nListSaveAt=3;
				pSettings->sizeListSaveAt=lstrlen(szDir)+1;
				if(pSettings->szListSaveAt) delete pSettings->szListSaveAt;
				pSettings->szListSaveAt=new char[pSettings->sizeListSaveAt];
				strcpy(pSettings->szListSaveAt, szDir);
			}

			if(IsDlgButtonChecked(hWnd, IDC_RADNAME_1))
				pSettings->nListName=0;
			if(IsDlgButtonChecked(hWnd, IDC_RADNAME_2))
			{
				char szFileName[_MAX_FNAME];
				GetDlgItemText(hWnd, IDC_NAME, szFileName, _MAX_FNAME);
				pSettings->nListName=1;
				pSettings->sizeListName=lstrlen(szFileName)+1;
				if(pSettings->szListName) delete pSettings->szListName;
				pSettings->szListName=new char[pSettings->sizeListName];
				strcpy(pSettings->szListName, szFileName);
			}
		
		SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);
			return TRUE;
		}
	}
	return FALSE;
}

void SwapListBoxValues(HWND hWnd, int nIDListBox1, int nIDListBox2)
{
	int iSelected=SendDlgItemMessage(hWnd, nIDListBox1, LB_GETCURSEL, 0, 0);
	int iCount=SendDlgItemMessage(hWnd, nIDListBox1, LB_GETCOUNT, 0, 0);
	if(iSelected!=LB_ERR)
	{
		char szSelected[256];
		SendDlgItemMessage(hWnd, nIDListBox1, LB_GETTEXT, iSelected, (LPARAM)szSelected);
		int nData=SendDlgItemMessage(hWnd, nIDListBox1, LB_GETITEMDATA, iSelected, 0);

		SendDlgItemMessage(hWnd, nIDListBox2, LB_INSERTSTRING, -1, (LPARAM)szSelected);
		int iItem=SendDlgItemMessage(hWnd, nIDListBox2, LB_FINDSTRINGEXACT, 0, (LPARAM)szSelected);
		SendDlgItemMessage(hWnd, nIDListBox2, LB_SETITEMDATA, iItem, nData);

		SendDlgItemMessage(hWnd, nIDListBox1, LB_DELETESTRING, iSelected, 0);
		if(iSelected>=iCount-1) iSelected=iCount-2;
		if(iSelected>=0)
			SendDlgItemMessage(hWnd, nIDListBox1, LB_SETCURSEL, iSelected, 0);
	}
}

////////////////////////////////////
// Saver Procedures
int SetIntParamDump(int nPlugID, char* szParamName, DWORD dwValue){return 0;}
int SetStrParamDump(int nPlugID, char* szParamName, char* szValue){return 0;}

int SetIntParam(int nPlugID, char* szParamName, DWORD dwValue)
{
	CAppRegistry *pReg=new CAppRegistry("Fiziki\\VSL", m_Plugins.FindPlugin(nPlugID)->GetPluginName(), TRUE);
	LONG lError=pReg->AppSetValue(szParamName, dwValue);
	delete pReg;
	return lError;
}

int SetStrParam(int nPlugID, char* szParamName, char* szValue)
{
	CAppRegistry *pReg=new CAppRegistry("Fiziki\\VSL", m_Plugins.FindPlugin(nPlugID)->GetPluginName(), TRUE);
	LONG lError=pReg->AppSetValue(szParamName, szValue);
	delete pReg;
	return lError;
}

void GetStrParam(int nPlugID, char* szParamName, char** pszValue, int* pnSize, char* szDefaultValue)
{
	CAppRegistry *pReg=new CAppRegistry("Fiziki\\VSL", m_Plugins.FindPlugin(nPlugID)->GetPluginName(), TRUE);
	CString sValue=pReg->AppGetValue(szParamName, szDefaultValue);
	char* szValue=sValue.GetBufferSetLength(sValue.GetLength());
	if(lstrlen(szValue)<*pnSize)
	{
		*pnSize=lstrlen(szValue);
		lstrcpy(*pszValue, szValue);
	}
	else
		strncpy(*pszValue, szValue, *pnSize);

	delete pReg;
}

DWORD GetIntParam(int nPlugID, char* szParamName, DWORD dwDefault)
{
	CAppRegistry *pReg=new CAppRegistry("Fiziki\\VSL", m_Plugins.FindPlugin(nPlugID)->GetPluginName(), TRUE);
	DWORD dwRet=pReg->AppGetNumericValue(szParamName, dwDefault);
	delete pReg;
	return dwRet;
}

BOOL ProcessFile(CString& sFilename, CTemplateArray* ctaLineTemplate, int nLineNumber)
{
	int i;
	CMediaInfo mi;
	BOOL bMediaSupport=ctaLineTemplate->SupportsMedia();
	if(bMediaSupport)
		if(!mi.ReadFile(sFilename)) return FALSE;

	SPLITINFO si=CFileSystem::SplitFileName(sFilename.GetBuffer(sFilename.GetLength()));
	CString sFileLength;
	CFile fileTmp;
	if(fileTmp.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
	{
		sFileLength.Format("%d", fileTmp.GetLength());
		fileTmp.Close();
	}

	CString sParity;
	sParity.Format("%d", nLineNumber%2);
	ctaLineTemplate->Replace("%%PARITY%%", sParity);

	if(bMediaSupport)
	{
		int nStreams=mi.GetStreamsCount();
		CStreamInfo* pInfo;
			CString sVWidth, sVHeight, sVFramerate, sVBitrate, sVCodec, sFOURCC;
		for(i=0;i<nStreams;i++)
		{
			pInfo=new CStreamInfo;
			mi.GetStreamInfo(i, pInfo);
			if(pInfo->GetStreamType()==STREAM_TYPE_VIDEO1)
			{
				sVWidth.Format("%d", pInfo->GetFrameWidth());
				sVHeight.Format("%d", pInfo->GetFrameHeight());
				sVFramerate.Format("%f", mi.GetFramerate(i));
				sVBitrate.Format("%f", mi.GetStreamBPS(i)/1024);
				sVCodec.Format("%s", CStreamInfo::GetFourCCDescription(pInfo->GetFourCCCodec()));
				sFOURCC.Format("%s", pInfo->GetFourCCCodec());
				delete pInfo;
				break;
			}
			delete pInfo;
		}

		CString sACodec, sAChannels, sASampling, sABitrate;
		for(i=0;i<nStreams;i++)
		{
			pInfo=new CStreamInfo;
			mi.GetStreamInfo(i, pInfo);
			if(pInfo->GetStreamType()==STREAM_TYPE_WAVE)
			{
				sACodec.Format("%s",CStreamInfo::GetAudioDescription(pInfo->GetAudioFormat()));
				sAChannels.Format("%d", pInfo->GetAudioChannels());
				sASampling.Format("%d", pInfo->GetAudioSamplesPerSecond());
				sABitrate.Format("%f",pInfo->GetAudioBPS()/1024);
				delete pInfo;
				break;
			}
			delete pInfo;
		}

		ctaLineTemplate->Replace("%%VWIDTH%%", sVWidth);
		ctaLineTemplate->Replace("%%VHEIGHT%%", sVHeight);
		ctaLineTemplate->Replace("%%VFRAMERATE%%", sVFramerate);
		ctaLineTemplate->Replace("%%VBITRATE%%", sVBitrate);
		ctaLineTemplate->Replace("%%VCODEC%%", sVCodec);
		ctaLineTemplate->Replace("%%VFOURCC%%", sFOURCC);

		ctaLineTemplate->Replace("%%ACODEC%%", sACodec);
		ctaLineTemplate->Replace("%%ACHANNELS%%", sAChannels);
		ctaLineTemplate->Replace("%%ASAMPLING%%", sASampling);
		ctaLineTemplate->Replace("%%ABITRATE%%", sABitrate);
	}

	CString sArtist, sTitle, sType;
	SplitFileName(sFilename, &sArtist, &sTitle, &sType);
	ctaLineTemplate->Replace("%%ARTIST%%",sArtist);
	ctaLineTemplate->Replace("%%TITLE%%",sTitle);
	ctaLineTemplate->Replace("%%TYPE%%",sType);
	ctaLineTemplate->Replace("%%FILESIZE%%", sFileLength);

	ctaLineTemplate->Replace("%%FULLPATH%%", sFilename);
//	ctaLineTemplate->Replace("%%UFULLPATH%%", sFilename);
	ctaLineTemplate->Replace("%%SHORTFILENAME%%", si.FilenameWithoutExtension);

	return TRUE;
}

void SplitFileName(CString sFilename, CString *psArtist, CString *psTitle, CString *psType)
{
	SPLITINFO si=CFileSystem::SplitFileName(sFilename.GetBufferSetLength(sFilename.GetLength()));
	sFilename=si.FilenameWithExtension;
	int nDiv=sFilename.Find(" - ", 0);
	int nDot=sFilename.ReverseFind('.');

	*psArtist=sFilename.Left(nDiv);
	*psTitle=sFilename.Mid(nDiv+3, nDot-nDiv-3);
	*psType=sFilename.Mid(nDot+1);
	psType->MakeUpper();
	if(!psType->Compare("MPG")) psType->Format("MPEG");
}

BOOL CALLBACK ProgressDlg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

void GetOutFileName(CString& sBaseFilename, char* m_szFile)
{
	pSettings=new CSettings(g_szSettings);
	switch(pSettings->nListSaveAt)
	{
	case 0:
		sBaseFilename=m_szFile;
		break;
	case 1:
		sBaseFilename=m_szFile;
		int nSlash;
		if((nSlash=sBaseFilename.ReverseFind('\\'))!=-1)
			sBaseFilename=sBaseFilename.Left(nSlash);
		break;
	case 2:
		char szPath[MAX_PATH];
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE);
		sBaseFilename=szPath;
		break;
	case 3:
		sBaseFilename=pSettings->szListSaveAt;
		break;
	}
		if(sBaseFilename[sBaseFilename.GetLength()-1]!='\\') sBaseFilename+='\\';
		switch(pSettings->nListName)
	{
	case 0:
	{
		SPLITINFO si=CFileSystem::SplitFileName(m_szFile);
		sBaseFilename+=si.FilenameWithoutExtension;
	}
		break;
	case 1:
		sBaseFilename+=pSettings->szListName;
		break;
	}
	delete pSettings;
}

void InitProgressBar(HWND hProgress, char* szText, int nRange, int nStep)
{
	SetDlgItemText(hProgress, IDC_INFO, _T(szText));
	SendDlgItemMessage(hProgress, IDC_PROGRESS, PBM_SETRANGE, 0, nRange);
	SendDlgItemMessage(hProgress, IDC_PROGRESS, PBM_SETSTEP, (WPARAM)nStep, 0); 
	SendDlgItemMessage(hProgress, IDC_PROGRESS, PBM_SETPOS, 0, 0);
}

void StepProgressBar(HWND hProgress, int n, int max)
{
	int r,g,b;

	r=RED_START+(RED_END-RED_START)*n/(max-1);
	g=GREEN_START+(GREEN_END-GREEN_START)*n/(max-1);
	b=BLUE_START+(BLUE_END-BLUE_START)*n/(max-1);

	SendDlgItemMessage(hProgress, IDC_PROGRESS, PBM_SETBARCOLOR, 0, (LPARAM)RGB(r,g,b));
	SendDlgItemMessage(hProgress, IDC_PROGRESS, PBM_STEPIT, 0, 0);
}

CString GetNameField(int nField)
{
	for(int i=0;i<PROPSIZE;i++)
		if(proplist[i].nID==nField)
			return proplist[i].szTitle;

	return "";
}

CString GetTextField(int nField)
{
	switch(nField)
	{
	case PROP_ARTIST:		return "%%ARTIST%%";
	case PROP_TITLE:		return "%%TITLE%%";
	case PROP_TYPE:			return "%%TYPE%%";
	case PROP_FILESIZE:		return "%%FILESIZE%%";
	case PROP_VWIDTH:		return "%%VWIDTH%%";
	case PROP_VHEIGHT:		return "%%VHEIGHT%%";
	case PROP_VFRAMERATE:	return "%%VFRAMERATE%%";
	case PROP_VBITRATE:		return "%%VBITRATE%%";
	case PROP_VCODEC:		return "%%VCODEC%%";
	case PROP_VFOURCC:		return "%%VFOURCC%%";
	case PROP_ACODEC:		return "%%ACODEC%%";
	case PROP_ACHANNELS:	return "%%ACHANNELS%%";
	case PROP_ASAMPLING:	return "%%ASAMPLING%%";
	case PROP_ABITRATE:		return "%%ABITRATE%%";
	default:				return "";
	}
}

CString GenerateTemplateString(CStringArray &csaList, char* szHead, char* szDelim, char* szEnd)
{
	CString out;
	out=szHead;

	for(int n=0;n<csaList.GetSize();n++)
	{
		if(n!=0) out+=szDelim;
		out+=csaList[n];
	}

	out+=szEnd;

	return out;
}

void SplitExtensions(CString sExt, CStringArray& csaExt)
{
	int nPos=0, nOldPos=0;
	do
	{
		nPos=sExt.Find(',', nOldPos);
		if(nPos==-1)
			csaExt.Add(sExt.Mid(nOldPos));
		else
			csaExt.Add(sExt.Mid(nOldPos, nPos-nOldPos));
		nOldPos=nPos+1;
	}while(nPos!=-1);
}

BOOL InsertExtension(HWND hWnd, int nListBox, char* szExt)
{
	if(lstrlen(szExt)==0) return FALSE;

	LVFINDINFO find;

	find.flags=LVFI_STRING;
	find.psz=szExt;

	if(SendDlgItemMessage(hWnd, nListBox, LVM_FINDITEM, (WPARAM)(-1), (LPARAM)&find)!=-1)
		return FALSE;

	CRegKey RegKey;

	CRegistry reg;
	CString ext=".";
	ext+=szExt;
	CString classtype=reg.GetValue(HKEY_CLASSES_ROOT, ext, "");
	classtype=reg.GetValue(HKEY_CLASSES_ROOT, classtype, "");
	classtype.Replace("Key doesn't exist", "");
			
	LVITEM item;

	item.mask=LVIF_TEXT;
	item.iItem=0;
	item.iSubItem=0;
	item.pszText=szExt;
	item.cchTextMax=lstrlen(szExt);
			
	int nInserted=SendDlgItemMessage(hWnd, nListBox, LVM_INSERTITEM , (WPARAM)0, (LPARAM)&item);

	if(nInserted!=-1)
	{
		item.iItem=nInserted;
		item.iSubItem=1;
		item.pszText=classtype.GetBuffer(classtype.GetLength());
		item.cchTextMax=classtype.GetLength();
			
		SendDlgItemMessage(hWnd, nListBox, LVM_SETITEM, (WPARAM)0, (LPARAM)&item);
		return TRUE;
	}
	return FALSE;
}