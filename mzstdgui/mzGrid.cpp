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
#include "mzGrid.h"
#include "mzstdguiGlobals.h"
#include "mzstd.h"
#include "mzFileUtils.h"
#include "mzImageList.h"
#include "mzTheme.h"
#include "mzBitmap.h"
#include "mzKeyboard.h"
#include "mzStyledWindow.h"
#include "mzMessages.h"
#include "mzApplication.h"
#include "mzArrayList.h"
#include "mzTextField.h"
#include "mzScrollbar.h"
#include "mzScrollbarVertical.h"
#include "mzScrollbarHorizontal.h"
#include "mzStdDialogs.h"
using namespace mzstd;

CGridCell::CGridCell(CmzString value, BOOL selected) {
    this->setRealClassName(L"CGridCell");
    this->value = value;
    this->selected = selected;
}

CGridCell::~CGridCell() {
}


CGridHeaderCell::CGridHeaderCell(CmzString text, LONG size, BOOL selected) {
    this->setRealClassName(L"CGridHeaderCell");
    this->text = text;
    this->size = size;
    this->selected = selected;
}

CGridHeaderCell::CGridHeaderCell(LONG size, BOOL selected) {
    this->setRealClassName(L"CGridHeaderCell");
    this->text = COMMON_EMPTY_STRING;
    this->size = size;
    this->selected = selected;
}

CGridHeaderCell::~CGridHeaderCell() {
}




CGrid::CGrid(CSprite *parent, CmzString name, LONG columnCount, LONG rowCount, LONG x, LONG y, LONG width, LONG height, BOOL editable)
    : CGuiContainer(parent, name, width, height, x, y)
{
    this->_commonInit(columnCount, rowCount, width, height, editable, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE);
}

CGrid::CGrid(CSprite *parent, CmzString name, LONG columnCount, LONG rowCount, LONG x, LONG y, LONG width, LONG height, BOOL editable, BOOL showLeftHeader, BOOL showTopHeader, BOOL entireRowSelect, BOOL entireColumnSelect, BOOL drawVerticalLines, BOOL drawHorizontalLines)
    : CGuiContainer(parent, name, width, height, x, y)
{
    this->_commonInit(columnCount, rowCount, width, height, editable, showLeftHeader, showTopHeader, entireRowSelect, entireColumnSelect, drawVerticalLines, drawHorizontalLines);
}

VOID CGrid::_commonInit(LONG columnCount, LONG rowCount, LONG width, LONG height, BOOL editable, BOOL showLeftHeader, BOOL showTopHeader, BOOL entireRowSelect, BOOL entireColumnSelect, BOOL drawVerticalLines, BOOL drawHorizontalLines) {
    this->setRealClassName(L"CGrid");
    this->_state = CGRID_STATE_NORMAL;
    this->_imageList = NULL;

    this->_showTopHeader = showTopHeader;
    this->_showLeftHeader = showLeftHeader;
    this->_drawVerticalLines = drawVerticalLines;
    this->_drawHorizontalLines = drawHorizontalLines;
    this->_entireRowSelect = entireRowSelect;
    this->_entireColumnSelect = entireColumnSelect;

    this->_leftHeaderWidth = CGRID_DEFAULT_LEFT_HEADER_WIDTH;
    this->_defColumnWidth = CGRID_DEFAULT_COLUMN_WIDTH;

    this->_defRowHeight = this->_theme->grid_cell_inner_bg_normal->height() + 2;
    this->_topHeaderHeight = this->_defRowHeight;

    this->_startCellCoordX = 1;
    this->_startCellCoordY = 1;
    this->_coordX = 1;
    this->_coordY = 1;

    this->_colData = NULL;
    this->_rowData = NULL;
    this->setGridSize(columnCount, rowCount);

    //this->_backgroundColor = RGB(0xff, 0xff, 0xff);
    this->setSize(width, height);
    // Adding scrollbars and editor
    BOOL addVScr = static_cast<LONG>(this->getTotalRowsHeight()) > this->height;
    BOOL addHScr = static_cast<LONG>(this->getTotalColumnsWidth()) > this->width;
    this->addScrollbars(addVScr, addHScr, 0, 1000, 0, 1000);
    CScrollbar* scr = dynamic_cast<CScrollbar*>(this->getChild(this->name + "_VSB"));
    if (scr)
    {
        (dynamic_cast<CScrollbarVertical*>(scr))->setSteps(50, 100);
    }
    scr = dynamic_cast<CScrollbar*>(this->getChild(this->name + "_HSB"));
    if (scr)
    {
        (dynamic_cast<CScrollbarHorizontal*>(scr))->setSteps(50, 100);
    }
    this->_tabStop = TRUE;
    if (editable) {
        this->addEditor();
    }

    this->_cachedFonts = new CmzArrayList(10);
    this->_updateGridParameters();

    if (this->_visibleWidthOfLastColumn < 4) {
        this->_xPageStep = this->getFullVisibleColumnCount();
    } else {
        this->_xPageStep = this->getVisibleColumnCount();
    }
    if (this->_visibleHeightOfLastRow < 4) {
        this->_yPageStep = this->getFullVisibleRowCount();
    } else {
        this->_yPageStep = this->getVisibleRowCount();
    }
}

CGrid::~CGrid() {
    this->_cleanGridContent();
    if (this->_cachedFonts->size() > 0) {
        this->cleanCachedFonts();
        delete this->_cachedFonts;
        this->_cachedFonts = NULL;
    }
}

VOID CGrid::cleanData() {
    CPairExt* p;
    CGridCell* c;
    ULONGLONG size = this->_data.size();
    LONGLONG* items = this->_data.getItemsBuf();
    for (ULONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p) {
            c = (CGridCell*)p->second;
            delete c;
        }
    }
    // clearing below does delete on CPairExt objects
    this->_data.clear();
}

VOID CGrid::cleanCachedFonts() {
    ULONGLONG size = this->_cachedFonts->size();
    LONGLONG* items = this->_cachedFonts->getItemsBuf();
    CmzFont* font;
    for (ULONGLONG i = 0; i < size; i++) {
        font = (CmzFont*)items[i];
        delete font;
    }
    this->_cachedFonts->clear();
}

CmzFont* CGrid::getFont(const WCHAR* fontName, LONG fontSize, LONG fontColor) {
    CmzFont* defFont = this->_theme->commonGridControlFont;
    ULONGLONG size = this->_cachedFonts->size();
    if (fontName == COMMON_EMPTY_STRING || size < 1) {
        return defFont;
    }

    LONGLONG* items = this->_cachedFonts->getItemsBuf();
    CmzFont* font;
    for (ULONGLONG i = 0; i < size; i++) {
        font = (CmzFont*)items[i];
        if (font->name == fontName && font->size == fontSize && font->color == fontColor) {
            return font;
        }
    }

    font = new CmzFont(NULL, fontColor, fontName, fontSize);
    this->_cachedFonts->append((LONGLONG)font);

    return font;
}

