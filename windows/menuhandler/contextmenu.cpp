#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <strsafe.h>
#include <shellapi.h>
#include "menuhandler.h"
#include "contextmenu.h"
#include "resource.h"

ContextMenu::ContextMenu()
    : m_ref(1)
	, m_hbmp(NULL)
	, m_pdtobj(NULL)
	, m_appPath(NULL)
{
	DllAddRef();

	//m_hbmp = IconToBitmap(IDI_ICON1);
	m_hbmp = (HBITMAP)LoadImage(g_hInst,
								MAKEINTRESOURCE(IDB_SIMPLESHARE),
								IMAGE_BITMAP,
								GetSystemMetrics(SM_CXMENUCHECK), GetSystemMetrics(SM_CYMENUCHECK),
								LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);

	// Get the path and module name.
	WCHAR buff[MAX_PATH];
	GetModuleFileNameW(g_hInst, buff, MAX_PATH);
	WCHAR* str = wcsrchr(buff, L'\\');
	if (str != NULL)
	{
		*str = 0;
		m_appPath = new WCHAR[MAX_PATH];
		if (m_appPath)
		{
			wcscpy(m_appPath, buff);
			wcscat(m_appPath, L"\\SimpleShare.exe");
		}
	}
}

ContextMenu::~ContextMenu()
{
	delete m_appPath;

	DeleteObject(m_hbmp);

    if (m_pdtobj)
    {
        m_pdtobj->Release();
    }

    DllRelease();
}

HRESULT ContextMenu::QueryInterface(REFIID riid, __out void **ppv)
{
    HRESULT hr = S_OK;

	if (IsEqualIID(IID_IUnknown, riid) || IsEqualIID(IID_IContextMenu, riid))
    {
        *ppv = static_cast<IContextMenu*>(this);
        AddRef();
    }
	else if (IsEqualIID(IID_IShellExtInit, riid))
    {
        *ppv = static_cast<IShellExtInit*>(this);
        AddRef();
	}
    else
    {
        hr = E_NOINTERFACE;
        *ppv = NULL;
    }

    return hr;
}

ULONG ContextMenu::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG ContextMenu::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);
    if (!ref)
    {
        delete this;
    }
    return ref;
}

HRESULT ContextMenu::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject *pdtobj, HKEY hkeyProgID)
{
    if (m_pdtobj)
    {
        m_pdtobj->Release();
        m_pdtobj = NULL;
    }

    m_pdtobj = pdtobj;
    if (m_pdtobj)
    {
        m_pdtobj->AddRef();
    }

	return S_OK;
}

STDMETHODIMP ContextMenu::QueryContextMenu(HMENU hMenu,
                                             UINT indexMenu,
                                             UINT idCmdFirst,
                                             UINT idCmdLast,
                                             UINT uFlags)
{
    //HRESULT hr;
	
    if (!(CMF_DEFAULTONLY & uFlags))
    {
        InsertMenu(hMenu, 
                   indexMenu, 
                   MF_STRING | MF_BYPOSITION, 
                   idCmdFirst + IDM_SHAREWITHBOX, 
                   MENU_A);

	    if (m_hbmp)
		{
		    SetMenuItemBitmaps( hMenu, 
                   indexMenu, 
                   MF_STRING | MF_BYPOSITION,
				   m_hbmp, m_hbmp);
		}

        // TODO: Add error handling to verify HRESULT return values.
		
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_SHAREWITHBOX + 1));
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
}


IFACEMETHODIMP ContextMenu::GetCommandString(UINT_PTR idCommand,
                                             UINT uFlags,
                                             LPUINT lpReserved,
                                             LPSTR pszName,
                                             UINT uMaxNameLen)
{
    HRESULT  hr = E_INVALIDARG;

    if (idCommand != IDM_SHAREWITHBOX)
    {
        return hr;
    }

    switch(uFlags)
    {
        case GCS_HELPTEXTA:
            hr = StringCchCopyNA(pszName, 
                                 uMaxNameLen, 
                                 HELP_A, 
                                 uMaxNameLen);
            break; 

        case GCS_HELPTEXTW: 
            hr = StringCchCopyNW((LPWSTR)pszName, 
                                 uMaxNameLen, 
                                 HELP_W, 
                                 uMaxNameLen);
            break; 

        case GCS_VERBA:
            hr = StringCchCopyNA(pszName, 
                                  uMaxNameLen, 
                                  VERB_A, 
                                  uMaxNameLen);
            break; 

        case GCS_VERBW:
            hr = StringCchCopyNW((LPWSTR)pszName, 
                                 uMaxNameLen, 
                                 VERB_W, 
                                 uMaxNameLen);
            break; 

        default:
            hr = S_OK;
            break; 
    }
    return hr;
}

