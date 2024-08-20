// mzstd.cpp : Defines the initialization routines for the DLL.
//
#include "pch.h"
#include "mzstdGlobals.h"
#include "mzString.h"

namespace mzstd {

USHORT WINAPI GetNumberOfDigits(ULONGLONG number) {
    // ULONGLONG is 4 Bytes, so max ULONGLONG is 0xffffffff = 4294967295
    if (number < 10UL) {
        return 1;
    }
    if (number < 100UL) {
        return 2;
    }
    if (number < 1000UL) {
        return 3;
    }
    if (number < 10000UL) {
        return 4;
    }
    if (number < 100000UL) {
        return 5;
    }
    if (number < 1000000UL) {
        return 6;
    }
    if (number < 10000000UL) {
        return 7;
    }
    if (number < 100000000UL) {
        return 8;
    }
    if (number < 1000000000UL) {
        return 9;
    }
    return 10;
}


CmzString WINAPI GetSystemErrorMessage(DWORD errorCode) {
	LPVOID msgbuf;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, errorCode, 0, (LPTSTR)&msgbuf, 0, NULL);
    CmzString msg((WCHAR*)msgbuf);
	LocalFree(msgbuf);
    msg.trim();
    return msg;
}

CmzString WINAPI GetLastSystemErrorMessage() {
    return GetSystemErrorMessage(GetLastError());
}

CmzString WINAPI GetDetailedSystemErrorMessage(DWORD errorCode, WCHAR* prependString) {
    CmzString msg = FormatMessage(L"System error code:\n    %s   %ddx\nSystem error message:\n    %s", L"ERROR_*", errorCode, GetSystemErrorMessage(errorCode).getBuffer());
    if (prependString) {
        msg.prepend(prependString);
    }
    return msg;
}

CmzString WINAPI GetDetailedLastSystemErrorMessage(WCHAR* prependString) {
    return GetDetailedSystemErrorMessage(GetLastError(), prependString);
}

}