VOID CGrid::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas || !this->isVisible()) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    BOOL drawScrollbarsEmptySquare = FALSE;
    if (this->_vscrollbar || this->_hscrollbar) {
        drawScrollbarsEmptySquare = TRUE;
    }

    LONG w, h;
    CGridHeaderCell *xheaderCell, *yheaderCell;
    SIZE cellSize;

	this->canvas->beginPaint(hdc);

    RECT clientRect, noheadersClientRect;
    LONG maxRightX = 0;
    LONG maxBottomY = 0;
    clientRect.left = 0;
    clientRect.top = 0;
    clientRect.right = this->width;
    clientRect.bottom = this->height;
    if (this->_showLeftHeader) {
        clientRect.left = this->_leftHeaderWidth - 1;
    }
    if (this->_showTopHeader) {
        clientRect.top = this->_topHeaderHeight - 1;
    }
    if (this->_vscrollbar) {
        clientRect.right -= this->_vscrollbar->width;
    }
    if (this->_hscrollbar) {
        clientRect.bottom -= this->_hscrollbar->height;
    }

    clientRect.right -= 1;      // apply border size
    clientRect.bottom -= 1;     // apply border size
    noheadersClientRect = clientRect;
    noheadersClientRect.left = 0;
    noheadersClientRect.top = 0;

    LONG theaderSize = this->_showTopHeader ? this->_topHeaderHeight : 0;
    LONG lheaderSize = this->_showLeftHeader ? this->_leftHeaderWidth : 0;

    // clear canvas
    this->canvas->fillRectangle(this->_backgroundColor, 0, 0, this->width, this->height);

    // ================================================================
    // Rysujemy lewą górną komórkę nagłówka
    // ================================================================
    if (this->_showLeftHeader && this->_showTopHeader) {
        w = this->_leftHeaderWidth;
        h = this->_topHeaderHeight;
        this->canvas->stretchBitmap(t->grid_left_top_header_inner_bg, x + 1, y + 1, w - 2, h - 2);
        this->canvas->drawBitmap(t->grid_left_top_header_mark, x + w - t->grid_left_top_header_mark->width() - 3, y + (h - t->grid_left_top_header_mark->height() - 2));
        this->canvas->drawRectangle(t->commonGridControlLeftTopHeaderCellBorderColor, x, y, x + w, y + h);
        x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
    }
    // ================================================================
    // Koniec rysowania lewej górnej komórki nagłówka
    // ================================================================

    BOOL selected = FALSE, xHeaderColSelected = FALSE, yHeaderColSelected = FALSE;
    CmzBitmap *inner_bg;
    COLORREF border_color;
    CmzFont *f = this->getFont();
    LONG xIndex, yIndex;
    RECT rc;
    ULONGLONG xheader_items_count, yheader_items_count;
    LONGLONG *xheader_items, *yheader_items;

    xheader_items_count = this->_colData->size();
    xheader_items = this->_colData->getItemsBuf();
    yheader_items_count = this->_rowData->size();
    yheader_items = this->_rowData->getItemsBuf();

    // ================================================================
    // Rysujemy górny nagłówek
    // ================================================================
    if (this->_showTopHeader) {
        h = this->_topHeaderHeight;
        xIndex = this->_startCellCoordX;
        while ((x < this->_dataCanvasWidth + lheaderSize) && (xIndex-1 < xheader_items_count)) {
            xheaderCell = (CGridHeaderCell*)xheader_items[xIndex-1];
            w = xheaderCell->size;
            // najpierw ustalamy czy komórka nagłówka jest aktywna
            if (xIndex == this->_coordX) {
                xHeaderColSelected = TRUE;
            } else {
                if (xheaderCell) {
                    xHeaderColSelected = xheaderCell->selected;
                } else {
                    xHeaderColSelected = FALSE;
                }
            }

            if (xHeaderColSelected) {
                inner_bg = t->grid_top_header_inner_bg_selected_cell;
                border_color = t->commonGridControlTopHeaderCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_top_header_inner_bg_normal_cell;
                border_color = t->commonGridControlTopHeaderCellBorderColor_Normal;
            }

            rc.left = x;
            rc.top = y;
            rc.right = rc.left + w;
            rc.bottom = rc.top + h;

            this->canvas->stretchBitmap(inner_bg, x + 1, y + 1, w - 2, h - 2);
            this->canvas->drawRectangle(border_color, rc);
            if (xheaderCell && xIndex <= this->_maxColumnNumber) {
                this->canvas->drawText(f, xheaderCell->text, rc, TRUE, 0, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0);
            }

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            xIndex += 1;
        }
    }
    // ================================================================
    // Koniec rysowania górnego nagłówka
    // ================================================================


    // ================================================================
    // Rysujemy lewy nagłówek
    // ================================================================
    if (this->_showLeftHeader) {
        x = 0;
        if (this->_showTopHeader) {
            y = t->grid_top_header_inner_bg_normal_cell->height() + 2 - 1;
        } else {
            y = 0;
        }
        w = this->_leftHeaderWidth;
        yIndex = this->_startCellCoordY;
        while ((y < this->_dataCanvasHeight + theaderSize) && (yIndex-1 < yheader_items_count)) {
            yheaderCell = (CGridHeaderCell*)yheader_items[yIndex-1];
            h = yheaderCell->size;
            // najpierw ustalamy czy komórka nagłówka jest aktywna
            if (yIndex == this->_coordY) {
                yHeaderColSelected = TRUE;
            } else {
                if (yheaderCell) {
                    yHeaderColSelected = yheaderCell->selected;
                } else {
                    yHeaderColSelected = FALSE;
                }
            }

            if (yHeaderColSelected) {
                inner_bg = t->grid_left_header_inner_bg_selected_cell;
                border_color = t->commonGridControlLeftHeaderCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_left_header_inner_bg_normal_cell;
                border_color = t->commonGridControlLeftHeaderCellBorderColor_Normal;
            }

            rc.left = x;
            rc.top = y;
            rc.right = rc.left + w;
            rc.bottom = rc.top + h;

            this->canvas->stretchBitmap(inner_bg, x + 1, y + 1, w - 2, h - 2);
            this->canvas->drawRectangle(border_color, rc);
            if (yheaderCell && yIndex <= this->_maxRowNumber) {
                this->canvas->drawText(f, yheaderCell->text, rc, TRUE, 0, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0);
            }

            y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            yIndex++;
        }
    }
    // ================================================================
    // Koniec rysowania lewego nagłówka
    // ================================================================


    CGridCell *cellData;
    CPairExt *cellPair;
    LONGLONG coord;
    LONGLONG foundCellPairPosition;
    // ================================================================
    // Rysujemy komórki wewnętrzne (z danymi)
    // ================================================================
    x = lheaderSize ? lheaderSize - 1 : 0;
    y = theaderSize ? theaderSize - 1 : 0;
    w = this->_defColumnWidth;
    h = t->grid_cell_inner_bg_normal->height();

    xIndex = this->_startCellCoordX;
    yIndex = this->_startCellCoordY;
    LONG startX = x;

    while ((y <= this->_dataCanvasHeight + theaderSize) && (yIndex <= yheader_items_count)) {
        x = startX;
        xIndex = this->_startCellCoordX;
        while ((x <= this->_dataCanvasWidth + lheaderSize) && (xIndex <= xheader_items_count)) {
            xheaderCell = (CGridHeaderCell*)xheader_items[xIndex-1];
            yheaderCell = (CGridHeaderCell*)yheader_items[yIndex-1];
            w = xheaderCell->size;
            h = yheaderCell->size;

            coord = this->_cellCoordsToLONG(xIndex, yIndex);
            cellPair = this->_data.findFirstItemByFirst(coord, &foundCellPairPosition);
            if (cellPair) {
                cellData = (CGridCell*)cellPair->second;
                selected = cellData->selected || (this->_entireRowSelect && (xIndex == this->_coordX)) || (this->_entireColumnSelect && (yIndex == this->_coordY));
            } else {
                selected = (this->_entireRowSelect && (xIndex == this->_coordX)) || (this->_entireColumnSelect && (yIndex == this->_coordY));
            }

            if (selected) {
                inner_bg = t->grid_cell_inner_bg_selected;
                border_color = t->commonGridControlCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_cell_inner_bg_normal;
                border_color = t->commonGridControlCellBorderColor_Normal;
            }

            rc.left = x;
            rc.top = y;
            rc.right = rc.left + w;
            rc.bottom = rc.top + h;

            this->canvas->stretchBitmap(inner_bg, x + 1, y + 1, w - 2, h - 2);
            if (this->_drawVerticalLines && this->_drawHorizontalLines) {
                this->canvas->drawRectangle(border_color, rc);
            } else {
                if (this->_drawHorizontalLines) {
                    this->canvas->drawHLine(border_color, rc.left, rc.top, w);
                    this->canvas->drawHLine(border_color, rc.left, rc.top + h - 1, w);
                } else if (yIndex == this->_maxRowNumber) {
                    // last (closing) grid horizontal line we draw always
                    this->canvas->drawHLine(border_color, rc.left, rc.top + h - 1, w);
                }
                if (this->_drawVerticalLines) {
                    this->canvas->drawVLine(border_color, rc.left, rc.top, h);
                    this->canvas->drawVLine(border_color, rc.left + w - 1, rc.top, h);
                } else if (xIndex == this->_maxColumnNumber) {
                    // last (closing) grid vertical line we draw always
                    this->canvas->drawVLine(border_color, rc.left + w - 1, rc.top, h);
                }
            }

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            xIndex++;
        }

        y += h - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        yIndex++;
    }
    // ================================================================
    // Koniec rysowania komórek wewnętrznych (z danymi)
    // ================================================================


    LONG cellMarginLeft = 2;
    LONG cellMarginRight = 2;
    CmzString cellValue;
    // ================================================================
    // Rysujemy zawartości (dane) komórek wewnętrznych
    // ================================================================
    x = this->_leftHeaderWidth - 1;
    y = t->grid_top_header_inner_bg_normal_cell->height() + 2 - 1;
    w = this->_defColumnWidth;
    h = t->grid_cell_inner_bg_normal->height();
    xIndex = this->_startCellCoordX;
    yIndex = this->_startCellCoordY;
    while (y < this->_dataCanvasHeight && (yIndex - this->_startCellCoordY < yheader_items_count)) {
        x = this->_leftHeaderWidth - 1;
        xIndex = this->_startCellCoordX;
        while (x < this->_dataCanvasWidth && (xIndex - this->_startCellCoordX < xheader_items_count)) {
            xheaderCell = (CGridHeaderCell*)xheader_items[xIndex - this->_startCellCoordX];
            yheaderCell = (CGridHeaderCell*)yheader_items[yIndex - this->_startCellCoordY];
            w = xheaderCell->size;
            h = yheaderCell->size - 2;

            if (selected) {
                inner_bg = t->grid_cell_inner_bg_selected;
                border_color = t->commonGridControlCellBorderColor_Selected;
            } else {
                inner_bg = t->grid_cell_inner_bg_normal;
                border_color = t->commonGridControlCellBorderColor_Normal;
            }


            rc.left = x + cellMarginLeft;
            rc.top = y;
            rc.right = x + w - 1 - cellMarginRight;
            rc.bottom = y + h + 2;

            coord = this->_cellCoordsToLONG(xIndex, yIndex);
            cellPair = this->_data.findFirstItemByFirst(coord, &foundCellPairPosition);
            if (!cellPair) {
                cellValue = COMMON_EMPTY_STRING;
            } else {
                cellData = (CGridCell*)cellPair->second;
                cellValue = cellData->value;
            }

            this->canvas->drawText(f, cellValue, rc, TRUE, DT_VCENTER, -1, 0);

            x += w - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
            xIndex++;
        }

        y += h + 2 - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        yIndex++;
    }
    // ================================================================
    // Koniec rysowania zawartości (dane) komórek wewnętrznych
    // ================================================================

    // ================================================================
    // Rysowanie końcowych obramowań
    // ================================================================
    // Obramowanie całego Grida
    if (this->_state == CGRID_STATE_FOCUSED) {
        this->canvas->drawRectangle(t->commonGridControlFocusedGridBorderColor, 0, 0, this->width, this->height);
    } else {
        this->canvas->drawRectangle(t->commonGridControlLeftTopHeaderCellBorderColor, 0, 0, this->width, this->height);
    }

    // Obramowanie dolne górnego nagłówka (gdyż zostało nadpisane przez komórki)
    if (this->_showTopHeader) {
        x = 1;
        y = this->_topHeaderHeight - 1;
        this->canvas->drawLine(t->commonGridControlTopHeaderCellBorderColor_Normal, x, y, this->_dataCanvasWidth + lheaderSize - 1, y);
    }
    // Obramowanie prawe lewego nagłówka (gdyż zostało nadpisane przez komórki)
    if (this->_showLeftHeader) {
        x = this->_leftHeaderWidth - 1;
        y = 1;
        this->canvas->drawLine(t->commonGridControlTopHeaderCellBorderColor_Normal, x, y, x, this->_dataCanvasHeight + theaderSize - 1);
    }
    // ================================================================
    // Koniec rysowania końcowych obramowań
    // ================================================================
    /*
    // If grid data cells occupy smaller area than entire grid control then fill remained regions with bg color
    if (maxRightX < clientRect.right) {
        this->canvas->fillRectangle(this->_backgroundColor, maxRightX, clientRect.top, clientRect.right - maxRightX, this->_dataCanvasHeight);
    }
    if (maxBottomY < clientRect.bottom) {
        this->canvas->fillRectangle(this->_backgroundColor, clientRect.left, maxBottomY, clientRect.right - clientRect.left, clientRect.bottom - maxBottomY);
    }
    */
    // ================================================================
    // Rysowanie aktywnej komórki (a właściwie jej obramowania)
    // ================================================================
    if (this->cellVisible(this->_coordX, this->_coordY, TRUE)) {
        this->cellCoordsToCanvasCoords(this->_coordX, this->_coordY, &x, &y);
        // Inkrementujemy obie współrzedne, bo prostokąt rysujemy pogrubioną linią
        x += 1;
        y += 1;
        cellSize = this->getCellSize(this->_coordX, this->_coordY);
        rc.left = x;
        rc.top = y;
        rc.right = x + cellSize.cx - 1; // -1, bo prostokąt rysujemy pogrubioną linią
        rc.bottom = y + cellSize.cy - 1; // -1, bo prostokąt rysujemy pogrubioną linią
        this->canvas->drawRectangle(t->commonGridControlCellBorderColor_Active, rc, 2);
        rc.left = rc.right - 4;
        rc.right += 3;
        rc.top = rc.bottom - 4;
        rc.bottom += 3;
        this->canvas->fillRectangle(RGB(0xff, 0xff, 0xff), t->commonGridControlCellBorderColor_Active, rc, 1);
    }
    // ================================================================
    // Koniec rysowania aktywnej komórki (a właściwie jej obramowania)
    // ================================================================

    if (drawScrollbarsEmptySquare) {
        LONG scw = 0;
        LONG sch = 0;
        if (this->_vscrollbar) {
            scw = this->_vscrollbar->width;
        }
        if (this->_hscrollbar) {
            sch = this->_hscrollbar->height;
        }
        this->canvas->fillRectangle(RGB(0xff, 0xff, 0xff), this->width - scw, this->height - sch, scw, sch);
    }

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID CGrid::onDraw(CmzBitmap *dest) {
    if (!this->_visible) return;
	if (!this->canvas) return;

    if (this->_needUpdate) {
        this->invalidateChildSpritesCanvases();
        this->_updateCanvas();
    }

    // ================================================================
    // drawing child-sprites
    // ================================================================
	HDC hdc = GetDC(0);
	this->canvas->beginPaint(hdc);
    this->drawChildSprites(this->canvas);
	this->canvas->endPaint();
	ReleaseDC(0, hdc);
    // ================================================================
    // end of drawing child-sprites
    // ================================================================

    dest->drawBitmap(this->canvas, this->x, this->y, this->_currentFrame);
}