STDMETHODIMP ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    BOOL fEx = FALSE;
    BOOL fUnicode = FALSE;

    if(lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
    {
        fEx = TRUE;
        if((lpcmi->fMask & CMIC_MASK_UNICODE))
        {
            fUnicode = TRUE;
        }
    }

    if( !fUnicode && HIWORD(lpcmi->lpVerb))
    {
        if(StrCmpIA(lpcmi->lpVerb, VERB_A))
        {
            return E_FAIL;
        }
    }
    else if( fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX *) lpcmi)->lpVerbW))
    {
        if(StrCmpIW(((CMINVOKECOMMANDINFOEX *)lpcmi)->lpVerbW, VERB_W))
        {
            return E_FAIL;
        }
    }
    else if(LOWORD(lpcmi->lpVerb) != IDM_SHAREWITHBOX)
    {
        return E_FAIL;
    }
    else
    {
		if (m_pdtobj)
		{
			return ShareWithBoxItems(m_pdtobj, lpcmi->hwnd);
		}
    }

    return S_OK;
}

HBITMAP ContextMenu::IconToBitmap(int ResourceID)
{
    RECT rect;

    rect.right  = GetSystemMetrics(SM_CXMENUCHECK);
    rect.bottom = GetSystemMetrics(SM_CYMENUCHECK);

    rect.left = rect.top  = 0;

	HICON hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(ResourceID), IMAGE_BITMAP, rect.right, rect.bottom, LR_DEFAULTCOLOR);
    if (!hIcon)
    {
        return NULL;
    }

    HWND desktop = GetDesktopWindow();
    if (desktop == NULL)
    {
        DestroyIcon(hIcon);
        return NULL;
    }

    HDC screen_dev = GetDC(desktop);
    if (screen_dev == NULL)
    {
        DestroyIcon(hIcon);
        return NULL;
    }

    // Create a compatible DC
    HDC dst_hdc = CreateCompatibleDC(screen_dev);
    if (dst_hdc == NULL)
    {
        DestroyIcon(hIcon);
        ReleaseDC(desktop, screen_dev);
        return NULL;
    }

    // Create a new bitmap of icon size
    HBITMAP bmp = CreateCompatibleBitmap(screen_dev, rect.right, rect.bottom);
    if (bmp == NULL)
    {
        DestroyIcon(hIcon);
        DeleteDC(dst_hdc);
        ReleaseDC(desktop, screen_dev);
        return NULL;
    }

    // Select it into the compatible DC
    HBITMAP old_dst_bmp = (HBITMAP)SelectObject(dst_hdc, bmp);
    if (old_dst_bmp == NULL)
    {
        DestroyIcon(hIcon);
        return NULL;
    }

    // Fill the background of the compatible DC with the given colour
    SetBkColor(dst_hdc, RGB(255, 255, 255));
    ExtTextOut(dst_hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

    // Draw the icon into the compatible DC
    DrawIconEx(dst_hdc, 0, 0, hIcon, rect.right, rect.bottom, 0, NULL, DI_NORMAL);

    // Restore settings
    SelectObject(dst_hdc, old_dst_bmp);
    DeleteDC(dst_hdc);
    ReleaseDC(desktop, screen_dev);
    DestroyIcon(hIcon);
    return bmp;
}

HRESULT ContextMenu::DisplayItems(IShellItemArray *psia, HWND hwndParent)
{
	DWORD count;
	psia->GetCount(&count);

	for(DWORD i = 0; i < count; ++i)
	{
	    IShellItem *psi;
		HRESULT hr = psia->GetItemAt(i, &psi);
		if (SUCCEEDED(hr))
		{
			PWSTR pszDisplayName;
			hr = psi->GetDisplayName(SIGDN_NORMALDISPLAY, &pszDisplayName);
			if (SUCCEEDED(hr))
			{
				MessageBoxW(hwndParent, pszDisplayName, pszDisplayName, MB_OK);
				CoTaskMemFree(pszDisplayName);
			}
			psi->Release();
		}
		else
		{
			return hr;
		}
    }  

    return S_OK;
}

