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
#include "mzScrollbarHorizontal.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
#include "mzWindow.h"
#include "mzStyledWindow.h"
using namespace mzstd;

CScrollbarHorizontal::CScrollbarHorizontal(CSprite *parent, CmzString name, LONG x, LONG y, LONG position, LONG maxValue, LONG minValue, BOOL autosize)
    : CScrollbar(parent, name, x, y, position, maxValue, minValue, autosize)
{
    this->setRealClassName(L"CScrollbarHorizontal");
    this->_type = CSCROLLBAR_TYPE_HORIZONTAL;

    // w konstruktorze ustawiamy minimalną wielkość rozmiaru suwaka
    this->_thumbSize = 0;
}

CScrollbarHorizontal::~CScrollbarHorizontal() {
}

VOID CScrollbarHorizontal::setSize(LONG width, LONG height) {
    if (!this->_autosize) {
        CSprite::setSize(width, this->_theme->scrollbar_horizontal_thumb_middle->height());
        this->setThumbSize();
    }
}

VOID CScrollbarHorizontal::setSizeRelativeToParent(LONG parentWidth, LONG parentHeight) {
    if (this->_autosize) {
        CTheme* theme = this->getTheme();
        CSprite::setSize(parentWidth, this->_theme->scrollbar_horizontal_thumb_middle->height());
        this->setThumbSize();
    }
}

