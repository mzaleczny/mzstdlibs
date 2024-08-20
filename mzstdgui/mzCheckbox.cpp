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
#include "mzCheckbox.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
#include "mzBitmap.h"
#include "mzKeyboard.h"
#include "mzStyledWindow.h"
#include "mzStdObject.h"
#include "mzMessages.h"
#include "mzApplication.h"
using namespace mzstd;

VOID CmzCheckbox::_commonInit(CmzString checkedStateText, CmzString uncheckedStateText, BOOL checked, LONG width, LONG textPlacement, LONG textSpacing) {
    this->setRealClassName(L"CmzCheckbox");
    this->_font = NULL;
    this->_textPlacement = textPlacement;
    this->_textSpacing = textSpacing;
    this->_checked = checked;
    
    this->setText(checkedStateText, uncheckedStateText, FALSE);
    LONG min_height = this->_theme->checkbox->height() + 2 + 2;
    if (min_height < this->_textSize.cy + 2 + 2) {
        min_height = this->_textSize.cy + 2 + 2;
    }
    LONG min_width = this->_theme->checkbox->width() + this->_textSpacing + this->_textSize.cx + 2 + 2;
    if (width < min_width) {
        width = min_width;
    }
    this->setSize(width, min_height);
    this->_tabStop = TRUE;
}

CmzCheckbox::CmzCheckbox(CSprite *parent, CmzString name, CmzString text, BOOL checked, LONG x, LONG y)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit(text, text, checked, 0, CHECKBOX_TEXT_PLACEMENT_RIGHT, CHECKBOX_DEFAULT_SPACING);
}

CmzCheckbox::CmzCheckbox(CSprite *parent, CmzString name, CmzString checkedStateText, CmzString uncheckedStateText, BOOL checked, LONG x, LONG y, LONG width, LONG textPlacement, LONG textSpacing)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit(checkedStateText, uncheckedStateText, checked, width, textPlacement, textSpacing);
}

CmzCheckbox::~CmzCheckbox() {
}

VOID CmzCheckbox::setText(CmzString checkedStateText, CmzString uncheckedStateText, BOOL redraw) {
    this->_checkedStateText = checkedStateText;
    this->_uncheckedStateText = uncheckedStateText;
    
    CmzString text = this->_checkedStateText;
    SIZE size = {0,0};
    this->_textSize = size;
    if (this->_font) {
        size = this->_font->measureString(text, NULL);
        this->_textSize = size;
        text = this->_uncheckedStateText;
        size = this->_font->measureString(text, NULL);
        if (size.cx > this->_textSize.cx) {
            this->_textSize.cx = size.cx;
        }
        if (size.cy > this->_textSize.cy) {
            this->_textSize.cy = size.cy;
        }
    }
    
    if (this->checked()) {
        this->_text = this->_checkedStateText;
     } else {
        this->_text = this->_uncheckedStateText;
    }
    
    this->_text.removeCharsWithCodeLessThan(32);
    if (redraw) {
        this->redraw();
    }
}

VOID CmzCheckbox::setText(CmzString text, BOOL redraw) {
    this->setText(text, text, redraw);
}

CmzString CmzCheckbox::getText() {
    return this->_text;
}

CmzFont* CmzCheckbox::getFont() {
    CmzFont* font = this->_theme->commonCheckboxControlFont;
    if (this->_font) {
        font = this->_font;
    }
    return font;
}

