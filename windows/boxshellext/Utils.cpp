/**************************************************************************
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#include <windows.h>
#include <shlobj.h>
#include "resource.h"
#include "Utils.h"
#include "shlwapi.h"

STDAPI StringToStrRet(PCWSTR pszName, STRRET *pStrRet)
{
    pStrRet->uType = STRRET_WSTR;
    return SHStrDupW(pszName, &pStrRet->pOleStr);
}

HRESULT DisplayItem(IShellItemArray *psia, HWND hwnd)
{
    // Get the first ShellItem and display its name
    IShellItem *psi;
    HRESULT hr = psia->GetItemAt(0, &psi);
    if (SUCCEEDED(hr))
    {
        PWSTR pszDisplayName;
        hr = psi->GetDisplayName(SIGDN_NORMALDISPLAY, &pszDisplayName);
        if (SUCCEEDED(hr))
        {
            MessageBoxW(hwnd, pszDisplayName, pszDisplayName, MB_OK);
            CoTaskMemFree(pszDisplayName);
        }
        psi->Release();
    }
    return hr;
}