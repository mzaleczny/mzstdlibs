/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdmathext
 * Version: 1.05
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdmathext library
 * 
 * mzstdmathext library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdmathext library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZSTDMATHEXT_COMMON_H__
#define __MZSTDMATHEXT_COMMON_H__

#include "mzstdCommon.h"


// if we compile dll based project
#ifdef MZSTDMATHEXT_DLL
    // if we compile dll library
    #ifdef MZSTDMATHEXT_INTERNAL
        #define DECLSPEC_MZSTDMATHEXT __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDMATHEXT __declspec(dllimport)
    #endif
#endif

// if we compile static lib based project
#ifdef MZSTDMATHEXT_STATIC
    #ifdef MZSTDMATHEXT_INTERNAL
        #define DECLSPEC_MZSTDMATHEXT __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTDMATHEXT
    #endif
#endif

#ifndef DECLSPEC_MZSTDMATHEXT
    #define DECLSPEC_MZSTDMATHEXT __declspec(dllimport)
#endif



#endif
