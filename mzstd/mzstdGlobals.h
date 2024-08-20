#pragma once

#include "mzString.h"

namespace mzstd {

DECLSPEC_MZSTD USHORT WINAPI GetNumberOfDigits(ULONGLONG number);
DECLSPEC_MZSTD CmzString WINAPI GetSystemErrorMessage(DWORD errorCode);
DECLSPEC_MZSTD CmzString WINAPI GetLastSystemErrorMessage();
DECLSPEC_MZSTD CmzString WINAPI GetDetailedSystemErrorMessage(DWORD errorCode, WCHAR* prependString = NULL);
DECLSPEC_MZSTD CmzString WINAPI GetDetailedLastSystemErrorMessage(WCHAR* prependString = NULL);

}
