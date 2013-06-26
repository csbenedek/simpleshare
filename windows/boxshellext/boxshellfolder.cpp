#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <strsafe.h>
#include <shellapi.h>
#include "boxshellfolder.h"
#include "enumidlist.h"
#include "resource.h"
#include "Utils.h"
#include "itemdata.h"
#include "guid.h"
#include "dataobject.h"

HRESULT CALLBACK ShellFoldCallBack( IShellFolder *psf, HWND hwnd, IDataObject *pdtobj, UINT uMsg, WPARAM wParam, LPARAM lParam );

BoxShellFolder::BoxShellFolder()
    : m_ref(1)
	, m_pidlRoot(NULL)
{
	DllAddRef();
}

BoxShellFolder::~BoxShellFolder()
{
	DllRelease();
}

HRESULT BoxShellFolder::QueryInterface(REFIID riid, __out void **ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(BoxShellFolder, IShellFolder),
        //QITABENT(BoxShellFolder, IShellFolder2),
        QITABENT(BoxShellFolder, IPersist),
        QITABENT(BoxShellFolder, IPersistFolder),
        QITABENT(BoxShellFolder, IPersistFolder2),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

ULONG BoxShellFolder::AddRef()
{
	return InterlockedIncrement(&m_ref);
}

ULONG BoxShellFolder::Release()
{
	ULONG ref = InterlockedDecrement(&m_ref);
	if (!ref)
	{
		delete this;
	}
	return ref;
}


//  Translates a display name into an item identifier list.
HRESULT BoxShellFolder::ParseDisplayName(HWND hwnd, IBindCtx *pbc, PWSTR pszName,
                                                ULONG *pchEaten, PIDLIST_RELATIVE *ppidl, ULONG *pdwAttributes)
{
	UNREFERENCED_PARAMETER(pdwAttributes);
	UNREFERENCED_PARAMETER(ppidl);
	UNREFERENCED_PARAMETER(pchEaten);
	UNREFERENCED_PARAMETER(pbc);
	UNREFERENCED_PARAMETER(hwnd);

//	MessageBoxW(NULL, L"ParseDisplayName", NULL, 0);

    return E_NOTIMPL;
}

//  Allows a client to determine the contents of a folder by
//  creating an item identifier enumeration object and returning
//  its IEnumIDList interface. The methods supported by that
//  interface can then be used to enumerate the folder's contents.
HRESULT BoxShellFolder::EnumObjects(HWND /* hwnd */, DWORD grfFlags, IEnumIDList **ppenumIDList)
{
	//	MessageBoxW(NULL, L"EnumObjects", NULL, 0);

	HRESULT hr;   
           
	BoxEnumIDList *penum = new BoxEnumIDList(grfFlags, 1, this);      
    hr = penum ? S_OK : E_OUTOFMEMORY;   
    if (SUCCEEDED(hr))   
    {   
		hr = penum->Initialize();   
        if (SUCCEEDED(hr))   
        {   
			hr = penum->QueryInterface(IID_PPV_ARGS(ppenumIDList));   
		}         
        penum->Release();   
    }   
           
    return hr;
}

//  Factory for handlers for the specified item.
HRESULT BoxShellFolder::BindToObject(PCUIDLIST_RELATIVE pidl, IBindCtx *pbc, REFIID riid, void **ppv)
{
	//MessageBoxW(0, L"BindToObject", NULL, 0);

	// If the passed pidl is not ours, fail.
	if (!ItemData::IsOwn(pidl))
	{
		return E_INVALIDARG;
	}

	IShellFolder* DesktopPtr = NULL;

	if (!m_pidlMgr.IsSingle(pidl))
	{
		HRESULT hr;
		hr = SHGetDesktopFolder(&DesktopPtr);
		if (FAILED(hr))
			return hr;

		LPITEMIDLIST pidlLocal;
		hr = DesktopPtr->ParseDisplayName(NULL, pbc, ItemData::GetPath(pidl), NULL, &pidlLocal, NULL);
		if (FAILED(hr))
			return hr;

		// Bind to the root folder of the favorite folder
		IShellFolder* RootFolderPtr = NULL;
		hr = DesktopPtr->BindToObject(pidlLocal, NULL, IID_IShellFolder, (void**)&RootFolderPtr);
		ILFree(pidlLocal);
		if (FAILED(hr))
			return hr;

		// And now bind to the sub-item of it
		hr = RootFolderPtr->BindToObject(m_pidlMgr.GetNextItem(pidl), pbc, riid, ppv);

		if (DesktopPtr)
		{
			DesktopPtr->Release();
		}

		if (RootFolderPtr)
		{
			RootFolderPtr->Release();
		}
	}

	// Okay, browsing into a favorite item will redirect to its real path.
	HRESULT hr;
	hr = SHGetDesktopFolder(&DesktopPtr);
	if (FAILED(hr))
		return hr;

	LPITEMIDLIST pidlLocal;
	hr = DesktopPtr->ParseDisplayName(NULL, pbc, ItemData::GetPath(pidl), NULL, &pidlLocal, NULL);
	if (FAILED(hr))
		return hr;

	hr = DesktopPtr->BindToObject(pidlLocal, pbc, riid, ppv);

	ILFree(pidlLocal);

	DesktopPtr->Release();

	return hr;

	// could also use this one? ILCreateFromPathW 
}

HRESULT BoxShellFolder::BindToStorage(PCUIDLIST_RELATIVE pidl,
                                             IBindCtx *pbc, REFIID riid, void **ppv)
{
//	MessageBoxW(NULL, L"BindToStorage", NULL, 0);

	return BindToObject(pidl, pbc, riid, ppv);
}

//  Called to determine the equivalence and/or sort order of two idlists.
HRESULT BoxShellFolder::CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2)
{
	//MessageBoxW(NULL, L"CompareIDs", NULL, 0);

	// Here, we are just comparing the strings in the last PIDLs.   
        HRESULT hr = S_OK;   
           
		hr = ResultFromShort(1);   
           
        return hr;   
}