VOID CmzCheckbox::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 1;
    LONG y = 1;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    CmzBitmap* checkbox = NULL;

    if (this->checked()) {
        if (this->_state & CONTROL_STATE_DISABLED) {
            checkbox = this->_theme->checkbox_checked_disabled;
        } else if (this->_state & CONTROL_STATE_HOVER) {
            checkbox = this->_theme->checkbox_checked_hover;
        } else {
            checkbox = this->_theme->checkbox_checked;
        }
    } else {
        if (this->_state & CONTROL_STATE_DISABLED) {
            checkbox = this->_theme->checkbox_disabled;
        } else if (this->_state & CONTROL_STATE_HOVER) {
            checkbox = this->_theme->checkbox_hover;
        } else {
            checkbox = this->_theme->checkbox;
        }
    }
    
    LONG checkbox_width = checkbox ? checkbox->width() : 0;
    LONG checkbox_height = checkbox ? checkbox->height() : 0;
    
    CmzFont* font = this->getFont();
    SIZE size = {0,0};
    if (font) {
        size = font->measureString(this->_text, NULL);
    }
    if (this->_textPlacement == CHECKBOX_TEXT_PLACEMENT_RIGHT) {
        x = 1;
        y = (this->height - checkbox_height)/2;
    } else {
        x = 1 + size.cx + this->_textSpacing;
        y = (this->height - checkbox_height)/2;
    }
    
	this->canvas->beginPaint(hdc);
    this->canvas->setBkMode(TRANSPARENT);
	this->canvas->fillWithColor(this->_transparentColor);
    // ================================================================
    // Draw checkbox
    // ================================================================
    if (checkbox) {
        this->canvas->drawBitmap(checkbox, x, y, 0);
        x += checkbox_width + this->_textSpacing;
    }
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw focus if applied
    // ================================================================
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw label
    // ================================================================
    if (this->_textPlacement == CHECKBOX_TEXT_PLACEMENT_RIGHT) {
        x = 1;
        if (checkbox_width) {
            x += checkbox_width + this->_textSpacing;
        }
        y = 7;
    } else {
        x = 1;
        y = 7;
    }
    this->canvas->drawText(font, this->_text, x, y, TRUE);
    // ================================================================
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID CmzCheckbox::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID CmzCheckbox::focus() {
    CStyledWindow* swnd = this->getParentWindow();
    if (swnd && swnd->getActiveControl() != this) {
        swnd->setActiveControl(this);
        return;
    }
    this->addState(CONTROL_STATE_FOCUSED);
}

VOID CmzCheckbox::looseFocus() {
    CStyledWindow* wnd = this->getParentWindow();
    LONG x = MININT, y = MININT;
    wnd->setOnlyActiveControlPointer(NULL);
    this->getCurrentMousePosition(x, y);

    if (x != MININT && y != MININT) {
        if (this->pointIn(x, y)) {
            this->addState(CONTROL_STATE_HOVER);
        } else {
            this->removeState(CONTROL_STATE_HOVER);
        }
    } else {
        this->removeState(CONTROL_STATE_HOVER);
    }
}

BOOL CmzCheckbox::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        this->addState(CONTROL_STATE_HOVER);
        return TRUE;
    } else {
        this->removeState(CONTROL_STATE_HOVER);
    }

    return FALSE;
}

BOOL CmzCheckbox::onMouseDown(LONG x, LONG y) {
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
        this->toggleCheckedState();

        return TRUE;
    }

    return FALSE;
}

BOOL CmzCheckbox::onMouseUp(LONG x, LONG y) {
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

VOID CmzCheckbox::setChecked() {
    this->_checked = TRUE;
    this->_text = this->_checkedStateText;
    this->redraw();
}

VOID CmzCheckbox::setUnchecked() {
    this->_checked = FALSE;
    this->_text = this->_uncheckedStateText;
    this->redraw();
}

VOID CmzCheckbox::toggleCheckedState() {
    if (this->checked()) {
        this->setUnchecked();
    } else {
        this->setChecked();
    }
}

VOID CmzCheckbox::onClick() {
    __super::onClick();
}

BOOL CmzCheckbox::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw) {
    redraw = FALSE;

    CKeyboard* kbd = this->getKbd();

    return TRUE;
}

BOOL CmzCheckbox::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
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
            BOOL processed = FALSE;
            if (kbd) {
                if (virtualCode == VK_SPACE) {
                    this->toggleCheckedState();
                }
            }
        }
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL CmzCheckbox::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
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

BOOL CmzCheckbox::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    return TRUE;
}
