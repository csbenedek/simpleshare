#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "utils.h"
#include "ObjIdl.h"
#include "MPidlMgr.h"

#pragma once

//========================================================================================
// Light implementation of IDataObject.
//
// This object is used when you double-click on an item in the FileDialog.
// It's purpose is simply to encapsulate the complete pidl for the item (remember it's a Favorite item)
// into the IDataObject, so that the FileDialog can pass it further to our IShellFolder::BindToObject().
// Because I'm only interested in the FileDialog behaviour, every methods returns E_NOTIMPL except GetData().

class CDataObject :	public IDataObject, public IEnumFORMATETC
{
public:
	CDataObject();
	~CDataObject();

	// Ensure the owner object is not freed before this one
	void Init(IUnknown *pUnkOwner);

	// Populate the object with the Favorite Item pidl.
	// This member must be called before any IDataObject member.
	void SetPidl(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidl);


    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

	//-------------------------------------------------------------------------------
	// IDataObject methods

	STDMETHOD(GetData) (LPFORMATETC pFE, LPSTGMEDIUM pStgMedium);
	STDMETHOD(GetDataHere) (LPFORMATETC, LPSTGMEDIUM);
	STDMETHOD(QueryGetData) (LPFORMATETC);
	STDMETHOD(GetCanonicalFormatEtc) (LPFORMATETC, LPFORMATETC);
	STDMETHOD(SetData) (LPFORMATETC, LPSTGMEDIUM, BOOL);
	STDMETHOD(EnumFormatEtc) (DWORD, IEnumFORMATETC**);
	STDMETHOD(DAdvise) (LPFORMATETC, DWORD, IAdviseSink*, LPDWORD);
	STDMETHOD(DUnadvise) (DWORD dwConnection);
	STDMETHOD(EnumDAdvise) (IEnumSTATDATA** ppEnumAdvise);

	//-------------------------------------------------------------------------------
	// IEnumFORMATETC members

	STDMETHOD(Next) (ULONG, LPFORMATETC, ULONG*);
	STDMETHOD(Skip) (ULONG);
	STDMETHOD(Reset) ();
	STDMETHOD(Clone) (LPENUMFORMATETC*);

protected:
    long		_cRef;
	IUnknown*	m_UnkOwnerPtr;
	CPidlMgr	m_PidlMgr;

	UINT m_cfShellIDList;

	LPITEMIDLIST m_pidl;
	LPITEMIDLIST m_pidlParent;
};
