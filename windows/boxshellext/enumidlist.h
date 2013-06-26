/************************************************************************** 
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
   PARTICULAR PURPOSE. 
  
   (c) Microsoft Corporation. All Rights Reserved. 
**************************************************************************/ 
 
#pragma once 

#include <vector>
#include "itemdata.h"

class BoxEnumIDList : public IEnumIDList 
{ 
public: 
    BoxEnumIDList(DWORD flags, int nCurrent, BoxShellFolder* folder); 
 
    // IUnknown methods 
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv); 
    IFACEMETHODIMP_(ULONG) AddRef(); 
    IFACEMETHODIMP_(ULONG) Release(); 
 
    // IEnumIDList 
    IFACEMETHODIMP Next(ULONG celt,  
                        __out_ecount_part(celt, *pceltFetched) PITEMID_CHILD *rgelt,  
                        __out_opt ULONG *pceltFetched); 
    IFACEMETHODIMP Skip(DWORD celt); 
    IFACEMETHODIMP Reset(); 
    IFACEMETHODIMP Clone(__deref_out IEnumIDList **ppenum); 
 
    HRESULT Initialize(); 
 
private: 
    ~BoxEnumIDList(); 
 
    LONG m_cRef; 
    DWORD m_flags; 
    int m_nItem; 

	std::vector<ItemData> m_items;
 
    BoxShellFolder *m_folder; 
}; 
