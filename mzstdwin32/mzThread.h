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

#ifndef __MZSTDWIN32_THREAD_H__
#define __MZSTDWIN32_THREAD_H__

#include "mzstdwin32Common.h"
#include "mzObject.h"
#include "mzMap.h"

class DECLSPEC_MZSTDWIN32 CThread : public mzstd::CmzObject {
public:
    WINAPI CThread();
    WINAPI CThread(mzstd::CmzMap* threadData);
	virtual WINAPI ~CThread();
    
    VOID WINAPI start();
    virtual VOID WINAPI run() {};

    inline mzstd::CmzMap* WINAPI getData() { return this->_data; }
    inline HANDLE WINAPI getHandle() { return this->_handle; }
    inline DWORD WINAPI getId() { return this->_id; }
    
protected:
    mzstd::CmzMap* _data;
    HANDLE _handle;
    DWORD _id;
    
    VOID WINAPI _commonInit(mzstd::CmzMap* threadData);
    
    static DWORD WINAPI ThreadProc(LPVOID lpParameter);
};
typedef CThread* PThread;


DECLSPEC_MZSTDWIN32 DWORD WINAPI Wait(DWORD MilliSeconds);

#endif
