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
#include "mzButton.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
#include "mzBitmap.h"
#include "mzKeyboard.h"
#include "mzStyledWindow.h"
#include "mzStdObject.h"
#include "mzMessages.h"
#include "mzApplication.h"
#include "mzImageList.h"
using namespace mzstd;

CmzButton::CmzButton(CSprite *parent, CmzString name, CmzString text, LONG x, LONG y, LONG width)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit(text, width);
}

VOID CmzButton::_commonInit(CmzString text, LONG width) {
    this->setRealClassName(L"CmzButton");
    this->_font = NULL;
    this->_state = CONTROL_STATE_NORMAL;
    this->_imageList = NULL;
    LONG min_width = this->_theme->button_left->width() + this->_theme->button_right->width() + 1;
    if (width < min_width) {
        width = min_width;
    }
    this->setSize(width, this->_theme->button_left->height());
    this->setText(text, FALSE);

    CmzFont* font = this->getFont();
    this->_tabStop = TRUE;
}

CmzButton::~CmzButton() {
}

VOID CmzButton::setText(CmzString text, BOOL redraw) {
    this->_text = text;
    this->cleanupText();
    if (redraw) {
        this->redraw();
    }
}

VOID CmzButton::cleanupText() {
    this->_text.removeCharsWithCodeLessThan(32);
}

CmzFont* CmzButton::getFont() {
    CmzFont* font = this->_theme->commonButtonControlFont;
    if (this->_font) {
        font = this->_font;
    }
    return font;
}

VOID CmzButton::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    CmzBitmap* btn_left = this->_theme->button_left;
    CmzBitmap* btn_middle = this->_theme->button_middle;
    CmzBitmap* btn_right = this->_theme->button_right;

    if (this->_state & CONTROL_STATE_DISABLED) {
        btn_left = this->_theme->button_left_disabled;
        btn_middle = this->_theme->button_middle_disabled;
        btn_right = this->_theme->button_right_disabled;
    } else if (this->_state & CONTROL_STATE_HOVER) {
        btn_left = this->_theme->button_left_hover;
        btn_middle = this->_theme->button_middle_hover;
        btn_right = this->_theme->button_right_hover;
    } else if (this->_state & CONTROL_STATE_PUSHED) {
        btn_left = this->_theme->button_left_pushed;
        btn_middle = this->_theme->button_middle_pushed;
        btn_right = this->_theme->button_right_pushed;
    }
    LONG frame_left_width = btn_left->width();
    LONG frame_right_width = btn_right->width();

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy tło button-a
    // ================================================================
    this->canvas->drawBitmap(btn_left, x, y, 0);
    x += btn_left->width();

    LONG middle_width = this->width - frame_left_width - frame_right_width;
    this->canvas->stretchBitmap(btn_middle,
        x, y,
        middle_width,
        btn_middle->height()
    );
    x += middle_width;

    this->canvas->drawBitmap(btn_right, x, y, 0);
    // ================================================================
    // Koniec rysowania tła button-a
    // ================================================================

    // ================================================================
    // Rysujemy ewentualną obwódkę wskazującą focus
    // ================================================================
    RECT selRect;
    ZeroMemory(&selRect, sizeof(selRect));
    if (this->_state & CONTROL_STATE_FOCUSED) {
    }
    // ================================================================
    // Koniec rysowania ewentualnej obwódki wskazującej focus
    // ================================================================

    // ================================================================
    // Rysujemy tekst i ewentualną ikonkę
    // ================================================================
    CmzFont* font = this->getFont();
    RECT rc = this->getRect();
    rc.left += frame_left_width;
    rc.right -= frame_right_width;

    LONG oldBkMode = this->canvas->setBkMode(TRANSPARENT);
    if (this->_state & CONTROL_STATE_PUSHED) {
        rc.left += 2;
        rc.top += 2;
    }

    LONG textDrawingAttribs = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
    if (this->_imageList) {
        LONG image_index = this->_normalStateImageIndex;
        switch (this->_state) {
        case CONTROL_STATE_HOVER:
            image_index = this->_hoverStateImageIndex;
            break;
        case CONTROL_STATE_PUSHED:
            image_index = this->_pushedStateImageIndex;
            break;
        case CONTROL_STATE_FOCUSED:
            image_index = this->_focusedStateImageIndex;
            break;
        case CONTROL_STATE_DISABLED:
            image_index = this->_disabledStateImageIndex;
            break;
        }
        // TUTAJ KOD rysujący ikonkę
        if (image_index >= 0) {
            textDrawingAttribs = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
            LONG yDelta = 0;
            if (this->_state & CONTROL_STATE_PUSHED) {
                yDelta = -1;
            }
            this->_imageList->transparentBitblt(this->canvas->getDC(), rc.left + 2, rc.top + yDelta + (this->height - this->_imageList->imHeight) / 2, image_index);
            rc.left += this->_imageList->imWidth + 4;
        }
    }

    this->canvas->drawText(font, this->_text, rc, -1, textDrawingAttribs);
    this->canvas->setBkColor(oldBkMode);
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID CmzButton::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID CmzButton::focus() {
    CStyledWindow* swnd = this->getParentWindow();
    if (swnd && swnd->getActiveControl() != this) {
        swnd->setActiveControl(this);
        return;
    }
    this->setState(CONTROL_STATE_FOCUSED);
}

VOID CmzButton::looseFocus() {
    CStyledWindow* wnd = this->getParentWindow();
    LONG x = MININT, y = MININT;
    wnd->setOnlyActiveControlPointer(NULL);
    this->getCurrentMousePosition(x, y);

    if (x != MININT && y != MININT) {
        if (this->pointIn(x, y)) {
            this->setState(CONTROL_STATE_HOVER);
        } else {
            this->setState(CONTROL_STATE_NORMAL);
        }
    } else {
        this->setState(CONTROL_STATE_NORMAL);
    }
}

