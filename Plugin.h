#include "Plugins.h"
#include "PlugConst.h"

#define PLER_OK				0
#define PLER_LOADLIBRARY	1
#define PLER_LOADSTRING		2

class CPluginInstance;
void FreePluginInstance(CPluginInstance* pI);

class CPlugin
{
private:
	char szPluginPath[MAX_PATH];
	char szPluginTitle[MAX_PTITLE];
	char szPluginFileExt[MAX_EXT];
	char szPluginMessage[MAX_PMESSAGE];
	char szPluginStatusString[MAX_PSTATUS];
	char szPluginName[MAX_PNAME];

	int nPluginID;

	PLUGININFO PlInfo;

	HBITMAP hPluginBmp;

public:
	CPlugin();
	~CPlugin();

	int LoadPlugin(const char* szFilename);
	CPluginInstance* InitPlugin(PLUGINMODE mode, SAVER* saver);

	void SetID(int nID) {nPluginID=nID;};
	int GetID() {return nPluginID;};

	BOOL IsPlaylist() {return PlInfo.bPlaylist;};
	BOOL IsTable() {return !PlInfo.bPlaylist;};
	BOOL IsSetup() {return PlInfo.bSetup;};
	BOOL IsSelfEx() {return PlInfo.bSelfEx;};

	HBITMAP GetBitmap() {return hPluginBmp;};

	char* GetStatusString() {return szPluginStatusString;};
	char* GetFileExtension() {return szPluginFileExt;};
	char* GetOKMessage() {return szPluginMessage;};
	char* GetMenuString() {return szPluginTitle;};
	char* GetPluginPath() {return szPluginPath;};
	char* GetPluginName() {return szPluginName;};
};

class CPlugins
{
private:
	CArray<CPlugin*, CPlugin*> pArray;
public:
	CPlugins();
	~CPlugins();

	int GetSize() {return pArray.GetSize();};
	int GetBound() {return pArray.GetSize()-1;};

	CPlugin* CreateElement();
	void DeleteElement();
	void DeleteAll();

	CPlugin* GetAt(int nElement);
	CPlugin* operator [](int nElement) {return GetAt(nElement);};

	CPlugin* FindPlugin(int nPluginID);
};

class CPluginInstance
{
friend class CPlugin;
private:
	HINSTANCE hInst;
	CPlugin* pPlugin;

	BOOL bInitialized;

	PLUGINMODE mode;

	PLUGININITPLUGIN InitPlugin;
	PLUGINGETSTRING GetString;
	DLGPROC DialogProc;
	CFile m_File;

/*	char szFormatTemplate[MAX_FORMAT_TEMPLATE];
	char szLineStart[MAX_LINE_START];
	char szLineEnd;
	char szTblStart;
	char szTblEnd;
*/
public:
	CPluginInstance(HINSTANCE hPlugin, CPlugin* pPlugin, SAVER* pSaver, PLUGINMODE PluginMode);
	~CPluginInstance();

	BOOL PluginInitialized() {return bInitialized;};

	DLGPROC GetDialogProc(){return DialogProc;};
	HINSTANCE GetInstance(){return hInst;};

/*	char* GetFormatTemplate(){if(bInitialized)return szFormatTemplate;else return NULL;};
	char* GetLineStart(){if(bInitialized)return szLineStart;else return NULL;};
	char* GetLineEnd(){if(bInitialized)return szLineEnd;else return NULL;};
	char* GetTblStart(){if(bInitialized)return szTblStart;else return NULL;};
	char* GetTblEnd(){if(bInitialized)return szTblEnd;else return NULL;};
*/
	char* TransferString(int nType, int nDiv) {if(bInitialized) return GetString(nType, nDiv);else return NULL;};
	BOOL OpenFile(const char* szFilename);
	BOOL CloseFile();

	BOOL WriteFormatHeader();
	BOOL WriteOutLine(const char* szTemplate);
	BOOL WriteFormatEnd();

	BOOL WriteTblHeader();
	BOOL WriteTblEnd();

	void ExitInstance();
};

class CTemplateArray:public CStringArray
{
public:
	void Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	BOOL SupportsMedia();
};
