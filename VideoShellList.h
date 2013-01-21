/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Jan 18 17:34:43 2005
 */
/* Compiler settings for S:\Documents\Программирование\Development\VideoShellList\VideoShellList.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __VideoShellList_h__
#define __VideoShellList_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IVSLExt_FWD_DEFINED__
#define __IVSLExt_FWD_DEFINED__
typedef interface IVSLExt IVSLExt;
#endif 	/* __IVSLExt_FWD_DEFINED__ */


#ifndef __VSLExt_FWD_DEFINED__
#define __VSLExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class VSLExt VSLExt;
#else
typedef struct VSLExt VSLExt;
#endif /* __cplusplus */

#endif 	/* __VSLExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IVSLExt_INTERFACE_DEFINED__
#define __IVSLExt_INTERFACE_DEFINED__

/* interface IVSLExt */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IVSLExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0F2226A1-47FC-47F2-B01D-C8DDC2CA1949")
    IVSLExt : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IVSLExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IVSLExt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IVSLExt __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IVSLExt __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IVSLExt __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IVSLExt __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IVSLExt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IVSLExt __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IVSLExtVtbl;

    interface IVSLExt
    {
        CONST_VTBL struct IVSLExtVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSLExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSLExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSLExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSLExt_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVSLExt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVSLExt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVSLExt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVSLExt_INTERFACE_DEFINED__ */



#ifndef __VIDEOSHELLLISTLib_LIBRARY_DEFINED__
#define __VIDEOSHELLLISTLib_LIBRARY_DEFINED__

/* library VIDEOSHELLLISTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VIDEOSHELLLISTLib;

EXTERN_C const CLSID CLSID_VSLExt;

#ifdef __cplusplus

class DECLSPEC_UUID("3CA443C1-5F38-45D5-9D7E-7542ADF9E612")
VSLExt;
#endif
#endif /* __VIDEOSHELLLISTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
