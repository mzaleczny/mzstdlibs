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

#ifndef __MZSTDGUI_SCROLLBAR_VERTICAL_H__
#define __MZSTDGUI_SCROLLBAR_VERTICAL_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzScrollbar.h"


class DECLSPEC_MZSTDGUI CScrollbarVertical : public CScrollbar {
public:
	WINAPI CScrollbarVertical(CSprite *parent, mzstd::CmzString name, LONG x = 0, LONG y = 0, LONG position = 1, LONG maxValue = 100, LONG minValue = 1, BOOL autosize = FALSE);
	virtual WINAPI ~CScrollbarVertical();

    virtual BOOL WINAPI update();
    virtual VOID WINAPI setSize(LONG width, LONG height);
    virtual VOID WINAPI setSizeRelativeToParent(LONG parentWidth, LONG parentHeight);
    virtual VOID WINAPI setThumbSize(LONG ForceSize = 0);
    virtual VOID WINAPI calculateThumbLeftTopCorner(LONG& x, LONG& y, LONG position = MININT);
    // Wylicza najbliższą pozycję (this->_position) dla podanych współrzędnych suwaka. Jeśli
    // współrzędna y jest równa MININT, to wyliczana jest pozycja dla bieżącego położenia
    // suwaka.
    virtual LONG WINAPI calculatePositionForThumbCoords(LONG x = MININT, LONG y = MININT);
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
    virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

    virtual BOOL WINAPI mouseOnThumb(LONG localX, LONG localY);
    virtual BOOL WINAPI mouseOnArrowUp(LONG localX, LONG localY);
    virtual BOOL WINAPI mouseOnArrowDown(LONG localX, LONG localY);
    virtual BOOL WINAPI mouseOnBgUp(LONG localX, LONG localY);
    virtual BOOL WINAPI mouseOnBgDown(LONG localX, LONG localY);

private:
    virtual VOID WINAPI _updateCanvas();
};
typedef CScrollbarVertical* PScrollbarVertical;

#endif
