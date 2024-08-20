#ifndef __MZSTD_MESSAGE_BOX_H__
#define __MZSTD_MESSAGE_BOX_H__

#include "mzstdCommon.h"
#include "mzString.h"

namespace mzstd {

DECLSPEC_MZSTD LONGLONG WINAPI MessageBox(CmzString caption, CmzString message);
DECLSPEC_MZSTD LONGLONG WINAPI AlertBox(CmzString message);
DECLSPEC_MZSTD LONGLONG WINAPI WarningBox(CmzString message);
DECLSPEC_MZSTD LONGLONG WINAPI QuestionBox(CmzString message);
DECLSPEC_MZSTD LONGLONG WINAPI QuestionCancelBox(CmzString message);
DECLSPEC_MZSTD LONGLONG WINAPI ErrorBox(CmzString message);
DECLSPEC_MZSTD LONGLONG WINAPI InformationBox(CmzString message);
DECLSPEC_MZSTD BOOL WINAPI isMessageBoxOpened();

DECLSPEC_MZSTD VOID WINAPI ShowLastSystemError(WCHAR* prependString = NULL);
DECLSPEC_MZSTD VOID WINAPI ShowSystemError(DWORD errorCode, WCHAR* prependString = NULL);
DECLSPEC_MZSTD VOID WINAPI ShowDetailedLastSystemError(WCHAR* prependString = NULL);
DECLSPEC_MZSTD VOID WINAPI ShowDetailedSystemError(DWORD errorCode, WCHAR* prependString = NULL);

}

#endif
