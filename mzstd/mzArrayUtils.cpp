#include "pch.h"
#include "mzArrayUtils.h"

namespace mzstd {

BOOL WINAPI in_array(BYTE *arr, ULONGLONG arr_size, BYTE elem) {
	for (ULONGLONG i = 0; i < arr_size; i++) {
		if (arr[i] == elem) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL WINAPI in_array(WORD *arr, ULONGLONG arr_size, WORD elem) {
	for (ULONGLONG i = 0; i < arr_size; i++) {
		if (arr[i] == elem) {
			return TRUE;
		}
	}
	return FALSE;
}

}
