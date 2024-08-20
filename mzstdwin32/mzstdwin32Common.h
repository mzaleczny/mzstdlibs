#ifndef __MZSTDWIN32_COMMON_H__
#define __MZSTDWIN32_COMMON_H__

#include "mzstdCommon.h"


// if we compile dll based project
#ifdef MZSTDWIN32_DLL
    // if we compile dll library
    #ifdef MZSTDWIN32_INTERNAL
        #define DECLSPEC_MZSTDWIN32 __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDWIN32 __declspec(dllimport)
    #endif
#endif

// if we compile static lib based project
#ifdef MZSTDWIN32_STATIC
    #ifdef MZSTDWIN32_INTERNAL
        #define DECLSPEC_MZSTDWIN32 __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDWIN32
    #endif
#endif

#ifndef DECLSPEC_MZSTDWIN32
    #define DECLSPEC_MZSTDWIN32 __declspec(dllimport)
#endif


#endif
