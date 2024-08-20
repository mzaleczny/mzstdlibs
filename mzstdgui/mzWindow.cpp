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
#include "mzWindow.h"
#include "mzstdguiGlobals.h"
#include "mzApplication.h"
#include "mzMenu.h"
#include "mzStdObject.h"
#include "mzMessageBox.h"
#include "mzScreen.h"
#include "mzKeyboard.h"
#include "mzObjectList.h"
using namespace mzstd;

VOID CWindow::_commonInit() {
    this->setRealClassName(L"CWindow");
    this->_isActive = TRUE;
    this->_wc = NULL;
    this->_canvas = NULL;
    this->isMainWindow = FALSE;
    this->_needUpdate = FALSE;
    this->_isModal = FALSE;
    this->_isBlur = FALSE;
}

CWindow::CWindow(CApplication *app, CWindow* parentWindow, CmzString name, CWindowClass *wc, HWND hWnd, DWORD exStyle, DWORD style)
    : _app(app), _parentWindow(parentWindow), _name(name), _exStyle(exStyle), _style(style), _allowResizing(FALSE), _firstShow(TRUE)
{
    this->_commonInit();
    if (!this->_app) return;

    if (!hWnd) {
        this->_initWindowClass(wc);
        if (this->_wc) {
            this->_create(SW_HIDE, this->_exStyle, this->_style);
        }
    }

    this->_setWindowParameters();
}

CWindow::CWindow(CWindow* parentWindow)
    : _app(NULL), _parentWindow(parentWindow), _exStyle(WS_EX_APPWINDOW), _style(WS_OVERLAPPEDWINDOW), _firstShow(TRUE)
{
    this->_commonInit();
}

VOID CWindow::_setWindowParameters() {
    if (this->_hWnd) {
        WINDOWINFO wi;
        GetWindowInfo(this->_hWnd, &wi);

        POINT p;
        p.x = wi.rcWindow.left;
        p.y = wi.rcWindow.top;
        this->windowOrigin = p;

        p.x = wi.rcClient.left;
        p.y = wi.rcClient.top;
        this->clientOrigin = p;

        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = wi.rcClient.right - wi.rcClient.left;
        rc.bottom = wi.rcClient.bottom - wi.rcClient.top;
        this->rcClient = rc;

        rc.left = 0;
        rc.top = 0;
        rc.right = wi.rcWindow.right - wi.rcWindow.left;
        rc.bottom = wi.rcWindow.bottom - wi.rcWindow.top;
        this->rcWindow = rc;

        this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
    }
}

VOID CWindow::_initWindowClass(CWindowClass *wc) {
    BOOL localWCCreated = FALSE;

    this->_wc = NULL;

    // jeśli nie podano klasy okna
    if (!wc) {
        // to tworzymy domyślną klasę
        wc = new CWindowClass(this->_app);
        if (!wc) return;
        // i jeśli się ją udało utworzyć, to zaznaczamy, że została
        // stworzona lokalnie
        localWCCreated = TRUE;
    }

    // próbujemy zarejestrować klasę
    if (!this->_app->registerClass(wc)) {
        // jeśli się nie udało, to prawdopodobnie taka klasa
        // jest już zarejestrowana, więc pobieramy klasę o tej
        // nazwie
        this->_wc = this->_app->getRegisteredClass(wc->className);
        // jeśli klasa wc została stworzona lokalnie, to
        // zwalniamy przydzielone jej zasoby
        if (localWCCreated) {
            delete wc;
        }
    } else {
        this->_wc = wc;
    }
}

CWindow::~CWindow() {
    if (this->_isModal && this->_app->unModal(this)) {
        this->_isModal = FALSE;
    }
    
    if (this->_parentWindow) {
        SetForegroundWindow(this->_parentWindow->_hWnd);
    }

    // wskaźników this->wc nie zwalniamy, bo tym się zajmuje
    // klasa CApplication!!!
    if (this->_app) {
        this->_app->unregisterWindow(this);
    }

    // we are avoiding subsequent try to destroy window if window was closed by 'Close Button'
    // if window was destroyed by calling delete on CWindow pointer, then we do destroying it
    if (GetWindowLongPtr(this->_hWnd, GWLP_USERDATA) != 0) {
        DestroyWindow(this->_hWnd);
    }

    if (this->_canvas) {
        DeleteObject(this->_canvas);
        this->_canvas = NULL;
    }
}

BOOL CWindow::_create(LONG cmdShow, DWORD exStyle, DWORD style) {
    this->_hWnd = CreateWindowEx(
        exStyle,
        this->_wc->className.getBuffer(),
        this->_name.getBuffer(),
        style,
        CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0,
        NULL, NULL,
        this->_app->getInstance(), NULL);

    if (!this->_hWnd) {
        return FALSE;
    }

    this->_app->registerWindow(this);
    // zapisujemy w parametrach użytkownika okna wskaźnik do obiektu
    // aplikacji
    SetWindowLongPtr(this->_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this->_app));
    ShowWindow(this->_hWnd, cmdShow);
    UpdateWindow(this->_hWnd);

    return TRUE;
}

