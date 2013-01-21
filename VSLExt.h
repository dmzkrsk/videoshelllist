// VSLExt.h : Declaration of the CVSLExt

#ifndef __VSLEXT_H_
#define __VSLEXT_H_

#include "resource.h"       // main symbols

#include <shlobj.h>
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// CVSLExt
class ATL_NO_VTABLE CVSLExt : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVSLExt, &CLSID_VSLExt>,
	public IDispatchImpl<IVSLExt, &IID_IVSLExt, &LIBID_VIDEOSHELLLISTLib>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CVSLExt()
	{
//		m_BMP = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_ICON));
	}

DECLARE_REGISTRY_RESOURCEID(IDR_VSLEXT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CVSLExt)
	COM_INTERFACE_ENTRY(IVSLExt)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()

// IVSLExt
public:
	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);
	STDMETHOD(GetCommandString)(UINT, UINT, UINT*, LPSTR, UINT);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
	STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);

	static void GenerateFileList(CString sPath, CStringArray &csaExtensions, CStringArray &csaList, BOOL bSub, HWND hCallbackWnd=NULL);
	void FillMenu(BOOL bUseList, HMENU hMenu, int *i, int uidCmd);
	BOOL Setup();
private:
	HBITMAP m_hMenuBmp, m_hMenuTools;
	HWND hDlgWnd;
	TCHAR m_szFile[MAX_PATH];

	UINT nSetupID;
};

#endif //__VSLEXT_H_
