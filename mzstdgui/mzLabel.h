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

#ifndef __MZSTDGUI_LABEL_H__
#define __MZSTDGUI_LABEL_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzFont.h"
#include "mzStringList.h"


#define CLABEL_BORDER_TYPE_NONE                 0L
#define CLABEL_BORDER_TYPE_SOLID                1L
#define CLABEL_BORDER_TYPE_BEVEL_RAISEDINNER    2L
#define CLABEL_BORDER_TYPE_BEVEL_SUNKENINNER    3L
#define CLABEL_BORDER_TYPE_BEVEL_RAISEDOUTER    4L
#define CLABEL_BORDER_TYPE_BEVEL_SUNKENOUTER    5L
#define CLABEL_BORDER_TYPE_BEVEL_BUMP           6L
#define CLABEL_BORDER_TYPE_BEVEL_ETCHED         7L
#define CLABEL_BORDER_TYPE_BEVEL_RAISED         8L
#define CLABEL_BORDER_TYPE_BEVEL_SUNKEN         9L


class DECLSPEC_MZSTDGUI CLabel : public CGuiControl {
public:
	WINAPI CLabel(CSprite *parent, mzstd::CmzString name, LONG x = 0, LONG y = 0, mzstd::CmzString text = L"", LONG width = 0, LONG height = 0, BOOL autosize = FALSE);
	virtual WINAPI ~CLabel();

    VOID WINAPI setText(mzstd::CmzString& text, BOOL redraw = TRUE);
    mzstd::CmzString WINAPI getText();
    CmzFont* WINAPI getFont();
    virtual VOID WINAPI setSize(LONG width, LONG height);
    inline VOID WINAPI setTransparentBg() { this->_bgColor = NULL; }
    inline VOID WINAPI setBgColor(COLORREF color, BOOL redraw = TRUE) {
        this->_bgColor = color;
        if (redraw) {
            this->redraw();
        }
    }
    inline VOID WINAPI setBorderColor(COLORREF color, BOOL redraw = TRUE) {
        this->_borderColor = color;
        if (redraw) {
            this->redraw();
        }
    }
    VOID WINAPI setBorder(LONG borderType, BOOL redraw = TRUE);
    LONG WINAPI getBorderSize();
    VOID WINAPI setPadding(ULONG padding, BOOL redraw = TRUE);

    virtual VOID WINAPI onDraw(CmzBitmap *dest);
    virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

protected:
    mzstd::CmzStringList* _lines;
    CmzFont* _font;
    // jeśli poniższa zmienna jest = NULL, to labelka jest transparentna (domyślnie)
    COLORREF _bgColor;
    // wysokość pojedyńczej linii tekstu
    LONG _lineHeight;
    // typ obramowania, domyślnie brak
    LONG _borderType;
    // kolor obramowania w przypadku typu: CLABEL_BORDER_TYPE_SOLID
    COLORREF _borderColor;
    // Paddingi od krawędzi
    ULONG _paddingLeft;
    ULONG _paddingRight;
    ULONG _paddingTop;
    ULONG _paddingBottom;
    // Szerokość i wysokość labelki niezbędna do wyświeltenia wszystkich potencjalnych
    // linii ustawionego tekstu. Wartości te są ustawiane przy każdorazowym ustawianiu tekstu.
    LONG _baseWidth;
    LONG _baseHeight;

    // Zwraca wartość parametru 'edge' przekazywanego do funkcji DrawEdge, odpowiadającą bieżącemu
    // typowi obramowania.
    LONG WINAPI _getSdkEdgeTypeForCurrentBorderType();

private:
    virtual VOID WINAPI _updateCanvas();
};
typedef CLabel* PLabel;

#endif
