#ifndef regLinkage
#define regLinkage __declspec (dllimport)
#endif

class regLinkage CRegistry
{
public:
	long GetNumericValue(HKEY Root, CString Path, CString Key);
	int DeleteKey(HKEY Root, CString Path, CString Key);
	BOOL IsKeyExist(HKEY Root, CString Path, CString Key);
	BOOL IsNumericKeyExist(HKEY Root, CString Path, CString Key);
	int SetValue(HKEY Root, CString Path, CString Key, CString Value);
	int SetValue(HKEY Root, CString Path, CString Key, DWORD Value);
	int DeleteValue(HKEY Root, CString Path, CString Key);
	CString GetValue(HKEY Root, CString Path, CString Key);
	CRegistry();
	~CRegistry();

private:
	class CRegKey* pRegKey;
};

class regLinkage CAppRegistry : CRegistry
{
public:
	long AppGetNumericValue(CString Key, long DefaultValue=0);
	CString AppGetValue(CString Key, CString DefaultValue="");
	int AppDeleteKey(CString Key) {return DeleteValue(RegRoot, RegPath, Key);};
	BOOL AppIsKeyExist(CString Key) {return IsKeyExist(RegRoot, RegPath, Key);};
	BOOL AppIsNumericKeyExist(CString Key) {return IsNumericKeyExist(RegRoot, RegPath, Key);};
	int AppSetValue(CString Key, CString Value) {return SetValue(RegRoot, RegPath, Key, Value);};
	int AppSetValue(CString Key, DWORD Value) {return SetValue(RegRoot, RegPath, Key, Value);};
	int AppDeleteValue(CString Key) {return DeleteValue(RegRoot, RegPath, Key);};

	CAppRegistry(CString Company, CString AppName, BOOL ToLocalMachine=FALSE);
private:
	HKEY RegRoot;
	CString RegPath;
};