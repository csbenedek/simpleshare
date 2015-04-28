#include "extracticon.h"

//========================================================================================
// CExtractIcon

void CExtractIcon::Init(IUnknown *pUnkOwner)
{
	m_UnkOwnerPtr = pUnkOwner;
}

void CExtractIcon::SetIconLocation(CIconLocation &IconLocation)
{
	m_IconLocation = IconLocation;
}

//-------------------------------------------------------------------------------

STDMETHODIMP CExtractIcon::GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax, int *piIndex, UINT *pwFlags)
{
	if (pwFlags != NULL)
		*pwFlags = 0;

	if (piIndex == NULL)
		return E_INVALIDARG;

	CString Filename;

	if ((uFlags & GIL_OPENICON) && !m_IconLocation.Opened_Filename.IsEmpty())
	{
		Filename = m_IconLocation.Opened_Filename;
		*piIndex = m_IconLocation.Opened_Index;
	}
	else
	{
		Filename = m_IconLocation.Normal_Filename;
		*piIndex = m_IconLocation.Normal_Index;
	}

	if (cchMax <= (UINT)Filename.GetLength())
		return E_INVALIDARG;

	CStringCopyTo(Filename, szIconFile);
	return S_OK;
}

STDMETHODIMP CExtractIcon::Extract(LPCTSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize)
{
	// Caller should extract the icons itself. (We supplied a valid filename and index)
	return S_FALSE;
}