VOID CGrid::focus() {
    CStyledWindow* wnd = this->getParentWindow();
    if (wnd && wnd->getActiveControl() != this) {
        wnd->setActiveControl(this);
        return;
    }
    this->setState(CGRID_STATE_FOCUSED);
}

VOID CGrid::looseFocus() {
    //CStyledWindow* wnd = this->getParentWindow();
    //LONG x = MININT, y = MININT;
    //wnd->setOnlyActiveControlPointer(NULL);
    // this->getCurrentMousePosition(x, y);

    this->setState(CGRID_STATE_NORMAL);
}

CmzString CGrid::_indexToLetter(LONG index) {
    // ABCDEFGHIJKLMNOPQRSTUVWXYZ - 26 liter
    WCHAR* letters = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // dopuszczamy numery maksymalnie 3-literowe
    if (index < 1 || index > 17576) { // 17576 = 26 * 26 * 26
        return COMMON_EMPTY_STRING;
    }

    if (index <= 26) {
        return CmzString(letters[index-1]);
    }
    if (index <= 676) { // 676 = 26 * 26
        return CmzString(letters[index / 26 - 1]) + CmzString(letters[index % 26]);
    }
    if (index <= 17576) { // 17576 = 26 * 26 * 26
        LONG tmp = index / 26;
        return CmzString(letters[tmp / 26 - 1]) + CmzString(letters[tmp % 26]) + CmzString(letters[index % 26]);
    }

    return COMMON_EMPTY_STRING;
}

