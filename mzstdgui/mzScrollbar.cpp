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
#include "mzScrollbar.h"
#include "mzstdguiGlobals.h"
#include "mzApplication.h"
#include "mzTheme.h"
#include "mzStyledWindow.h"
using namespace mzstd;

CScrollbar::CScrollbar(CSprite *parent, CmzString name, LONG x, LONG y, LONG position, LONG maxValue, LONG minValue, BOOL autosize)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->setRealClassName(L"CScrollbar");
    this->_type = CSCROLLBAR_TYPE_UNDEFINED;
    this->_state = CSCROLLBAR_STATE_NORMAL;
    this->_stateInitiatedByMouseDown = CSCROLLBAR_STATE_NORMAL;

    this->_autosize = autosize;
    this->_smallStep = 1;
    this->_largeStep = 10;
    this->_lMouseButtonPressed = FALSE;

    this->_minValue = minValue;
    if (maxValue > minValue) {
        this->_maxValue = maxValue;
    } else {
        this->_maxValue = minValue + 1;
    }
    if (position >= this->_minValue && position <= this->_maxValue) {
        this->_position = position;
    } else {
        this->_position = this->_minValue;
    }

    // w konstruktorze ustawiamy minimalną wielkość rozmiaru suwaka
    this->_thumbSize = 0;
    
    LONG delta = this->_maxValue - this->_minValue;
    if (this->_largeStep >= delta) {
        this->_largeStep = delta / 2;
    }
}

CScrollbar::~CScrollbar() {
    this->_spritesToNotify.clear();
    this->_windowsToNotify.clear();
}

VOID WINAPI CScrollbar::registerPositionNotificationForSprite(CSprite* sprite) {
    this->_spritesToNotify.push((LONGLONG)sprite);
}

VOID WINAPI CScrollbar::registerPositionNotificationForWindow(CStyledWindow* window) {
    this->_windowsToNotify.push((LONGLONG)window);
}

VOID WINAPI CScrollbar::setMinValue(LONG value) {
    this->_minValue = value;
    this->setThumbSize();
}

VOID WINAPI CScrollbar::setMaxValue(LONG value) {
    this->_maxValue = value;
    this->setThumbSize();
}

VOID WINAPI CScrollbar::_onPositionChanged(LONG oldPosition) {
    if (this->_spritesToNotify.size() < 1 && this->_windowsToNotify.size() < 1) {
        return;
    }

    CSprite* sprite;
    CWindow* window;

    // Najpierw wysyłamy powiadomienia do okienek
    CmzLinkListNode* n = this->_windowsToNotify.first();
    while (n != NULL) {
        window = (CWindow*)n->data;
        if (window) {
            window->onThumbChange(oldPosition, this->_position, this);
        }
        n = n->next;
    }

    // wysyłamy powiadomienia do sprit'ów
    n = this->_spritesToNotify.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite) {
            CGuiControl* gc = dynamic_cast<CGuiControl*>(sprite);
            if (gc) {
                gc->onThumbChange(oldPosition, this->_position, this);
            }
        }
        n = n->next;
    }
}

LONG WINAPI CScrollbar::calculatePositionForStep(LONG step) {
    LONG position = this->_position;

    if (step < 0) {
        if (position + step < this->_minValue) {
            position = this->_minValue;
        } else {
            position += step;
        }
    } else {
        if (position + step > this->_maxValue) {
            position = this->_maxValue;
        } else {
            position += step;
        }
    }

    return position;
}

VOID WINAPI CScrollbar::stepBy(LONG step, BOOL redraw) {
    if (step == 0) {
        return;
    }

    LONG oldPosition = this->_position;
    this->_position = this->calculatePositionForStep(step);

    if (this->_position != oldPosition) {
        this->_onPositionChanged(oldPosition);
        this->invalidate();
        if (redraw) {
            this->redraw();
        }
    }
}

VOID WINAPI CScrollbar::setPosition(LONG position, BOOL fireEvent, BOOL redraw) {
    if (this->_position == position) {
        return;
    }

    if (position < this->_minValue) {
        position = this->_minValue;
    }
    if (position > this->_maxValue) {
        position = this->_maxValue;
    }

    LONG oldPosition = this->_position;
    this->_position = position;

    if (fireEvent) {
        this->_onPositionChanged(oldPosition);
    }
    this->invalidate();
    if (redraw) {
        this->redraw();
    }
}

BOOL WINAPI CScrollbar::update() {
    BOOL retval = TRUE;
    
    static LONG lastTick = ::application->getCurTickCount();
    LONG curTick = ::application->getCurTickCount();
    LONG delta = curTick - lastTick;
    
    if (delta < 100) {
        return TRUE;
    }

    lastTick = curTick;
    return FALSE;
}

VOID WINAPI CScrollbar::setSteps(LONG smallStep, LONG largeStep) {
    this->_smallStep = smallStep;
    this->_largeStep = largeStep;
}
