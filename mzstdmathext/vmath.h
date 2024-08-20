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

#ifndef __MZ_VMATH_H__
#define __MZ_VMATH_H__

#include "mzstdmathextCommon.h"
#include "mzObject.h"
#include "mzString.h"

DECLSPEC_MZSTDMATHEXT mzstd::CmzString WINAPI SlowTrzy(LONG i); //Zamienia liczbę trzycyfrową na opisujący ją znak
DECLSPEC_MZSTDMATHEXT mzstd::CmzString WINAPI SlowSzesc(LONG i); //Zamienia liczbę sześciocyfrową na opisujący ją znak
DECLSPEC_MZSTDMATHEXT mzstd::CmzString WINAPI SlowDziewiec(LONG i); //Zamienia liczbę dziewięciocyfrową na opisujący ją znak

#endif
