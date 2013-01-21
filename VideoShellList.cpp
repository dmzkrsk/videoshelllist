// VideoShellList.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f VideoShellListps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "VideoShellList.h"

#include "VideoShellList_i.c"
#include "VSLExt.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_VSLExt, CVSLExt)
END_OBJECT_MAP()

class CVideoShellListApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoShellListApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CVideoShellListApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CVideoShellListApp, CWinApp)
	//{{AFX_MSG_MAP(CVideoShellListApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CVideoShellListApp theApp;

BOOL CVideoShellListApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_VIDEOSHELLLISTLib);
    return CWinApp::InitInstance();
}

int CVideoShellListApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	CRegKey reg;
	LONG    lRet;

	if ( 0 == (GetVersion() & 0x80000000UL) )
	{
        lRet=reg.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), KEY_SET_VALUE);
        if(ERROR_SUCCESS!=lRet)	return E_ACCESSDENIED;
        lRet=reg.SetValue(_T("VideoShellList extension"), _T("{3CA443C1-5F38-45D5-9D7E-7542ADF9E612}"));
        if(ERROR_SUCCESS != lRet) return E_ACCESSDENIED;
    }

	lRet=reg.Create(HKEY_LOCAL_MACHINE, _T("Software\\Fiziki\\"));
	if(ERROR_SUCCESS!=lRet) return E_ACCESSDENIED;
	lRet=reg.Create(HKEY_LOCAL_MACHINE, _T("Software\\Fiziki\\VSL"));
	if(ERROR_SUCCESS!=lRet) return E_ACCESSDENIED;
	char szDir[MAX_PATH];
	if(GetCurrentDirectory(MAX_PATH, szDir))
	{
		reg.SetValue(_T(szDir), _T("InstallDir"));
		if(ERROR_SUCCESS!=lRet) return E_ACCESSDENIED;
	}
	else return E_UNEXPECTED;

    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	if ( 0 == (GetVersion() & 0x80000000UL) )
	{
		CRegKey reg;
		LONG    lRet;

		lRet = reg.Open ( HKEY_LOCAL_MACHINE,
                          _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
                          KEY_SET_VALUE );

		if ( ERROR_SUCCESS == lRet )
			lRet = reg.DeleteValue ( _T("{3CA443C1-5F38-45D5-9D7E-7542ADF9E612}") );

		lRet=reg.Create(HKEY_LOCAL_MACHINE, _T("Software\\Fiziki"));
		reg.RecurseDeleteKey(_T("VSL"));
	}
    return _Module.UnregisterServer(TRUE);
}


