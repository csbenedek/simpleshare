#pragma once

#include "MPidlMgr.h"

//========================================================================================
// This class handles our data that gets embedded in a pidl.


class ItemData : public CPidlData
{
public:

	ItemData();

	//-------------------------------------------------------------------------------
	// used by the manager to embed data, previously set by clients, into a pidl

	// The pidl signature
	enum { MAGIC = 0xAA000055 | ('DF'<<8) };

	// return the size of the pidl data. Not counting the mkid.cb member.
	long GetSize();

	// copy the data to the target
	void CopyTo(void *pTarget);

	//-------------------------------------------------------------------------------
	// Used by clients to set data

	// Item Type can be :
	enum
	{
		TypeFile,
		TypeFolder,
	};

	// The target path
	void SetPath(LPCWSTR Path);

	// The display name (may contain any chars)
	void SetName(LPCWSTR Name);

	// The item type (see above)
	void SetItemType(USHORT ItemType);

	//-------------------------------------------------------------------------------
	// Used by clients to get data from a given pidl

	// Is this pidl really one of ours?
	static bool IsOwn(LPCITEMIDLIST pidl);

	// Retrieve target path.
	// The pidl MUST remain valid until the caller has finished with the returned string.
	static LPOLESTR GetPath(LPCITEMIDLIST pidl);

	// Retrieve display name.
	// The pidl MUST remain valid until the caller has finished with the returned string.
	static LPOLESTR GetName(LPCITEMIDLIST pidl);

	// Retrieve the item type (see above)
	static USHORT GetItemType(LPCITEMIDLIST pidl);

	//-------------------------------------------------------------------------------

protected:
	USHORT		m_itemType;
	USHORT		m_reserved;
	OLECHAR*	m_path;
	OLECHAR*	m_name;
};


