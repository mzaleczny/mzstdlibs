#ifndef __MZSTDSYSTEM_SYSTEM_PARAMETERS_H
#define __MZSTDSYSTEM_SYSTEM_PARAMETERS_H

#include "mzstdsystemCommon.h"

namespace mzstd {

DECLSPEC_MZSTDSYSTEM NONCLIENTMETRICS WINAPI GetNonClientMetrics();
DECLSPEC_MZSTDSYSTEM DWORD WINAPI GetAllocationGranularity();
DECLSPEC_MZSTDSYSTEM VOID WINAPI GetSystemWindowFrameSize(LONG& cxSizeFrame, LONG& cySizeFrame);
DECLSPEC_MZSTDSYSTEM VOID WINAPI GetXPTaskbarSize(LONG& width, LONG& height);

}

#endif
