#include <shlobj.h>
#include "boxshellfolder.h"
#include "contextmenu.h"
#include "classFactory.h"
#include "guid.h"
#include "Utils.h"

BoxShellClassFactory::BoxShellClassFactory(REFCLSID rclsid)
    : m_ref(1)
    , m_rclsid(rclsid)
{
    DllAddRef();
}

BoxShellClassFactory::~BoxShellClassFactory()
{
    DllRelease();
}

HRESULT BoxShellClassFactory::QueryInterface(__in REFIID riid, __deref_out void **ppv)
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

DWORD BoxShellClassFactory::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

DWORD BoxShellClassFactory::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);
    if (0 == ref)
    {
        delete this;
    }
    return ref;
}

HRESULT BoxShellClassFactory::CreateInstance(__in_opt IUnknown* punkOuter, 
                                                __in REFIID riid, 
                                                __deref_out void **ppv)
{
    *ppv = NULL;

    HRESULT hr = !punkOuter ? S_OK : CLASS_E_NOAGGREGATION;
    if (SUCCEEDED(hr))
    {
	if (m_rclsid == CLSID_BoxShellFolder)
        {
            BoxShellFolder* folder = new BoxShellFolder();
            hr = folder ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {          
                hr = folder->QueryInterface(riid, ppv);
                folder->Release();
            }
        }
        else if (m_rclsid == CLSID_BoxShellContextMenu)
        {
            BoxFolderContextMenu* menu = new BoxFolderContextMenu();
            hr = menu ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {          
                hr = menu->QueryInterface(riid, ppv);
                menu->Release();
            }
        }
        else
        {
            hr = E_NOTIMPL;
        }
    }
    return hr;
}

HRESULT BoxShellClassFactory::LockServer(BOOL fLock)
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
