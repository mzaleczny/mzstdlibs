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

#ifndef __MZSTDGUI_CARET_H__
#define __MZSTDGUI_CARET_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"

class CStyledWindow;

class DECLSPEC_MZSTDGUI CCaret : public mzstd::CmzObject {
public:
    // współrzędne karetki w oknie
    LONG x, y;
    // współrzędne karetki w aktywnej kontrolce
    LONG controlX, controlY;
    LONG width, height;
    // czy karetka jest aktywna, tzn. czy migotanie ma miejsce
    BOOL active;
    CStyledWindow* parentWindow;

    WINAPI CCaret(CStyledWindow* wnd, LONG x, LONG y, LONG width = 2, LONG height = 20);
    virtual WINAPI ~CCaret();

    inline VOID WINAPI resetLastFlippedTime() { this->_lastFlippedTime = 0; }
    virtual VOID WINAPI draw(HDC hdc);
    VOID WINAPI update(ULONG time);
    inline VOID WINAPI setColor(COLORREF color) {
        this->_color = color;
        if (this->_hBrush) {
            DeleteObject(this->_hBrush);
        }
        this->_hBrush = CreateSolidBrush(this->_color);
    }
    virtual inline RECT WINAPI getRect() {
        RECT rc;
        rc.left = this->x;
        rc.top = this->y;
        rc.right = this->x + this->width;
        rc.bottom = this->y + this->height;
        return rc;
    }
    inline VOID WINAPI show() { this->_visible = TRUE; }
    inline VOID WINAPI hide() { this->_visible = FALSE; }
    inline BOOL WINAPI isVisible() { return this->_visible; }

protected:
    // kolor wypełnienia karetki
    HBRUSH _hBrush;
    // szybkość migotania w milisekundach
    LONG _blinkingDelay;
    // czas ostatniej zmiany widoczności karetki
    ULONG _lastFlippedTime;
    // czy karetka jest aktualnie widoczna
    BOOL _visible;
    // kolor karetki
    COLORREF _color;
};

typedef CCaret* PCaret;

#endif
