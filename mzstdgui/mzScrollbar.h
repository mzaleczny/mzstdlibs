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

#ifndef __MZSTDGUI_SCROLLBAR_H__
#define __MZSTDGUI_SCROLLBAR_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzLinkList.h"

#define CSCROLLBAR_TYPE_UNDEFINED               0L
#define CSCROLLBAR_TYPE_VERTICAL                1L
#define CSCROLLBAR_TYPE_HORIZONTAL              2L

#define CSCROLLBAR_STATE_NORMAL                 0L
#define CSCROLLBAR_STATE_SMALL_SCROLLING_UP     1L
#define CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN   2L
#define CSCROLLBAR_STATE_LARGE_SCROLLING_UP     3L
#define CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN   4L
#define CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT   5L
#define CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT  6L
#define CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT   7L
#define CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT  8L
#define CSCROLLBAR_STATE_THUMB_DRAGGING         9L

class CStyledWindow;

class DECLSPEC_MZSTDGUI CScrollbar : public CGuiControl {
public:
	WINAPI CScrollbar(CSprite *parent, mzstd::CmzString name, LONG x = 0, LONG y = 0, LONG position = 1, LONG maxValue = 100, LONG minValue = 1, BOOL autosize = FALSE);
	virtual WINAPI ~CScrollbar();

    VOID WINAPI setSteps(LONG smallStep, LONG largeStep);
    VOID WINAPI setMinValue(LONG value);
    LONG WINAPI getMinValue() const { return _minValue; }
    VOID WINAPI setMaxValue(LONG value);
    LONG WINAPI getMaxValue() const { return _maxValue; }
    VOID WINAPI stepBy(LONG step, BOOL redraw = TRUE);
    VOID WINAPI setPosition(LONG position, BOOL fireEvent = TRUE, BOOL redraw = TRUE);
    LONG WINAPI calculatePositionForStep(LONG step);
    VOID WINAPI registerPositionNotificationForSprite(CSprite* sprite);
    VOID WINAPI registerPositionNotificationForWindow(CStyledWindow* window);
    virtual VOID WINAPI setSize(LONG width, LONG height) {}
    virtual VOID WINAPI setSizeRelativeToParent(LONG parentWidth, LONG parentHeight) {}
    virtual VOID WINAPI setThumbSize() {}
    // Poniższa funkcja zwraca współrzędne (x,y) suwaka dla bieżącej pozycji w przypadku, gdy
    // parametr position jest równy MININT. Jeśli jest różny od MININT, to wyliczone są
    // współrzędne dla podanej pozycji.
    virtual VOID WINAPI calculateThumbLeftTopCorner(LONG& x, LONG& y, LONG position = MININT) {x = 0; y = 0; }
    // Wylicza najbliższą pozycję (this->_position) dla podanych współrzędnych suwaka.
    virtual LONG WINAPI calculatePositionForThumbCoords(LONG x = MININT, LONG y = MININT) { return 0; }
    virtual VOID WINAPI onDraw(CmzBitmap *dest) {}
    virtual BOOL WINAPI onMouseMove(LONG x, LONG y) { return FALSE; }
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y) { return FALSE; }
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y) { return FALSE; }
    virtual BOOL WINAPI update();

    virtual BOOL WINAPI mouseOnThumb(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnArrowUp(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnArrowDown(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnBgUp(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnBgDown(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnArrowLeft(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnArrowRight(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnBgLeft(LONG localX, LONG localY) { return FALSE; }
    virtual BOOL WINAPI mouseOnBgRight(LONG localX, LONG localY) { return FALSE; }

    virtual inline LONG WINAPI getPosition() { return this->_position; }

protected:
    LONG _position;
    LONG _minValue;
    LONG _maxValue;
    LONG _type;
    // Wielkość suwaka paska. Zależy od wielkości różnicy pomiędzy this->_minValue a this->_maxValue paska.
    LONG _thumbSize;
    // wielkość o jaką przeswany jest suwak po kliknięciu strzałki paska
    LONG _smallStep;
    // wielkość o jaką przeswany jest suwak po kliknięciu tła pod lub nad nim
    LONG _largeStep;
    // stan w który przeszliśmy podczas ostatniego wcśnięcia przycisku myszki
    LONG _stateInitiatedByMouseDown;
    // współrzędna x-owa względem lewego górnego rogu suwaka paska w momencie rozpoczęcia
    // przeciągania suwaka (kliknięcia lewym przyciskiem myszki na suwaku)
    LONG _thumbOffsetX;
    // współrzędna y-owa względem lewego górnego rogu suwaka paska w momencie rozpoczęcia
    // przeciągania suwaka (kliknięcia lewym przyciskiem myszki na suwaku)
    LONG _thumbOffsetY;

    // Listy wskaźniki na obiekty dziedziczące po klasie CSprite lub CStyledWindow, do których
    // należy wysyłać powiadomienia o zmianie pozycji suwaka. Przechowywanych tu wskaźników
    // nie zwalniamy.
    mzstd::CmzLinkList _spritesToNotify;
    mzstd::CmzLinkList _windowsToNotify;
    
    // Funkcja wywoływana po każdej zmianie wartości atrybutu this->_position
    VOID WINAPI _onPositionChanged(LONG oldPosition);

private:
    virtual VOID WINAPI _updateCanvas() {}
};
typedef CScrollbar* PScrollbar;

#endif
