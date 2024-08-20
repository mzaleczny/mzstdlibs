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
#include "mzLabel.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
using namespace mzstd;

CLabel::CLabel(CSprite *parent, CmzString name, LONG x, LONG y, CmzString text, LONG width, LONG height, BOOL autosize)
    : CGuiControl(parent, name, NULL, x, y)
{    
    this->setRealClassName(L"CLabel");

    this->_font = NULL;
    this->_bgColor = NULL;
    this->_lines = NULL;
    this->width = width;
    this->height = height;
    this->_borderType = CLABEL_BORDER_TYPE_NONE;
    this->_borderColor = RGB(0x00, 0x00, 0x00);
    this->setPadding(0);
    this->_baseWidth = 0;
    this->_baseHeight = 0;

    // domyślnie wysokością linii jest wysokość kontrolki, wartość ta jest korygowana w metodzie
    // this->setText
    this->_lineHeight = height;

    this->setAutoSize(autosize);
    this->setText(text);
}

CLabel::~CLabel() {
    if (this->_font) {
        delete this->_font;
        this->_font = NULL;
    }
    if (this->_lines) {
        delete this->_lines;
        this->_lines = NULL;
    }
}

VOID CLabel::setSize(LONG width, LONG height) {
    if (this->_autosize) {
        LONG w = this->_baseWidth;
        LONG h = this->_baseHeight;

        if (this->_borderType != CLABEL_BORDER_TYPE_NONE) {
            w += 2 * this->getBorderSize() + this->_paddingLeft + this->_paddingRight;
            h += 2 * this->getBorderSize() + this->_paddingTop + this->_paddingBottom;
        }

        width = w;
        height = h;
    }
    this->width = width;
    this->height = height;
}

VOID CLabel::onDraw(CmzBitmap *dest) {
    if (!this->parent || !this->parent->canvas) {
		return;
    }

    // autosize jest tylko przy pierwszym ustawieniu tekstu
    if (this->_autosize) {
        this->_autosize = FALSE;
    }

    CmzFont* font = this->getFont();

	HDC hdc = GetDC(0);
    HDC hdcParent = NULL;
    BOOL parentBeginPaintCalled = FALSE;
    RECT rc = this->getRect();
    CmzBitmap* parentCanvas = this->parent->canvas;

    // ustalamy współrzędne na canvasie parenta
    rc.left += this->x;
    rc.right += this->x;
    rc.top += this->y;
    rc.bottom += this->y;

    hdcParent = parentCanvas->getDC();
    if (!hdcParent) {
        parentCanvas->beginPaint(hdc);
        hdcParent = parentCanvas->getDC();
        parentBeginPaintCalled = TRUE;
    }

    TEXTMETRIC tm;
    HFONT oldFont = (HFONT)SelectObject(hdcParent, font->hFont);
    GetTextMetrics(hdcParent, &tm);
    SelectObject(hdcParent, oldFont);

    // jeśli jest podany nietransparentny kolor tła, to rysujemy tło etykiety
    HBRUSH bgBrush = NULL;
    if (this->_bgColor) {
        bgBrush = CreateSolidBrush(this->_bgColor);
        FillRect(hdcParent, &rc, bgBrush);
    }

    if (this->_lines && this->_lines->size() > 0) {
        ULONG i;
        LONG oldBkMode = parentCanvas->setBkMode(TRANSPARENT);
        CmzString s;
        RECT rcLine = rc;
        LONG borderSize = this->getBorderSize();
        rcLine.left += borderSize + this->_paddingLeft;
        rcLine.right -= borderSize + this->_paddingRight;
        for (i = 0; i < this->_lines->size(); i++) {
            rcLine.top = this->y + borderSize + this->_paddingTop + i * this->_lineHeight;
            rcLine.bottom = rcLine.top + this->_lineHeight;
            s = this->_lines->getAt(i);
            parentCanvas->drawText(font, s, rcLine);
        }
        parentCanvas->setBkMode(oldBkMode);
    }

    if (this->_borderType == CLABEL_BORDER_TYPE_SOLID) {
        LOGBRUSH lb;
        ZeroMemory(&lb, sizeof(lb));
        lb.lbStyle = BS_NULL;

        HBRUSH transparentBrush = CreateBrushIndirect(&lb);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdcParent, transparentBrush);
        HPEN borderPen = CreatePen(PS_SOLID, 1, this->_borderColor);
        HPEN oldPen = (HPEN)SelectObject(hdcParent, borderPen);

        // See also: PatBlt function
        Rectangle(hdcParent, rc.left, rc.top, rc.right, rc.bottom);

        SelectObject(hdcParent, oldBrush);
        SelectObject(hdcParent, oldPen);
        DeleteObject(transparentBrush);
        DeleteObject(borderPen);
    }

    if (this->_borderType != CLABEL_BORDER_TYPE_NONE && this->_borderType != CLABEL_BORDER_TYPE_SOLID) {
        DrawEdge(hdcParent, &rc, this->_getSdkEdgeTypeForCurrentBorderType(), BF_RECT);
    }

    if (parentBeginPaintCalled) {
	    parentCanvas->endPaint();
    }

    if (bgBrush) {
        DeleteObject(bgBrush);
    }
	ReleaseDC(0, hdc);
}

