/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdshell
 * Version: 1.04
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdshell library
 * 
 * mzstdshell library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdshell library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZSTDSHELL_SHELL_H__
#define __MZSTDSHELL_SHELL_H__

#include "mzstdshellCommon.h"
#include "mzObject.h"

class DECLSPEC_MZSTDSHELL CShell : public CObject {
public:
	WINAPI CShell() {};
    virtual WINAPI ~CShell() {};
};
typedef CShell* PShell;

#endif
