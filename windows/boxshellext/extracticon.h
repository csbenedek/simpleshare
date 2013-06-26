#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "utils.h"
#include "ObjIdl.h"
#include "MPidlMgr.h"

#pragma once

//========================================================================================
// CIconLocation

struct CIconLocation
{
	CString Normal_Filename;
	LONG Normal_Index;

	CString Opened_Filename;
	LONG Opened_Index;

	static void ExtractLocation(LPCTSTR IconLoc, CString &Filename, LONG &Index)
	{
		Index = 0;				// Default for a filename without an index
		Filename = IconLoc;
		int CommaPos = Filename.ReverseFind(',');
		if (CommaPos != -1)
		{
			Index = StrToInt(LPCTSTR(Filename)+CommaPos+1);
			Filename.Delete(CommaPos, Filename.GetLength()-CommaPos);
		}
	}

	void SetNormalLocation(LPCTSTR IconLoc)
	{
		ExtractLocation(IconLoc, Normal_Filename, Normal_Index);
	}

	void SetOpenedLocation(LPCTSTR IconLoc)
	{
		ExtractLocation(IconLoc, Opened_Filename, Opened_Index);
	}
};


//========================================================================================
// Implements IExtracIcon.
//
// This interface is used to get the icon related to the Favorites Items.
// You can configure which icons are used for 'Normal' and 'Opened' states. See registry entries in *.rgs

class CExtractIcon : public IExtractIcon
{
public:

	//-------------------------------------------------------------------------------

	// Ensure the owner object is not freed before this one
	void Init(IUnknown *pUnkOwner);

	// Set the icon location
	// This member must be called before any IExtractIcon member.
	void SetIconLocation(CIconLocation &IconLocation);


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


	//-------------------------------------------------------------------------------
	// IExtractIcon methods

	STDMETHOD(GetIconLocation) (UINT uFlags, LPTSTR szIconFile, UINT cchMax, int *piIndex, UINT *pwFlags);
	STDMETHOD(Extract) (LPCTSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize);

protected:
    long		_cRef;
	IUnknown* m_UnkOwnerPtr;
	CIconLocation m_IconLocation;
};