//  Called by the Shell to create the View Object and return it.
HRESULT BoxShellFolder::CreateViewObject(HWND hwnd, REFIID riid, void **ppv)
{
	//MessageBoxW(NULL, L"CreateViewObject", NULL, 0);

    *ppv = NULL;

    HRESULT hr = E_NOINTERFACE;

    if (riid == IID_IShellView)
    {
        SFV_CREATE csfv = { sizeof(csfv), 0 };
        hr = QueryInterface(IID_PPV_ARGS(&csfv.pshf));
        if (SUCCEEDED(hr))
        {
            // Add our callback to the SFV_CREATE.  This is optional.  We
            // are adding it so we can enable searching within our
            // namespace.
            //hr = CFolderViewCB_CreateInstance(IID_PPV_ARGS(&csfv.psfvcb));
            //if (SUCCEEDED(hr))
            {
                hr = SHCreateShellFolderView(&csfv, (IShellView**)ppv);
                //csfv.psfvcb->Release();
            }
            csfv.pshf->Release();
        }
    }
    else if (riid == IID_IContextMenu)
    {
        // This is the background context menu for the folder itself, not the context menu on items within it.
        //DEFCONTEXTMENU dcm = { hwnd, NULL, m_pidlRoot, static_cast<IShellFolder*>(this), 0, NULL, NULL, 0, NULL };
        //hr = SHCreateDefaultContextMenu(&dcm, riid, ppv);
		//hr = CDefFolderMenu_Create2(m_pidlRoot, hwnd, 0, NULL, static_cast<IShellFolder*>(this), &ShellFoldCallBack, 0, NULL, reinterpret_cast<IContextMenu**>(ppv));

    }

	return hr;
}

