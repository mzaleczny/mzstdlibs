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
#include "mzScrollbarVertical.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
#include "mzWindow.h"
#include "mzStyledWindow.h"
using namespace mzstd;

CScrollbarVertical::CScrollbarVertical(CSprite *parent, CmzString name, LONG x, LONG y, LONG position, LONG maxValue, LONG minValue, BOOL autosize)
    : CScrollbar(parent, name, x, y, position, maxValue, minValue, autosize)
{
    this->setRealClassName(L"CScrollbarVertical");
    this->_type = CSCROLLBAR_TYPE_VERTICAL;

    // w konstruktorze ustawiamy minimalną wielkość rozmiaru suwaka
    this->_thumbSize = this->_theme->scrollbar_vertical_thumb_top->height() + this->_theme->scrollbar_vertical_thumb_middle->height() + this->_theme->scrollbar_vertical_thumb_bottom->height();
}

CScrollbarVertical::~CScrollbarVertical() {
}

VOID CScrollbarVertical::setSize(LONG width, LONG height) {
    if (!this->_autosize) {
        CSprite::setSize(this->_theme->scrollbar_vertical_thumb_middle->width(), height);
        this->setThumbSize();
    }
}

VOID CScrollbarVertical::setSizeRelativeToParent(LONG parentWidth, LONG parentHeight) {
    if (this->_autosize) {
        CSprite::setSize(this->_theme->scrollbar_vertical_thumb_middle->width(), parentHeight);
        this->setThumbSize();
    }
}

VOID CScrollbarVertical::setThumbSize(LONG ForceSize) {
    // ustawiamy wielkość suwaka
    LONG delta = this->_maxValue - this->_minValue;
    LONG thumb_size = this->height / delta;
    if (thumb_size < this->height / 5) {
        thumb_size = this->height / 5;
    }
    if (thumb_size > this->height - this->_theme->scrollbar_vertical_arrow_up->height() - this->_theme->scrollbar_vertical_arrow_down->height()) {
        thumb_size = this->height - this->_theme->scrollbar_vertical_arrow_up->height() - this->_theme->scrollbar_vertical_arrow_down->height();
    }
    if (thumb_size < this->_theme->scrollbar_vertical_thumb_top->height() + this->_theme->scrollbar_vertical_thumb_middle->height() + this->_theme->scrollbar_vertical_thumb_bottom->height()) {
        thumb_size = this->_theme->scrollbar_vertical_thumb_top->height() + this->_theme->scrollbar_vertical_thumb_middle->height() + this->_theme->scrollbar_vertical_thumb_bottom->height();
    }


    if (ForceSize > 0)
    {
        this->_thumbSize = ForceSize;
    }
    else
    {
        this->_thumbSize = thumb_size;
    }
}

VOID CScrollbarVertical::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID CScrollbarVertical::calculateThumbLeftTopCorner(LONG& x, LONG& y, LONG position) {
    if (position == MININT) {
        position = this->_position;
    }

    LONG middle_height = this->height - this->_theme->scrollbar_vertical_arrow_up->height() - this->_theme->scrollbar_vertical_arrow_down->height();
    // musimy obliczyć położenie suwaka
    LONG y_thumb = this->_theme->scrollbar_vertical_arrow_up->height() + (LONG)(((FLOAT)(position - this->_minValue) / (this->_maxValue - this->_minValue)) * (middle_height - this->_thumbSize));

    x = 0;
    y = y_thumb;
}

LONG CScrollbarVertical::calculatePositionForThumbCoords(LONG x, LONG y) {
    if (y == MININT) {
        this->calculateThumbLeftTopCorner(x, y);
    }

    LONG middle_height = this->height - this->_theme->scrollbar_vertical_arrow_up->height() - this->_theme->scrollbar_vertical_arrow_down->height();
    y -= this->_theme->scrollbar_vertical_arrow_down->height();

    FLOAT scale = (FLOAT)y / (middle_height - this->_thumbSize);
    LONG position = this->_minValue + (LONG)(scale * (this->_maxValue - this->_minValue));
    if (position < this->_minValue) {
        position = this->_minValue;
    }
    if (position > this->_maxValue) {
        position = this->_maxValue;
    }

    return position;
}

