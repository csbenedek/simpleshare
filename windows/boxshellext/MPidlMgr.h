//========================================================================================
//
// Module:			PidlMgr
// Author:          Pascal Hurni
// Creation Date:	11.07.2004
//
// Copyright 2004 Mortimer Systems
// This software is free. I grant you a non-exclusive license to use it.
//
// Greatly inspired from several examples from: Microsoft, Michael Dunn, ...
//
//========================================================================================


#ifndef __MORTIMER_PIDLMGR_H__
#define __MORTIMER_PIDLMGR_H__

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//========================================================================================
// encapsulate these classes in a namespace


//========================================================================================

class CPidlData
{
public:
//	CPidlData();
//	CPidlData(const CPidlData &src);
//	virtual CPidlData &operator=(const CPidlData &src);

	virtual long GetSize() = 0;
	virtual void CopyTo(void *pTarget) = 0;
};


class CPidlMgr
{
public:
	CPidlMgr()
	{
	}

	LPITEMIDLIST Create(CPidlData &Data)
	{
		// Total size of the PIDL, including SHITEMID
		UINT TotalSize = sizeof(ITEMIDLIST) + Data.GetSize();

		// Also allocate memory for the final null SHITEMID.
		LPITEMIDLIST pidlNew = (LPITEMIDLIST) CoTaskMemAlloc(TotalSize + sizeof(ITEMIDLIST));
		if (pidlNew)
		{
			LPITEMIDLIST pidlTemp = pidlNew;

			// Prepares the PIDL to be filled with actual data
			pidlTemp->mkid.cb = TotalSize;

			// Fill the PIDL
			Data.CopyTo((void*)pidlTemp->mkid.abID);

			// Set an empty PIDL at the end
			pidlTemp = GetNextItem(pidlTemp);
			pidlTemp->mkid.cb = 0;
			pidlTemp->mkid.abID[0] = 0;
		}

		return pidlNew;
	}

	void Delete(LPITEMIDLIST pidl)
	{
		if (pidl)
			CoTaskMemFree(pidl);
	}

	LPITEMIDLIST GetNextItem(LPCITEMIDLIST pidl)
	{
		if (!pidl)
			return NULL;

		return LPITEMIDLIST(LPBYTE(pidl) + pidl->mkid.cb);
	}

	LPITEMIDLIST GetLastItem(LPCITEMIDLIST pidl)
	{
		LPITEMIDLIST pidlLast = NULL;

		//get the PIDL of the last item in the list
		while (pidl && pidl->mkid.cb)
		{
			pidlLast = (LPITEMIDLIST)pidl;
			pidl = GetNextItem(pidl);
		}

		return pidlLast;
	}

	LPITEMIDLIST Copy(LPCITEMIDLIST pidlSrc)
	{
		LPITEMIDLIST pidlTarget = NULL;
		UINT Size = 0;

		if (pidlSrc == NULL)
			return NULL;

		// Allocate memory for the new PIDL.
		Size = GetSize(pidlSrc);
		pidlTarget = (LPITEMIDLIST) CoTaskMemAlloc(Size);

		if (pidlTarget == NULL)
			return NULL;

		// Copy the source PIDL to the target PIDL.
		CopyMemory(pidlTarget, pidlSrc, Size);

		return pidlTarget;
	}

	UINT GetSize(LPCITEMIDLIST pidl)
	{
		UINT Size = 0;
		LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;

		if (!pidl)
			return 0;

		while (pidlTemp->mkid.cb != 0)
		{
			Size += pidlTemp->mkid.cb;
			pidlTemp = GetNextItem(pidlTemp);
		}  

		// add the size of the NULL terminating ITEMIDLIST
		Size += sizeof(ITEMIDLIST);

		return Size;
	}

	bool IsSingle(LPCITEMIDLIST pidl)
	{
		LPITEMIDLIST pidlTemp = GetNextItem(pidl);
		return pidlTemp->mkid.cb == 0;
	}

};


//========================================================================================

#endif // __MORTIMER_PIDLMGR_H__