//  Retrieves the attributes of one or more file objects or subfolders.
HRESULT BoxShellFolder::GetAttributesOf(UINT cidl, PCUITEMID_CHILD_ARRAY apidl, ULONG *pdwAttribs)
{
	//MessageBoxW(NULL, L"GetAttributesOf", NULL, 0);

	HRESULT hr = S_OK;   


	// We limit the tree, by indicating that the favorites folder does not contain sub-folders

	if ((cidl == 0) || (apidl[0] == 0))
	{
	    //*pdwAttribs &= SFGAO_HASSUBFOLDER |SFGAO_FOLDER /*| SFGAO_FILESYSTEM |SFGAO_FILESYSANCESTOR | SFGAO_BROWSABLE*/;
	}
	else 
	{
		USHORT type = ItemData::GetItemType(apidl[0]);

		*pdwAttribs &= SFGAO_BROWSABLE;

		if (type == ItemData::TypeFolder)
		{
			*pdwAttribs &= SFGAO_FOLDER | SFGAO_HASSUBFOLDER;// | SFGAO_FILESYSTEM |SFGAO_FILESYSANCESTOR | SFGAO_BROWSABLE*/;
		}
	}
         
	return hr;   
}

//  Retrieves an OLE interface that can be used to carry out
//  actions on the specified file objects or folders.
STDMETHODIMP BoxShellFolder::GetUIObjectOf(HWND hwnd, UINT uCount, LPCITEMIDLIST* pPidl, REFIID riid, LPUINT puReserved, void** ppv)
{
	HRESULT hr;

	if (ppv == NULL)
		return E_POINTER;

	*ppv = NULL;
	
    if (riid == IID_IDataObject)
    {
		// Only one item at a time
		if (uCount != 1)
			return E_INVALIDARG;

		// Is this really one of our item?
		if (!ItemData::IsOwn(*pPidl))
			return E_INVALIDARG;

		// Create a COM object that exposes IDataObject
		CDataObject* pDataObject = new CDataObject();
//		if (FAILED(hr))
//			return hr;

		// AddRef it while we are working with it, this prevent from an early destruction.
		pDataObject->AddRef();

		// Tight its lifetime with this object (the IShellFolder object)
		pDataObject->Init(static_cast<IShellFolder*>(this));

		// Okay, embed the pidl in the data
		pDataObject->SetPidl(m_pidlRoot, *pPidl);

		// Return the requested interface to the caller
        hr = pDataObject->QueryInterface(riid, ppv);

		// We do no more need our ref (note that the object will not die because the QueryInterface above, AddRef'd it)
		pDataObject->Release();
		pDataObject->Release();
		return hr;
	}
	
	if (uCount == 0)
		return E_INVALIDARG;

	// All other requests are delegated to the target path's IShellFolder

	// because multiple items can point to different storages, we can't (easily) handle groups of items.
	if (uCount > 1)
		return E_NOINTERFACE;

	IShellFolder* DesktopPtr = NULL;
	IShellFolder* TargetParentShellFolderPtr = NULL;

	hr = SHGetDesktopFolder(&DesktopPtr);
	if (FAILED(hr))
	{
		return hr;
	}


	LPITEMIDLIST pidlLocal = NULL;
	WCHAR* path = ItemData::GetPath(*pPidl);
	hr = DesktopPtr->ParseDisplayName(NULL, NULL, path, NULL, &pidlLocal, NULL);

	if (FAILED(hr))
	{
		DesktopPtr->Release();
		return hr;
	}

	
	LPITEMIDLIST pidlRelative;

	//------------------------------
	// this block emulate the following line (not available to shell version 4.7x)
	//		hr = SHBindToParent(pidlLocal, IID_IShellFolder, (void**)&pTargetParentShellFolder, &pidlRelative);
	LPITEMIDLIST pidlTmp = ILFindLastID(pidlLocal);
	pidlRelative = ILClone(pidlTmp);
	ILRemoveLastID(pidlLocal);
	hr = DesktopPtr->BindToObject(pidlLocal, NULL, IID_IShellFolder, (void**)&TargetParentShellFolderPtr);
	ILFree(pidlLocal);
	if (FAILED(hr))
	{
		ILFree(pidlRelative);
		return hr;
	}
	//------------------------------

	hr = TargetParentShellFolderPtr->GetUIObjectOf(hwnd, 1, (LPCITEMIDLIST*)&pidlRelative, riid, puReserved, ppv);


	ILFree(pidlRelative);

	TargetParentShellFolderPtr->Release();

	return hr;
}

