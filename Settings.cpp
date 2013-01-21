#include "StdAfx.h"
#include "Settings.h"

Properties proplist[]=
{
	{PROP_ARTIST, "Исполнитель"},
	{PROP_TITLE, "Название"},
	{PROP_TYPE, "Тип видео"},
	{PROP_FILESIZE, "Размер файла"},
	{PROP_VWIDTH, "Ширина видео"},
	{PROP_VHEIGHT, "Высота видео"},
	{PROP_VFRAMERATE, "Частота кадров"},
	{PROP_VBITRATE, "Битрейт видео"},
	{PROP_VCODEC, "Видео-кодек"},
	{PROP_VFOURCC, "FOURCC"},
	{PROP_ACODEC, "Аудио кодек"},
	{PROP_ACHANNELS, "Аудиоканалы"},
	{PROP_ASAMPLING, "Частота дискретизации"},
	{PROP_ABITRATE, "Битрейт аудио"}
};

CSettings::CSettings(char* szFile)
{
	pnExporting=NULL;
	szListName=NULL;
	szListSaveAt=NULL;

	lstrcpy(szSaveFile,szFile);

	Load();
}

CSettings::~CSettings()
{
	Clear();
}

void CSettings::Load()
{
	CFile setFile;

	Clear();

	if(setFile.Open(szSaveFile, CFile::modeRead))
	{
		setFile.Read(&sizeListSaveAt, sizeof(sizeListSaveAt));

		szListSaveAt=new char[sizeListSaveAt];

		setFile.Read(szListSaveAt, sizeListSaveAt);
		setFile.Read(&nListSaveAt, sizeof(nListSaveAt));

		setFile.Read(&sizeListName, sizeof(sizeListName));

		szListName=new char[sizeListName];

		setFile.Read(szListName, sizeListName);
		setFile.Read(&nListName, sizeof(nListName));

		setFile.Read(&sizeExporting, sizeof(sizeExporting));
		setFile.Read(&bOpenAfterAll, sizeof(bOpenAfterAll));
		setFile.Read(szExtensions, MAX_EXTLIST);
		
		pnExporting=new int[sizeExporting];

		for(int n=0;n<sizeExporting;n++)
			setFile.Read(&(pnExporting[n]), sizeof(pnExporting[n]));

		setFile.Read(&nSheet, sizeof(nSheet));
		setFile.Close();
	}
	else
	{
		sizeListSaveAt=0;
		szListSaveAt=NULL;
		nListSaveAt=2;

		sizeListName=0;
		szListName=NULL;
		nListName=0;

		sizeExporting=0;
		pnExporting=NULL;

		bOpenAfterAll=false;
		strcpy(szExtensions, DEFAULT_EXTENSIONS);

		nSheet=1;
	}
}

BOOL CSettings::Dump()
{
	CFile setFile;
	if(setFile.Open(szSaveFile, CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive))
	{
		setFile.Write(&sizeListSaveAt, sizeof(sizeListSaveAt));
		setFile.Write(szListSaveAt, sizeListSaveAt);
		setFile.Write(&nListSaveAt, sizeof(nListSaveAt));

		setFile.Write(&sizeListName, sizeof(sizeListName));
		setFile.Write(szListName, sizeListName);
		setFile.Write(&nListName, sizeof(nListName));

		setFile.Write(&sizeExporting, sizeof(sizeExporting));
		setFile.Write(&bOpenAfterAll, sizeof(bOpenAfterAll));
		setFile.Write(szExtensions, MAX_EXTLIST);
		
		for(int n=0;n<sizeExporting;n++)
			setFile.Write(&(pnExporting[n]), sizeof(pnExporting[n]));

		setFile.Write(&nSheet, sizeof(nSheet));
		setFile.Close();

		return TRUE;
	}
	else return FALSE;
}

void CSettings::Clear()
{
	if(pnExporting)
	{
		delete pnExporting;
		pnExporting=NULL;
	}
	if(szListSaveAt) {delete szListSaveAt; szListSaveAt=NULL;}
	if(szListName) {delete szListName; szListName=NULL;}
}

BOOL CSettings::IsExported(int nVal)
{
	for(int i=0;i<sizeExporting;i++)
		if(pnExporting[i]==nVal)
			return TRUE;

	return FALSE;
}