LONG CGrid::_cellCoordsToLONG(LONG x, LONG y) {
    DWORD result = (((DWORD)y) << (sizeof(WORD) << 3)) | ((WORD)x);
    return (LONG)result;
}

VOID CGrid::_LONGToCellCoords(LONG data, LONG* x, LONG* y) {
    *x = LOWORD((DWORD)data);
    *y = HIWORD((DWORD)data);
}

VOID CGrid::cellCoordsToCanvasCoords(LONG cellCoordX, LONG cellCoordY, LONG* x, LONG* y) {
    *x = this->_showLeftHeader ? this->_leftHeaderWidth - 1 : 0;
    *y = this->_showTopHeader ? this->_topHeaderHeight - 1 : 0;

    LONG xIndex = this->_startCellCoordX;
    LONG yIndex = this->_startCellCoordY;
    CGridHeaderCell *headerData;
    ULONGLONG xheader_items_count, yheader_items_count;
    LONGLONG *xheader_items, *yheader_items;

    xheader_items_count = this->_colData->size();
    xheader_items = this->_colData->getItemsBuf();
    yheader_items_count = this->_rowData->size();
    yheader_items = this->_rowData->getItemsBuf();

    while (xIndex < cellCoordX) {
        headerData = (CGridHeaderCell*)xheader_items[xIndex - this->_startCellCoordX];
        *x += headerData->size - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        xIndex++;
    }

    while (yIndex < cellCoordY) {
        headerData = (CGridHeaderCell*)yheader_items[yIndex - this->_startCellCoordY];
        *y += headerData->size - 1; // - 1, żeby krawędzie sąsiednich komórek były wspólne
        yIndex++;
    }
}

SIZE CGrid::getCellSize(LONG cellCoordX, LONG cellCoordY) {
    SIZE retval;
    retval.cx = 0;
    retval.cy = 0;
    CGridHeaderCell* headerData;

    headerData = this->getHeaderCellX(cellCoordX);
    if (headerData) {
        retval.cx = headerData->size;
    }
    headerData = this->getHeaderCellY(cellCoordY);
    if (headerData) {
        retval.cy = headerData->size;
    }

    return retval;
}

CGridCell* CGrid::getCell(LONG cellCoordX, LONG cellCoordY) {
    LONGLONG pos;
    LONGLONG first = this->_cellCoordsToLONG(cellCoordX, cellCoordY);
    CPairExt* p = this->_data.findFirstItemByFirst(first, &pos);
    if (p) {
        return (CGridCell*)p->second;
    }
    return NULL;
}

CGridCell* CGrid::getCurrentCell() {
    return this->getCell(this->_coordX, this->_coordY);
}

CGridHeaderCell* CGrid::getHeaderCellX(LONG coordX) {
    return (CGridHeaderCell*)this->_colData->getAt(coordX-1);
}

CGridHeaderCell* CGrid::getCurrentHeaderCellX() {
    return (CGridHeaderCell*)this->_colData->getAt(this->_coordX-1);
}

CGridHeaderCell* CGrid::getHeaderCellY(LONG coordY) {
    return (CGridHeaderCell*)this->_rowData->getAt(coordY-1);
}

CGridHeaderCell* CGrid::getCurrentHeaderCellY() {
    return (CGridHeaderCell*)this->_rowData->getAt(this->_coordY-1);
}

VOID CGrid::setStartCellCoordX(LONG coordX) {
    if (coordX <= 0) {
        return;
    }

    this->_startCellCoordX = coordX;

    if (this->_startCellCoordX < 1) {
        this->_startCellCoordX = 1;
        return;
    }

    if (this->_startCellCoordX > this->_maxAllowableStartCellCoordX) {
        this->_startCellCoordX = this->_maxAllowableStartCellCoordX;
    }

//    InformationBox(FormatMessage(L"_maxAllowableStartCellCoordX: %d, _maxAllowableStartCellCoordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
//        this->_maxAllowableStartCellCoordX, this->_maxAllowableStartCellCoordY, this->_startCellCoordX, this->_startCellCoordY));
}

VOID CGrid::setStartCellCoordY(LONG coordY) {
    if (coordY <= 0) {
        return;
    }

    this->_startCellCoordY = coordY;

    if (this->_startCellCoordY < 1) {
        this->_startCellCoordY = 1;
        return;
    }

    if (this->_startCellCoordY > this->_maxAllowableStartCellCoordY) {
        this->_startCellCoordY = this->_maxAllowableStartCellCoordY;
    }

//    InformationBox(FormatMessage(L"_maxAllowableStartCellCoordX: %d, _maxAllowableStartCellCoordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
//        this->_maxAllowableStartCellCoordX, this->_maxAllowableStartCellCoordY, this->_startCellCoordX, this->_startCellCoordY));
}

BOOL CGrid::moveRight(LONG count) {
    if (this->_coordX < this->_maxColumnNumber) {
        if (this->_coordX + count <= this->_maxColumnNumber) {
            this->_coordX += count;
        } else {
            count = this->_maxColumnNumber - this->_coordX;
            this->_coordX = this->_maxColumnNumber;
        }
        if (this->_coordX > this->_maxFullVisibleColumnNumber && this->_coordX < this->_maxColumnNumber) {
            this->setStartCellCoordX(this->_startCellCoordX + count);
            this->_updateGridParameters();
        } else if (this->_coordX == this->_maxColumnNumber) {
            this->setStartCellCoordX(this->_maxColumnNumber);
            this->_updateGridParameters();
        }
        return TRUE;
    }

    return FALSE;
}

BOOL CGrid::moveLeft(LONG count) {
    if (this->_coordX > 1) {
        if (this->_coordX - count >= 1) {
            this->_coordX -= count;
        } else {
            this->_coordX = 1;
        }
        if (this->_startCellCoordX > this->_coordX) {
            this->setStartCellCoordX(this->_coordX);
            this->_updateGridParameters();
        }
        return TRUE;
    }

    return FALSE;
}