VOID CScrollbarHorizontal::setThumbSize(LONG ForceSize) {
    // ustawiamy wielkość suwaka
    LONG delta = this->_maxValue - this->_minValue;
    LONG thumb_size = this->width / delta;
    if (thumb_size < this->width / 5) {
        thumb_size = this->width / 5;
    }
    CTheme* theme = this->getTheme();
    if (thumb_size > this->width - this->_theme->scrollbar_horizontal_arrow_left->width() - this->_theme->scrollbar_horizontal_arrow_right->width()) {
        thumb_size = this->width - this->_theme->scrollbar_horizontal_arrow_left->width() - this->_theme->scrollbar_horizontal_arrow_right->width();
    }
    if (thumb_size < this->_theme->scrollbar_horizontal_thumb_left->width() + this->_theme->scrollbar_horizontal_thumb_middle->width() + this->_theme->scrollbar_horizontal_thumb_right->width()) {
        thumb_size = this->_theme->scrollbar_horizontal_thumb_left->width() + this->_theme->scrollbar_horizontal_thumb_middle->width() + this->_theme->scrollbar_horizontal_thumb_right->width();
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

VOID CScrollbarHorizontal::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID CScrollbarHorizontal::calculateThumbLeftTopCorner(LONG& x, LONG& y, LONG position) {
    if (position == MININT) {
        position = this->_position;
    }

    CTheme* theme = this->getTheme();
    LONG middle_width = this->width - this->_theme->scrollbar_horizontal_arrow_left->width() - this->_theme->scrollbar_horizontal_arrow_right->width();
    // musimy obliczyć położenie suwaka
    LONG x_thumb = this->_theme->scrollbar_horizontal_arrow_left->width() + (LONG)(((FLOAT)(position - this->_minValue) / (this->_maxValue - this->_minValue)) * (middle_width - this->_thumbSize));

    x = x_thumb;
    y = 0;
}

LONG CScrollbarHorizontal::calculatePositionForThumbCoords(LONG x, LONG y) {
    if (x == MININT) {
        this->calculateThumbLeftTopCorner(x, y);
    }

    CTheme* theme = this->getTheme();
    LONG middle_width = this->width - this->_theme->scrollbar_horizontal_arrow_left->width() - this->_theme->scrollbar_horizontal_arrow_right->width();
    x -= this->_theme->scrollbar_horizontal_arrow_right->width();

    FLOAT scale = (FLOAT)x / (middle_width - this->_thumbSize);
    LONG position = this->_minValue + (LONG)(scale * (this->_maxValue - this->_minValue));
    if (position < this->_minValue) {
        position = this->_minValue;
    }
    if (position > this->_maxValue) {
        position = this->_maxValue;
    }

    return position;
}

VOID CScrollbarHorizontal::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme* theme = this->getTheme();
    CmzBitmap* bgLeft = this->_theme->scrollbar_horizontal_bg;
    CmzBitmap* bgRight = this->_theme->scrollbar_horizontal_bg;
    if (this->_state == CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT) {
        bgLeft = this->_theme->scrollbar_horizontal_bg_clicked;
        bgRight = this->_theme->scrollbar_horizontal_bg;
    }
    if (this->_state == CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT) {
        bgLeft = this->_theme->scrollbar_horizontal_bg;
        bgRight = this->_theme->scrollbar_horizontal_bg_clicked;
    }

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy lewą strzałkę scrollbara
    // ================================================================
    if (this->_state == CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT) {
        this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_arrow_left_clicked, x, y);
        x += this->_theme->scrollbar_horizontal_arrow_left_clicked->width();
    } else {
        this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_arrow_left, x, y);
        x += this->_theme->scrollbar_horizontal_arrow_left->width();
    }

    // ================================================================
    // Koniec rysowania lewej strzałki scrollbara
    // ================================================================


    LONG middle_width = this->width - this->_theme->scrollbar_horizontal_arrow_left->width() - this->_theme->scrollbar_horizontal_arrow_right->width();
    // musimy obliczyć położenie suwaka
    LONG x_thumb = x + (LONG)(((FLOAT)(this->_position - this->_minValue) / (this->_maxValue - this->_minValue)) * (middle_width - this->_thumbSize));
    LONG thumb_size = this->_thumbSize;
    if (thumb_size > middle_width) {
        thumb_size = middle_width;
    }
    // ================================================================
    // Rysujemy wnętrze scrollbara
    // ================================================================
    // najpierw tło po lewej stronie suwaka
    if (x_thumb > x) {
        this->canvas->stretchBitmap(bgLeft,
            x, y,
            x_thumb - x,
            this->height
        );
    }
    x = x_thumb;

    // teraz suwak
    this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_thumb_left, x, y);
    x += this->_theme->scrollbar_horizontal_thumb_left->width();
    // tło suwaka
    LONG thumb_size_bg = thumb_size - this->_theme->scrollbar_horizontal_thumb_left->width() - this->_theme->scrollbar_horizontal_thumb_right->width();
    if (thumb_size_bg > 0) {
        this->canvas->stretchBitmap(this->_theme->scrollbar_horizontal_thumb_bg,
            x, y,
            thumb_size_bg,
            this->height
        );
        // teraz na tle suwaka centrujemy w poziomie część środkową
        this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_thumb_middle, x + (thumb_size_bg - this->_theme->scrollbar_horizontal_thumb_middle->width()) / 2, y);
        x += thumb_size_bg;
    }
    this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_thumb_right, x, y);
    x += this->_theme->scrollbar_horizontal_thumb_right->width();
    // na koniec tło po prawej stronie suwaka
    if (x < this->width - this->_theme->scrollbar_horizontal_arrow_right->width()) {
        this->canvas->stretchBitmap(bgRight,
            x, y,
            this->width - this->_theme->scrollbar_horizontal_arrow_right->width() - x,
            this->height
        );
        x += (this->width - this->_theme->scrollbar_horizontal_arrow_right->width() - x);
    }
    // ================================================================
    // Koniec rysowania wnętrza scrollbara
    // ================================================================


    // ================================================================
    // Rysujemy prawą strzałkę scrollbara
    // ================================================================
    if (this->_state == CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT) {
        if (x + this->_theme->scrollbar_horizontal_arrow_right_clicked->width() >= this->width) {
            x = this->width - this->_theme->scrollbar_horizontal_arrow_right_clicked->width();
        }
        this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_arrow_right_clicked, x, y);
    } else {
        if (x + this->_theme->scrollbar_horizontal_arrow_right->width() >= this->width) {
            x = this->width - this->_theme->scrollbar_horizontal_arrow_right->width();
        }
        this->canvas->drawBitmap(this->_theme->scrollbar_horizontal_arrow_right, x, y);
    }
    // ================================================================
    // Koniec rysowania prawej strzałki scrollbara
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

