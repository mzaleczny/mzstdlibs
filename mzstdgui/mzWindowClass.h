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

#ifndef __MZSTDGUI_WINDOW_CLASS_H__
#define __MZSTDGUI_WINDOW_CLASS_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"
#include "mzString.h"

class CApplication;
class CWindow;
class CStyledWindow;

class DECLSPEC_MZSTDGUI CWindowClass : public mzstd::CmzObject {
public:
    friend class CApplication;
    friend class CWindow;
    friend class CStyledWindow;
    mzstd::CmzString className;

	WINAPI CWindowClass(CApplication *app, mzstd::CmzString className = L"MZSTD_STYLED_WINDOW");
	virtual WINAPI ~CWindowClass();

    static LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK AboutDlgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    inline WNDCLASSEX* WINAPI getClass() { return &this->_wcex; }

protected:
	WNDCLASSEX _wcex;
    CApplication *_app;

	VOID WINAPI _initDefault();
    BOOL WINAPI _registerClass();
    BOOL WINAPI _unregisterClass();
private:
};

typedef CWindowClass* PWindowClass;

#endif