BOOL CGrid::moveUp(LONG count) {
    if (this->_coordY > 1) {
        if (this->_coordY - count >= 1) {
            this->_coordY -= count;
        } else {
            this->_coordY = 1;
        }
        if (this->_startCellCoordY > this->_coordY) {
            this->setStartCellCoordY(this->_coordY);
            this->_updateGridParameters();
        }
        return TRUE;
    }

    return FALSE;
}

BOOL CGrid::moveDown(LONG count) {
    if (this->_coordY < this->_maxRowNumber) {
        if (this->_coordY + count <= this->_maxRowNumber) {
            this->_coordY += count;
        } else {
            count = this->_maxRowNumber - this->_coordY;
            this->_coordY = this->_maxRowNumber;
        }
        if (this->_coordY > this->_maxFullVisibleRowNumber) {
            this->setStartCellCoordY(this->_startCellCoordY + count);
            this->_updateGridParameters();
        }
        return TRUE;
    }

    return FALSE;
}

BOOL CGrid::movePageUp() {
    return this->moveUp(this->_yPageStep);
}

BOOL CGrid::movePageDown() {
    return this->moveDown(this->_yPageStep);
}

BOOL CGrid::movePageLeft() {
    return this->moveLeft(this->_xPageStep);
}

BOOL CGrid::movePageRight() {
    return this->moveRight(this->_xPageStep);
}

BOOL CGrid::moveHome() {
    return this->moveToCell(1, 1);
}

BOOL CGrid::moveHomeVertical() {
    return this->moveToCell(this->_coordX, 1);
}

BOOL CGrid::moveHomeHorizontal() {
    return this->moveToCell(1, this->_coordY);
}

BOOL CGrid::moveEnd() {
    return this->moveToCell(this->_maxColumnNumber, this->_maxRowNumber);
}

BOOL CGrid::moveEndVertical() {
    return this->moveToCell(this->_coordX, this->_maxRowNumber);
}

BOOL CGrid::moveEndHorizontal() {
    return this->moveToCell(this->_maxColumnNumber, this->_coordY);
}

BOOL CGrid::moveToCell(LONG coordX, LONG coordY) {
    if (
        coordX < 1 || coordX > this->_maxColumnNumber ||
        coordY < 1 || coordY > this->_maxRowNumber
       ) {
        return FALSE;
    }
    LONG deltaX = coordX - this->_coordX;
    LONG deltaY = coordY - this->_coordY;
    if (this->cellVisible(this->_coordX + deltaX, this->_coordY + deltaY)) {
        this->_coordX = coordX;
        this->_coordY = coordY;
    } else {
        this->_coordX = coordX;
        this->_coordY = coordY;
        this->setStartCellCoordX(this->_startCellCoordX + deltaX > 0 ? this->_startCellCoordX + deltaX : 1);
        this->setStartCellCoordY(this->_startCellCoordY + deltaY > 0 ? this->_startCellCoordY + deltaY : 1);
        this->_updateGridParameters();
    }

    return deltaX != 0 || deltaY != 0;
}

BOOL CGrid::cellVisible(LONG coordX, LONG coordY, BOOL acceptPartialVisibility) {
    LONG maxCoordX, maxCoordY;
    if (!acceptPartialVisibility) {
        maxCoordX = this->_maxFullVisibleColumnNumber;
        maxCoordY = this->_maxFullVisibleRowNumber;
    } else {
        maxCoordX = this->_maxVisibleColumnNumber;
        maxCoordY = this->_maxVisibleRowNumber;
    }

    if (
        (this->_startCellCoordX <= coordX) && (coordX <= maxCoordX) &&
        (this->_startCellCoordY <= coordY) && (coordY <= maxCoordY)) {
        return TRUE;
    }

    return FALSE;
}

BOOL CGrid::getCellCoordsAtMousePos(LONG mouseX, LONG mouseY, LONG* coordX, LONG* coordY) {
    *coordX = -1;
    *coordY = -1;

    LONG startMouseX;
    LONG startMouseY;
    this->cellCoordsToCanvasCoords(this->_startCellCoordX, this->_startCellCoordY, &startMouseX, &startMouseY);

    LONG xIndex = this->_startCellCoordX;
    LONG yIndex = this->_startCellCoordY;
    CGridHeaderCell *headerData;
    LONGLONG *xheader_items, *yheader_items;

    if (mouseX < startMouseX) {
        mouseX = startMouseX;
    }
    if (mouseY < startMouseY) {
        mouseY = startMouseY;
    }

    BOOL coordSet = FALSE;
    ULONGLONG size = this->_colData->size();
    xheader_items = this->_colData->getItemsBuf();
    LONG curX = startMouseX;
    LONG curY = startMouseY;
    while ((xIndex - this->_startCellCoordX < size) && (curX <= this->width)) {
        headerData = (CGridHeaderCell*)xheader_items[xIndex - this->_startCellCoordX];
        if (curX <= mouseX && mouseX <= curX + headerData->size) {
            coordSet = TRUE;
            break;
        }
        curX += headerData->size - 1; // -1, bo sąsiednie komórki mają wspólne krawędzie
        xIndex += 1;
    }
    if (coordSet) {
        *coordX = xIndex;
        coordSet = FALSE;
        size = this->_rowData->size();
        yheader_items = this->_rowData->getItemsBuf();
        while ((yIndex - this->_startCellCoordY < size) && (curY <= this->height)) {
            headerData = (CGridHeaderCell*)yheader_items[yIndex - this->_startCellCoordY];
            if (curY <= mouseY && mouseY <= curY + headerData->size) {
                coordSet = TRUE;
                break;
            }
            curY += headerData->size - 1; // -1, bo sąsiednie komórki mają wspólne krawędzie
            yIndex += 1;
        }
        if (coordSet) {
            *coordY = yIndex;
        } else {
            *coordX = -1;
        }
    }

    if (*coordX != -1 && *coordY != -1) {
        return TRUE;
    }

    return FALSE;
}

BOOL CGrid::activateCellAtMousePos(LONG x, LONG y) {
    LONG coordX, coordY;
    if (this->getCellCoordsAtMousePos(x, y, &coordX, &coordY)) {
        this->_coordX = coordX;
        this->_coordY = coordY;
        return TRUE;
    }

    return FALSE;
}

VOID CGrid::clearSelection(BOOL redraw) {
    BOOL had_selection = FALSE;

    CPairExt *p;
    CGridCell *gridCell;
    ULONGLONG items_count = (LONG)this->_data.size();
    LONGLONG *items = this->_data.getItemsBuf();

    for (LONG i = 0; i < items_count; i++) {
        p = (CPairExt*)items[i];
        gridCell = (CGridCell*)p->second;
        if (gridCell) {
            if (gridCell->selected) {
                had_selection = TRUE;
                gridCell->selected = FALSE;
            }
        }
    }

    if (!had_selection) {
        redraw = FALSE;
    }

    if (redraw) {
        this->redraw();
    }
}

BOOL CGrid::isSelection() {
    BOOL is_selection = FALSE;

    CPairExt *p;
    CGridCell *gridCell;
    ULONGLONG items_count = (LONG)this->_data.size();
    LONGLONG *items = this->_data.getItemsBuf();

    for (LONG i = 0; i < items_count; i++) {
        p = (CPairExt*)items[i];
        gridCell = (CGridCell*)p->second;
        if (gridCell) {
            if (gridCell->selected) {
                is_selection = TRUE;
                break;
            }
        }
    }

    return is_selection;
}

BOOL CGrid::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw) {
    redraw = FALSE;
    return FALSE;
}