BOOL CmzButton::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        if (!this->_lMouseButtonPressed) {
            this->setState(CONTROL_STATE_HOVER);
        } else {
            this->setState(CONTROL_STATE_PUSHED);
        }

        return TRUE;
    }

    if ((this->_state & CONTROL_STATE_HOVER) || (this->_state & CONTROL_STATE_PUSHED)) {
        this->setState(CONTROL_STATE_NORMAL);
    }

    return FALSE;
}

BOOL CmzButton::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        this->_lMouseButtonPressed = TRUE;
        CStyledWindow* wnd = this->getParentWindow();
        wnd->captureMouse(this);

        if (wnd->getActiveControl() != this) {
            wnd->setActiveControl(this);
        }
        this->setState(CONTROL_STATE_PUSHED);

        return TRUE;
    }

    return FALSE;
}

BOOL CmzButton::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    BOOL mouseWasPressed = this->_lMouseButtonPressed;
    this->_lMouseButtonPressed = FALSE;
    CStyledWindow* wnd = this->getParentWindow();
    wnd->captureMouse(NULL);

    if (this->pointIn(x, y)) {
        if (wnd->getActiveControl() == this) {
            this->setState(CONTROL_STATE_FOCUSED);
        } else {
            this->setState(CONTROL_STATE_HOVER);
        }

        if (mouseWasPressed) {
            this->onClick();
        }
        return TRUE;
    }


    return FALSE;
}

VOID CmzButton::onClick() {
    CStyledWindow *wnd = this->getParentWindow();

    if (wnd) {
        // tutaj kliknięto dany przycisk, więc wysyłamy odpowiednie zdarzenie
        CmzStdObject *eventInfo = new CmzStdObject();
        eventInfo->set(L"name", this->name);
        eventInfo->set(L"form_name", wnd->getName());

        CApplication* app = wnd->getApplication();
        eventInfo->set(L"event_type", static_cast<LONGLONG>(WM_BUTTON_CLICKED));
        app->pushEvent(eventInfo);
        PostMessage(wnd->getHwnd(), WM_BUTTON_CLICKED, (WPARAM)app, NULL);
    }
}

BOOL CmzButton::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw) {
    redraw = FALSE;

    CKeyboard* kbd = this->getKbd();

    return TRUE;
}

BOOL CmzButton::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
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

BOOL CmzButton::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
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

BOOL CmzButton::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    return TRUE;
}

VOID CmzButton::setImage(CmzImageList* imageList, CmzString normalStateImageName,
                                CmzString hoverStateImageName, CmzString pushedStateImageName,
                                CmzString focusedStateImageName, CmzString disabledStateImageName) {
    this->_imageList = imageList;
    if(this->_imageList) {
        this->_normalStateImageIndex = this->_imageList->getIndexForImageName(normalStateImageName);
        this->_hoverStateImageIndex = this->_imageList->getIndexForImageName(hoverStateImageName);
        this->_pushedStateImageIndex = this->_imageList->getIndexForImageName(pushedStateImageName);
        this->_focusedStateImageIndex = this->_imageList->getIndexForImageName(focusedStateImageName);
        this->_disabledStateImageIndex = this->_imageList->getIndexForImageName(disabledStateImageName);
        if (this->_hoverStateImageIndex < 0) {
            this->_hoverStateImageIndex = this->_normalStateImageIndex;
        }
        if (this->_pushedStateImageIndex < 0) {
            this->_pushedStateImageIndex = this->_normalStateImageIndex;
        }
        if (this->_focusedStateImageIndex < 0) {
            this->_focusedStateImageIndex = this->_normalStateImageIndex;
        }
        if (this->_disabledStateImageIndex < 0) {
            this->_disabledStateImageIndex = this->_normalStateImageIndex;
        }
    } else {
        this->removeImage();
    }
    this->redraw();
}

VOID CmzButton::setImage(CmzImageList* imageList, LONG normalStateImageIndex,
                                LONG hoverStateImageIndex, LONG pushedStateImageIndex,
                                LONG focusedStateImageIndex, LONG disabledStateImageIndex) {
    this->_imageList = imageList;
    if (this->_imageList) {
        this->_normalStateImageIndex = normalStateImageIndex;
        this->_hoverStateImageIndex = hoverStateImageIndex;
        this->_pushedStateImageIndex = pushedStateImageIndex;
        this->_focusedStateImageIndex = focusedStateImageIndex;
        this->_disabledStateImageIndex = disabledStateImageIndex;
        if (this->_hoverStateImageIndex < 0) {
            this->_hoverStateImageIndex = this->_normalStateImageIndex;
        }
        if (this->_pushedStateImageIndex < 0) {
            this->_pushedStateImageIndex = this->_normalStateImageIndex;
        }
        if (this->_focusedStateImageIndex < 0) {
            this->_focusedStateImageIndex = this->_normalStateImageIndex;
        }
        if (this->_disabledStateImageIndex < 0) {
            this->_disabledStateImageIndex = this->_normalStateImageIndex;
        }
    } else {
        this->removeImage();
    }
    this->redraw();
}

VOID CmzButton::removeImage() {
    this->_normalStateImageIndex = -1;
    this->_hoverStateImageIndex = -1;
    this->_pushedStateImageIndex = -1;
    this->_focusedStateImageIndex = -1;
    this->_disabledStateImageIndex = -1;
}
