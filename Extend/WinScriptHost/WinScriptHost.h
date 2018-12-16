

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Nov 04 10:57:14 2018
 */
/* Compiler settings for WinScriptHost.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __WinScriptHost_h__
#define __WinScriptHost_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IObjectProxy_FWD_DEFINED__
#define __IObjectProxy_FWD_DEFINED__
typedef interface IObjectProxy IObjectProxy;

#endif 	/* __IObjectProxy_FWD_DEFINED__ */


#ifndef __IDelegateProxy_FWD_DEFINED__
#define __IDelegateProxy_FWD_DEFINED__
typedef interface IDelegateProxy IDelegateProxy;

#endif 	/* __IDelegateProxy_FWD_DEFINED__ */


#ifndef __IPluginWinScript_FWD_DEFINED__
#define __IPluginWinScript_FWD_DEFINED__
typedef interface IPluginWinScript IPluginWinScript;

#endif 	/* __IPluginWinScript_FWD_DEFINED__ */


#ifndef __ObjectProxy_FWD_DEFINED__
#define __ObjectProxy_FWD_DEFINED__

#ifdef __cplusplus
typedef class ObjectProxy ObjectProxy;
#else
typedef struct ObjectProxy ObjectProxy;
#endif /* __cplusplus */

#endif 	/* __ObjectProxy_FWD_DEFINED__ */


#ifndef __DelegateProxy_FWD_DEFINED__
#define __DelegateProxy_FWD_DEFINED__

#ifdef __cplusplus
typedef class DelegateProxy DelegateProxy;
#else
typedef struct DelegateProxy DelegateProxy;
#endif /* __cplusplus */

#endif 	/* __DelegateProxy_FWD_DEFINED__ */


#ifndef __PluginWinScript_FWD_DEFINED__
#define __PluginWinScript_FWD_DEFINED__

#ifdef __cplusplus
typedef class PluginWinScript PluginWinScript;
#else
typedef struct PluginWinScript PluginWinScript;
#endif /* __cplusplus */

#endif 	/* __PluginWinScript_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IObjectProxy_INTERFACE_DEFINED__
#define __IObjectProxy_INTERFACE_DEFINED__

