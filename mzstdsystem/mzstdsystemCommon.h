#ifndef __MZSTDSYSTEM_COMMON_H__
#define __MZSTDSYSTEM_COMMON_H__

#include "mzstdCommon.h"


// if we compile dll based project
#ifdef MZSTDSYSTEM_DLL
    // if we compile dll library
    #ifdef MZSTDSYSTEM_INTERNAL
        #define DECLSPEC_MZSTDSYSTEM __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDSYSTEM __declspec(dllimport)
    #endif
#endif

// if we compile static lib based project
#ifdef MZSTDSYSTEM_STATIC
    #ifdef MZSTDSYSTEM_INTERNAL
        #define DECLSPEC_MZSTDSYSTEM __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDSYSTEM
    #endif
#endif

#ifndef DECLSPEC_MZSTDSYSTEM
    #define DECLSPEC_MZSTDSYSTEM __declspec(dllimport)
#endif


#endif