VOID CScrollbarVertical::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
    CmzBitmap* bgAbove = this->_theme->scrollbar_vertical_bg;
    CmzBitmap* bgBottom = this->_theme->scrollbar_vertical_bg;
    if (this->_state == CSCROLLBAR_STATE_LARGE_SCROLLING_UP) {
        bgAbove = this->_theme->scrollbar_vertical_bg_clicked;
        bgBottom = this->_theme->scrollbar_vertical_bg;
    }
    if (this->_state == CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN) {
        bgAbove = this->_theme->scrollbar_vertical_bg;
        bgBottom = this->_theme->scrollbar_vertical_bg_clicked;
    }

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy górną strzałkę scrollbara
    // ================================================================
    if (this->_state == CSCROLLBAR_STATE_SMALL_SCROLLING_UP) {
        this->canvas->drawBitmap(this->_theme->scrollbar_vertical_arrow_up_clicked, x, y);
        y += this->_theme->scrollbar_vertical_arrow_up_clicked->height();
    } else {
        this->canvas->drawBitmap(this->_theme->scrollbar_vertical_arrow_up, x, y);
        y += this->_theme->scrollbar_vertical_arrow_up->height();
    }

    // ================================================================
    // Koniec rysowania górnej strzałki scrollbara
    // ================================================================


    LONG middle_height = this->height - this->_theme->scrollbar_vertical_arrow_up->height() - this->_theme->scrollbar_vertical_arrow_down->height();
    // musimy obliczyć położenie suwaka
    LONG y_thumb = y + (LONG)(((FLOAT)(this->_position - this->_minValue) / (this->_maxValue - this->_minValue)) * (middle_height - this->_thumbSize));
    LONG thumb_size = this->_thumbSize;
    if (thumb_size > middle_height) {
        thumb_size = middle_height;
    }
    // ================================================================
    // Rysujemy wnętrze scrollbara
    // ================================================================
    // najpierw tło nad suwakiem
    if (y_thumb > y) {
        this->canvas->stretchBitmap(bgAbove,
            x, y,
            this->width,
            y_thumb - y
        );
    }
    y = y_thumb;

    // teraz suwak
    this->canvas->drawBitmap(this->_theme->scrollbar_vertical_thumb_top, x, y);
    y += this->_theme->scrollbar_vertical_thumb_top->height();
    // tło suwaka
    LONG thumb_size_bg = thumb_size - this->_theme->scrollbar_vertical_thumb_top->height() - this->_theme->scrollbar_vertical_thumb_bottom->height();
    if (thumb_size_bg > 0) {
        // teraz na tle suwaka centrujemy w pionie część środkową
        this->canvas->stretchBitmap(this->_theme->scrollbar_vertical_thumb_bg,
            x, y,
            this->width,
            thumb_size_bg
        );
        this->canvas->stretchBitmap(this->_theme->scrollbar_vertical_thumb_middle, x, y, this->width, thumb_size_bg);
        y += thumb_size_bg;
    }
    this->canvas->drawBitmap(this->_theme->scrollbar_vertical_thumb_bottom, x, y);
    y += this->_theme->scrollbar_vertical_thumb_bottom->height();
    // na koniec tło pod suwakiem
    if (y < this->height - this->_theme->scrollbar_vertical_arrow_down->height()) {
        this->canvas->stretchBitmap(bgBottom,
            x, y,
            this->width,
            this->height - this->_theme->scrollbar_vertical_arrow_down->height() - y
        );
        y += (this->height - this->_theme->scrollbar_vertical_arrow_down->height() - y);
    }
    // ================================================================
    // Koniec rysowania wnętrza scrollbara
    // ================================================================


    // ================================================================
    // Rysujemy dolną strzałkę scrollbara
    // ================================================================
    if (this->_state == CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN) {
        if (y + this->_theme->scrollbar_vertical_arrow_down_clicked->height() >= this->height) {
            y = this->height - this->_theme->scrollbar_vertical_arrow_down_clicked->height();
        }
        this->canvas->drawBitmap(this->_theme->scrollbar_vertical_arrow_down_clicked, x, y);
    } else {
        if (y + this->_theme->scrollbar_vertical_arrow_down->height() >= this->height) {
            y = this->height - this->_theme->scrollbar_vertical_arrow_down->height();
        }
        this->canvas->drawBitmap(this->_theme->scrollbar_vertical_arrow_down, x, y);
    }
    // ================================================================
    // Koniec rysowania dolnej strzałki scrollbara
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