/* interface IObjectProxy */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IObjectProxy;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BC77B89E-0F28-4BC1-854B-92E9143CF1EB")
    IObjectProxy : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ BSTR *type) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAttrib( 
            /* [in] */ BSTR key,
            /* [in] */ VARIANT *var) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAttrib( 
            /* [in] */ BSTR key,
            /* [retval][out] */ VARIANT *var) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHost( 
            /* [in] */ LONGLONG base) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IObjectProxyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjectProxy * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjectProxy * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjectProxy * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IObjectProxy * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IObjectProxy * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IObjectProxy * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IObjectProxy * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IObjectProxy * This,
            /* [retval][out] */ BSTR *type);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAttrib )( 
            IObjectProxy * This,
            /* [in] */ BSTR key,
            /* [in] */ VARIANT *var);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAttrib )( 
            IObjectProxy * This,
            /* [in] */ BSTR key,
            /* [retval][out] */ VARIANT *var);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHost )( 
            IObjectProxy * This,
            /* [in] */ LONGLONG base);
        
        END_INTERFACE
    } IObjectProxyVtbl;

    interface IObjectProxy
    {
        CONST_VTBL struct IObjectProxyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectProxy_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IObjectProxy_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IObjectProxy_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IObjectProxy_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IObjectProxy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IObjectProxy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IObjectProxy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IObjectProxy_GetType(This,type)	\
    ( (This)->lpVtbl -> GetType(This,type) ) 

#define IObjectProxy_SetAttrib(This,key,var)	\
    ( (This)->lpVtbl -> SetAttrib(This,key,var) ) 

#define IObjectProxy_GetAttrib(This,key,var)	\
    ( (This)->lpVtbl -> GetAttrib(This,key,var) ) 

#define IObjectProxy_SetHost(This,base)	\
    ( (This)->lpVtbl -> SetHost(This,base) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IObjectProxy_INTERFACE_DEFINED__ */


#ifndef __IDelegateProxy_INTERFACE_DEFINED__
#define __IDelegateProxy_INTERFACE_DEFINED__

/* interface IDelegateProxy */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IDelegateProxy;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F32760AD-AFC2-4F7A-87E8-CBDC062523FC")
    IDelegateProxy : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ BSTR *type) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProxy( 
            /* [in] */ LONGLONG base) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProxy( 
            /* [retval][out] */ LONGLONG *base) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHost( 
            /* [in] */ LONGLONG host) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDelegateProxyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDelegateProxy * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDelegateProxy * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDelegateProxy * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDelegateProxy * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDelegateProxy * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDelegateProxy * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDelegateProxy * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDelegateProxy * This,
            /* [retval][out] */ BSTR *type);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProxy )( 
            IDelegateProxy * This,
            /* [in] */ LONGLONG base);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProxy )( 
            IDelegateProxy * This,
            /* [retval][out] */ LONGLONG *base);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHost )( 
            IDelegateProxy * This,
            /* [in] */ LONGLONG host);
        
        END_INTERFACE
    } IDelegateProxyVtbl;

    interface IDelegateProxy
    {
        CONST_VTBL struct IDelegateProxyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDelegateProxy_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDelegateProxy_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDelegateProxy_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDelegateProxy_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDelegateProxy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDelegateProxy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDelegateProxy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IDelegateProxy_GetType(This,type)	\
    ( (This)->lpVtbl -> GetType(This,type) ) 

#define IDelegateProxy_SetProxy(This,base)	\
    ( (This)->lpVtbl -> SetProxy(This,base) ) 

#define IDelegateProxy_GetProxy(This,base)	\
    ( (This)->lpVtbl -> GetProxy(This,base) ) 

#define IDelegateProxy_SetHost(This,host)	\
    ( (This)->lpVtbl -> SetHost(This,host) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDelegateProxy_INTERFACE_DEFINED__ */


#ifndef __IPluginWinScript_INTERFACE_DEFINED__
#define __IPluginWinScript_INTERFACE_DEFINED__

/* interface IPluginWinScript */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPluginWinScript;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7138D80B-A188-4C31-989D-2CD81A71373A")
    IPluginWinScript : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PluginMain( 
            /* [in] */ LPDISPATCH host,
            /* [in] */ LONGLONG vm,
            /* [retval][out] */ BSTR *pluginName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PluginConfig( 
            /* [in] */ LONGLONG hWnd) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPluginWinScriptVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPluginWinScript * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPluginWinScript * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPluginWinScript * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPluginWinScript * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPluginWinScript * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPluginWinScript * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPluginWinScript * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PluginMain )( 
            IPluginWinScript * This,
            /* [in] */ LPDISPATCH host,
            /* [in] */ LONGLONG vm,
            /* [retval][out] */ BSTR *pluginName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PluginConfig )( 
            IPluginWinScript * This,
            /* [in] */ LONGLONG hWnd);
        
        END_INTERFACE
    } IPluginWinScriptVtbl;

    interface IPluginWinScript
    {
        CONST_VTBL struct IPluginWinScriptVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPluginWinScript_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPluginWinScript_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPluginWinScript_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPluginWinScript_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPluginWinScript_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPluginWinScript_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPluginWinScript_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IPluginWinScript_PluginMain(This,host,vm,pluginName)	\
    ( (This)->lpVtbl -> PluginMain(This,host,vm,pluginName) ) 

#define IPluginWinScript_PluginConfig(This,hWnd)	\
    ( (This)->lpVtbl -> PluginConfig(This,hWnd) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPluginWinScript_INTERFACE_DEFINED__ */



#ifndef __WINSCRIPTHOSTLib_LIBRARY_DEFINED__
#define __WINSCRIPTHOSTLib_LIBRARY_DEFINED__

/* library WINSCRIPTHOSTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_WINSCRIPTHOSTLib;

EXTERN_C const CLSID CLSID_ObjectProxy;

#ifdef __cplusplus

class DECLSPEC_UUID("3498D505-0642-43C5-96DA-927ABED9350F")
ObjectProxy;
#endif

EXTERN_C const CLSID CLSID_DelegateProxy;

#ifdef __cplusplus

class DECLSPEC_UUID("EB0A15CB-E94A-4BCF-861B-6828B241FA53")
DelegateProxy;
#endif

EXTERN_C const CLSID CLSID_PluginWinScript;

#ifdef __cplusplus

class DECLSPEC_UUID("F97880BD-3C38-44DC-AC98-5952C776840D")
PluginWinScript;
#endif
#endif /* __WINSCRIPTHOSTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


