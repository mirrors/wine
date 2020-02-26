/*
 * Active Directory services LDAP Provider
 *
 * Copyright 2018 Dmitry Timoshkov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#define COBJMACROS

#include "windef.h"
#include "winbase.h"
#include "initguid.h"
#include "objbase.h"
#include "rpcproxy.h"
#include "iads.h"
#define SECURITY_WIN32
#include "security.h"

#include "wine/heap.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(adsldp);

DEFINE_GUID(CLSID_LDAPNamespace,0x228d9a82,0xc302,0x11cf,0x9a,0xa4,0x00,0xaa,0x00,0x4a,0x56,0x91);

static HMODULE adsldp_hinst;

typedef struct
{
    IADsADSystemInfo IADsADSystemInfo_iface;
    LONG ref;
} AD_sysinfo;

static inline AD_sysinfo *impl_from_IADsADSystemInfo(IADsADSystemInfo *iface)
{
    return CONTAINING_RECORD(iface, AD_sysinfo, IADsADSystemInfo_iface);
}

static HRESULT WINAPI sysinfo_QueryInterface(IADsADSystemInfo *iface, REFIID riid, void **obj)
{
    TRACE("%p,%s,%p\n", iface, debugstr_guid(riid), obj);

    if (!riid || !obj) return E_INVALIDARG;

    if (IsEqualGUID(riid, &IID_IADsADSystemInfo) ||
        IsEqualGUID(riid, &IID_IDispatch) ||
        IsEqualGUID(riid, &IID_IUnknown))
    {
        IADsADSystemInfo_AddRef(iface);
        *obj = iface;
        return S_OK;
    }

    FIXME("interface %s is not implemented\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI sysinfo_AddRef(IADsADSystemInfo *iface)
{
    AD_sysinfo *sysinfo = impl_from_IADsADSystemInfo(iface);
    return InterlockedIncrement(&sysinfo->ref);
}

static ULONG WINAPI sysinfo_Release(IADsADSystemInfo *iface)
{
    AD_sysinfo *sysinfo = impl_from_IADsADSystemInfo(iface);
    LONG ref = InterlockedDecrement(&sysinfo->ref);

    if (!ref)
    {
        TRACE("destroying %p\n", iface);
        heap_free(sysinfo);
    }

    return ref;
}

static HRESULT WINAPI sysinfo_GetTypeInfoCount(IADsADSystemInfo *iface, UINT *count)
{
    FIXME("%p,%p: stub\n", iface, count);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_GetTypeInfo(IADsADSystemInfo *iface, UINT index, LCID lcid, ITypeInfo **info)
{
    FIXME("%p,%u,%#x,%p: stub\n", iface, index, lcid, info);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_GetIDsOfNames(IADsADSystemInfo *iface, REFIID riid, LPOLESTR *names,
                                            UINT count, LCID lcid, DISPID *dispid)
{
    FIXME("%p,%s,%p,%u,%u,%p: stub\n", iface, debugstr_guid(riid), names, count, lcid, dispid);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_Invoke(IADsADSystemInfo *iface, DISPID dispid, REFIID riid, LCID lcid, WORD flags,
                                     DISPPARAMS *params, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr)
{
    FIXME("%p,%d,%s,%04x,%04x,%p,%p,%p,%p: stub\n", iface, dispid, debugstr_guid(riid), lcid, flags,
          params, result, excepinfo, argerr);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_UserName(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_ComputerName(IADsADSystemInfo *iface, BSTR *retval)
{
    UINT size;
    WCHAR *name;

    TRACE("%p,%p\n", iface, retval);

    size = 0;
    GetComputerObjectNameW(NameFullyQualifiedDN, NULL, &size);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        return HRESULT_FROM_WIN32(GetLastError());

    name = SysAllocStringLen(NULL, size);
    if (!name) return E_OUTOFMEMORY;

    if (!GetComputerObjectNameW(NameFullyQualifiedDN, name, &size))
    {
        SysFreeString(name);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    *retval = name;
    return S_OK;
}

static HRESULT WINAPI sysinfo_get_SiteName(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_DomainShortName(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_DomainDNSName(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_ForestDNSName(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_PDCRoleOwner(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_SchemaRoleOwner(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_get_IsNativeMode(IADsADSystemInfo *iface, VARIANT_BOOL *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_GetAnyDCName(IADsADSystemInfo *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_GetDCSiteName(IADsADSystemInfo *iface, BSTR server, BSTR *retval)
{
    FIXME("%p,%s,%p: stub\n", iface, debugstr_w(server), retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_RefreshSchemaCache(IADsADSystemInfo *iface)
{
    FIXME("%p: stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI sysinfo_GetTrees(IADsADSystemInfo *iface, VARIANT *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static const IADsADSystemInfoVtbl IADsADSystemInfo_vtbl =
{
    sysinfo_QueryInterface,
    sysinfo_AddRef,
    sysinfo_Release,
    sysinfo_GetTypeInfoCount,
    sysinfo_GetTypeInfo,
    sysinfo_GetIDsOfNames,
    sysinfo_Invoke,
    sysinfo_get_UserName,
    sysinfo_get_ComputerName,
    sysinfo_get_SiteName,
    sysinfo_get_DomainShortName,
    sysinfo_get_DomainDNSName,
    sysinfo_get_ForestDNSName,
    sysinfo_get_PDCRoleOwner,
    sysinfo_get_SchemaRoleOwner,
    sysinfo_get_IsNativeMode,
    sysinfo_GetAnyDCName,
    sysinfo_GetDCSiteName,
    sysinfo_RefreshSchemaCache,
    sysinfo_GetTrees
};

static HRESULT ADSystemInfo_create(REFIID riid, void **obj)
{
    AD_sysinfo *sysinfo;
    HRESULT hr;

    sysinfo = heap_alloc(sizeof(*sysinfo));
    if (!sysinfo) return E_OUTOFMEMORY;

    sysinfo->IADsADSystemInfo_iface.lpVtbl = &IADsADSystemInfo_vtbl;
    sysinfo->ref = 1;

    hr = IADsADSystemInfo_QueryInterface(&sysinfo->IADsADSystemInfo_iface, riid, obj);
    IADsADSystemInfo_Release(&sysinfo->IADsADSystemInfo_iface);

    return hr;
}

typedef struct
{
    IADs IADs_iface;
    IADsOpenDSObject IADsOpenDSObject_iface;
    LONG ref;
} LDAP_namespace;

static inline LDAP_namespace *impl_from_IADs(IADs *iface)
{
    return CONTAINING_RECORD(iface, LDAP_namespace, IADs_iface);
}

static HRESULT WINAPI ldapns_QueryInterface(IADs *iface, REFIID riid, void **obj)
{
    LDAP_namespace *ldap = impl_from_IADs(iface);

    TRACE("%p,%s,%p\n", iface, debugstr_guid(riid), obj);

    if (!riid || !obj) return E_INVALIDARG;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IDispatch) ||
        IsEqualGUID(riid, &IID_IADs))
    {
        IADs_AddRef(iface);
        *obj = iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IADsOpenDSObject))
    {
        IADs_AddRef(iface);
        *obj = &ldap->IADsOpenDSObject_iface;
        return S_OK;
    }

    FIXME("interface %s is not implemented\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI ldapns_AddRef(IADs *iface)
{
    LDAP_namespace *ldap = impl_from_IADs(iface);
    return InterlockedIncrement(&ldap->ref);
}

static ULONG WINAPI ldapns_Release(IADs *iface)
{
    LDAP_namespace *ldap = impl_from_IADs(iface);
    LONG ref = InterlockedDecrement(&ldap->ref);

    if (!ref)
    {
        TRACE("destroying %p\n", iface);
        heap_free(ldap);
    }

    return ref;
}

static HRESULT WINAPI ldapns_GetTypeInfoCount(IADs *iface, UINT *count)
{
    FIXME("%p,%p: stub\n", iface, count);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_GetTypeInfo(IADs *iface, UINT index, LCID lcid, ITypeInfo **info)
{
    FIXME("%p,%u,%#x,%p: stub\n", iface, index, lcid, info);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_GetIDsOfNames(IADs *iface, REFIID riid, LPOLESTR *names,
                                           UINT count, LCID lcid, DISPID *dispid)
{
    FIXME("%p,%s,%p,%u,%u,%p: stub\n", iface, debugstr_guid(riid), names, count, lcid, dispid);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_Invoke(IADs *iface, DISPID dispid, REFIID riid, LCID lcid, WORD flags,
                                    DISPPARAMS *params, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr)
{
    FIXME("%p,%d,%s,%04x,%04x,%p,%p,%p,%p: stub\n", iface, dispid, debugstr_guid(riid), lcid, flags,
          params, result, excepinfo, argerr);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_get_Name(IADs *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_get_Class(IADs *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_get_GUID(IADs *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_get_ADsPath(IADs *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_get_Parent(IADs *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_get_Schema(IADs *iface, BSTR *retval)
{
    FIXME("%p,%p: stub\n", iface, retval);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_GetInfo(IADs *iface)
{
    FIXME("%p: stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_SetInfo(IADs *iface)
{
    FIXME("%p: stub\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_Get(IADs *iface, BSTR name, VARIANT *prop)
{
    FIXME("%p,%s,%p: stub\n", iface, debugstr_w(name), prop);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_Put(IADs *iface, BSTR name, VARIANT prop)
{
    FIXME("%p,%s,%s: stub\n", iface, debugstr_w(name), wine_dbgstr_variant(&prop));
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_GetEx(IADs *iface, BSTR name, VARIANT *prop)
{
    FIXME("%p,%s,%p: stub\n", iface, debugstr_w(name), prop);
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_PutEx(IADs *iface, LONG code, BSTR name, VARIANT prop)
{
    FIXME("%p,%d,%s,%s: stub\n", iface, code, debugstr_w(name), wine_dbgstr_variant(&prop));
    return E_NOTIMPL;
}

static HRESULT WINAPI ldapns_GetInfoEx(IADs *iface, VARIANT prop, LONG reserved)
{
    FIXME("%p,%s,%d: stub\n", iface, wine_dbgstr_variant(&prop), reserved);
    return E_NOTIMPL;
}

static const IADsVtbl IADs_vtbl =
{
    ldapns_QueryInterface,
    ldapns_AddRef,
    ldapns_Release,
    ldapns_GetTypeInfoCount,
    ldapns_GetTypeInfo,
    ldapns_GetIDsOfNames,
    ldapns_Invoke,
    ldapns_get_Name,
    ldapns_get_Class,
    ldapns_get_GUID,
    ldapns_get_ADsPath,
    ldapns_get_Parent,
    ldapns_get_Schema,
    ldapns_GetInfo,
    ldapns_SetInfo,
    ldapns_Get,
    ldapns_Put,
    ldapns_GetEx,
    ldapns_PutEx,
    ldapns_GetInfoEx
};

static inline LDAP_namespace *impl_from_IADsOpenDSObject(IADsOpenDSObject *iface)
{
    return CONTAINING_RECORD(iface, LDAP_namespace, IADsOpenDSObject_iface);
}

static HRESULT WINAPI openobj_QueryInterface(IADsOpenDSObject *iface, REFIID riid, void **obj)
{
    TRACE("%p,%s,%p\n", iface, debugstr_guid(riid), obj);

    if (!riid || !obj) return E_INVALIDARG;

    if (IsEqualGUID(riid, &IID_IADsOpenDSObject) ||
        IsEqualGUID(riid, &IID_IDispatch) ||
        IsEqualGUID(riid, &IID_IUnknown))
    {
        IADsOpenDSObject_AddRef(iface);
        *obj = iface;
        return S_OK;
    }

    FIXME("interface %s is not implemented\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI openobj_AddRef(IADsOpenDSObject *iface)
{
    LDAP_namespace *ldap = impl_from_IADsOpenDSObject(iface);
    return InterlockedIncrement(&ldap->ref);
}

static ULONG WINAPI openobj_Release(IADsOpenDSObject *iface)
{
    LDAP_namespace *ldap = impl_from_IADsOpenDSObject(iface);
    LONG ref = InterlockedDecrement(&ldap->ref);

    if (!ref)
    {
        TRACE("destroying %p\n", iface);
        HeapFree(GetProcessHeap(), 0, ldap);
    }

    return ref;
}

static HRESULT WINAPI openobj_GetTypeInfoCount(IADsOpenDSObject *iface, UINT *count)
{
    FIXME("%p,%p: stub\n", iface, count);
    return E_NOTIMPL;
}

static HRESULT WINAPI openobj_GetTypeInfo(IADsOpenDSObject *iface, UINT index, LCID lcid, ITypeInfo **info)
{
    FIXME("%p,%u,%#x,%p: stub\n", iface, index, lcid, info);
    return E_NOTIMPL;
}

static HRESULT WINAPI openobj_GetIDsOfNames(IADsOpenDSObject *iface, REFIID riid, LPOLESTR *names,
                                            UINT count, LCID lcid, DISPID *dispid)
{
    FIXME("%p,%s,%p,%u,%u,%p: stub\n", iface, debugstr_guid(riid), names, count, lcid, dispid);
    return E_NOTIMPL;
}

static HRESULT WINAPI openobj_Invoke(IADsOpenDSObject *iface, DISPID dispid, REFIID riid, LCID lcid, WORD flags,
                                     DISPPARAMS *params, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr)
{
    FIXME("%p,%d,%s,%04x,%04x,%p,%p,%p,%p: stub\n", iface, dispid, debugstr_guid(riid), lcid, flags,
          params, result, excepinfo, argerr);
    return E_NOTIMPL;
}

static HRESULT WINAPI openobj_OpenDSObject(IADsOpenDSObject *iface, BSTR path, BSTR user, BSTR password,
                                           LONG reserved, IDispatch **obj)
{
    FIXME("%p,%s,%s,%s,%d,%p: stub\n", iface, debugstr_w(path), debugstr_w(user), debugstr_w(password),
          reserved, obj);
    return E_NOTIMPL;
}

static const IADsOpenDSObjectVtbl IADsOpenDSObject_vtbl =
{
    openobj_QueryInterface,
    openobj_AddRef,
    openobj_Release,
    openobj_GetTypeInfoCount,
    openobj_GetTypeInfo,
    openobj_GetIDsOfNames,
    openobj_Invoke,
    openobj_OpenDSObject
};

static HRESULT LDAPNamespace_create(REFIID riid, void **obj)
{
    LDAP_namespace *ldap;
    HRESULT hr;

    ldap = heap_alloc(sizeof(*ldap));
    if (!ldap) return E_OUTOFMEMORY;

    ldap->IADs_iface.lpVtbl = &IADs_vtbl;
    ldap->IADsOpenDSObject_iface.lpVtbl = &IADsOpenDSObject_vtbl;
    ldap->ref = 1;

    hr = IADs_QueryInterface(&ldap->IADs_iface, riid, obj);
    IADs_Release(&ldap->IADs_iface);

    return hr;
}

static const struct class_info
{
    const CLSID *clsid;
    HRESULT (*constructor)(REFIID, void **);
} class_info[] =
{
    { &CLSID_ADSystemInfo, ADSystemInfo_create },
    { &CLSID_LDAPNamespace, LDAPNamespace_create },
};

typedef struct
{
    IClassFactory IClassFactory_iface;
    LONG ref;
    const struct class_info *info;
} class_factory;

static inline class_factory *impl_from_IClassFactory(IClassFactory *iface)
{
    return CONTAINING_RECORD(iface, class_factory, IClassFactory_iface);
}

static HRESULT WINAPI factory_QueryInterface(IClassFactory *iface, REFIID riid, LPVOID *obj)
{
    TRACE("%p,%s,%p\n", iface, debugstr_guid(riid), obj);

    if (!riid || !obj) return E_INVALIDARG;

    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IClassFactory))
    {
        IClassFactory_AddRef(iface);
        *obj = iface;
        return S_OK;
    }

    *obj = NULL;
    FIXME("interface %s is not implemented\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef(IClassFactory *iface)
{
    class_factory *factory = impl_from_IClassFactory(iface);
    ULONG ref = InterlockedIncrement(&factory->ref);

    TRACE("(%p) ref %u\n", iface, ref);

    return ref;
}

static ULONG WINAPI factory_Release(IClassFactory *iface)
{
    class_factory *factory = impl_from_IClassFactory(iface);
    ULONG ref = InterlockedDecrement(&factory->ref);

    TRACE("(%p) ref %u\n", iface, ref);

    if (!ref)
        heap_free(factory);

    return ref;
}

static HRESULT WINAPI factory_CreateInstance(IClassFactory *iface, IUnknown *outer, REFIID riid, void **obj)
{
    class_factory *factory = impl_from_IClassFactory(iface);

    TRACE("%p,%s,%p\n", outer, debugstr_guid(riid), obj);

    if (!riid || !obj) return E_INVALIDARG;

    *obj = NULL;
    if (outer) return CLASS_E_NOAGGREGATION;

    return factory->info->constructor(riid, obj);
}

static HRESULT WINAPI factory_LockServer(IClassFactory *iface, BOOL lock)
{
    FIXME("%p,%d: stub\n", iface, lock);
    return S_OK;
}

static const struct IClassFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    factory_CreateInstance,
    factory_LockServer
};

static HRESULT factory_constructor(const struct class_info *info, REFIID riid, void **obj)
{
    class_factory *factory;
    HRESULT hr;

    factory = heap_alloc(sizeof(*factory));
    if (!factory) return E_OUTOFMEMORY;

    factory->IClassFactory_iface.lpVtbl = &factory_vtbl;
    factory->ref = 1;
    factory->info = info;

    hr = IClassFactory_QueryInterface(&factory->IClassFactory_iface, riid, obj);
    IClassFactory_Release(&factory->IClassFactory_iface);

    return hr;
}

HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID iid, LPVOID *obj)
{
    const struct class_info *info = NULL;
    int i;

    TRACE("%s,%s,%p\n", debugstr_guid(clsid), debugstr_guid(iid), obj);

    if (!clsid || !iid || !obj) return E_INVALIDARG;

    *obj = NULL;

    for (i = 0; i < ARRAY_SIZE(class_info); i++)
    {
        if (IsEqualCLSID(class_info[i].clsid, clsid))
        {
            info = &class_info[i];
            break;
        }
    }

    if (info)
        return factory_constructor(info, iid, obj);

    FIXME("class %s/%s is not implemented\n", debugstr_guid(clsid), debugstr_guid(iid));
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllCanUnloadNow(void)
{
    return S_FALSE;
}

HRESULT WINAPI DllRegisterServer(void)
{
    return __wine_register_resources(adsldp_hinst);
}

HRESULT WINAPI DllUnregisterServer(void)
{
    return __wine_unregister_resources(adsldp_hinst);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, void *reserved)
{
    TRACE("%p,%u,%p\n", hinst, reason, reserved);

    switch (reason)
    {
    case DLL_WINE_PREATTACH:
        return FALSE; /* prefer native version */

    case DLL_PROCESS_ATTACH:
        adsldp_hinst = hinst;
        DisableThreadLibraryCalls(hinst);
        break;
    }

    return TRUE;
}