BOOL WINAPI CScrollbarHorizontal::update() {
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
    case CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT:
        step = -this->_smallStep;
        retval = TRUE;
        break;
    case CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT:
        step = this->_smallStep;
        retval = TRUE;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT:
        step = -this->_largeStep;
        if (localX != MININT && localY != MININT) {
            calculatedPosition = this->calculatePositionForStep(step);
            this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbX + this->_thumbSize < localX || thumbX + this->_thumbSize - localX < thumbMargin) {
                step /= 2;
                calculatedPosition = this->calculatePositionForStep(step);
                this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
                if (step == 0) {
                    break;
                }
            }
            if (step != -this->_largeStep) {
                if (thumbX + this->_thumbSize < localX || thumbX + this->_thumbSize - localX < thumbMargin) {
                    step = 0;
                }
                this->setState(CSCROLLBAR_STATE_NORMAL);
            }
        }
        retval = TRUE;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT:
        step = this->_largeStep;
        if (localX != MININT && localY != MININT) {
            calculatedPosition = this->calculatePositionForStep(step);
            this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbX > localX || localX - thumbX < thumbMargin) {
                step /= 2;
                calculatedPosition = this->calculatePositionForStep(step);
                this->calculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
                if (step == 0) {
                    break;
                }
            }
            if (step != this->_largeStep) {
                if (thumbX > localX || localX - thumbX < thumbMargin) {
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

BOOL WINAPI CScrollbarHorizontal::mouseOnThumb(LONG localX, LONG localY) {
    LONG _currentThumbX, _currentThumbY;
    this->calculateThumbLeftTopCorner(_currentThumbX, _currentThumbY);

    if (localX >= _currentThumbX && localX <= _currentThumbX + this->_thumbSize &&
        localY >= _currentThumbY &&
        localY <= _currentThumbY + this->_theme->scrollbar_horizontal_thumb_middle->height()) {
        return TRUE;
    }
    return FALSE;
}

BOOL WINAPI CScrollbarHorizontal::mouseOnArrowLeft(LONG localX, LONG localY) {
    CTheme* theme = this->getTheme();
    if (localX >= 0 && localX <= this->_theme->scrollbar_horizontal_arrow_left->width() &&
        localY >= 0 && localY <= this->_theme->scrollbar_horizontal_arrow_left->height()) {
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI CScrollbarHorizontal::mouseOnArrowRight(LONG localX, LONG localY) {
    CTheme* theme = this->getTheme();
    if (localX >= this->width - this->_theme->scrollbar_horizontal_arrow_right->width()  && localX <= this->width &&
        localY >= 0 && localY <= this->_theme->scrollbar_horizontal_arrow_right->height()) {
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI CScrollbarHorizontal::mouseOnBgLeft(LONG localX, LONG localY) {
    LONG _currentThumbX, _currentThumbY;
    this->calculateThumbLeftTopCorner(_currentThumbX, _currentThumbY);

    if (localX >= this->_theme->scrollbar_horizontal_arrow_left->width() &&
        localX <= _currentThumbX &&
        localY >= 0 && localY < this->_theme->scrollbar_horizontal_bg->height()) {
        return TRUE;
    }
    return FALSE;
}

BOOL WINAPI CScrollbarHorizontal::mouseOnBgRight(LONG localX, LONG localY) {
    LONG _currentThumbX, _currentThumbY;
    this->calculateThumbLeftTopCorner(_currentThumbX, _currentThumbY);

    if (localX >= _currentThumbX + this->_thumbSize &&
        localX <= this->width - this->_theme->scrollbar_horizontal_arrow_right->width() &&
        localY >= 0 &&
        localY < this->_theme->scrollbar_horizontal_bg->height()) {
        return TRUE;
    }
    return FALSE;
}

BOOL CScrollbarHorizontal::onMouseMove(LONG x, LONG y) {
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

            // jeśli jesteśmy nad strzałką w lewo, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT && this->mouseOnArrowLeft(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT);
                this->stepBy(-this->_smallStep, TRUE);
            }

            // jeśli jesteśmy nad strzałką w prawo, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT && this->mouseOnArrowRight(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT);
                this->stepBy(this->_smallStep, TRUE);
            }

            // jeśli jesteśmy nad tłem na lewo od suwaka, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT && this->mouseOnBgLeft(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT);
                this->stepBy(-this->_largeStep, TRUE);
            }

            // jeśli jesteśmy nad tłem na prawo od suwaka, to przechodzimy do odpowiedniego stanu
            if (this->_stateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT && this->mouseOnBgRight(localX, localY)) {
                this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT);
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

BOOL CScrollbarHorizontal::onMouseDown(LONG x, LONG y) {
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

        // jeśli kliknięto w strzałkę w lewo, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnArrowLeft(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT);
            this->stepBy(-this->_smallStep, TRUE);
        }

        // jeśli kliknięto w strzałkę w prawo, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnArrowRight(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT);
            this->stepBy(this->_smallStep, TRUE);
        }

        // jeśli kliknięto w tło na lewo od suwaka, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnBgLeft(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT);
            this->stepBy(-this->_largeStep, TRUE);
        }

        // jeśli kliknięto w tło na prawo od suwaka, to przechodzimy do odpowiedniego stanu
        if (this->mouseOnBgRight(localX, localY)) {
            this->setState(CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT);
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

BOOL CScrollbarHorizontal::onMouseUp(LONG x, LONG y) {
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
