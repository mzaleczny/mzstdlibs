#include "pch.h"
#include "mzstdsystemCommon.h"
#include "mzSystemParameters.h"

namespace mzstd {

NONCLIENTMETRICS WINAPI GetNonClientMetrics() {
	NONCLIENTMETRICS ncm;
    memset(&ncm, 0, sizeof(ncm));
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

	return ncm;
}

DWORD WINAPI GetAllocationGranularity() {
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(si));
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
}

VOID WINAPI GetSystemWindowFrameSize(LONG& cxSizeFrame, LONG& cySizeFrame) {
    cxSizeFrame = GetSystemMetrics(SM_CXSIZEFRAME);
    cySizeFrame = GetSystemMetrics(SM_CYSIZEFRAME);
}

VOID WINAPI GetXPTaskbarSize(LONG& width, LONG& height) {
    HWND hwnd = FindWindowW(L"Shell_TrayWnd", NULL);
    RECT rc;
    GetWindowRect(hwnd, &rc);
    width = rc.right - rc.left;
    height = rc.bottom - rc.top;
}

}