HRESULT ContextMenu::ShareWithBox(WCHAR *fileName, HWND hwndParent)
{
	if (m_appPath == NULL || fileName == NULL)
	{
		return E_FAIL;
	}

	size_t size = wcslen(fileName)+3;
	WCHAR* quotedFileName = new WCHAR[size];
	if (quotedFileName == NULL)
	{
		return E_FAIL;
	}
	wcscpy(quotedFileName, L"\"");
	wcscat(quotedFileName, fileName);
	wcscat(quotedFileName, L"\"");

#ifdef _DEBUG
	MessageBoxW(hwndParent, quotedFileName, m_appPath, MB_OK);
#endif
	ShellExecuteW(hwndParent, NULL, m_appPath, quotedFileName, NULL, SW_SHOWNORMAL);

	delete quotedFileName;

	return  S_OK;
}

HRESULT ContextMenu::ShareWithBox(HWND hwndParent)
{
	if (m_appPath == NULL || m_paths.size() == 0)
	{
		return E_FAIL;
	}

	// calculate total size:
	WCHAR* buff = NULL;
	size_t size = 0;

	if (m_paths.size() > 1)
	{
		for(size_t i = 0; i < m_paths.size(); ++i)
		{
			WCHAR* path = m_paths.at(i);
			size += wcslen(path);
			size += 4;
		}
		size += 1;
		buff = new WCHAR[size];
		*buff = 0;
	}

	if (buff == NULL)
	{
		// can't allocate buffer for all paths. trying to call separatelly:
		for(size_t i = 0; i < m_paths.size(); ++i)
		{
			WCHAR* path = m_paths.at(i);
			ShareWithBox(path, hwndParent);
		}
	}
	else
	{

		// ok buff allocated. coping all paths into it:

		for(size_t i = 0; i < m_paths.size(); ++i)
		{
			WCHAR* path = m_paths.at(i);
			wcscat(buff, L" \"");
			wcscat(buff, path);
			wcscat(buff, L"\"");
#ifdef _DEBUG
			MessageBoxW(hwndParent, buff, path, MB_OK);
#endif
		}
#ifdef _DEBUG
//		MessageBoxW(hwndParent, buff, m_appPath, MB_OK);
#endif
		ShellExecuteW(hwndParent, NULL, m_appPath, buff, NULL, SW_SHOWNORMAL);

		delete buff;
	}

	for(size_t i = 0; i < m_paths.size(); ++i)
	{
		WCHAR* path = m_paths.at(i);
		delete path;
	}
	m_paths.clear();

	return S_OK;
}

HRESULT ContextMenu::ShareWithBoxItems(IDataObject *dataObject, HWND hwndParent)
{
	HRESULT hr = S_OK;

    if (dataObject == NULL) 
    {
		return E_FAIL;
	}

	m_paths.clear();

#if (WINVER >= 0x0600)
	IShellItemArray *psia;
	hr = SHCreateShellItemArrayFromDataObject(dataObject, IID_PPV_ARGS(&psia));
	if (SUCCEEDED(hr))
	{
		//hr = DisplayItems(psia, hwndParent);
	
		DWORD count;
		psia->GetCount(&count);

		for(DWORD i = 0; i < count; ++i)
		{
			IShellItem *psi;
			hr = psia->GetItemAt(i, &psi);
			if (SUCCEEDED(hr))
			{
				PWSTR pszDisplayName;
				hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszDisplayName);
				if (SUCCEEDED(hr))
				{
					m_paths.push_back(wcsdup(pszDisplayName));
					CoTaskMemFree(pszDisplayName);
				}
				psi->Release();
			}
			else
			{
				break;
			}
		}  
		psia->Release();      
	}
#else
	STGMEDIUM   medium;
    FORMATETC   fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

	if(SUCCEEDED(dataObject->GetData(&fe, &medium)))
	{
		// Get the count of files dropped.
		UINT uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

		// Get the first file name from the CF_HDROP.
		for(UINT i = 0; i < uCount; ++i)
		{
			WCHAR fileName[MAX_PATH];
			if(DragQueryFileW((HDROP)medium.hGlobal, i, fileName, MAX_PATH))
			{
					m_paths.push_back(wcsdup(fileName));
			}
        }

		ReleaseStgMedium(&medium);
    }
#endif
	ShareWithBox(hwndParent);

    return hr;
}
