#include <shlobj.h>   
#include <shlwapi.h>   
#include "Utils.h"   
#include "boxshellfolder.h"   
#include "EnumIDList.h"   
#include <stdio.h>   
#include <strsafe.h>   
       
BoxEnumIDList::BoxEnumIDList(DWORD flags, int nLevel, BoxShellFolder *folder)
	: m_cRef(1)
	, m_flags(flags)
	, m_nItem(0)
	, m_folder(folder)   
{   
    m_folder->AddRef();   
}   
       
BoxEnumIDList::~BoxEnumIDList()   
{   
    m_folder->Release();   
}   
       
HRESULT BoxEnumIDList::QueryInterface(__in REFIID riid, __deref_out void **ppv)   
{   
    static const QITAB qit[] = 
	{   
        QITABENT (BoxEnumIDList, IEnumIDList),   
        { 0 },   
    };   
    return QISearch(this, qit, riid, ppv);   
}   
       
DWORD BoxEnumIDList::AddRef()   
{   
    return InterlockedIncrement(&m_cRef);   
}   
       
DWORD BoxEnumIDList::Release()   
{   
    ULONG cRef = InterlockedDecrement(&m_cRef);   
    if (0 == cRef)   
    {   
        this;
    }   
       
    return cRef;   
}   
       
// This initializes the enumerator.  In this case we set up a default array of items, this represents our   
// data source.  In a real-world implementation the array would be replaced and would be backed by some   
// other data source that you traverse and convert into IShellFolder item IDs.   
HRESULT BoxEnumIDList::Initialize()   
{   
	WIN32_FIND_DATAW ffd;

	LARGE_INTEGER filesize;
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;

	WCHAR* path = m_folder->GetPath();

	WCHAR szDir[MAX_PATH];

	SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDir);

	//StringCchCopyW(szDir, MAX_PATH, L"C:\\Documents and Settings\\user\\Local Settings\\Application Data\\box.net");
	StringCchCatW(szDir, MAX_PATH, L"\\box.net\\*");

	if (path) CoTaskMemFree(path);

   // Find the first file in the directory.

	hFind = FindFirstFileW(szDir, &ffd);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			ItemData itemData;
			if (ffd.cFileName && ffd.cFileName[0] != '.')
			{
				WCHAR path[MAX_PATH];

				wcscpy(path, szDir);
				path[wcslen(path)-1] = 0; // delete last char.

				wcscat(path, ffd.cFileName);
				
				itemData.SetPath(path);
				itemData.SetName(ffd.cFileName);
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					itemData.SetItemType(ItemData::TypeFolder);
				}
				else
				{
					itemData.SetItemType(ItemData::TypeFile);
				}
				m_items.push_back(itemData);
			}
	   }
	   while (FindNextFileW(hFind, &ffd) != 0);
	}
 
	FindClose(hFind);

	HRESULT hr = S_OK;   
	return hr;   
}   
       
// Retrieves the specified number of item identifiers in
// the enumeration sequence and advances the current position
// by the number of items retrieved.

HRESULT BoxEnumIDList::Next(ULONG celt,
							LPITEMIDLIST *rgelt,    
                            __out_opt ULONG *pceltFetched)   
{   
    int celtFetched = 0;   
          
    HRESULT hr = (pceltFetched || celt <= 1) ? S_OK : E_INVALIDARG;   
    if (SUCCEEDED(hr))   
    {   
        ULONG i = 0;   
		while (SUCCEEDED(hr) && i < celt && m_nItem < m_items.size())   
        {   
			LPITEMIDLIST p = m_folder->m_pidlMgr.Create(m_items[m_nItem]);

			rgelt[i] = p;
            if (SUCCEEDED(hr))
            {   
				celtFetched++;
                i++;
            }   
            m_nItem++;
        }   
    }   
       
    if (pceltFetched)   
    {   
        *pceltFetched = celtFetched;   
    }   
       
    return (celtFetched == celt) ? S_OK : S_FALSE;   
}   
       
HRESULT BoxEnumIDList::Skip(DWORD celt)   
{   
    m_nItem += celt;   
    return S_OK;   
}   
       
HRESULT BoxEnumIDList::Reset()   
{   
    m_nItem = 0;   
    return S_OK;   
}   
       
HRESULT BoxEnumIDList::Clone(__deref_out IEnumIDList **ppenum)   
{   
    // this method is rarely used and it's acceptable to not implement it.   
    return E_NOTIMPL;   
}   

  