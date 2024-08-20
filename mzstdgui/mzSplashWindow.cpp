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
#include "mzSplashWindow.h"
#include "mzstdguiGlobals.h"
#include "mzApplication.h"
#include "mzMessageBox.h"
#include "mzTheme.h"
#include "mzWindowClass.h"
#include "mzSprite.h"
#include "mzPanel.h"
#include "mzScreen.h"
#include "mzFileUtils.h"
using namespace mzstd;

WINAPI CSplashWindow::CSplashWindow(CApplication *app, CWindow* parentWindow, CmzString name, CmzString bitmapFileName, CWindowClass *wc, HWND hWnd)
    : CStyledWindow(app, parentWindow, name, wc, hWnd)
{
    this->setRealClassName(L"CSplashWindow");
    if (FileExists(bitmapFileName)) {
        this->_bitmapFilename = bitmapFileName;
        CmzBitmap* splashBitmap = new CmzBitmap(this->_app->getInstance(), this->_bitmapFilename);
        if (splashBitmap) {
            this->setSize(splashBitmap->width(), splashBitmap->height());
            this->centerOnScreen();
            delete splashBitmap;
        }
    }
}

WINAPI CSplashWindow::~CSplashWindow() {
}

// Funkcje obsługi zdarzeń
VOID WINAPI CSplashWindow::onDraw(HDC hdc) {
    if (!this->_canvas) {
        return;
    }

    if (this->_needUpdate) {
        this->_updateCanvas();
    }

    this->_canvas->beginPaint(hdc);
    this->_canvas->bitblt(hdc, 0, 0);
    this->_canvas->endPaint();
}

VOID WINAPI CSplashWindow::_updateCanvas() {
    if (this->_bitmapFilename == COMMON_EMPTY_STRING) {
        return;
    }
    if (!this->_canvas || !this->isVisible() || this->isIconic()) {
		return;
    }
    this->_needUpdate = FALSE;

    CmzBitmap* splashBitmap = NULL;
    if (this->_bitmapFilename != COMMON_EMPTY_STRING) {
        splashBitmap = new CmzBitmap(this->_app->getInstance(), this->_bitmapFilename);
        if (splashBitmap) {
            this->setSize(splashBitmap->width(), splashBitmap->height());
            this->centerOnScreen();
        } else {
            return;
        }
    }
    
    ULONG x = 0;
    ULONG y = 0;
    //RECT rc;
    CTheme *t = this->_app->getTheme();
    if (!t) {
        return;
    }
    HDC hdc = GetDC(0);
        
    this->_canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy splasha
    // ================================================================
    this->_canvas->drawBitmap(splashBitmap, x, y);
    // ================================================================
    // Koniec rysowania splasha
    // ================================================================

    delete splashBitmap;
    
    this->_canvas->endPaint();
    ReleaseDC(0, hdc);
}
