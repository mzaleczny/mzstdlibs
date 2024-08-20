#include "pch.h"
#include <stdlib.h>
#include "mzstdCommon.h"
#include "mzMath.h"
#include <wchar.h>

namespace mzstd {

DECLSPEC_MZSTD ULONG WINAPI random() {
    return rand();
}

DECLSPEC_MZSTD ULONG WINAPI random(ULONGLONG max) {
    UINT rnd;

    rnd = rand();
    if (max < UINT_MAX) {
        return rnd % (max + 1);
    }
    return rnd;
}

DECLSPEC_MZSTD ULONG WINAPI random(ULONG min, ULONG max) {
    ULONG rnd;

    rnd = rand();
    if (rnd + min <= UINT_MAX) {
        rnd += min;
    }
    if (max < UINT_MAX) {
        return (rnd + min) % (max + 1);
    }
	return rnd;
}

DECLSPEC_MZSTD ULONGLONG WINAPI random(ULONGLONG min, ULONGLONG max) {
    ULONGLONG rnd;

    rnd = rand();
    if (rnd + min <= ULONG_MAX) {
        rnd += min;
    }
    if (max < ULONG_MAX) {
        return (rnd + min) % (max + 1);
    }
    return rnd;
}

}