BOOL CGrid::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
    BOOL vkLAlt, BOOL vkRAlt,
    BOOL vkLShift, BOOL vkRShift,
    BOOL vkLControl, BOOL vkRControl,
    BOOL systemKey,
    ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    BOOL redraw = FALSE;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, redraw);
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL CGrid::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
    BOOL vkLAlt, BOOL vkRAlt,
    BOOL vkLShift, BOOL vkRShift,
    BOOL vkLControl, BOOL vkRControl,
    BOOL systemKey,
    ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    BOOL redraw = FALSE;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        BOOL process = !this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, redraw);

        if (process) {
            CKeyboard* kbd = this->getKbd();

            BOOL activeCellChanged = FALSE;
            switch (virtualCode) {
            case VK_DELETE:
                this->clearCurrentCellValue();
                redraw = TRUE;
                break;
            case VK_RIGHT:
                if (vkControl) {
                    activeCellChanged = this->moveEndHorizontal();
                } else {
                    activeCellChanged = this->moveRight();
                }
                break;
            case VK_LEFT:
                if (vkControl) {
                    activeCellChanged = this->moveHomeHorizontal();
                } else {
                    activeCellChanged = this->moveLeft();
                }
                break;
            case VK_UP:
                if (vkControl) {
                    activeCellChanged = this->moveHomeVertical();
                } else {
                    activeCellChanged = this->moveUp();
                }
                break;
            case VK_DOWN:
                if (vkControl) {
                    activeCellChanged = this->moveEndVertical();
                } else {
                    activeCellChanged = this->moveDown();
                }
                break;
            case VK_NEXT:
                activeCellChanged = this->movePageDown();
                break;
            case VK_PRIOR:
                activeCellChanged = this->movePageUp();
                break;
            case VK_HOME:
                activeCellChanged = this->moveHome();
                break;
            case VK_END:
                activeCellChanged = this->moveEnd();
                break;
            }

            if (activeCellChanged) {
                this->_updateScrollbars();
                redraw = activeCellChanged;
            }
        }
    }

    //InformationBox(FormatMessage(L"_coordX: %d, _coordY: %d, _startCellCoordX: %d, _startCellCoordY: %d",
    //        this->_coordX, this->_coordY, this->_startCellCoordX, this->_startCellCoordY));
    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL CGrid::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
    BOOL vkLAlt, BOOL vkRAlt,
    BOOL vkLShift, BOOL vkRShift,
    BOOL vkLControl, BOOL vkRControl,
    BOOL systemKey,
    ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    return TRUE;
}

BOOL CGrid::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this && !this->_sprites.contains((LONGLONG)spriteThatCapturedMouse)) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        BOOL result = this->doChildOnMouseMove(x - this->x, y - this->y);
        if (result) {
            return TRUE;
        }

        if (this->_lMouseButtonPressed) {
        }

        return TRUE;
    } else {
        this->doChildOnMouseMove(-1, -1);
    }

    return FALSE;
}

BOOL CGrid::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this && !this->_sprites.contains((LONGLONG)spriteThatCapturedMouse)) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        BOOL result = this->doChildOnMouseDown(x - this->x, y - this->y);
        if (result) {
            return TRUE;
        }

        this->_lMouseButtonPressed = TRUE;
        this->getParentWindow()->captureMouse(this);

        // pozycjonujemy karetkę na odpowiednim znaku
        this->activateCellAtMousePos(x - this->x, y - this->y);

        if (!(this->_state & CGRID_STATE_FOCUSED)) {
            this->_state = CGRID_STATE_FOCUSED;
            this->getParentWindow()->setActiveControl(this);
        }

        this->clearSelection(FALSE);
        this->redraw();
        return TRUE;
    } else {
        this->doChildOnMouseDown(-1, -1);
    }

    return FALSE;
}

BOOL CGrid::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this && !this->_sprites.contains((LONGLONG)spriteThatCapturedMouse)) {
        return FALSE;
    }

    this->_lMouseButtonPressed = FALSE;
    if (spriteThatCapturedMouse == this) {
        this->getParentWindow()->captureMouse(NULL);
    }

    if (this->pointIn(x, y)) {
        BOOL result = this->doChildOnMouseUp(x - this->x, y - this->y);
        if (result) {
            return TRUE;
        }
        return TRUE;
    } else {
        this->doChildOnMouseUp(x - this->x, y - this->y);
    }

    return FALSE;
}

VOID CGrid::onThumbChange(LONG oldPosition, LONG curPosition, CScrollbar* scrollbar) {
    float CurPositionPercent = ((float)curPosition - scrollbar->getMinValue()) / (scrollbar->getMaxValue() - scrollbar->getMinValue());
    if (scrollbar == this->_hscrollbar) {
        this->_startCellCoordX = static_cast<LONG>(CurPositionPercent * (this->_maxAllowableStartCellCoordX-1)) + 1; //curPosition + 1;
        this->_updateGridParameters();
        this->redraw();
    }
    if (scrollbar == this->_vscrollbar) {
        this->_startCellCoordY = static_cast<LONG>(CurPositionPercent * (this->_maxAllowableStartCellCoordY-1)) + 1; //curPosition + 1;
        this->_updateGridParameters();
        this->redraw();
    }
}

ULONG CGrid::getTotalColumnsWidth() {
    LONG i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    CGridHeaderCell* headerData;
    ULONG totalWidth = 0;
    for (i = 0; i < this->_maxColumnNumber; i++) {
        headerData = (CGridHeaderCell*)this->_colData->getAt(i);
        totalWidth += headerData->size;
    }

    return totalWidth;
}

ULONG CGrid::getTotalRowsHeight() {
    LONG i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    CGridHeaderCell* headerData;
    ULONG totalHeight = 0;
    for (i = 0; i < this->_maxRowNumber; i++) {
        headerData = (CGridHeaderCell*)this->_rowData->getAt(i);
        totalHeight += headerData->size;
    }

    return totalHeight;
}

LONG CGrid::getVisibleColumnCount() {
    LONG count = (this->_maxVisibleColumnNumber - this->_startCellCoordX) + 1;
    //WriteLogMessage(L"_maxVisibleColumnNumber: %d, : _startCellCoordX: %d", this->_maxVisibleColumnNumber, this->_startCellCoordX);
    return count;
}

LONG CGrid::getFullVisibleColumnCount() {
    LONG count = (this->_maxFullVisibleColumnNumber - this->_startCellCoordX) + 1;
    return count;
}

LONG CGrid::getVisibleRowCount() {
    LONG count = (this->_maxVisibleRowNumber - this->_startCellCoordY) + 1;
    //WriteLogMessage(L"_maxVisibleRowNumber: %d, : _startCellCoordY: %d", this->_maxVisibleRowNumber, this->_startCellCoordY);
    return count;
}

LONG CGrid::getFullVisibleRowCount() {
    LONG count = (this->_maxFullVisibleRowNumber - this->_startCellCoordY) + 1;
    return count;
}

VOID CGrid::onApplyEditorChanges(CmzStdObject* value) {
    CmzString v = value->getAsString(L"text");
    this->setValue(v);
}

CmzStdObject CGrid::getValue() {
    CGridCell* c = this->getCell(this->_coordX, this->_coordY);
    CmzStdObject v;

    if (c) {
        v.set(L"text", c->value);
    }

    return v;
}

VOID CGrid::setValue(CmzString value, BOOL redraw) {
    this->setCellValue(this->_coordX, this->_coordY, value, redraw);
}

VOID CGrid::setCurrentCellValue(CmzString value, BOOL redraw) {
    this->setCellValue(this->_coordX, this->_coordY, value, redraw);
}