//  Retrieves the display name for the specified file object or subfolder.
HRESULT BoxShellFolder::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF shgdnFlags, STRRET *pName)
{
    if ((pidl == NULL) || (pName == NULL))
	{
        return E_POINTER;
	}

    // Return name of Root

    if (pidl->mkid.cb == 0)
    {
        switch (shgdnFlags)
        {
        case SHGDN_NORMAL | SHGDN_FORPARSING :
			WCHAR TempPath[MAX_PATH];
            if (GetTempPathW(MAX_PATH, TempPath) == 0)
                return E_FAIL;

			return StringToStrRet(TempPath, pName);   

        }
        // We dont' handle other combinations of flags

        return E_FAIL;
    }

	HRESULT hr = StringToStrRet(ItemData::GetName(pidl), pName);   
           
	return hr;   
}

//  Sets the display name of a file object or subfolder, changing
//  the item identifier in the process.
HRESULT BoxShellFolder::SetNameOf(HWND /* hwnd */, PCUITEMID_CHILD /* pidl */,
                                         PCWSTR /* pszName */,  DWORD /* uFlags */, PITEMID_CHILD *ppidlOut)
{
	//MessageBoxW(NULL, L"SetNameOf", NULL, 0);

	HRESULT hr = E_NOTIMPL;
    *ppidlOut = NULL;
    return hr;
}

HRESULT BoxShellFolder::_ValidatePidl(PCUIDLIST_RELATIVE pidl)
{
	UNREFERENCED_PARAMETER(pidl);

    return S_OK;
}

WCHAR* BoxShellFolder::GetPath() const
{
	return ItemData::GetPath(m_pidlRoot);
}

//  IPersistFolder method
HRESULT BoxShellFolder::Initialize(PCIDLIST_ABSOLUTE pidl)
{
	m_pidlRoot = m_pidlMgr.Copy(pidl);
	return m_pidlRoot ? S_OK : E_FAIL;
}

STDMETHODIMP BoxShellFolder::GetCurFolder(LPITEMIDLIST *ppidl)
{
	if (m_pidlRoot == NULL)
	{
		return S_FALSE;
	}
	if (ppidl == NULL)
	{
		return E_POINTER;
	}

	*ppidl = m_pidlMgr.Copy(m_pidlRoot);

	return *ppidl ? S_OK : E_FAIL;
}

STDMETHODIMP BoxShellFolder::GetClassID(CLSID* pClsid)
{
	if ( NULL == pClsid )
		return E_POINTER;

	// Return our GUID to the shell.
	*pClsid = CLSID_BoxShellFolder;

	return S_OK;
}

HRESULT CALLBACK ShellFoldCallBack( IShellFolder *psf, HWND hwnd, IDataObject *pdtobj, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
switch (uMsg)
{
case DFM_MERGECONTEXTMENU:
	// Must return S_OK even if we do nothing else or Vista and later
	// won't add standard verbs
	return S_OK;
case DFM_INVOKECOMMAND: // Required to invoke default action
case DFM_INVOKECOMMANDEX:
case DFM_GETDEFSTATICID: // Required for Windows 7 to pick a default
	return S_FALSE; 
default:
	return E_NOTIMPL; // Required for Windows 7 to show any menu at all
}
}