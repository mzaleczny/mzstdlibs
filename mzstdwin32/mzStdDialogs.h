/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdwin32
 * Version: 1.05
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdwin32 library
 * 
 * mzstdwin32 library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdwin32 library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZSTDWIN32_STDDIALOGS_H__
#define __MZSTDWIN32_STDDIALOGS_H__

#include "mzstdwin32Common.h"
#include "mzString.h"
#include <CommDlg.h>

DECLSPEC_MZSTDWIN32 OPENFILENAME* WINAPI CreateOpenFileNameStruct(WCHAR* caption, mzstd::CmzString initialDir = COMMON_EMPTY_STRING, mzstd::CmzString filter = L"All files (*.*)", mzstd::CmzString defExt = L"*", DWORD filterIndex = 0);
DECLSPEC_MZSTDWIN32 VOID WINAPI DeleteOpenFileNameStruct(OPENFILENAME* ofn);
DECLSPEC_MZSTDWIN32 mzstd::CmzString WINAPI CheckIfDialogError();
DECLSPEC_MZSTDWIN32 mzstd::CmzString WINAPI FileOpenDlg(mzstd::CmzString initialDir = COMMON_EMPTY_STRING, mzstd::CmzString filter = L"All files (*.*)", mzstd::CmzString defExt = L"*", DWORD filterIndex = 0);
DECLSPEC_MZSTDWIN32 mzstd::CmzString WINAPI FileSaveDlg(mzstd::CmzString initialDir = COMMON_EMPTY_STRING, mzstd::CmzString filter = L"All files (*.*)", mzstd::CmzString defExt = L"*", DWORD filterIndex = 0);

#endif
