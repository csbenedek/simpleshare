#include "stdafx.h"
#include <shlobj.h>
#include <shldisp.h>
#include <exdisp.h>
#include <mshtml.h>

//#import <SHDocVw.dll>
//#import <Shell32.dll>

wchar_t* __stdcall SelectedFiles()
{
	wchar_t* result = NULL;
	long size = 0;

    static int init = CoInitializeEx(0, COINIT_APARTMENTTHREADED); // init at first time function called

    IShellWindows* psw;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL,
        CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&psw))))
    {
		VARIANT v = { VT_I4 };
		if (SUCCEEDED(psw->get_Count(&v.lVal)))
        {
            // walk backwards to make sure the windows that close
            // don't cause the array to be re-ordered
            while (--v.lVal >= 0)
            {
                IDispatch *pdisp;
                if (S_OK == psw->Item(v, &pdisp))
                {
					IWebBrowser2* pwb;
					if (SUCCEEDED(pdisp->QueryInterface(IID_PPV_ARGS(&pwb))))
					{

						SHANDLE_PTR hWndPtr;
						if (SUCCEEDED(pwb->get_HWND(&hWndPtr)))
						{
							HWND hWnd = reinterpret_cast<HWND>(hWndPtr);
							HWND hActiveWnd = GetForegroundWindow();
							if (hWnd != hActiveWnd)
							{
								continue;
							}
						}

						IDispatch* pdispDocument;
						if (SUCCEEDED(pwb->get_Document(&pdispDocument)))
						{
							IShellFolderViewDual2* shellFolder;
							if (SUCCEEDED(pdispDocument->QueryInterface(IID_PPV_ARGS(&shellFolder))))							
							{
								FolderItems* items;
								if (SUCCEEDED(shellFolder->SelectedItems(&items)))
								{
									long count;
									items->get_Count(&count);
									for(long i = 0; i < count; i++)
									{
										FolderItem* item;
										VARIANT index = { VT_I4 };
										index.lVal = i;
										if (SUCCEEDED(items->Item(index, &item)))
										{
											BSTR path;
											if (SUCCEEDED(item->get_Path(&path)))
											{
												if (path)
												{
													size += (SysStringLen(path) + 1 + 2) * sizeof(wchar_t);
													wchar_t* newResult = new wchar_t[size];
													if (newResult)
													{
														if (result)
														{
															wcscpy_s(newResult, size, result);
															wcscat_s(newResult, size, L";");
															delete result;
														}
														else
														{
															wcscpy_s(newResult, size, L"");
														}
														result = newResult;
														wcscat_s(result, size, path);
													}
													SysFreeString(path);

#ifdef _DEBUG
													//MessageBoxW(NULL, result, L"MESSAGE", 0);
#endif
												}
											}
											item->Release();
										}
									}
									items->Release();
								}
								shellFolder->Release();
							}
							pdispDocument->Release();
						}
						//pwb->Quit();
						pwb->Release();
					}
					pdisp->Release();
				}
			}
        }
        psw->Release();
    }   

	if (result == NULL)
	{
		// can't find selection in explorer windows
		// try to find selection on desktop
	}

	return result;
}

void __stdcall FreeSelectedFiles(wchar_t* buff)
{
	delete buff;
}

/*
  -------------------------------------------------------------------
  Description:
    Creates the actual 'lnk' file (assumes COM has been initialized).

  Parameters:
    pszTargetfile    - File name of the link's target.
    pszTargetargs    - Command line arguments passed to link's target.
    pszLinkfile      - File name of the actual link file being created.
    pszDescription   - Description of the linked item.
    iShowmode        - ShowWindow() constant for the link's target.
    pszCurdir        - Working directory of the active link. 
    pszIconfile      - File name of the icon file used for the link.
    iIconindex       - Index of the icon in the icon file.

  Returns:
    HRESULT value >= 0 for success, < 0 for failure.
  --------------------------------------------------------------------
  *./
HRESULT __stdcall CreateShortCut(LPCSTR pszTargetfile, LPCSTR pszTargetargs,
                                LPCSTR pszLinkfile, LPCSTR pszDescription, 
                                int iShowmode, LPCSTR pszCurdir, 
                                LPCSTR pszIconfile, int iIconindex)
{
	HRESULT			hr = E_INVALIDARG;
    IShellLink*		shellLink;
    IPersistFile*	persistFile;
    WCHAR			wszLinkfile[MAX_PATH];

    if ((pszTargetfile != NULL) && (strlen(pszTargetfile) > 0) &&
         (pszTargetargs != NULL) &&
         (pszLinkfile != NULL) && (strlen(pszLinkfile) > 0) &&
         (pszDescription != NULL) && 
         (iShowmode >= 0) &&
         (pszCurdir != NULL) && 
         (pszIconfile != NULL) &&
         (iIconindex >= 0))
    {
		hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&shellLink));
		if (SUCCEEDED(hr))
		{
			hr = shellLink->SetPath(pszTargetfile);
			hr = shellLink->SetArguments(pszTargetargs);
			if (strlen(pszDescription) > 0)
			{
				hr = shellLink->SetDescription(pszDescription);
			}
			if (iShowmode > 0)
			{
				hr = shellLink->SetShowCmd(iShowmode);
			}
			if (strlen(pszCurdir) > 0)
			{
				hr = shellLink->SetWorkingDirectory(pszCurdir);
			}
			if (strlen(pszIconfile) > 0 && iIconindex >= 0)
			{
				hr = shellLink->SetIconLocation(pszIconfile, iIconindex);
			}

			hr = shellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persistFile));
			if (SUCCEEDED(hr))
			{
				int iWideCharsWritten = MultiByteToWideChar(CP_ACP, 0, 
                                               pszLinkfile, -1, 
                                               wszLinkfile, MAX_PATH);
				hr = persistFile->Save(wszLinkfile, TRUE);
				persistFile->Release();
			}
			shellLink->Release();
		}
	}

    return hr;

}
*/