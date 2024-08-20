#ifndef __MZSTD_MATH_H__
#define __MZSTD_MATH_H__

#include "mzstdCommon.h"
// Below define is to make defined math constants in math.h in PSDK
#define _USE_MATH_DEFINES
#include <math.h>

namespace mzstd {

DECLSPEC_MZSTD inline LONG WINAPI round(double f_val) {
    return (f_val < 0.0) ? (LONG)(f_val - 0.5) : (LONG)(f_val + 0.5);
}

DECLSPEC_MZSTD ULONG WINAPI random();
DECLSPEC_MZSTD ULONG WINAPI random(ULONG max);
DECLSPEC_MZSTD ULONG WINAPI random(ULONG min, ULONG max);
DECLSPEC_MZSTD ULONGLONG WINAPI random(ULONGLONG min, ULONGLONG max);

}

#endif