VOID CGrid::setCellValue(LONG cellCoordX, LONG cellCoordY, CmzString value, BOOL redraw) {
    CGridCell* c = this->getCell(cellCoordX, cellCoordY);
    if (c) {
        c->value = value;
    } else {
        c = new CGridCell(value, FALSE);
        LONGLONG first = this->_cellCoordsToLONG(cellCoordX, cellCoordY);
        this->_data.set(first, (LONGLONG)c);
    }

    if (redraw) {
        this->redraw();
    }
}

VOID CGrid::clearCurrentCellValue(BOOL redraw) {
    this->setCellValue(this->_coordX, this->_coordY, COMMON_EMPTY_STRING, redraw);
}

VOID CGrid::clearCellValue(LONG cellCoordX, LONG cellCoordY, BOOL redraw) {
    this->setCellValue(cellCoordX, cellCoordY, COMMON_EMPTY_STRING, redraw);
}

VOID CGrid::onEditorShow() {
    LONG x, y, w;
    this->cellCoordsToCanvasCoords(this->_coordX, this->_coordY, &x, &y);
    CTextField* tf = (CTextField*)this->_editor;
    this->_editor->x = x;
    this->_editor->y = y;
    w = this->width - x;
    if (this->_vscrollbar) {
        w -= this->_vscrollbar->width;
    }
    this->_editor->setSize(w - 2, this->_editor->height);
    tf->selectAll();
}

VOID CGrid::_updateScrollbars() {
    LONG pos;
    if (this->_vscrollbar) {
        pos = this->_startCellCoordY-1;
        this->_vscrollbar->setPosition(pos, FALSE, FALSE);
    }
    if (this->_hscrollbar) {
        pos = this->_startCellCoordX-1;
        this->_hscrollbar->setPosition(pos, FALSE, FALSE);
    }
}

VOID CGrid::showTopHeader(BOOL showTopHeader, BOOL redraw) {
    this->_showTopHeader = showTopHeader;
    this->_updateGridParameters();
    if (redraw) {
        this->redraw();
    }
}

VOID CGrid::showLeftHeader(BOOL showLeftHeader, BOOL redraw) {
    this->_showLeftHeader = showLeftHeader;
    this->_updateGridParameters();
    if (redraw) {
        this->redraw();
    }
}

VOID CGrid::setGridSize(LONG columnCount, LONG rowCount) {
    this->_maxColumnNumber = columnCount;
    this->_maxRowNumber = rowCount;

    if (this->_colData && this->_rowData) {
        this->_compactData();
        return;
    }

    if (!this->_colData) {
        this->_colData = new CmzArrayList(this->_maxColumnNumber);
    }
    if (!this->_rowData) {
        this->_rowData = new CmzArrayList(this->_maxRowNumber);
    }

    LONG i;
    // Inicjalizujemy właściwości komórek nagłówka górnego
    CGridHeaderCell *headerData;
    LONG index;
    for (i = 0; i < this->_maxColumnNumber; i++) {
        index = i + 1;
        headerData = new CGridHeaderCell(this->_indexToLetter(index) + L"[" + index + L"]", this->_defColumnWidth, FALSE);
        this->_colData->insert(i, (LONGLONG)headerData);
    }
    // Inicjalizujemy właściwości komórek nagłówka lewego
    for (i = 0; i < this->_maxRowNumber; i++) {
        index = i + 1;
        headerData = new CGridHeaderCell(CmzString(index), this->_theme->grid_top_header_inner_bg_normal_cell->height() + 2, FALSE);
        this->_rowData->insert(i, (LONGLONG)headerData);
    }
}

VOID CGrid::_cleanGridContent() {
    LONG i;
    CGridHeaderCell *headerCell;
    LONGLONG *items;
    ULONGLONG items_count;

    if (this->_colData) {
        items_count = this->_colData->size();
        items = this->_colData->getItemsBuf();
        for (i = 0; i < items_count; i++) {
            headerCell = (CGridHeaderCell*)items[i];
            if (headerCell) {
                delete headerCell;
            }
        }
        this->_colData->clear();
        delete this->_colData;
        this->_colData = NULL;
    }

    if (this->_rowData) {
        items_count = (LONG)this->_rowData->size();
        items = this->_rowData->getItemsBuf();
        for (i = 0; i < items_count; i++) {
            headerCell = (CGridHeaderCell*)items[i];
            if (headerCell) {
                delete headerCell;
            }
        }
        this->_rowData->clear();
        delete this->_rowData;
        this->_rowData = NULL;
    }

    if (this->_data.size() > 0) {
        this->cleanData();
    }
}

VOID CGrid::_compactData() {
    CPairExt* p;
    CGridCell* c;
    ULONGLONG size = this->_data.size();
    LONGLONG* items = this->_data.getItemsBuf();
    LONG x,y;
    ULONG i;

    for (i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p) {
            this->_LONGToCellCoords(static_cast<LONG>(p->first), &x, &y);
            if (x > this->_maxColumnNumber || y > this->_maxRowNumber) {
                c = (CGridCell*)p->second;
                delete c;
                items[i] = NULL;
            }
        }
    }
    // throw off all NULL-s
    this->_data.remove(NULL);

    BOOL removeNulls = FALSE;
    CGridHeaderCell* hc;
    size = this->_colData->size();
    items = this->_colData->getItemsBuf();
    for (i = this->_maxColumnNumber; i < size; i++) {
        hc = (CGridHeaderCell*)items[i];
        if (hc) {
            items[i] = NULL;
            delete hc;
            removeNulls = TRUE;
        }
    }
    if (removeNulls) {
        this->_colData->del(0);
    }

    removeNulls = FALSE;
    size = this->_rowData->size();
    items = this->_rowData->getItemsBuf();
    for (i = this->_maxRowNumber; i < size; i++) {
        hc = (CGridHeaderCell*)items[i];
        if (hc) {
            items[i] = NULL;
            delete hc;
            removeNulls = TRUE;
        }
    }
    if (removeNulls) {
        this->_rowData->del(0);
    }
}

