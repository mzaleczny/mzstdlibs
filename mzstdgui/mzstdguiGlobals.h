/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdgui
 * Version: 1.07
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdgui library
 * 
 * mzstdgui library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdgui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZSTDGUI_GLOBALS_H__
#define __MZSTDGUI_GLOBALS_H__

#ifndef __MZSTDGUI_COMMON_H__
    #include "mzstdguiCommon.h"
#endif

#include "mzStringList.h"

class CApplication;
extern CApplication* application;


DECLSPEC_MZSTDGUI VOID WINAPI initMzstdguiLibrary();
DECLSPEC_MZSTDGUI VOID WINAPI cleanupMzstdguiLibrary();
DECLSPEC_MZSTDGUI CApplication* WINAPI getApplication();
DECLSPEC_MZSTDGUI VOID WINAPI EditValues(WCHAR* windowTitle, mzstd::CmzStringList* labels, mzstd::CmzStringList* initialValues);

#endif
