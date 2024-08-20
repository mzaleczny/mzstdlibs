#include "pch.h"
#include "mzDateTime.h"

namespace mzstd {

WINAPI CmzDateTime::CmzDateTime() {
    this->setRealClassName(L"CmzDateTime");
}

WINAPI CmzDateTime::~CmzDateTime() {
}

CmzString CmzDateTime::date(WCHAR* format) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    CmzString retval = format;
    if (retval.strpos(L"ms") != -1) {
        retval.replace(L"ms", CmzString(st.wMilliseconds, FALSE, 2));
    }
    if (retval.strpos(L"H") != -1) {
        retval.replace(L"H", CmzString(st.wHour, FALSE, 2));
    }
    if (retval.strpos(L"i") != -1) {
        retval.replace(L"i", CmzString(st.wMinute, FALSE, 2));
    }
    if (retval.strpos(L"s") != -1) {
        retval.replace(L"s", CmzString(st.wSecond, FALSE, 2));
    }
    if (retval.strpos(L"Y") != -1) {
        retval.replace(L"Y", CmzString(st.wYear, FALSE, 4));
    }
    if (retval.strpos(L"m") != -1) {
        retval.replace(L"m", CmzString(st.wMonth, FALSE, 2));
    }
    if (retval.strpos(L"d") != -1) {
        retval.replace(L"d", CmzString(st.wDay, FALSE, 2));
    }

    return retval;
}

}
