#include "dataobject.h"

//========================================================================================
// CDataObject

// helper function that creates a CFSTR_SHELLIDLIST format from given pidls.
HGLOBAL CreateShellIDList(LPCITEMIDLIST pidlParent, LPCITEMIDLIST *aPidls, UINT uItemCount)
{
	HGLOBAL hGlobal = NULL;
	LPIDA pData;
	UINT iCurPos;
	UINT cbPidl;
	UINT i;
	CPidlMgr PidlMgr;

	// get the size of the parent folder's PIDL
	cbPidl = PidlMgr.GetSize(pidlParent);

	// get the total size of all of the PIDLs
	for (i=0; i<uItemCount; i++)
	{
		cbPidl += PidlMgr.GetSize(aPidls[i]);
	}

	// Find the end of the CIDA structure. This is the size of the 
	// CIDA structure itself (which includes one element of aoffset) plus the 
	// additional number of elements in aoffset.
	iCurPos = sizeof(CIDA) + (uItemCount * sizeof(UINT));

	// Allocate the memory for the CIDA structure and it's variable length members.
	hGlobal = GlobalAlloc(GPTR | GMEM_SHARE, (DWORD)
	                                         (iCurPos +        // size of the CIDA structure and the additional aoffset elements
	                                         (cbPidl + 1)));   // size of the pidls
	if (!hGlobal)
		return NULL;

	if (pData = (LPIDA)GlobalLock(hGlobal))
	{
		pData->cidl = uItemCount;
		pData->aoffset[0] = iCurPos;

		// add the PIDL for the parent folder
		cbPidl = PidlMgr.GetSize(pidlParent);
		CopyMemory((LPBYTE)(pData) + iCurPos, (LPBYTE)pidlParent, cbPidl);
		iCurPos += cbPidl;

		for (i=0; i<uItemCount; i++)
		{
			// get the size of the PIDL
			cbPidl = PidlMgr.GetSize(aPidls[i]);

			// fill out the members of the CIDA structure.
			pData->aoffset[i + 1] = iCurPos;

			// copy the contents of the PIDL
			CopyMemory((LPBYTE)(pData) + iCurPos, (LPBYTE)aPidls[i], cbPidl);

			// set up the position of the next PIDL
			iCurPos += cbPidl;
		}

		GlobalUnlock(hGlobal);
	}

	return hGlobal;
}

CDataObject::CDataObject() : m_pidl(NULL), m_pidlParent(NULL)
{
	m_cfShellIDList = RegisterClipboardFormat(CFSTR_SHELLIDLIST);
}

CDataObject::~CDataObject()
{
	m_PidlMgr.Delete(m_pidl);
	m_PidlMgr.Delete(m_pidlParent);
}

void CDataObject::Init(IUnknown *pUnkOwner)
{
	m_UnkOwnerPtr = pUnkOwner;
}

void CDataObject::SetPidl(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidl)
{
	m_pidlParent = m_PidlMgr.Copy(pidlParent);
	m_pidl = m_PidlMgr.Copy(pidl);
}

// IUnknown
IFACEMETHODIMP CDataObject::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = {
        QITABENT(CDataObject, IDataObject),
        QITABENT(CDataObject, IEnumFORMATETC),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CDataObject::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CDataObject::Release()
{
    long cRef = InterlockedDecrement(&_cRef);
    if (!cRef)
    {
        delete this;
    }
    return cRef;
}


//-------------------------------------------------------------------------------

STDMETHODIMP CDataObject::GetData(LPFORMATETC pFE, LPSTGMEDIUM pStgMedium)
{
	if (pFE->cfFormat == m_cfShellIDList)
	{
		pStgMedium->hGlobal = CreateShellIDList(m_pidlParent, (LPCITEMIDLIST*)&m_pidl, 1);

		if (pStgMedium->hGlobal)
		{
			pStgMedium->tymed = TYMED_HGLOBAL;
			pStgMedium->pUnkForRelease = NULL;	// Even if our tymed is HGLOBAL, WinXP calls ReleaseStgMedium() which tries to call pUnkForRelease->Release() : BANG!
			return S_OK;
		}
	}

	return E_INVALIDARG;
}

STDMETHODIMP CDataObject::GetDataHere(LPFORMATETC, LPSTGMEDIUM)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::QueryGetData(LPFORMATETC)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::GetCanonicalFormatEtc(LPFORMATETC, LPFORMATETC)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::SetData(LPFORMATETC, LPSTGMEDIUM, BOOL)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumFormatEtc(DWORD, IEnumFORMATETC**)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::DAdvise(LPFORMATETC, DWORD, IAdviseSink*, LPDWORD)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::DUnadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumDAdvise(IEnumSTATDATA** ppEnumAdvise)
{
	return E_NOTIMPL;
}

//-------------------------------------------------------------------------------

STDMETHODIMP CDataObject::Next(ULONG, LPFORMATETC, ULONG*)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::Skip(ULONG)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::Reset()
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::Clone(LPENUMFORMATETC*)
{
	return E_NOTIMPL;
}


