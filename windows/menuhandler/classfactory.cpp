#include <shlobj.h>
#include "menuhandler.h"
#include "contextmenu.h"
#include "classFactory.h"
#include "guid.h"

ContextMenuClassFactory::ContextMenuClassFactory(REFCLSID rclsid)
    : m_ref(1)
    , m_rclsid(rclsid)
{
    DllAddRef();
}

ContextMenuClassFactory::~ContextMenuClassFactory()
{
    DllRelease();
}

HRESULT ContextMenuClassFactory::QueryInterface(__in REFIID riid, __deref_out void **ppv)
{
     HRESULT hr = S_OK;

	if (IsEqualIID(IID_IUnknown, riid)
		|| IsEqualIID(IID_IClassFactory, riid))
    {
        *ppv = static_cast<IClassFactory*>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
        *ppv = NULL;
    }

    return hr;
}

DWORD ContextMenuClassFactory::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

DWORD ContextMenuClassFactory::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);
    if (0 == ref)
    {
        delete this;
    }
    return ref;
}

HRESULT ContextMenuClassFactory::CreateInstance(__in_opt IUnknown* punkOuter, 
                                                __in REFIID riid, 
                                                __deref_out void **ppv)
{
    *ppv = NULL;

    HRESULT hr = !punkOuter ? S_OK : CLASS_E_NOAGGREGATION;
    if (SUCCEEDED(hr))
    {
        if (CLSID_SimpleShareContextMenu == m_rclsid)
        {
            ContextMenu* contextMenu = new ContextMenu();
            hr = contextMenu ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {          
                hr = contextMenu->QueryInterface(riid, ppv);
                contextMenu->Release();
            }
        }
        else
        {
            hr = E_NOTIMPL;
        }
    }
    return hr;
}

HRESULT ContextMenuClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        DllAddRef();
    }
    else
    {
        DllRelease();
    }
    return S_OK;
}
