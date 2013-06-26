#include "itemdata.h"

// ItemData

ItemData::ItemData()
	: m_itemType(0)
	, m_path(NULL)
	, m_name(NULL)
{
}

/* ITEMID layout
 *
 * DWORD	Magic
 * USHORT	ItemType
 * USHORT	Rank
 * USHORT[]	PathName
 * USHORT[]	DisplayName
 *
 */


long ItemData::GetSize()
{
	return   4
		   + 2
		   + 2
		   + (wcslen(m_path)+1)*sizeof(OLECHAR)
		   + (wcslen(m_name)+1)*sizeof(OLECHAR)
		   ;
}

void ItemData::CopyTo(void *pTarget)
{
	*(DWORD*)pTarget = MAGIC;
	*((USHORT*)pTarget+2) = m_itemType;
	wcscpy((OLECHAR*)pTarget+4, m_path);
	wcscpy((OLECHAR*)pTarget+4+wcslen(m_path)+1, m_name);
}

//-------------------------------------------------------------------------------

void ItemData::SetPath(LPCWSTR path)
{
	if (m_path) CoTaskMemFree(m_path);
	SHStrDupW(path, &m_path);
}

void ItemData::SetName(LPCWSTR name)
{
	if (m_path) CoTaskMemFree(m_name);
	SHStrDupW(name, &m_name);
}

void ItemData::SetItemType(USHORT itemType)
{
	m_itemType = itemType;
}

//-------------------------------------------------------------------------------

bool ItemData::IsOwn(LPCITEMIDLIST pidl)
{
	if ((pidl == NULL) || (pidl->mkid.cb < 4))
		return false;

	return *((DWORD*)(pidl->mkid.abID)) == MAGIC;
}

LPOLESTR ItemData::GetPath(LPCITEMIDLIST pidl)
{
	return (OLECHAR*)pidl+5;
}

LPOLESTR ItemData::GetName(LPCITEMIDLIST pidl)
{
	return (OLECHAR*) ( (BYTE*)pidl + 10 + (wcslen((OLECHAR*)pidl+5)+1) * sizeof(OLECHAR) );
}

USHORT ItemData::GetItemType(LPCITEMIDLIST pidl)
{
	return *((USHORT*)pidl+3);
}