LONG CLabel::getBorderSize() {
    switch (this->_borderType) {
    case CLABEL_BORDER_TYPE_SOLID:
        return 1;
    case CLABEL_BORDER_TYPE_BEVEL_RAISEDINNER:
    case CLABEL_BORDER_TYPE_BEVEL_SUNKENINNER:
    case CLABEL_BORDER_TYPE_BEVEL_RAISEDOUTER:
    case CLABEL_BORDER_TYPE_BEVEL_SUNKENOUTER:
    case CLABEL_BORDER_TYPE_BEVEL_BUMP:
    case CLABEL_BORDER_TYPE_BEVEL_ETCHED:
    case CLABEL_BORDER_TYPE_BEVEL_RAISED:
    case CLABEL_BORDER_TYPE_BEVEL_SUNKEN:
        return 2;
    }

    return 0;
}

BOOL CLabel::onMouseMove(LONG x, LONG y) {
    return FALSE;
}

BOOL CLabel::onMouseDown(LONG x, LONG y) {
    return FALSE;
}

BOOL CLabel::onMouseUp(LONG x, LONG y) {
    return FALSE;
}

VOID CLabel::_updateCanvas() {
}

CmzFont* CLabel::getFont() {
    CmzFont* font = this->_theme->commonLabelFont;
    if (this->_font) {
        font = this->_font;
    }
    return font;
}

VOID CLabel::setText(CmzString& text, BOOL redraw) {
    if (this->_lines) {
        delete this->_lines;
        this->_lines = NULL;
    }

    this->_lines = text.explode(L'\n');
    if (!this->_lines || this->_lines->size() < 1) {
        return;
    }

    ULONG i;
    SIZE size;
    CmzFont* font = this->getFont();
    CmzString s;
    HDC hdc = this->parent->canvas->getDC();
    if (hdc == 0) {
        this->parent->canvas->beginPaint();
    }
    size = font->measureString(COMMON_MEASURE_STRING, this->parent->canvas->getDC());
    this->_lineHeight = size.cy;
    for (i = 0; i < this->_lines->size(); i++) {
        this->_lines->getAt(i).remove(L'\r');
    }

    LONG w = 0, h = 0;
    for (i = 0; i < this->_lines->size(); i++) {
        s = this->_lines->getAt(i);
        size = font->measureString(s, this->parent->canvas->getDC());
        if (size.cx > w) {
            w = size.cx;
        }
    }
    if (hdc == 0) {
        this->parent->canvas->endPaint();
    }
        
    h = static_cast<LONG>(this->_lines->size() * this->_lineHeight);
    this->_baseWidth = w;
    this->_baseHeight = h;

    if (this->_autosize) {
        this->setSize(w, h);
    }
    if (redraw) this->redraw();
}

CmzString CLabel::getText() {
    if (!this->_lines || this->_lines->size() < 1) {
        return L"";
    }

    CmzString retval = L"";
    ULONG len = static_cast<ULONG>(this->_lines->size());
    for (ULONG i = 0; i < len; i++) {
        retval += this->_lines->getAt(i);
        if (i < len - 1) {
            retval += L'\n';
        }
    }

    return retval;
}

VOID CLabel::setBorder(LONG borderType, BOOL redraw) {
    if (this->_borderType == borderType) {
        return;
    }

    this->_borderType = borderType;
    // poniższe wywołanie gwarantuje w przypadku ustawionego atrybutu this->_autosize, powiększenie
    // rozmiaru labelki o rozmiar obramowania i paddingu
    if (this->_autosize && this->_baseWidth > 0 && this->_baseHeight > 0) {
        this->setSize(this->_baseWidth, this->_baseHeight);
    }

    if (redraw) {
        this->redraw();
    }
}

LONG CLabel::_getSdkEdgeTypeForCurrentBorderType() {
    switch (this->_borderType) {
    case CLABEL_BORDER_TYPE_NONE:
        return 0;
    case CLABEL_BORDER_TYPE_SOLID:
        return 0;
    case CLABEL_BORDER_TYPE_BEVEL_RAISEDINNER:
        return BDR_RAISEDINNER;
    case CLABEL_BORDER_TYPE_BEVEL_SUNKENINNER:
        return BDR_SUNKENINNER;
    case CLABEL_BORDER_TYPE_BEVEL_RAISEDOUTER:
        return BDR_RAISEDOUTER;
    case CLABEL_BORDER_TYPE_BEVEL_SUNKENOUTER:
        return BDR_SUNKENOUTER;
    case CLABEL_BORDER_TYPE_BEVEL_BUMP:
        return EDGE_BUMP;
    case CLABEL_BORDER_TYPE_BEVEL_ETCHED:
        return EDGE_ETCHED;
    case CLABEL_BORDER_TYPE_BEVEL_RAISED:
        return EDGE_RAISED;
    case CLABEL_BORDER_TYPE_BEVEL_SUNKEN:
        return EDGE_SUNKEN;
    }

    return 0;
}

VOID CLabel::setPadding(ULONG padding, BOOL redraw) {
    this->_paddingLeft = padding;
    this->_paddingRight = padding;
    this->_paddingTop = padding;
    this->_paddingBottom = padding;

    // poniższe wywołanie gwarantuje w przypadku ustawionego atrybutu this->_autosize, powiększenie
    // rozmiaru labelki o rozmiar obramowania i paddingu
    if (this->_autosize && this->_baseWidth > 0 && this->_baseHeight > 0) {
        this->setSize(this->_baseWidth, this->_baseHeight);
    }

    if (redraw) {
        this->redraw();
    }
}
