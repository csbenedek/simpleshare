/* shellutils.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef SHELLUTILS_H
#define SHELLUTILS_H

#ifdef SHELLUTILS_EXPORTS
#define SHELLUTILS_API __declspec(dllexport) __stdcall
#else
#define SHELLUTILS_API __declspec(dllimport)
#endif

extern "C"
{

typedef wchar_t* (__stdcall *SelectedFilesFunc)();
typedef void (__stdcall *FreeSelectedFilesFunc)(wchar_t* buff);
/*typedef HRESULT (__stdcall *CreateShortCutFunc)(LPCSTR pszTargetfile, LPCSTR pszTargetargs,
                                LPCSTR pszLinkfile, LPCSTR pszDescription, 
                                int iShowmode, LPCSTR pszCurdir, 
                                LPCSTR pszIconfile, int iIconindex);*/

SHELLUTILS_API wchar_t* SelectedFiles();
SHELLUTILS_API void FreeSelectedFiles(wchar_t* buff);
/*SHELLUTILS_API HRESULT __stdcall CreateShortCut(LPCSTR pszTargetfile, LPCSTR pszTargetargs,
                                LPCSTR pszLinkfile, LPCSTR pszDescription, 
                                int iShowmode, LPCSTR pszCurdir, 
                                LPCSTR pszIconfile, int iIconindex);*/

}

#endif //SHELLUTILS_H
