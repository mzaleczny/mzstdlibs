#ifndef __MZSTD_ARRAY_UTILS_H__
#define __MZSTD_ARRAY_UTILS_H__

#include "mzstdCommon.h"

namespace mzstd {

DECLSPEC_MZSTD BOOL WINAPI in_array(BYTE *arr, ULONGLONG arr_size, BYTE elem);
DECLSPEC_MZSTD BOOL WINAPI in_array(WORD *arr, ULONGLONG arr_size, WORD elem);

}

#endif
