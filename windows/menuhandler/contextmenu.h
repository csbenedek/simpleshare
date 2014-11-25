/* contextmenu.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <deque>

#define IDM_SHAREWITHBOX 0
#define NAME_W L"SimpleShareExt"
#define MENU_A  "&Upload with SimpleShare"
#define VERB_A  "shareWithBox"
#define VERB_W L"shareWithBox"
#define HELP_A  "Share files or folders with Box SimpleShare"
#define HELP_W L"Share files or folders with Box SimpleShare"

class ContextMenu
    : public IShellExtInit
    , public IContextMenu
{
public:
    ContextMenu();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, __out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject *pdtobj, HKEY hkeyProgID);
    
    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, __reserved UINT *pRes, __out_awcount(!(uType & GCS_UNICODE), cchMax) LPSTR pszName, UINT cchMax);

protected:
	HBITMAP IconToBitmap(int ResourceID);
	HRESULT DisplayItems(IShellItemArray *psia, HWND hwndParent);
	HRESULT ShareWithBox(HWND hwndParent);
	HRESULT ShareWithBox(WCHAR *fileName, HWND hwndParent);
	HRESULT ShareWithBoxItems(IDataObject* dataObject, HWND hwndParent);

private:
    long            m_ref;
	HBITMAP			m_hbmp;
    IDataObject*	m_pdtobj;
	WCHAR*			m_appPath;
	std::deque<WCHAR*> m_paths;

    ~ContextMenu();
};