VOID CGrid::_updateGridParameters() {
    LONG size;
    LONG maxAllowableSize;

    // data width
    CGridHeaderCell* hc;
    LONG i;
    ULONGLONG itemsize;
    LONGLONG* items;
    LONG curX, curY;

    this->_maxFullVisibleColumnNumber = this->_startCellCoordX;
    this->_maxVisibleColumnNumber = this->_startCellCoordX;
    this->_maxFullVisibleRowNumber = this->_startCellCoordY;
    this->_maxVisibleRowNumber = this->_startCellCoordY;
    this->_visibleWidthOfLastColumn = 0;
    this->_visibleHeightOfLastRow = 0;

    // X-based parameters
    curX = this->_showLeftHeader ? this->_leftHeaderWidth - 1 : 0;
    itemsize = this->_colData->size();
    items = this->_colData->getItemsBuf();
    size = 0;
    maxAllowableSize = this->width;
    if (this->_showLeftHeader) {
        maxAllowableSize -= this->_leftHeaderWidth;
    }
    if (this->_vscrollbar) {
        maxAllowableSize -= this->_vscrollbar->width;
    }
    for (i = this->_startCellCoordX - 1; i < itemsize; i++) {
        hc = (CGridHeaderCell*)items[i];
        if (hc) {
            if (curX + hc->size <= maxAllowableSize) {
                this->_maxFullVisibleColumnNumber = i+1;
                this->_maxVisibleColumnNumber = i+1;
            } else {
                if (curX < maxAllowableSize) {
                    this->_maxVisibleColumnNumber = i+1;
                    this->_visibleWidthOfLastColumn = maxAllowableSize - curX;
                }
            }
            size += hc->size-1; // -1 because cells share borders
            curX += hc->size-1;
            if (size > this->width) {
                break;
            }
        }
    }
    size += 1; // we count all width of the last column
    if (size > maxAllowableSize) {
        size = maxAllowableSize;
    }
    this->_dataCanvasWidth = size;




    // Y based parameters
    curY = this->_showTopHeader ? this->_topHeaderHeight - 1 : 0;
    itemsize = this->_rowData->size();
    items = this->_rowData->getItemsBuf();
    size = 0;
    maxAllowableSize = this->height;
    if (this->_showTopHeader) {
        maxAllowableSize -= this->_topHeaderHeight;
    }
    if (this->_hscrollbar) {
        maxAllowableSize -= this->_hscrollbar->height;
    }
    for (i = this->_startCellCoordY - 1; i < itemsize; i++) {
        hc = (CGridHeaderCell*)items[i];
        if (hc) {
            if (curY + hc->size <= maxAllowableSize) {
                this->_maxFullVisibleRowNumber = i+1;
                this->_maxVisibleRowNumber = i+1;
            } else {
                if (curY < maxAllowableSize) {
                    this->_maxVisibleRowNumber = i+1;
                    this->_visibleHeightOfLastRow = maxAllowableSize - curY;
                }
            }
            size += hc->size-1; // -1 because cells share borders
            curY += hc->size-1;
            if (size > this->height) {
                break;
            }
        }
    }
    size += 1; // we count all width of the last column
    if (size > maxAllowableSize) {
        size = maxAllowableSize;
    }
    this->_dataCanvasHeight = size;


    itemsize = this->_colData->size();
    items = this->_colData->getItemsBuf();
    size = 0;
    i = this->_maxColumnNumber-1;
    this->_maxAllowableStartCellCoordX = this->_maxColumnNumber;
    while (i >= 0) {
        hc = (CGridHeaderCell*)items[i];
        if (size + hc->size <= this->_dataCanvasWidth) {
            this->_maxAllowableStartCellCoordX = i+1;
            size += (hc->size-1); // -1 because cells share borders
        } else {
            break;
        }
        i -= 1;
    }

    itemsize = this->_rowData->size();
    items = this->_rowData->getItemsBuf();
    size = 0;
    i = this->_maxRowNumber - 1;
    this->_maxAllowableStartCellCoordY = this->_maxRowNumber;
    while (i >= 0) {
        hc = (CGridHeaderCell*)items[i];
        if (size + hc->size <= this->_dataCanvasHeight) {
            this->_maxAllowableStartCellCoordY = i+1;
            size += (hc->size-1); ; // -1 because cells share borders
        } else {
            break;
        }
        i -= 1;
    }

    /*
    if (this->_hscrollbar) {
        this->_hscrollbar->setMaxValue(this->_maxAllowableStartCellCoordX-1);
    }
    if (this->_vscrollbar) {
        this->_vscrollbar->setMaxValue(this->_maxAllowableStartCellCoordY-1);
    }
    */
}

VOID CGrid::_setHeaderCaptions(CmzArrayList* headerCells, CmzStringList* captions) {
    if (!headerCells || !captions) {
        return;
    }

    ULONGLONG hsize = headerCells->size();
    ULONGLONG csize = captions->size();
    CGridHeaderCell* hc;
    if (!hsize || !csize) {
        return;
    }

    for (ULONGLONG i = 0; i < hsize; i++) {
        hc = (CGridHeaderCell*)headerCells->getAt(i);
        if (hc && i < csize) {
            hc->text = captions->getAt(i);
        }
    }
}

VOID CGrid::setLeftHeaderCaptions(CmzStringList* captions) {
    this->_setHeaderCaptions(this->_rowData, captions);
}

VOID CGrid::setTopHeaderCaptions(CmzStringList* captions) {
    this->_setHeaderCaptions(this->_colData, captions);
}

BOOL CGrid::saveToFile(CmzString fname) {
    ULONG bufsize = 4096;
    BYTE* buffer = new BYTE[bufsize];

    if (buffer) {
        LONGLONG pos = 0;
        LONGLONG endpos = 0;
        ULONGLONG size = this->_data.size();
        HANDLE fh;
        if (size > 0) {
            fh = CreateFile(fname.toWCHAR(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (fh == INVALID_HANDLE_VALUE) {
                delete[] buffer;
                return FALSE;
            }

            BOOL result = TRUE;
            CPairExt* p;
            CGridCell* c;
            LONGLONG* items = this->_data.getItemsBuf();
            size_t len;
            ULONG* plBuffer;
            for (ULONG i = 0; i < size; i++) {
                p = (CPairExt*)items[i];
                if (p) {
                    c = (CGridCell*)p->second;
                    len = c->value.length();
                    endpos = pos + sizeof(p->first) + sizeof(len) + len;
                    if (endpos >= bufsize) {
                        if (AppendToFile(fh, buffer, pos) != pos) {
                            result = FALSE;
                            pos = 0;
                            break;
                        }
                        pos = 0;
                    }
                    plBuffer = (ULONG*)&buffer[pos];
                    plBuffer[0] = static_cast<ULONG>(p->first);
                    plBuffer[1] = static_cast<ULONG>(len+1);
                    wcscpy_s((WCHAR*)&plBuffer[2], bufsize - pos - sizeof(p->first) - sizeof(len), c->value.toWCHAR());
                    pos += sizeof(p->first) + sizeof(len) + 2*len;
                    buffer[pos]   = 0;
                    buffer[pos+1] = 0;
                    pos += 2;
                }
            }

            if (pos > 0) {
                if (AppendToFile(fh, buffer, pos) != pos) {
                    result = FALSE;
                }
            }
            CloseHandle(fh);
            delete[] buffer;
            return result;
        }
        delete[] buffer;
        fh = CreateFile(fname.toWCHAR(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fh != INVALID_HANDLE_VALUE) {
            CloseHandle(fh);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CGrid::loadFromFile(CmzString fname) {
    ULONG bufsize = 4096;
    BYTE* buffer = new BYTE[bufsize];
    BOOL result = FALSE;
    BOOL gridContentChanged = FALSE;
    
    if (buffer) {
        HANDLE fh = CreateFile(fname.getBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fh == INVALID_HANDLE_VALUE) {
            delete[] buffer;
            return 0;
        }

        DWORD read;
        DWORD offset;
        ULONG* plBuffer;
        LONG first;
        LONG len;
        LONG hdrDataLen = sizeof(ULONG) + sizeof(ULONG);
        ULONG dataToRead = bufsize;
        ULONG readInDataAtOffset = 0;
        ULONG dataProcessed = 0;
        do {
            ReadFile(fh, &buffer[readInDataAtOffset], dataToRead, &read, NULL);
            if (read > 0 || readInDataAtOffset > 0) {
                dataProcessed += read;
                offset = 0;
                while (offset + hdrDataLen <= read) {
                    plBuffer = (ULONG*)&buffer[offset];
                    first = plBuffer[0];
                    len = plBuffer[1]*2;
                    if (offset + hdrDataLen + len > read) {
                        len = 0;
                    }
                    if (len > 0) {
                        CGridCell* c = new CGridCell((WCHAR*)&buffer[offset+hdrDataLen], FALSE);
                        this->_data.set(first, (LONGLONG)c);
                        gridContentChanged = TRUE;
                    }
                    offset += hdrDataLen + len;
                }
                if (offset < bufsize) {
                    CopyMemory(buffer, &buffer[offset], bufsize - offset);
                    readInDataAtOffset = bufsize - offset;
                    dataToRead = bufsize - readInDataAtOffset;
                } else {
                    readInDataAtOffset = 0;
                    dataToRead = bufsize;
                }
            }
        } while (read > 0);
        CloseHandle(fh);
        result = dataProcessed == GetFileSize(fname);
    }

    if (gridContentChanged) {
        this->redraw();
    }
    
    return result;
}
