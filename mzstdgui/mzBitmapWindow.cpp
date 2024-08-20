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
#include "mzBitmapWindow.h"
#include "mzstdguiGlobals.h"
#include "mzApplication.h"
#include "mzMessageBox.h"
#include "mzTheme.h"
#include "mzWindowClass.h"
#include "mzSprite.h"
#include "mzPanel.h"
#include "mzScreen.h"
#include "mzFileUtils.h"
#include "mzSystemParameters.h"

using namespace mzstd;

WINAPI CmzBitmapWindow::CmzBitmapWindow(CApplication *app, CWindow* parentWindow, CmzString name, CmzString bitmapFileName, CWindowClass *wc, HWND hWnd)
    : CStyledWindow(app, parentWindow, name, wc, hWnd)
{
    this->_allowResizing = FALSE;
    
    this->setRealClassName(L"CmzBitmapWindow");
    if (bitmapFileName[1] != L':') {
        bitmapFileName = this->_app->homeDir + L"\\" + bitmapFileName;
    }
    
    if (FileExists(bitmapFileName)) {
        this->_bitmapFilename = bitmapFileName;
        CmzBitmap* regionBitmap = NULL;
        regionBitmap = new CmzBitmap(this->_app->getInstance(), this->_bitmapFilename);
        if (regionBitmap) {
            LONG w = regionBitmap->width();
            LONG h = regionBitmap->height();
            delete regionBitmap;
            this->setSize(w, h);
            this->centerOnScreen();
        } else {
            return;
        }
    }
}

WINAPI CmzBitmapWindow::~CmzBitmapWindow() {
}

VOID WINAPI CmzBitmapWindow::onDraw(HDC hdc) {
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

VOID WINAPI CmzBitmapWindow::_updateCanvas() {
    if (this->_bitmapFilename == COMMON_EMPTY_STRING) {
        return;
    }
    if (!this->_canvas || !this->isVisible() || this->isIconic()) {
		return;
    }
    this->_needUpdate = FALSE;
    
    CmzBitmap* regionBitmap = new CmzBitmap(this->_app->getInstance(), this->_bitmapFilename);
    if (!regionBitmap) {
        return;
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
    this->_canvas->drawBitmap(regionBitmap, x, y);
    // ================================================================
    // Koniec rysowania splasha
    // ================================================================

    delete regionBitmap;
    
    this->_canvas->endPaint();
    ReleaseDC(0, hdc);
}


VOID CmzBitmapWindow::calculateAndApplyRegion() {
    static BOOL firstCall = TRUE;
    if (!firstCall) {
        return;
    }

    this->_setWindowParameters();
    
    firstCall = FALSE;
    RECT rc;
    rc.left = this->clientOrigin.x - this->windowOrigin.x;
    rc.top = this->clientOrigin.y - this->windowOrigin.y;
    rc.right = rc.left + this->clientWidth();
    rc.bottom = rc.top + this->clientHeight();
    
    CmzBitmap* regionBitmap = NULL;
    if (FileExists(this->_bitmapFilename)) {
        regionBitmap = new CmzBitmap(this->_app->getInstance(), this->_bitmapFilename);
    } else {
        return;
    }

    this->_rcRegion = rc;
    if (this->_canvas) {
        delete this->_canvas;
    }
    
    this->_canvas = new CmzBitmap(this->regionWidth(), this->regionHeight(), RGB(255, 255, 255));
    this->_canvas->turnOnTrackingChanges();
    
    // we create rect region for full bitmap (including transparent colors)
    this->_hrgn = CreateRectRgn(this->_rcRegion.left, this->_rcRegion.top, this->_rcRegion.right, this->_rcRegion.bottom);
    
    LONG x, y;
    regionBitmap->beginPaint();
    HDC regionBitmapDC = regionBitmap->getDC();
    COLORREF pixel;
    BOOL inTransparent = FALSE;
    ULONG yStartTransparent = 0;
    HRGN tmpRgn;
    for (x = 0; x < regionBitmap->width(); x++) {
        for (y = 0; y < regionBitmap->height(); y++) {
            pixel = GetPixel(regionBitmapDC, x, y);
            if (pixel == 0x0000ff00) { // if we have pure green color (transparent marker)
                if (!inTransparent) {
                    yStartTransparent = y;
                    inTransparent = TRUE;
                }
            } else {
                if (inTransparent) {
                    //tmpRgn = CreateRectRgn(x, yStartTransparent, x+1, y);
                    tmpRgn = CreateRectRgn(this->_rcRegion.left + x, this->_rcRegion.top + yStartTransparent,
                                           this->_rcRegion.left + x + 1, this->_rcRegion.top + y);
                    CombineRgn(this->_hrgn, this->_hrgn, tmpRgn, RGN_XOR);
                    DeleteObject(tmpRgn);
                    inTransparent = FALSE;
                }
            }
            
            if (y == (regionBitmap->height() - 1) && inTransparent) {
                tmpRgn = CreateRectRgn(this->_rcRegion.left + x, this->_rcRegion.top + yStartTransparent,
                                       this->_rcRegion.left + x + 1, this->_rcRegion.top + y + 1);
                CombineRgn(this->_hrgn, this->_hrgn, tmpRgn, RGN_XOR);
                DeleteObject(tmpRgn);
                inTransparent = FALSE;
            }
        }
    }
    regionBitmap->endPaint();
    delete regionBitmap;
    
	SetWindowRgn(this->_hWnd, this->_hrgn, TRUE);
    rc = this->rcWindow;

    
    // skalujemy wszystkie wymagające tego komponenty
    this->_adjustChildrenSize();
    this->invalidate();
}

VOID WINAPI CmzBitmapWindow::setSize(LONG width, LONG height) {
    LONG cxSizeFrame = 0;
    LONG cySizeFrame = 0;
    LONG hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
    GetSystemWindowFrameSize(cxSizeFrame, cySizeFrame);
    width += 2*cxSizeFrame;
    height += hiddenHeight + cySizeFrame;
    SetWindowPos(this->_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    this->_setWindowParameters();
    this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
}
