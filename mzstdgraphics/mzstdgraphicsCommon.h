#ifndef __MZSTDGRAPHICS_COMMON_H__
#define __MZSTDGRAPHICS_COMMON_H__

#include "mzstdCommon.h"


// if we compile dll based project
#ifdef MZSTDGRAPHICS_DLL
    // if we compile dll library
    #ifdef MZSTDGRAPHICS_INTERNAL
        #define DECLSPEC_MZSTDGRAPHICS __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDGRAPHICS __declspec(dllimport)
    #endif
#endif

// if we compile static lib based project
#ifdef MZSTDGRAPHICS_STATIC
    #ifdef MZSTDGRAPHICS_INTERNAL
        #define DECLSPEC_MZSTDGRAPHICS __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDGRAPHICS
    #endif
#endif

#ifndef DECLSPEC_MZSTDGRAPHICS
    #define DECLSPEC_MZSTDGRAPHICS __declspec(dllimport)
#endif



#endif
