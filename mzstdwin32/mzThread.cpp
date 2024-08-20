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
#include "pch.h"
#include "mzstdwin32Common.h"
#include "mzThread.h"
using namespace mzstd;

VOID CThread::_commonInit(mzstd::CmzMap* threadData) {
    this->setRealClassName(L"CThread");
    if (threadData) {
        this->_data = threadData->clone();
    } else {
        this->_data = new mzstd::CmzMap();
    }
    this->_handle = NULL;
    this->_id = 0;
}

CThread::CThread() {
    this->_commonInit(NULL);
}

CThread::CThread(mzstd::CmzMap* threadData) {
    this->_commonInit(threadData);
}

CThread::~CThread() {
    if (this->_data) {
        delete this->_data;
    }
}

VOID CThread::start() {
    this->_handle = CreateThread(NULL, 0, CThread::ThreadProc, this, 0, &this->_id);
}

DWORD WINAPI CThread::ThreadProc(LPVOID lpParameter) {
    if (lpParameter) {
        CThread* thread = (CThread*)lpParameter;
        thread->run();
        CloseHandle(thread->_handle);
        thread->_handle = NULL;
        thread->_id = 0;
        return 1;
    }
    
    return 0;
}


DWORD WINAPI Wait(DWORD MilliSeconds) {
	DWORD dw;
	HANDLE hTimer;
	LARGE_INTEGER DueTime;
	LONGLONG MilliSecond=-10000;
    DueTime.QuadPart = MilliSeconds * MilliSecond;
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!SetWaitableTimer(hTimer, &DueTime, 0, NULL, NULL, FALSE)) {
	    CloseHandle(hTimer);
	    return WAIT_FAILED;
	}
    dw = WaitForSingleObject(hTimer, 10000);
    CloseHandle(hTimer);
    return dw;
}