LRESULT CWindow::onMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    /*
    WINDOWINFO wi;
    GetWindowInfo(this->_hWnd, &wi);

    POINT p;
    p.x = wi.rcWindow.left;
    p.y = wi.rcWindow.top;
    this->windowOrigin = p;

    p.x = wi.rcClient.left;
    p.y = wi.rcClient.top;
    this->clientOrigin = p;
    */
    this->_setWindowParameters();
    return 0;
    //return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CWindow::onSize(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    this->_setWindowParameters();
    return 0;
}

LRESULT CWindow::onSizing(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    this->_setWindowParameters();
    return TRUE;
}

VOID CWindow::setText(CmzString caption) {
    SendMessage(this->_hWnd, WM_SETTEXT, 0, (LPARAM)caption.getBuffer());
}

CmzString CWindow::getText() {
    LONG buf_size = 255;
    WCHAR* buf = new WCHAR[buf_size];
    if (buf) {
        buf[0] = NULL;
        SendMessage(this->_hWnd, WM_GETTEXT, (WPARAM)buf_size, (LPARAM)buf);
        CmzString s(buf, buf_size);
        return s;
    }
    return COMMON_EMPTY_STRING;
}

VOID WINAPI CWindow::getCurrentMousePosition(LONG& x, LONG& y) {
    POINT p;
    if (GetCursorPos(&p)) {
        x = p.x - this->windowOrigin.x;
        y = p.y - this->windowOrigin.y;
    } else {
        x = 0;
        y = 0;
    }
}

VOID WINAPI CWindow::setSize(LONG width, LONG height) {
    SetWindowPos(this->_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    this->_setWindowParameters();
    this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
}

SIZE WINAPI CWindow::getSize() {
    SIZE size;
    size.cx = this->width();
    size.cy = this->height();
    return size;
}

VOID WINAPI CWindow::centerOnScreen() {
    LONG x, y;

    this->getCenteredOnScreenPosition(&x, &y);
    SetWindowPos(this->_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
	this->invalidate();
}

VOID WINAPI CWindow::getCenteredOnScreenPosition(LONG* x, LONG* y) {
    CScreen* screen = this->_app->screen;
    RECT rc;
    GetWindowRect(this->_hWnd, &rc);
    *x = (screen->width - (rc.right - rc.left)) / 2;
    *y = (screen->height - (rc.bottom - rc.top)) / 2;
}

VOID WINAPI CWindow::createGuiLayout() {
}

VOID CWindow::onDraw(HDC hdc) {
}

VOID CWindow::onFirstShow() {
    this->createGuiLayout();
}

LRESULT CWindow::onWmShowWindow(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    if (wParam) { // if wParam is TRUE then window is being shown
        if (this->_firstShow) {
            this->onFirstShow();
            this->_firstShow = FALSE;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

VOID CWindow::setPosition(LONG x, LONG y) {
    SetWindowPos(this->_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
}

BOOL CWindow::isActive() {
    return this->_isActive;
}

BOOL CWindow::isVisible() {
    return IsWindowVisible(this->_hWnd);
}

BOOL CWindow::isZoomed() {
    return IsZoomed(this->_hWnd);
}

// isIconic and isMinimized are the same
BOOL CWindow::isIconic() {
    return IsIconic(this->_hWnd);
}

// isIconic and isMinimized are the same
BOOL CWindow::isMinimized() {
    return IsIconic(this->_hWnd);
}

VOID CWindow::invalidate() {
    this->_needUpdate = TRUE;
    InvalidateRect(this->_hWnd, &this->rcWindow, FALSE);
}

VOID CWindow::close() {
    DestroyWindow(this->_hWnd);
}

VOID CWindow::activateWindow() {
    this->_isActive = TRUE;
    SetForegroundWindow(this->_hWnd);
}

VOID CWindow::setIsActive(BOOL value) {
    BOOL stateChanged = FALSE;
    if (this->_isActive != value) {
        stateChanged = TRUE;
        // we do this to prevent pressing ENTER in any displayed MessageBox (to close it) cause additional
        // processing of the event in application window that gets active after closing MessageBox.
        this->_app->kbd->reset();
    }
    if (stateChanged) {
        if (value) {
            this->onActivation();
        } else {
            this->onDeactivation();
        }
        this->_isActive = value;
        this->invalidate();
    }
}

VOID CWindow::showModal() {
    if (this->_app->doModal(this)) {
        this->_isModal = TRUE;
    }
    ShowWindow(this->_hWnd, SW_SHOW);
}

VOID CWindow::setBlur(BOOL value) {
    if (this->_isBlur != value) {
        //HDC hdc;
        
        this->_isBlur = value;
        this->invalidate();
        UpdateWindow(this->_hWnd);
    }
}
