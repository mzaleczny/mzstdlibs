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
#include "mzCaret.h"
#include "mzstdguiGlobals.h"
#include "mzStyledWindow.h"
using namespace mzstd;

CCaret::CCaret(CStyledWindow* wnd, LONG x, LONG y, LONG width, LONG height) {
    this->setRealClassName(L"CCaret");
    this->parentWindow = wnd;
    this->_blinkingDelay = 500;
    this->_lastFlippedTime = 0;
    this->_visible = FALSE;
    this->active = FALSE;
    this->width = width;
    this->height = height;
    this->_color = RGB(0x00, 0x00, 0x00);
    this->_hBrush = CreateSolidBrush(this->_color);
}

CCaret::~CCaret() {
    if (this->_hBrush) {
        DeleteObject(this->_hBrush);
    }
}

VOID CCaret::draw(HDC hdc) {
    if (!this->active || !this->parentWindow) {
        return;
    }

    if (this->_visible) {
        RECT rc = this->getRect();
        FillRect(hdc, &rc, this->_hBrush);
    } else {
        if (this->parentWindow->getActiveControl()) {
            CmzBitmap* bmpSrc = this->parentWindow->getActiveControl()->canvas;
            if (bmpSrc) {
                HDC hdcSrc = bmpSrc->getDC();
                BOOL beginPaintCalled = FALSE;
                if (!hdcSrc) {
                    HDC hdcScreen = GetDC(NULL);
                    bmpSrc->beginPaint(hdc);
                    hdcSrc = bmpSrc->getDC();
                    ReleaseDC(NULL, hdcScreen);
                    beginPaintCalled = TRUE;
                }
                BitBlt(hdc, this->x, this->y, this->width, this->height, hdcSrc, this->controlX, this->controlY, SRCCOPY);
                if (beginPaintCalled) {
                    bmpSrc->endPaint();
                }
            }
        }
    }
}

VOID CCaret::update(ULONG time) {
    LONG delta = time - this->_lastFlippedTime;
    if (delta > this->_blinkingDelay || delta < 0) {
        this->_lastFlippedTime = time;
        this->_visible = !this->_visible;
    }
}
