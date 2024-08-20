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

#ifndef __MZSTDGUI_WINDOW_H__
#define __MZSTDGUI_WINDOW_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"
#include "mzWindowClass.h"
#include "mzBitmap.h"
#include "mzMap.h"
#include "mzStdObject.h"
#include "mzMenu.h"

class CApplication;
class CScrollbar;


class DECLSPEC_MZSTDGUI CWindow : public mzstd::CmzObject {
public:
    friend class CApplication;

    
    // prostokąt obszaru klienta okna (lewy górny róg ma współrzędne
    // (0,0) prawy dolny róg (clientWidth,clientHeight))
    RECT rcClient;
    // prostokąt obszaru całego okna (lewy górny róg ma współrzędne
    // (0,0) prawy dolny róg (width,height))
    RECT rcWindow;
    // lewy górny róg okna liczony względem całego ekranu
    POINT windowOrigin;
    // lewy górny róg części klienckiej okna liczony względem całego
    // ekranu
    POINT clientOrigin;
    // szerokość niewidocznej części okna. Różnica pomiędzy
    // współrzędymi windowRect.top a clientRect.top
    LONG hiddenHeight;

    // Atrybut informujący czy to okienko jest oknem głównym. Za okno główne uważane jest
    // pierwsze okno które zostało zarejestrowane w obiekcie aplikacji.
    BOOL isMainWindow;

    
    WINAPI CWindow(CApplication *app, CWindow* parentWindow, mzstd::CmzString name, CWindowClass *wc = NULL, HWND hWnd = NULL, DWORD exStyle = WS_EX_APPWINDOW, DWORD STYLE = WS_OVERLAPPEDWINDOW);
    virtual WINAPI ~CWindow();

    LONG inline WINAPI clientWidth() { return this->rcClient.right - this->rcClient.left; }
    LONG inline WINAPI clientHeight() { return this->rcClient.bottom - this->rcClient.top; }
    LONG inline WINAPI width() { return this->rcWindow.right - this->rcWindow.left; }
    LONG inline WINAPI height() { return this->rcWindow.bottom - this->rcWindow.top; }
    inline mzstd::CmzString WINAPI getName() { return this->_name; }
    VOID WINAPI setText(mzstd::CmzString caption);
    mzstd::CmzString WINAPI getText();
    // function for setting and getting window size
    virtual VOID WINAPI setSize(LONG width, LONG height);
    virtual VOID WINAPI setPosition(LONG x, LONG y);
    SIZE WINAPI getSize();
    
    BOOL WINAPI isActive();
    BOOL WINAPI isVisible();
    BOOL WINAPI isZoomed();
    // isIconic and isMinimized are the same
    BOOL WINAPI isIconic();
    // isIconic and isMinimized are the same
    BOOL WINAPI isMinimized();
    
    VOID WINAPI invalidate();
	inline HWND WINAPI getHwnd() { return this->_hWnd; }
    inline CApplication* WINAPI getApplication() { return this->_app; }
    inline VOID WINAPI show() { ShowWindow(this->_hWnd, SW_SHOW); }
    inline VOID WINAPI showModal();
    inline VOID WINAPI hide() { ShowWindow(this->_hWnd, SW_HIDE); }
    VOID WINAPI close();
    virtual VOID WINAPI createGuiLayout();
    // Funkcja zwraca bieżącą pozycję kursora względem lewego górnego narożnika okna. Zwraca
    // dokładnie te same wartości, które dostarczane są w zdarzenia WM_MOUSE...
    virtual VOID WINAPI getCurrentMousePosition(LONG& x, LONG& y);
    VOID WINAPI activateWindow();
    
    // funkcje obsługi zdarzeń
    virtual VOID WINAPI onDraw(HDC hdc);
    virtual LRESULT WINAPI onNChitTest(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onSize(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onSizing(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onGetMinMaxInfo(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onMouseMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onNCLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onNCLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onNCLButtonDblClk(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onEraseBkgnd(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onExitSizeMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual LRESULT WINAPI onWmShowWindow(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onWmUserEvent(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, message, wParam, lParam); }
    virtual VOID WINAPI onTimer(ULONG uID, ULONG curTickCount) {}
    virtual VOID WINAPI onActivation() {}
    virtual VOID WINAPI onDeactivation() {}

    virtual VOID WINAPI onMenuItemClicked(mzstd::CmzStdObject *item) {};
    virtual VOID WINAPI onToolbarItemClicked(mzstd::CmzStdObject *item) {};
    // on click event for Buttons
    virtual VOID WINAPI onButtonClicked(mzstd::CmzStdObject *item) {};
    // on click event for all gui controls except Button
    virtual VOID WINAPI onGuiControlClicked(mzstd::CmzStdObject *item) {};
    // Method onGetEditedValues is called after child window is closed by button of name:
    // DEFAULT_BTN_OK or DEFAULT_BTN_CANCEL
    // In first case we get as a parameter map CmzMap object of type: mzstd::CmzString (name) => mzstd::CmzString (value)
    //   After processing map object passed to the function should not be deleted because it is done automatically
    // In second case we get map=NULL.
    virtual VOID WINAPI onGetEditedValues(mzstd::CmzMap* map) {}

    // Funkcja wywoływana przez scrollbar który jest podczepiony do tego obiektu w momencie zmiany
    // jego pozycji (tj położenia suwaka).
    virtual VOID WINAPI onThumbChange(LONG oldPosition, LONG curPosition, CScrollbar* scrollbar) { return; }

    virtual VOID WINAPI allowResizing(BOOL value) { this->_allowResizing = value; }
    BOOL WINAPI isResizable() { return this->_allowResizing; }
    virtual VOID WINAPI centerOnScreen();
    virtual VOID WINAPI getCenteredOnScreenPosition(LONG* x, LONG* y);
    virtual VOID WINAPI onFirstShow();
    VOID WINAPI setIsActive(BOOL value);

    inline CWindowClass* WINAPI getWindowClass() { return this->_wc; }
    inline VOID WINAPI setIsModal(BOOL value) { this->_isModal = value; }
    inline BOOL WINAPI isModal() { return this->_isModal; }
    VOID WINAPI setBlur(BOOL value);
    inline BOOL WINAPI isBlur() { return this->_isBlur; }

protected:
    HWND _hWnd;
    CWindow* _parentWindow;
    BOOL _isActive;
    CmzBitmap *_canvas;
    CApplication *_app;
    CWindowClass *_wc;
    mzstd::CmzString _name;
    DWORD _exStyle;
    DWORD _style;

    // atrybut _needUpdate określa czy w metodzie onDraw ma być wywołana metoda
    // metoda _updateCanvas odpowiedzialna za przerysowanie canvasu
    BOOL _needUpdate;
    // TRUE if window is resizable (default)
    BOOL _allowResizing;
    // below attribute is TRUE until first display window on screen
    BOOL _firstShow;
    // if window is modal window
    BOOL _isModal;
    // if window is blur (mainly in case there are some modal windows opened after this window has been showed)
    BOOL _isBlur;

    WINAPI CWindow(CWindow* parentWindow = NULL);
    BOOL WINAPI _create(LONG cmdShow, DWORD exStyle = WS_EX_APPWINDOW, DWORD style = WS_OVERLAPPEDWINDOW);
    VOID WINAPI _initWindowClass(CWindowClass *wc = NULL);
    VOID WINAPI _init();
    VOID WINAPI _setWindowParameters();
    VOID WINAPI _commonInit();
private:
};

typedef CWindow* PWindow;

#endif
