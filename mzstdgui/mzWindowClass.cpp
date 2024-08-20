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

#include "pch.h"
#include "mzWindowClass.h"
#include "mzstdguiGlobals.h"
#include "mzResource.h"
#include "mzApplication.h"
#include "mzMessageBox.h"
#include "mzMessages.h"
#include "mzStyledWindow.h"
#include "mzLogger.h"
#include "mzFileUtils.h"
using namespace mzstd;

CWindowClass::CWindowClass(CApplication *app, CmzString className)
    : _app(app), className(className)
{
    this->setRealClassName(L"CWindowClass");
    if (this->_app) {
        this->_initDefault();
    }
}

CWindowClass::~CWindowClass() {
}

VOID CWindowClass::_initDefault() {
	this->_wcex.cbSize = sizeof(WNDCLASSEX);

	this->_wcex.style            = CS_HREDRAW | CS_VREDRAW;
    this->_wcex.lpfnWndProc      = CWindowClass::DefaultWndProc;
	this->_wcex.cbClsExtra       = 0;
	this->_wcex.cbWndExtra		= 0;
    this->_wcex.hInstance		= this->_app->getInstance();
    this->_wcex.hIcon			= NULL;
    if (this->_app->resources && this->_app->resources->appIcon) {
        this->_wcex.hIcon = this->_app->resources->appIcon->hIcon;
    }
	this->_wcex.hCursor		    = LoadCursor(NULL, IDC_ARROW);
	this->_wcex.hbrBackground    = (HBRUSH)(COLOR_WINDOW);
	this->_wcex.lpszMenuName     = NULL;
    this->_wcex.lpszClassName	= this->className.getBuffer();
	this->_wcex.hIconSm		    = NULL;
    if (this->_app->resources && this->_app->resources->appSmallIcon) {
        this->_wcex.hIconSm = this->_app->resources->appSmallIcon->hIcon;
    }
}

BOOL CWindowClass::_registerClass() {
    BOOL retval = RegisterClassEx(&this->_wcex);
    if (!retval) {
        //ShowDetailedLastSystemError(L"CWindowClass::_registerClass: RegisterClassEx failed\n");
    }
    return retval;
}

BOOL CWindowClass::_unregisterClass() {
    if (this->_app) {
        return UnregisterClass(this->_wcex.lpszClassName, this->_app->getInstance());
    }
    return FALSE;
}

LRESULT CALLBACK CWindowClass::DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
    CApplication* app = (CApplication*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    CWindow* wnd = NULL;
    CStyledWindow* styledWnd = NULL;
    CWindow* curModalWindow = NULL;

    if (app) {
        wnd = app->getRegisteredWindow(hWnd);
        curModalWindow = app->getCurModalWindow();
    }

    if (wnd && wnd->isBlur() &&
            (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP || message == WM_LBUTTONDBLCLK ||
             message == WM_RBUTTONDOWN || message == WM_RBUTTONUP || message == WM_RBUTTONDBLCLK ||
             message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONUP || message == WM_NCLBUTTONDBLCLK ||
             message == WM_NCRBUTTONDOWN || message == WM_NCRBUTTONUP || message == WM_NCRBUTTONDBLCLK ||
             message == WM_MOUSEACTIVATE || message == WM_MOUSEHOVER || message == WM_MOUSELEAVE ||
             message == WM_MOUSEMOVE || message == WM_NCHITTEST)) {
        return 0;
    }
    
	switch (message)
	{
    case WM_ACTIVATE:
        if (app && wnd) {
            if (curModalWindow && curModalWindow != wnd) {
                SetForegroundWindow(curModalWindow->getHwnd());
            }
            wnd->setIsActive(wParam != WA_INACTIVE);
        }        
        break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
        if (app && wnd && wnd->isVisible()) {
            wnd->onDraw(hdc);
        }
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
        if (app && wnd) {
            // mark not to call DestroyWindow system function in CWindow destructor, because it is done by system
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            delete wnd;
        }
		break;
    case WM_MOVE:
        if (wnd) {
            return wnd->onMove(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SIZE:
        if (wnd) {
            return wnd->onSize(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SIZING:
        if (wnd) {
            return wnd->onSizing(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCHITTEST:
        if (app && wnd) {
            return wnd->onNChitTest(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_GETMINMAXINFO:
        if (app && wnd) {
            return wnd->onGetMinMaxInfo(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_LBUTTONDOWN:
        if (app && wnd) {
            return wnd->onLButtonMouseDown(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_LBUTTONUP:
        if (app && wnd) {
            return wnd->onLButtonMouseUp(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCLBUTTONDOWN:
        if (app && wnd) {
            return wnd->onNCLButtonMouseDown(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCLBUTTONUP:
        if (app && wnd) {
            return wnd->onNCLButtonMouseUp(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCLBUTTONDBLCLK:
        if (app && wnd) {
            return wnd->onNCLButtonDblClk(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MOUSEMOVE:
        if (app && wnd) {
            return wnd->onMouseMove(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ERASEBKGND:
        if (app && wnd) {
            return wnd->onEraseBkgnd(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_EXITSIZEMOVE:
        if (app && wnd) {
            return wnd->onExitSizeMove(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SHOWWINDOW:
        if (app && wnd) {
            return wnd->onWmShowWindow(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MENUITEM_CLICKED:
    case WM_TOOLBARITEM_CLICKED:
    case WM_BUTTON_CLICKED:
    case WM_GUICONTROL_CLICKED:
        if (app && wnd) {
            return wnd->onWmUserEvent(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK CWindowClass::AboutDlgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    return 0;
}
