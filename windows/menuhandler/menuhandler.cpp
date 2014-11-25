/* menuhandler.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include <windows.h>
#include <shlwapi.h>
#include <objbase.h>
#include <Shlobj.h>
#include <olectl.h>
#include <strsafe.h>
#include "guid.h"
#include "classfactory.h"
#include "contextmenu.h"

//
//#pragma comment(lib, "shlwapi.lib")

#define SHELL_EXT_APPROVED        L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved\\%s"

HINSTANCE g_hInst = NULL;
LONG g_cRefModule = 0;

void DllAddRef()
{
    InterlockedIncrement(&g_cRefModule);
}

void DllRelease()
{
    InterlockedDecrement(&g_cRefModule);
}

STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void *lpReserved)
{ 
    if (DLL_PROCESS_ATTACH == dwReason)
    {
        g_hInst = hInstance;
    }
    return TRUE;
}                              

STDAPI DllCanUnloadNow(void)
{
    return g_cRefModule ? S_FALSE : S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    *ppv = NULL;

    HRESULT hr = E_OUTOFMEMORY;
    ContextMenuClassFactory* classFactory = new ContextMenuClassFactory(rclsid);
    if (classFactory)
    {
        hr = classFactory->QueryInterface(riid, ppv);
        classFactory->Release();
    }
    return hr;
}

// Structure to hold data for individual keys to register.
typedef struct
{
    HKEY  hRootKey;
    PCWSTR pszSubKey;
    PCWSTR pszClassID;
    PCWSTR pszValueName;
    BYTE *pszData;
    DWORD dwType;
} REGSTRUCT;

//
//  1. The classID must be created under HKCR\CLSID.
//     a. DefaultIcon must be set to <Path and Module>,0.
//     b. InprocServer32 set to path and module.
//        i. Threading model specified as Apartment.
//     c. ShellFolder attributes must be set.
//  2. If the extension in non-rooted, its GUID is entered at the desired folder.
//  3. It must be registered as approved for Windows NT or XP.
//
STDAPI DllRegisterServer()
{
    HRESULT hr = S_OK;

	WCHAR szContextMenuClassID[64];
	WCHAR szSubKey[MAX_PATH];
	WCHAR szData[MAX_PATH];
    StringFromGUID2(CLSID_SimpleShareContextMenu, szContextMenuClassID, ARRAYSIZE(szContextMenuClassID)); // Convert the IID to a string.

    // Get the path and module name.
    WCHAR szModulePathAndName[MAX_PATH];
    GetModuleFileNameW(g_hInst, szModulePathAndName, ARRAYSIZE(szModulePathAndName));

    // This will setup and register the basic ClassIDs. 
    DWORD dwData = SFGAO_FOLDER | SFGAO_HASSUBFOLDER  | SFGAO_CANDELETE;
    REGSTRUCT rgRegEntries[] = 
    {
        HKEY_CLASSES_ROOT,   L"CLSID\\%s",                  szContextMenuClassID,    NULL,                   (LPBYTE)NAME_W,   REG_SZ,    
        HKEY_CLASSES_ROOT,   L"CLSID\\%s\\InprocServer32",  szContextMenuClassID,    NULL,                   (LPBYTE)L"%s",          REG_SZ,
        HKEY_CLASSES_ROOT,   L"CLSID\\%s\\InprocServer32",  szContextMenuClassID,    L"ThreadingModel",      (LPBYTE)L"Apartment",   REG_SZ,

        // register the context menu handler for files and directories
        HKEY_CLASSES_ROOT,   L"*\\shellex\\ContextMenuHandlers\\%s",  NAME_W, NULL,  (LPBYTE)szContextMenuClassID, REG_SZ,
        HKEY_CLASSES_ROOT,   L"Directory\\shellex\\ContextMenuHandlers\\%s",  NAME_W, NULL,  (LPBYTE)szContextMenuClassID, REG_SZ,
    };

    HKEY hKey = NULL;
    
    for (int i = 0; SUCCEEDED(hr) && (i < ARRAYSIZE(rgRegEntries)); i++)
    {
        // Create the sub key string.
        hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), rgRegEntries[i].pszSubKey, rgRegEntries[i].pszClassID);

        if (SUCCEEDED(hr))
        {
			//MessageBoxW(0, szSubKey, szModulePathAndName, MB_OK);

			LONG lResult = RegCreateKeyExW(rgRegEntries[i].hRootKey, szSubKey, 0, NULL, 
                                           REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

			if (S_OK == lResult)
            {

                // If this is a string entry, create the string.
                if (REG_SZ == rgRegEntries[i].dwType)
                { 
                    hr = StringCchPrintfW(szData, ARRAYSIZE(szData), (LPWSTR)rgRegEntries[i].pszData, szModulePathAndName);

                    if (SUCCEEDED(hr))
                    {
						lResult = RegSetValueExW(hKey, 
                                       rgRegEntries[i].pszValueName, 
                                       0,
                                       rgRegEntries[i].dwType, 
                                       (LPBYTE)szData, 
                                       (lstrlenW(szData) + 1) * sizeof(WCHAR));
                    }
                }
                else if (REG_DWORD == rgRegEntries[i].dwType)
                {
                    RegSetValueExW(hKey, 
                                   rgRegEntries[i].pszValueName, 
                                   0, rgRegEntries[i].dwType,
                                   rgRegEntries[i].pszData, 
                                   sizeof(DWORD));
                }
                        
                RegCloseKey(hKey);
            }
            else
            {
				/*if (lResult == 0x5) // no access
				{
					MessageBoxW(0, szSubKey, L"NOT AUTOHORIZED", MB_OK);
				}*/
				hr = SELFREG_E_CLASS;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
		// It is assumed at this point that we are running on Windows XP or later
		// and therefore the extension needs to be registered as approved.
		PCWSTR rgszApprovedClassIDs[] = { szContextMenuClassID };
		for (int i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(rgszApprovedClassIDs); i++)
		{
			hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), SHELL_EXT_APPROVED, szContextMenuClassID);

			if (SUCCEEDED(hr))
            {
				hr = RegCreateKeyExW(HKEY_LOCAL_MACHINE, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
				if (SUCCEEDED(hr))
				{
					// Create the value string.
					hr = StringCchCopyW(szData, ARRAYSIZE(szData), NAME_W);

					if (SUCCEEDED(hr))
                    {
						RegSetValueExW(hKey, 
                                                 NULL, 
                                                 0, 
                                                 REG_SZ, 
                                                 (LPBYTE)szData, 
                                                 (lstrlenW(szData) + 1) * sizeof(WCHAR));
                                
						hr = S_OK;

					}
                        
                    RegCloseKey(hKey);
				}
			}
		}
    }

    if (FAILED(hr))
    {
        //Remove the stuff we added.
        DllUnregisterServer();
    }

    return hr;
}


//
// Registry keys are removed here.
//
STDAPI DllUnregisterServer()
{
    WCHAR szSubKey[MAX_PATH];
	WCHAR szContextMenuClassID[MAX_PATH];
    
    //Delete the context menu entries.
    HRESULT hr = StringFromGUID2(CLSID_SimpleShareContextMenu, 
                                  szContextMenuClassID, 
                                  ARRAYSIZE(szContextMenuClassID));
    if (SUCCEEDED(hr))
    {
        hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szContextMenuClassID);
        if (SUCCEEDED(hr))
        {
            hr = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
        }
    }

	hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"*\\shellex\\ContextMenuHandlers\\%s", NAME_W);
	if (SUCCEEDED(hr))
	{
		hr = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
    }

	hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"Directory\\shellex\\ContextMenuHandlers\\%s", NAME_W);
	if (SUCCEEDED(hr))
	{
		hr = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
    }

    return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}
