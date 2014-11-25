/* contextmenu.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "utils.h"
#include "resource.h"
#include <new>  // std::nothrow

#pragma once

#define MENUVERB_DISPLAY     0

// The "terminator" ICIVERBTOIDMAP structure is {NULL, NULL, (UINT)-1,}
typedef struct
{
    LPCWSTR pszCmd;         // verbW
    LPCSTR  pszCmdA;        // verbA
    UINT    idCmd;          // hmenu id
    UINT    idsHelpText;    // id of help text
} ICIVERBTOIDMAP;

const ICIVERBTOIDMAP c_FolderViewImplContextMenuIDMap[] =
{
    { L"display",    "display",   MENUVERB_DISPLAY, 0, },
    { NULL,          NULL,        (UINT)-1,         0, }
};


class BoxFolderContextMenu
	: public IContextMenu
        , public IShellExtInit
        , public IObjectWithSite
{
public:
    BoxFolderContextMenu() : _cRef(1), _punkSite(NULL), _pdtobj(NULL)
    {
        DllAddRef();
    }

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = {
            QITABENT(BoxFolderContextMenu, IContextMenu),
            QITABENT(BoxFolderContextMenu, IShellExtInit),
            QITABENT(BoxFolderContextMenu, IObjectWithSite),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&_cRef);
        if (!cRef)
        {
            delete this;
        }
        return cRef;
    }


    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pRes, LPSTR pszName, UINT cchMax);

    // IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject *pdtobj, HKEY hkeyProgID);

    // IObjectWithSite
    IFACEMETHODIMP SetSite(IUnknown *punkSite);
    IFACEMETHODIMP GetSite(REFIID riid, void **ppvSite);

private:
    long    _cRef;
    IDataObject *_pdtobj;
    IUnknown *_punkSite;

    ~BoxFolderContextMenu()
    {
        // _punkSite should be NULL due to SetSite(NULL).
        if (_pdtobj)
        {
            _pdtobj->Release();
        }
        DllRelease();
    }
};