BOOL WINAPI CScrollbarVertical::update() {
    BOOL retval = FALSE;

    if (!this->_visible) return FALSE;
    if (this->_state == CSCROLLBAR_STATE_NORMAL || this->_state == CSCROLLBAR_STATE_THUMB_DRAGGING) {
        return FALSE;
    }

    if (__super::update()) {
        return FALSE;
    }
    
    LONG step = MININT;
    LONG localX = MININT, thumbX = MININT;
    LONG localY = MININT, thumbY = MININT;
    LONG calculatedPosition;
    LONG thumbMargin = 4;

    switch (this->_state) {
    case CSCROLLBAR_STATE_SMALL_SCROLLING_UP:
        step = -this->_smallStep;
        retval = TRUE;
        break;
    case CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN:
        step = this->_smallStep;
        retval = TRUE;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_UP:
        step = -this->_largeStep;
        if (localX != MININT && localY != MININT) {
            calculatedPosition = this->calculatePositionForStep(step);
            this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbY + this->_thumbSize < localY || thumbY + this->_thumbSize - localY < thumbMargin) {
                step /= 2;
                calculatedPosition = this->calculatePositionForStep(step);
                this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
                if (step == 0) {
                    break;
                }
            }
            if (step != -this->_largeStep) {
                if (thumbY + this->_thumbSize < localY || thumbY + this->_thumbSize - localY < thumbMargin) {
                    step = 0;
                }
                this->setState(CSCROLLBAR_STATE_NORMAL);
            }
        }
        retval = TRUE;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN:
        step = this->_largeStep;
        if (localX != MININT && localY != MININT) {
            calculatedPosition = this->calculatePositionForStep(step);
            this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbY > localY || localY - thumbY < thumbMargin) {
                step /= 2;
                calculatedPosition = this->calculatePositionForStep(step);
                this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
                if (step == 0) {
                    break;
                }
            }
            if (step != this->_largeStep) {
                if (thumbY > localY || localY - thumbY < thumbMargin) {
                    step = 0;
                }
                this->setState(CSCROLLBAR_STATE_NORMAL);
            }
        }
        retval = TRUE;
        break;
    }


    if (step != MININT) {
        this->stepBy(step, FALSE);
        if (step == 0) {
            this->redraw();
        }
    }

    if (retval) {
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI CScrollbarVertical::mouseOnThumb(LONG localX, LONG localY) {
    LONG _currentThumbX, _currentThumbY;
    this->calculateThumbLeftTopCorner(_currentThumbX, _currentThumbY);

    if (localX >= _currentThumbX &&
        localX <= _currentThumbX + this->_theme->scrollbar_vertical_thumb_middle->width() &&
        localY >= _currentThumbY && localY <= _currentThumbY + this->_thumbSize) {
        return TRUE;
    }
    return FALSE;
}

BOOL WINAPI CScrollbarVertical::mouseOnArrowUp(LONG localX, LONG localY) {
    if (localX >= 0 && localX <= this->_theme->scrollbar_vertical_arrow_up->width() &&
        localY >= 0 && localY <= this->_theme->scrollbar_vertical_arrow_up->height()) {
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI CScrollbarVertical::mouseOnArrowDown(LONG localX, LONG localY) {
    if (localX >= 0  && localX <= this->_theme->scrollbar_vertical_arrow_down->width() &&
        localY >= this->height - this->_theme->scrollbar_vertical_arrow_down->height() && localY <= this->height) {
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI CScrollbarVertical::mouseOnBgUp(LONG localX, LONG localY) {
    LONG _currentThumbX, _currentThumbY;
    this->calculateThumbLeftTopCorner(_currentThumbX, _currentThumbY);

    if (localX >= 0 &&
        localX <= this->_theme->scrollbar_vertical_bg->width() &&
        localY >= this->_theme->scrollbar_vertical_arrow_up->height() && localY < _currentThumbY) {
        return TRUE;
    }
    return FALSE;
}

BOOL WINAPI CScrollbarVertical::mouseOnBgDown(LONG localX, LONG localY) {
    LONG _currentThumbX, _currentThumbY;
    this->calculateThumbLeftTopCorner(_currentThumbX, _currentThumbY);

    if (localX >= 0 &&
        localX <= this->_theme->scrollbar_vertical_bg->width() &&
        localY >= _currentThumbY + this->_thumbSize &&
        localY < this->height - this->_theme->scrollbar_vertical_arrow_down->height()) {
        return TRUE;
    }
    return FALSE;
}

BOOL CScrollbarVertical::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    LONG oldState = this->_state;

    if (this->pointIn(x, y)) {
        // jeśli lewy przycisk myszki został wciśnięty, ale nie zwolniony, to
        if (this->_lMouseButtonPressed) {
            LONG localX = x - this->x;
            LONG localY = y - this->y;

            // jeśli jesteśmy nad suwakiem, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_THUMB_DRAGGING && this->mouseOnThumb(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            }

            // jeśli przeciągamy suwak, to zmieniamy jego położenie
            if (this->_state == CSCROLLBAR_STATE_THUMB_DRAGGING) {
                LONG position = this->calculatePositionForThumbCoords(localX - this->_thumbOffsetX, localY - this->_thumbOffsetY);
                this->setPosition(position, TRUE, TRUE);
            }

            // jeśli jesteśmy nad strzałką do góry, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_UP && this->mouseOnArrowUp(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_UP);
                this->stepBy(-this->_smallStep, TRUE);
            }

            // jeśli jesteśmy nad strzałką w dół, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN && this->mouseOnArrowDown(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN);
                this->stepBy(this->_smallStep, TRUE);
            }

            // jeśli jesteśmy nad tłem nad suwakiem, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_UP && this->mouseOnBgUp(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_UP);
                this->stepBy(-this->_largeStep, TRUE);
            }

            // jeśli jesteśmy nad tłem pod suwakiem, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN && this->mouseOnBgDown(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN);
                this->stepBy(this->_largeStep, TRUE);
            }
        }
        return TRUE;
    }

    // jeśli przeciągamy suwak i jesteśmy poza nim, to i tak obsługujemy to zdarzenie
    if (this->_state == CSCROLLBAR_STATE_THUMB_DRAGGING) {
        LONG localX = x - this->x;
        LONG localY = y - this->y;
        LONG position = this->calculatePositionForThumbCoords(localX - this->_thumbOffsetX, localY - this->_thumbOffsetY);
        this->setPosition(position, TRUE, TRUE);
        return TRUE;
    }

    this->setState(CSCROLLBAR_STATE_NORMAL);
    if (this->_state != oldState) {
        this->redraw();
    }

    return FALSE;
}

