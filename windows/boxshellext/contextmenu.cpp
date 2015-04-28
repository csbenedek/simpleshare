#include "contextmenu.h"

HRESULT BoxFolderContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT /* idCmdLast */, UINT /* uFlags */)
{
    WCHAR szMenuItem[80];
	wcscpy(szMenuItem, L"Display");
    InsertMenuW(hmenu, indexMenu++, MF_BYPOSITION, idCmdFirst + MENUVERB_DISPLAY, szMenuItem);
    // other verbs could go here...

    // indicate that we added one verb.
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(1));
}

const ICIVERBTOIDMAP* _CmdIDToMap(UINT_PTR idCmd, BOOL fUnicode, const ICIVERBTOIDMAP* pmap)
{
    const ICIVERBTOIDMAP* pmapRet = NULL;
    if (IS_INTRESOURCE(idCmd))
    {
        UINT idVerb = (UINT)idCmd;
        while (!pmapRet && -1 != pmap->idCmd)
        {
            if (pmap->idCmd == idVerb)
            {
                pmapRet = pmap;
            }
            pmap++;
        }
    }
    else if (fUnicode)
    {
        LPCWSTR pszVerb = (LPCWSTR)idCmd;
        while (!pmapRet && -1 != pmap->idCmd)
        {
            if (pmap->pszCmd && 0 == StrCmpICW(pszVerb, pmap->pszCmd))
            {
                pmapRet = pmap;
            }
            pmap++;
        }
    }
    else
    {
        LPCSTR pszVerbA = (LPCSTR)idCmd;
        while (!pmapRet && -1 != pmap->idCmd)
        {
            if (pmap->pszCmdA && 0 == StrCmpICA(pszVerbA, pmap->pszCmdA))
            {
                pmapRet = pmap;
            }
            pmap++;
        }
    }
    return pmapRet;
}

#define IS_UNICODE_ICI(pici) (((pici)->fMask & CMIC_MASK_UNICODE) == CMIC_MASK_UNICODE)

HRESULT _MapICIVerbToCmdID(LPCMINVOKECOMMANDINFO pici, const ICIVERBTOIDMAP* pmap, UINT* pid)
{
    HRESULT hr = E_FAIL;

    if (!IS_INTRESOURCE(pici->lpVerb))
    {
        UINT_PTR idCmd;
        BOOL fUnicode;

        if (IS_UNICODE_ICI(pici) && ((LPCMINVOKECOMMANDINFOEX)pici)->lpVerbW)
        {
            fUnicode = TRUE;
            idCmd = (UINT_PTR)(((LPCMINVOKECOMMANDINFOEX)pici)->lpVerbW);
        }
        else
        {
            fUnicode = FALSE;
            idCmd = (UINT_PTR)(pici->lpVerb);
        }

        pmap = _CmdIDToMap(idCmd, fUnicode, pmap);
        if (pmap)
        {
            *pid = pmap->idCmd;
            hr = S_OK;
        }
    }
    else
    {
        *pid = LOWORD((UINT_PTR)pici->lpVerb);
        hr = S_OK;
    }
    return hr;
}

HRESULT BoxFolderContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    HRESULT hr = E_INVALIDARG;
    UINT uID;
    // Is this command for us?
    if (SUCCEEDED(_MapICIVerbToCmdID(pici, c_FolderViewImplContextMenuIDMap, &uID)) && uID == MENUVERB_DISPLAY && _pdtobj)
    {
		MessageBoxW(pici->hwnd, L"Invoke", L"Capition", 0);
        //IShellItemArray *psia;
        //hr = SHCreateShellItemArrayFromDataObject(_pdtobj, IID_PPV_ARGS(&psia));
        //if (SUCCEEDED(hr))
        //{
            //hr = DisplayItem(psia, pici->hwnd);
            //psia->Release();
        //}
    }
    return hr;
}

HRESULT BoxFolderContextMenu::GetCommandString(UINT_PTR /* idCmd */, UINT /* uType */, UINT * /* pRes */, LPSTR /* pszName */, UINT /* cchMax */)
{
    return E_NOTIMPL;
}

HRESULT BoxFolderContextMenu::Initialize(PCIDLIST_ABSOLUTE /* pidlFolder */, IDataObject *pdtobj, HKEY /* hkeyProgID */)
{
    if (_pdtobj)
    {
        _pdtobj->Release();
        _pdtobj = NULL;
    }

    _pdtobj = pdtobj;
    if (pdtobj)
    {
        pdtobj->AddRef();
    }
    return S_OK;
}

HRESULT BoxFolderContextMenu::SetSite(IUnknown *punkSite)
{
    if (_punkSite)
    {
        _punkSite->Release();
        _punkSite = NULL;
    }

    _punkSite = punkSite;
    if (punkSite)
    {
        punkSite->AddRef();
    }
    return S_OK;
}

HRESULT BoxFolderContextMenu::GetSite(REFIID riid, void **ppvSite)
{
    return _punkSite ? _punkSite->QueryInterface(riid, ppvSite) : E_FAIL;
}