BOOL CScrollbarVertical::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    LONG localX = x - this->x;
    LONG localY = y - this->y;

    if (this->pointIn(x, y)) {
        this->_lMouseButtonPressed = TRUE;
        this->getParentWindow()->captureMouse(this);

        // jeśli kliknięto w strzałkę do góry, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnArrowUp(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_UP);
            this->stepBy(-this->_smallStep, TRUE);
        }

        // jeśli kliknięto w strzałkę w dół, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnArrowDown(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN);
            this->stepBy(this->_smallStep, TRUE);
        }

        // jeśli kliknięto w tło nad suwakiem, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnBgUp(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_UP);
            this->stepBy(-this->_largeStep, TRUE);
        }

        // jeśli kliknięto w tło pod suwakiem, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnBgDown(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN);
            this->stepBy(this->_largeStep, TRUE);
        }

        // jeśli kliknięto w suwak, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnThumb(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            this->calculateThumbLeftTopCorner(this->_thumbOffsetX, this->_thumbOffsetY);
            this->_thumbOffsetX = localX - this->_thumbOffsetX;
            this->_thumbOffsetY = localY - this->_thumbOffsetY;
        }

        this->_stateInitiatedByMouseDown = this->_state;
        return TRUE;
    }

    return FALSE;
}

BOOL CScrollbarVertical::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    this->_lMouseButtonPressed = FALSE;
    this->getParentWindow()->captureMouse(NULL);
    if (this->_state != CSCROLLBAR_STATE_NORMAL) {
        this->redraw();
    }
    this->setState(CSCROLLBAR_STATE_NORMAL);
    this->_stateInitiatedByMouseDown = CSCROLLBAR_STATE_NORMAL;
    if (this->pointIn(x, y)) {
        return TRUE;
    }
    return FALSE;
}
