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
#include "mzTextField.h"
#include "mzstdguiGlobals.h"
#include "mzstd.h"
#include "mzstdwin32.h"
#include "mzstdgraphics.h"
#include "mzApplication.h"
#include "mzTheme.h"
#include "mzStyledWindow.h"
#include "mzKeyboard.h"
#include "mzClipboard.h"
using namespace mzstd;

CTextField::CTextField(CSprite *parent, CmzString name, LONG x, LONG y, LONG width, BOOL tabStop)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit(CmzString(L""), width, tabStop);
}

VOID CTextField::_commonInit(CmzString text, LONG width, BOOL tabStop) {
    this->setRealClassName(L"CTextField");
    CStyledWindow* wnd = this->getParentWindow();
    this->_caret = wnd->getCaret();
    this->_clipboard = wnd->getApplication()->clipboard;

    this->_font = NULL;
    this->_state = CTEXTFIELD_STATE_NORMAL;
    this->_startChar = 0;
    this->_caretAtChar = 0;
    this->_tabStop = tabStop;
    LONG min_width = this->getMinWidth();
    if (width < min_width) {
        width = min_width;
    }
    this->setSize(width, this->_theme->textfield_frame_left->height());
    this->setText(text, FALSE);

    CmzFont* font = this->getFont();
    HDC hdc = this->canvas->getDC();
    if (hdc == 0) {
        this->canvas->beginPaint();
    }
    this->_lineHeight = font->measureString(COMMON_MEASURE_STRING, this->canvas->getDC()).cy;
    if (hdc == 0) {
        this->canvas->endPaint();
    }

    this->_selStart = 0;
    this->_selEnd = 0;
    this->_selBegin = 0;

    this->_tabStop = TRUE;
}

CTextField::~CTextField() {
}

LONG CTextField::getMinWidth() {
    return this->_theme->textfield_frame_left->width() + 1 + this->_theme->textfield_frame_right->width();
}

VOID CTextField::setText(CmzString text, BOOL redraw) {
    this->_text = text;
    this->cleanupText();
    this->_startChar = 0;
    this->_caretAtChar = 0;
    if (redraw) {
        this->redraw();
    }
}

VOID CTextField::cleanupText() {
    this->_text.removeCharsWithCodeLessThan(32);
}

CmzFont* CTextField::getFont() {
    CmzFont* font = this->_theme->commonTextControlFont;
    if (this->_font) {
        font = this->_font;
    }
    return font;
}

VOID CTextField::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    CmzBitmap* frame_left = this->_theme->textfield_frame_left;
    CmzBitmap* frame_middle = this->_theme->textfield_frame_middle;
    CmzBitmap* frame_right = this->_theme->textfield_frame_right;

    if (this->_state & CTEXTFIELD_STATE_HOVER) {
        frame_left = this->_theme->textfield_frame_left_hover;
        frame_middle = this->_theme->textfield_frame_middle_hover;
        frame_right = this->_theme->textfield_frame_right_hover;
    } else if (this->_state & CTEXTFIELD_STATE_ACTIVE) {
        frame_left = this->_theme->textfield_frame_left_active;
        frame_middle = this->_theme->textfield_frame_middle_active;
        frame_right = this->_theme->textfield_frame_right_active;
    }
    LONG frame_left_width = frame_left->width();

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy tło textfield-a
    // ================================================================
    this->canvas->drawBitmap(frame_left, x, y);
    x += frame_left->width();

    LONG middle_width = this->width - frame_left_width - frame_right->width();
    this->canvas->stretchBitmap(frame_middle,
        x, y,
        middle_width,
        frame_middle->height()
    );
    x += middle_width;

    this->canvas->drawBitmap(frame_right, x, y);
    // ================================================================
    // Koniec rysowania tła textfield-a
    // ================================================================

    // ================================================================
    // Rysujemy zaznaczenie
    // ================================================================
    RECT selRect;
    ZeroMemory(&selRect, sizeof(selRect));
    if (this->_selStart < this->_selEnd) {
        selRect = this->_calculateSelectionRectForText(&this->_text);
        if (selRect.left < selRect.right) {
            this->canvas->stretchBitmap(this->_theme->textfield_selection,
                selRect.left, selRect.top,
                selRect.right - selRect.left,
                selRect.bottom - selRect.top
            );
        }
    }
    // ================================================================
    // Koniec rysowania zaznaczenia
    // ================================================================

    // ================================================================
    // Rysujemy tekst
    // ================================================================
    CmzFont* font = this->getFont();
    RECT rc = this->getRect();
    rc.left += frame_left_width;
    rc.right = this->_getMaxXPosAllowedForContent();
    LONG oldBkMode = this->canvas->setBkMode(TRANSPARENT);
    if (selRect.left == selRect.right) {
        LONG last_char_pos = this->getLastVisibleCharPos();
        last_char_pos += 1; // zwiększamy o 1, żeby ewentualnie wyświetlić fragment następnej
        // litery w wyznaczonym prostokącie
        CmzString s = this->_text.substr(this->_startChar, last_char_pos - this->_startChar + 1);
        font->color = this->_theme->commonTextControlNormalFontColor;
        this->canvas->drawText(font, s, rc, TRUE, DT_VCENTER | DT_NOPREFIX);
    } else {
        font->color = this->_theme->commonTextControlNormalFontColor;
        LONG last_char_pos = this->getLastVisibleCharPos();
        LONG char_pos = this->getLastVisibleCharPos(selRect.left - frame_left_width);
        size_t drawedChars = 0;
        CmzString s;
        // najpierw tekst przed zaznaczeniem
        if (char_pos >= 0 && char_pos - this->_startChar + 1 > 0) {
            s = this->_text.substr(this->_startChar, char_pos - this->_startChar + 1);
            drawedChars = s.length();
            font->color = this->_theme->commonTextControlNormalFontColor;
            this->canvas->drawText(font, s, rc, TRUE, DT_VCENTER | DT_NOPREFIX);
        }
        char_pos = this->getLastVisibleCharPos(selRect.right - frame_left_width);
        // następnie tekst w zaznaczeniu
        if (char_pos >= 0 && char_pos - this->_startChar - drawedChars + 1 > 0) {
            // jeśli zaznaczenie dobiega do końca pola tekstowego
            if (selRect.right == this->_getMaxXPosAllowedForContent()) {
                char_pos  += 1; // to zwiększamy char_pos, żeby wyświetlić fragment następnej
                // litery, która się cała nie mieści ale zaczyna się w wyznaczonym prostokącie
            }
            s = this->_text.substr(this->_startChar + drawedChars, char_pos - this->_startChar - drawedChars + 1);
            drawedChars += s.length();
            font->color = this->_theme->commonTextControlSelectedFontColor;
            rc.left = selRect.left;
            this->canvas->drawText(font, s, rc, TRUE, DT_VCENTER | DT_NOPREFIX);
        } else if (char_pos > 0 && selRect.right > selRect.left) {
            s = this->_text.substr(this->_startChar + drawedChars, 1);
            font->color = this->_theme->commonTextControlSelectedFontColor;
            rc.left = selRect.left;
            this->canvas->drawText(font, s, rc, TRUE, DT_VCENTER | DT_NOPREFIX);
        }
        char_pos = last_char_pos;
        char_pos += 1; // zwiększamy o 1 żeby wyświetlić fragment ostatniej litery
        // i na koniec tekst po zaznaczeniu
        if (char_pos >= 0 && char_pos - this->_startChar - drawedChars + 1 > 0) {
            s = this->_text.substr(this->_startChar + drawedChars, char_pos - this->_startChar - drawedChars + 1);
            font->color = this->_theme->commonTextControlNormalFontColor;
            rc.left = selRect.right;
            if (rc.right - rc.left > 0) {
                this->canvas->drawText(font, s, rc, TRUE, DT_VCENTER | DT_NOPREFIX);
            }
        }
    }
    this->canvas->setBkColor(oldBkMode);
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID CTextField::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID CTextField::focus() {
    CStyledWindow* swnd = this->getParentWindow();
    if (swnd && swnd->getActiveControl() != this) {
        swnd->setActiveControl(this);
        return;
    }

    this->removeState(CTEXTFIELD_STATE_NORMAL);
    this->removeState(CTEXTFIELD_STATE_HOVER);
    this->addState(CTEXTFIELD_STATE_ACTIVE);

    if (this->_caret) {
        this->_caret->height = this->getCaretHeight();
        this->updateCaretPos();
        this->_caret->update(0);
        this->_caret->show();

        if (this->_caret->isVisible()) {
            this->getParentWindow()->drawCaret(this->getParentWindow()->getCanvas()->getDC());
        }

        HWND hwnd = this->getParentWindow()->getHwnd();
        HDC hdc = GetDC(hwnd);
        this->_caret->draw(hdc);
        ReleaseDC(hwnd, hdc);
        this->_caret->resetLastFlippedTime();
    }
}

VOID CTextField::looseFocus() {
    CStyledWindow* wnd = this->getParentWindow();
    LONG x = MININT, y = MININT;
    wnd->setOnlyActiveControlPointer(NULL);
    this->getCurrentMousePosition(x, y);
    this->clearSelection(FALSE);

    if (x != MININT && y != MININT) {
        if (this->pointIn(x, y)) {
            this->setState(CTEXTFIELD_STATE_HOVER);
        } else {
            this->setState(CTEXTFIELD_STATE_NORMAL);
        }
    } else {
        this->setState(CTEXTFIELD_STATE_NORMAL);
    }
}

BOOL CTextField::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->_lMouseButtonPressed) {
        this->addState(CTEXTFIELD_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS);
    }

    if (this->pointIn(x, y)) {
        this->_cursor->setIBeamCursor();
        if (this->_state & CTEXTFIELD_STATE_NORMAL) {
            this->removeState(CTEXTFIELD_STATE_NORMAL);
            this->addState(CTEXTFIELD_STATE_HOVER);
        }

        return TRUE;
    }

    if (this->_state & CTEXTFIELD_STATE_HOVER) {
        this->removeState(CTEXTFIELD_STATE_HOVER);
        this->addState(CTEXTFIELD_STATE_NORMAL);
    }

    return FALSE;
}

BOOL CTextField::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        this->_cursor->setIBeamCursor();
        this->_lMouseButtonPressed = TRUE;
        this->getParentWindow()->captureMouse(this);

        // pozycjonujemy karetkę na odpowiednim znaku
        this->_positionCaretNearClickedPoint(x - this->x, y - this->y);

        if (!(this->_state & CTEXTFIELD_STATE_ACTIVE)) {
            this->getParentWindow()->setActiveControl(this);
        }

        this->clearSelection(FALSE);
        this->redraw();
        return TRUE;
    }

    return FALSE;
}

BOOL CTextField::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    this->_lMouseButtonPressed = FALSE;
    this->removeState(CTEXTFIELD_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS);
    this->getParentWindow()->captureMouse(NULL);

    if (this->pointIn(x, y)) {
        this->_cursor->setIBeamCursor();
        return TRUE;
    }


    return FALSE;
}

VOID CTextField::_positionCaretNearClickedPoint(LONG localX, LONG localY) {
    if (this->_text.length() > 0) {
        size_t strLen = this->_text.length();
        CmzString tmp;
        CmzFont* font = this->getFont();
        SIZE si;
        si.cx = 0;
        LONG count = 0;
        // poniższe dwie zmienne służą do określenia czy kursor powinien się znajdować
        // przed czy po klikniętej literze (w zależności czy kliknięto bliżej jej
        // początku czy końca). Jeśli kliknięto między literami, to kursor jest lokowany
        // dokładnie między nimi.
        LONG delta = localX - (si.cx + this->_theme->textfield_frame_left->width()), lastDelta = localX - (si.cx + this->_theme->textfield_frame_left->width());
        tmp = "";
        HDC hdc = this->canvas->getDC();
        if (hdc == 0) {
            this->canvas->beginPaint();
        }
        while ((size_t)(this->_startChar + count) < strLen && si.cx + this->_theme->textfield_frame_left->width() < localX) {
            count += 1;
            tmp = this->_text.substr(this->_startChar, count);
            si = font->measureString(tmp, this->canvas->getDC());
            lastDelta = delta;
            delta = localX - (si.cx + this->_theme->textfield_frame_left->width());
        }
        if (hdc == 0) {
            this->canvas->endPaint();
        }
        if (lastDelta < 0) {
            lastDelta = -lastDelta;
        }
        if (delta < 0) {
            delta = -delta;
        }
        if (count > 0 && (lastDelta < delta)) {
            count--;
        }
        this->_caretAtChar = this->_startChar + count;
        this->updateCaretPos();
    }
}

POINT CTextField::calculateCaretPos() {
    LONG lettersBeforeCaret = this->_caretAtChar - this->_startChar;
    SIZE size;
    POINT pt;
    ZeroMemory(&size, sizeof(size));
    ZeroMemory(&pt, sizeof(pt));

    if (lettersBeforeCaret > 0) {
        CmzString s = this->_text.substr(this->_startChar, lettersBeforeCaret);
        CmzFont* font = this->getFont();

        HDC hdc = this->canvas->getDC();
        if (hdc == 0) {
            this->canvas->beginPaint();
        }
        size = font->measureString(s, this->canvas->getDC());
        if (hdc == 0) {
            this->canvas->endPaint();
        }
    }

    if (this->_caret) {
        pt.x = this->_theme->textfield_frame_left->width() + size.cx;
        pt.y = (this->height - this->_caret->height) / 2;
    }

    return pt;
}

POINT CTextField::calculateCharPos(LONG currentChar) {
    SIZE size;
    POINT pt;
    ZeroMemory(&pt, sizeof(pt));
    ZeroMemory(&size, sizeof(size));

    if (currentChar < this->_startChar) {
        pt.x = -1;
        pt.y = -1;
        return pt;
    }

    LONG lettersBefore = currentChar - this->_startChar;

    if (lettersBefore > 0) {
        CmzString s = this->_text.substr(this->_startChar, lettersBefore);
        CmzFont* font = this->getFont();
        HDC hdc = this->canvas->getDC();
        if (hdc == 0) {
            this->canvas->beginPaint();
        }
        size = font->measureString(s, this->canvas->getDC());
        if (hdc == 0) {
            this->canvas->endPaint();
        }
    }

    pt.x = this->_theme->textfield_frame_left->width() + size.cx;
    pt.y = (this->height - this->_theme->textfield_selection->height()) / 2;

    if (pt.x < this->_theme->textfield_frame_left->width()) {
        pt.x = -1;
        pt.y = -1;
    }

    if (pt.x > this->_getMaxXPosAllowedForContent()) {
        pt.x = -2;
        pt.y = -2;
    }

    return pt;
}

VOID CTextField::updateCaretPos() {
    LONG x;
    LONG y;
    LONG controlX;
    LONG controlY;
    CStyledWindow* wnd;

    wnd = this->getParentWindow();
    if (wnd) {
        wnd->getSpritePosition(this, x, y);

        POINT pt = this->calculateCaretPos();

        controlX = pt.x;
        controlY = pt.y;

        x += controlX;
        y += controlY;

        this->_caret->x = x;
        this->_caret->y = y;
        this->_caret->controlX = controlX;
        this->_caret->controlY = controlY;
        this->_caret->show();
    }
}

VOID CTextField::_updateCursorPosition(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl, ULONG vkKey, BOOL& updateCaretPos, BOOL& redraw) {
    updateCaretPos = FALSE;
    redraw = FALSE;

    size_t strLen = this->_text.length();
    if (strLen < 1) {
        return;
    }

    POINT pt;
    LONG inner_width = this->_calculateInnerWidth();


    if (vkKey == VK_RIGHT) {
        // przetwarzamy zdarzenie jeśli jeszcze nie jesteśmy na końcu tekstu
        if (this->_caretAtChar < strLen) {
            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, po którym znajduje się znak nie-alfanumeryczny lub na koniec
            // tekstu jeśli po bieżącej pozycji są wyłącznie znaki alfanumeryczne
            if (vkControl) {
                this->_caretAtChar += 1;
                while (this->_caretAtChar < strLen && isCharAlphanumeric(this->_text.getCharAt(this->_caretAtChar))) {
                    this->_caretAtChar += 1;
                }
            } else {
                this->_caretAtChar += 1;
            }
            pt = this->calculateCaretPos();
            while (pt.x + this->_caret->width > this->_getMaxXPosAllowedForContent()) {
                this->_startChar += 3;
                pt = this->calculateCaretPos();
                redraw = TRUE;
            }
            updateCaretPos = TRUE;
        }
        return;
    }

    if (vkKey == VK_LEFT) {
        // jeśli nie jesteśmy na początku tekstu, to przetwarzamy zdarzenie
        if (this->_caretAtChar > 0) {
            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, przed którym znajduje się znak nie-alfanumeryczny lub na początek
            // tekstu jeśli przed bieżącą pozycją są wyłącznie znaki alfanumeryczne
            if (vkControl) {
                this->_caretAtChar -= 1;
                while (this->_caretAtChar > 0 && isCharAlphanumeric(this->_text.getCharAt(this->_caretAtChar - 1))) {
                    this->_caretAtChar -= 1;
                }
            } else {
                this->_caretAtChar -= 1;
            }
            if (this->_caretAtChar <= this->_startChar) {
                this->_startChar = this->_caretAtChar - 3;
                if (this->_startChar < 0) {
                    this->_startChar = 0;
                }
                redraw = TRUE;
            }

            updateCaretPos = TRUE;
        }
        return;
    }

    if (vkKey == VK_HOME) {
        if (this->_caretAtChar > 0) {
            this->_caretAtChar = 0;
            this->_startChar = 0;
            updateCaretPos = TRUE;
            redraw = TRUE;
        }
        return;
    }

    if (vkKey == VK_END) {
        // jeśli jesteśmy już na końcu pola tekstowego, to nie podejmujemy żadnej akcji
        if (this->_caretAtChar == strLen) {
            return;
        }

        this->_caretAtChar = static_cast<LONG>(strLen);
        this->adjustStartCharForCaretAtChar();
        updateCaretPos = TRUE;
        redraw = TRUE;
        return;
    }
}

BOOL CTextField::adjustStartCharForCaretAtChar() {
    POINT pt = this->calculateCaretPos();
    if (pt.x + this->_caret->width < this->_getMaxXPosAllowedForContent()) {
        return FALSE;
    }
    LONG lastStartChar = this->_startChar;
    this->_startChar = this->_caretAtChar - 1;
    pt = this->calculateCaretPos();
    while (this->_startChar > 0 && pt.x > 0 && pt.x + this->_caret->width < this->_getMaxXPosAllowedForContent()) {
        lastStartChar = this->_startChar;
        this->_startChar -= 1;
        pt = this->calculateCaretPos();
    }
    this->_startChar = lastStartChar;
    return TRUE;
}

BOOL CTextField::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& updateCaretPos, BOOL& redraw) {
    updateCaretPos = FALSE;
    redraw = FALSE;

    CKeyboard* kbd = this->getKbd();

    // jeśli mamy tyldę, to odpowiednio ustawiamy wartość zmiennej ch, bo z jakichś dziwnych
    // powodów system nie konwertuje poprawnie do tego znaku
    if (virtualCode == VK_OEM_3 && kbd->isShiftPressed()) {
        ch = L'~';
    }

    if (virtualCode != VK_BACK &&
            virtualCode != VK_OEM_1 &&
            virtualCode != VK_OEM_PLUS &&
            virtualCode != VK_OEM_COMMA &&
            virtualCode != VK_OEM_MINUS &&
            virtualCode != VK_OEM_PERIOD &&
            virtualCode != VK_OEM_2 &&
            virtualCode != VK_OEM_3 &&
            virtualCode != VK_OEM_4 &&
            virtualCode != VK_OEM_5 &&
            virtualCode != VK_OEM_6 &&
            virtualCode != VK_OEM_7 &&
            virtualCode != VK_OEM_8 &&
            virtualCode != VK_TAB &&
            (virtualCode < 0x20 || virtualCode > 0x6f)) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL CTextField::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    
    BOOL updateCaretPos = FALSE;
    BOOL redraw = FALSE;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, updateCaretPos, redraw);
    }

    if (updateCaretPos) {
        this->drawCaret();
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL CTextField::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    BOOL updateCaretPos = FALSE;
    BOOL redraw = FALSE;


    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        BOOL process = !this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, updateCaretPos, redraw);
            
        if (process) {
            CKeyboard* kbd = this->getKbd();
            BOOL processed = FALSE;
            if (kbd) {
                BOOL isCaretMovingKey = kbd->isCaretMovingKey(virtualCode);

                // najpierw obsługujemy domyślne skróty
                if (!processed && vkControl && !vkAlt && !vkShift) {
                    // obsługa Ctrl+A - Zaznacz wszystko
                    if (virtualCode == 0x41 /* 'A' */) {
                        this->selectAll(FALSE);

                        processed = TRUE;
                        redraw = TRUE;
                        return TRUE;
                    }
                    // obsługa Ctrl+C - Kopiuj do schowka
                    else if (virtualCode == 0x43 /* 'C' */) {
                        if (this->getSelectionLength() > 0) {
                            this->_clipboard->copyTextToClipboard(this->getParentWindow()->getHwnd(), this->getSelectedText());
                        }

                        processed = TRUE;
                        redraw = FALSE;
                        return TRUE;
                    }
                    // obsługa Ctrl+X - Wytnij do schowka
                    else if (virtualCode == 0x58 /* 'X' */) {
                        if (this->getSelectionLength() > 0) {
                            this->_clipboard->copyTextToClipboard(this->getParentWindow()->getHwnd(), this->getSelectedText());
                            this->removeSelectedText(FALSE);
                            redraw = TRUE;
                        }

                        processed = TRUE;
                        // jeśli nic nie wycięliśmy, to powrót z funkcji
                        if (!redraw) {
                            return TRUE;
                        }
                    }
                    // obsługa Ctrl+V - Wklej ze schowka
                    else if (virtualCode == 0x56 /* 'V' */) {
                        CmzString s = this->_clipboard->getTextFromClipboard(this->getParentWindow()->getHwnd());
                        if (s.length() > 0) {
                            this->insertText(s, FALSE);
                            updateCaretPos = TRUE;
                            redraw = TRUE;
                        }

                        processed = TRUE;
                        // jeśli nic nie wkleiliśmy, to powrót z funkcji
                        if (!redraw) {
                            return TRUE;
                        }
                    }
                }
                // teraz obsługa Tabulatora - nawigacja między sprite'ami
                if (!processed && virtualCode == VK_TAB) {
                    return TRUE;
                }
                if (!processed && isCaretMovingKey) {
                    LONG oldCaretAtChar = this->_caretAtChar;
                    this->_updateCursorPosition(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift, vkLControl, vkRControl, virtualCode, updateCaretPos, redraw);
                    // jeśli nie jest wciśnięty Shift, to czyścimy zaznaczenie
                    if (!vkShift) {
                        if (this->isSelection()) {
                            this->clearSelection(FALSE);
                            redraw = TRUE;
                        }
                    } else {
                        // jeśli pozycja kursora się zmieniła, to aktualizujemy zaznaczenie
                        if (oldCaretAtChar != this->_caretAtChar) {
                            this->_updateSelection(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift, vkLControl, vkRControl, virtualCode, oldCaretAtChar, updateCaretPos, redraw);
                            if (virtualCode == VK_RIGHT && this->isSelection()) {
                                // jeśli mamy zaznaczenie to w przypadku naciśnięcia klawisza VK_RIGHT
                                // gwarantujemy, że jeśli kończy ono pole tekstowe to nie będzie mniejsze
                                // niż 4 piksele
                                RECT rc = this->_calculateSelectionRectForText(&this->_text);
                                if (rc.right == this->_getMaxXPosAllowedForContent() && rc.right - rc.left < 4) {
                                    this->_startChar += 3;
                                    redraw = TRUE;
                                }
                            }
                        }
                    }
                    processed = TRUE;
                }
                if (!processed && virtualCode == VK_DELETE) {
                    if (this->isSelection()) {
                        this->removeSelectedText(FALSE);
                    } else if ((ULONG)this->_caretAtChar < this->_text.length()) {
                        this->_text.deleteCharAt(this->_caretAtChar);
                    }
                    redraw = TRUE;
                    processed = TRUE;
                }
                if (!processed && virtualCode == VK_BACK) {
                    if (this->_caretAtChar > 0 || this->isSelection()) {
                        if (this->isSelection()) {
                            this->removeSelectedText(FALSE);
                        } else {
                            // usuwamy poprzedni znak
                            this->_text.deleteCharAt(this->_caretAtChar - 1);

                            // jeśli usunęliśmy pierwszy wyświetlany w polu tekstowym znak, to zmniejszamy
                            // wartość pola this->_startChar tak, żeby widać było trochę tekstu ( i nie
                            // powstało wrażenie, że nie ma już żadnych znaków)
                            if (this->_caretAtChar - this->_startChar == 1) {
                                this->_startChar -= 3;
                                if (this->_startChar < 0) {
                                    this->_startChar = 0;
                                }
                            }
                            // i przesuwamy karetkę o jeden znak w lewo
                            this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_LEFT, updateCaretPos, redraw);
                        }

                        updateCaretPos = TRUE;
                        redraw = TRUE;
                    }
                    processed = TRUE;
                }

                if (!processed) {
                    // jeśli jeszcze nie obsłżyliśmy znaku i wartość zmiennej ch == L'.', ale virtualCode
                    // jest różne od VK_OEM_PERIOD, to pomijamy obsługę tego klawisza (czyli doklejenie go
                    // do tekstu), bo nie został on poprawnie przekonwertowany (tzn. nie jest to poprawny
                    // znak printable)
                    if (ch == L'.' && virtualCode != VK_OEM_PERIOD) {
                        return FALSE;
                    }
                    // jeśli trzymany jest Control i nie jest wciśnięty Alt, to nic nie robimy, żeby nie
                    // dodawać znaczków prostokątów do tesktu
                    if (vkControl  && !vkAlt) {
                        return FALSE;
                    }

                    if (this->isSelection()) {
                        this->replaceSelectionWith(ch, FALSE);
                    } else {
                        // wstawiamy znak
                        this->_text.insertCharAt(ch, this->_caretAtChar);
                        // i przesuwamy karetkę o jeden znak w prawo
                        this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_RIGHT, updateCaretPos, redraw);
                    }

                    updateCaretPos = TRUE;
                    redraw = TRUE;
                }
            }
        }
    }

    if (updateCaretPos) {
        this->drawCaret();
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL CTextField::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    return TRUE;
}

VOID CTextField::_updateSelection(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl, ULONG vkKey, LONG lastCaretAtChar,
        BOOL& updateCaretPos, BOOL& redraw) {

    // ta metoda jest wywołana po poprawnym zaktualizowaniu atrybutów:
    // this->_startChar i this->_caretAtChar
    if (vkKey == VK_LEFT) {
        if (abs(this->_caretAtChar - lastCaretAtChar) == 1) {
            if (this->_selStart == this->_selEnd) {
                if ((ULONG)this->_caretAtChar < this->_text.length()) {
                    this->_selStart = this->_caretAtChar;
                    this->_selEnd = this->_caretAtChar + 1;
                    this->_selBegin = this->_selEnd;
                    redraw = TRUE;
                }
            } else if (this->_caretAtChar >= this->_selBegin) {
                this->_selEnd = this->_caretAtChar;
                redraw = TRUE;
            } else if (this->_caretAtChar < this->_selBegin) {
                this->_selStart = this->_caretAtChar;
                redraw = TRUE;
            }
        } else {
            if (this->_selStart == this->_selEnd) {
                this->_selStart = this->_caretAtChar;
                this->_selEnd = lastCaretAtChar;
                this->_selBegin = this->_selEnd;
                redraw = TRUE;
            } else if (this->_caretAtChar < this->_selBegin) {
                this->_selStart = this->_caretAtChar;
                this->_selEnd = this->_selBegin;
                redraw = TRUE;
            } else {
                this->_selStart = this->_selBegin;
                this->_selEnd = this->_caretAtChar;
                redraw = TRUE;
            }
        }
        return;
    }

    if (vkKey == VK_RIGHT) {
        if (abs(this->_caretAtChar - lastCaretAtChar) == 1) {
            if (this->_selStart == this->_selEnd) {
                if (this->_caretAtChar > 0) {
                    this->_selStart = this->_caretAtChar - 1;
                    this->_selEnd = this->_caretAtChar;
                    this->_selBegin = this->_selStart;
                    redraw = TRUE;
                }
            } else if (this->_caretAtChar > this->_selBegin) {
                this->_selEnd = this->_caretAtChar;
                redraw = TRUE;
            } else if (this->_caretAtChar <= this->_selBegin) {
                this->_selStart = this->_caretAtChar;
                redraw = TRUE;
            }
        } else {
            if (this->_selStart == this->_selEnd) {
                this->_selStart = lastCaretAtChar;
                this->_selEnd = this->_caretAtChar;
                this->_selBegin = this->_selStart;
                redraw = TRUE;
            } else if (this->_caretAtChar < this->_selBegin) {
                this->_selStart = this->_caretAtChar;
                this->_selEnd = this->_selBegin;
                redraw = TRUE;
            } else if (this->_caretAtChar > this->_selBegin) {
                this->_selStart = this->_selBegin;
                this->_selEnd = this->_caretAtChar;
                redraw = TRUE;
            } else if (this->_caretAtChar == this->_selBegin) {
                this->clearSelection(FALSE);
                redraw = TRUE;
            }
        }
        return;
    }

    if (vkKey == VK_HOME) {
        if (this->isSelection()) {
            this->_selStart = 0;
            this->_selEnd = this->_selBegin;
        } else {
            this->_selStart = 0;
            this->_selEnd = lastCaretAtChar;
            this->_selBegin = this->_selEnd;
        }
        redraw = TRUE;
        return;
    }

    if (vkKey == VK_END) {
        if (this->isSelection()) {
            this->_selStart = this->_selBegin;
            this->_selEnd = static_cast<LONG>(this->_text.length());
        } else {
            this->_selStart = lastCaretAtChar;
            this->_selEnd = static_cast<LONG>(this->_text.length());
            this->_selBegin = this->_selStart;
        }
        redraw = TRUE;
        return;
    }
}

VOID CTextField::drawCaret() {
    this->_caret->hide();

    HWND hwnd = this->getParentWindow()->getHwnd();
    HDC hdc = GetDC(hwnd);
    this->_caret->draw(hdc);
    ReleaseDC(hwnd, hdc);

    this->_caret->show();

    this->updateCaretPos();

    hdc = GetDC(hwnd);
    this->_caret->draw(hdc);
    ReleaseDC(hwnd, hdc);
}

RECT CTextField::_calculateSelectionRectForText(CmzString* s) {
    RECT rc;
    ZeroMemory(&rc, sizeof(rc));

    if (
        (this->_selStart < this->_selEnd) &&
        (this->_selStart >= this->_startChar || this->_selEnd > this->_startChar)
        ) {
            POINT pt;
            POINT lastGoodPt;

            LONG currentChar = this->_selStart;
            if (currentChar < this->_startChar) {
                currentChar = this->_startChar;
            }

            pt = this->calculateCharPos(currentChar);
            // jeśli na starcie jesteśmy poza wyświetlanym obszarem, to powrót
            if ((pt.x == -2 && pt.y == -2) || (pt.x == -1 && pt.y == -1)) {
                return rc;
            }

            rc.left = pt.x;
            rc.top = pt.y;
            rc.right = rc.left;
            rc.bottom = rc.top + this->_theme->textfield_selection->height();

            currentChar++;
            lastGoodPt = pt;
            pt = this->calculateCharPos(currentChar);
            LONG inner_width = this->_calculateInnerWidth();
            LONG maxXPosAllowedForText = this->_getMaxXPosAllowedForContent();
            while (currentChar < this->_selEnd && pt.x > 0 && pt.x <= maxXPosAllowedForText) {
                currentChar += 1;
                lastGoodPt = pt;
                pt = this->calculateCharPos(currentChar);
            }
            if (pt.x > 0 && pt.x <= maxXPosAllowedForText) {
                rc.right = pt.x;
            } else {
                // UWAGA!!!
                // W tym miejscu celowo nie ustawiamy wartości maxXPosAllowedForText, bo obszar
                // zaznaczenia nie powinien wchodzić na prawy margines. Wyjątkiem jest sytuacja
                // w której ostatnia wyświetlana litera wymusza takie zaznaczenie.
                LONG max_x_allowed_for_selection = this->_getMaxXPosAllowedForContent();
                currentChar -= 1;
                WCHAR ch = NULL;
                if (currentChar >= 0 && (ULONG)currentChar < this->_text.length()) {
                    ch = this->_text.getCharAt(currentChar);
                }
                if (ch != NULL) {
                    CmzFont* font = this->getFont();

                    HDC hdc = this->canvas->getDC();
                    if (hdc == 0) {
                        this->canvas->beginPaint();
                    }
                    SIZE size = font->measureChar(ch, this->canvas->getDC());
                    if (hdc == 0) {
                        this->canvas->endPaint();
                    }
                    if (lastGoodPt.x + size.cx <= maxXPosAllowedForText) {
                        rc.right = lastGoodPt.x + size.cx;
                    } else {
                        rc.right = max_x_allowed_for_selection;
                    }
                } else {
                    rc.right = max_x_allowed_for_selection;
                }
            }
    }

    return rc;
}

LONG CTextField::getLastVisibleCharPos(LONG max_inner_width) {
    size_t strLen = this->_text.length();
    if (strLen < 1 || this->_startChar < 0) {
        return -1;
    }
    if (max_inner_width == 0) {
        return -1;
    }

    // zakładamy, że znak ma średnio 4 piksele, żeby później móc ograniczyć długość kopiowanego
    // podłańcucha
    LONG char_width = 2; // literka jak np. i może być jednopikselowa (ale do tego dochodzi minimum
                        // jeden piksel odstępu.
    LONG inner_width = this->_calculateInnerWidth();
    if (max_inner_width > 0 && inner_width > max_inner_width) {
        inner_width = max_inner_width;
    }
    LONG maxCopyChars = inner_width / char_width;

    SIZE size;
    POINT pt;
    ZeroMemory(&size, sizeof(size));
    ZeroMemory(&pt, sizeof(pt));

    CmzString s = this->_text.substr(this->_startChar, maxCopyChars);
    CmzFont* font = this->getFont();

    HDC hdc = this->canvas->getDC();
    if (hdc == 0) {
        this->canvas->beginPaint();
    }
    size = font->measureString(s, this->canvas->getDC());
    if (s.length() < 1) {
        if (hdc == 0) {
            this->canvas->endPaint();
        }
        return -1;
    }

    if (size.cx <= inner_width) {
        if (hdc == 0) {
            this->canvas->endPaint();
        }
        return static_cast<LONG>(this->_startChar + s.length() - 1);
    }

    while (size.cx > inner_width) {
        s.truncateAtEnd(1);
        size = font->measureString(s, this->canvas->getDC());
    }
    if (hdc == 0) {
        this->canvas->endPaint();
    }

    strLen = s.length();
    if (strLen < 1) {
        return -1;
    }

    return static_cast<LONG>(this->_startChar + strLen - 1);
}

WCHAR CTextField::getLastVisibleChar(LONG max_inner_width) {
    LONG pos = this->getLastVisibleCharPos(max_inner_width);
    if (pos >= 0) {
        return this->_text.getCharAt(pos);
    }

    return NULL;
}

BOOL CTextField::update() {
    BOOL retval = FALSE;

    if (!this->_visible) return FALSE;

    if (!(this->_state & CTEXTFIELD_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS)) {
        return FALSE;
    }

    size_t str_len = this->_text.length();
    if (str_len < 1) {
        return FALSE;
    }

    LONG localX = MININT, localY = MININT;
    BOOL updateCaretPos = FALSE, redraw = FALSE;

    this->getCurrentMousePosition(localX, localY);
    if (localX != MININT && localY != MININT) {
        LONG oldCaretAtChar = this->_caretAtChar;
        BOOL processed = FALSE;
        LONG frame_left_width = this->_theme->textfield_frame_left->width();
        LONG inner_width = this->_calculateInnerWidth();

        if (localX < 0) {
            localX = 1;
        }
        if (localX > this->width) {
            localX = this->width - 1;
        }
        LONG last_char_pos = this->getLastVisibleCharPos();

        if (localX <= frame_left_width && abs(this->_caretAtChar - this->_startChar) <= 2) {
            // przesuwamy karetkę o jeden znak w lewo
            this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_LEFT, updateCaretPos, redraw);
            // i jeśli trzeba to aktualizujemy zaznaczenie
            if (oldCaretAtChar != this->_caretAtChar) {
                this->_updateSelection(FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, VK_LEFT, oldCaretAtChar, updateCaretPos, redraw);
                updateCaretPos = TRUE;
                redraw = TRUE;
            }
            processed = TRUE;
        }

        if (!processed && localX > frame_left_width + inner_width && abs((last_char_pos+1) - this->_caretAtChar) <= 2) {
            // przesuwamy karetkę o jeden znak w prawo
            this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_RIGHT, updateCaretPos, redraw);
            // i jeśli trzeba to aktualizujemy zaznaczenie
            if (oldCaretAtChar != this->_caretAtChar) {
                this->_updateSelection(FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, VK_RIGHT, oldCaretAtChar, updateCaretPos, redraw);
            }
            processed = TRUE;
        }

        if (!processed && localX < this->_caret->controlX) {
            LONG char_pos_for_mouse_pointer = this->_startChar;
            if (localX - frame_left_width >= 0) {
                char_pos_for_mouse_pointer = this->getLastVisibleCharPos(localX - frame_left_width) + 1;
                if (char_pos_for_mouse_pointer <= 0) {
                    char_pos_for_mouse_pointer = this->_startChar;
                }
            }
            LONG char_pos_for_caret = this->getLastVisibleCharPos(this->_caret->controlX - frame_left_width) + 1;
            if (char_pos_for_mouse_pointer < char_pos_for_caret) {
                // pozycjonujemy kursor
                this->_caretAtChar = char_pos_for_mouse_pointer;
                // jeśli pozycja kursora się zmieniła, to ustawiamy odpowiednio zaznaczenie
                if (oldCaretAtChar != this->_caretAtChar) {
                    // jeśli zaznaczenia nie było to je inicjalizujey
                    if (this->_selStart == this->_selEnd) {
                        this->_selStart = this->_caretAtChar;
                        this->_selEnd = oldCaretAtChar;
                        this->_selBegin = this->_selEnd;
                    } else {
                        if (this->_caretAtChar < this->_selBegin) {
                            this->_selStart = this->_caretAtChar;
                            this->_selEnd = this->_selBegin;
                        } else if (this->_caretAtChar > this->_selBegin) {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_caretAtChar;
                        } else {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_selBegin;
                        }
                    }
                    redraw = TRUE;
                    updateCaretPos = TRUE;
                }
            }
            processed = TRUE;
        }

        if (!processed && localX > this->_caret->controlX) {
            LONG char_pos_for_mouse_pointer = this->getLastVisibleCharPos(localX - frame_left_width) + 1;
            LONG char_pos_for_caret = this->getLastVisibleCharPos(this->_caret->controlX - frame_left_width) + 1;
            if (char_pos_for_mouse_pointer > char_pos_for_caret) {
                // pozycjonujemy kursor
                this->_caretAtChar = char_pos_for_mouse_pointer;
                // jeśli pozycja kursora się zmieniła, to ustawiamy odpowiednio zaznaczenie
                if (oldCaretAtChar != this->_caretAtChar) {
                    // jeśli zaznaczenia nie było to je inicjalizujey
                    if (this->_selStart == this->_selEnd) {
                        this->_selStart = oldCaretAtChar;
                        this->_selEnd = this->_caretAtChar;
                        this->_selBegin = this->_selStart;
                    } else {
                        if (this->_caretAtChar < this->_selBegin) {
                            this->_selStart = this->_caretAtChar;
                            this->_selEnd = this->_selBegin;
                        } else if (this->_caretAtChar > this->_selBegin) {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_caretAtChar;
                        } else {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_selBegin;
                        }
                    }
                    redraw = TRUE;
                    updateCaretPos = TRUE;
                }
            }
            processed = TRUE;
        }
    }

    if (updateCaretPos) {
        this->updateCaretPos();
        this->drawCaret();
    }

    if (redraw) {
        this->redraw();
        retval = TRUE;
    }

    return retval;
}

VOID CTextField::clearSelection(BOOL redraw) {
    if (!this->isSelection()) {
        redraw = FALSE;
    }

    this->_selStart = 0;
    this->_selEnd = 0;
    this->_selBegin = 0;

    if (redraw) {
        this->redraw();
    }
}

ULONG CTextField::getSelectionLength() {
    if (this->_selStart > this->_selEnd) {
        return 0;
    }

    return this->_selEnd - this->_selStart;
}

CmzString CTextField::getSelectedText() {
    if (this->_selStart > this->_selEnd) {
        return L"";
    }

    ULONG sel_len = this->getSelectionLength();
    return this->_text.substr(this->_selStart, sel_len);
}

VOID CTextField::selectAll(BOOL redraw) {
    LONG len = static_cast<LONG>(this->_text.length());
    if (len < 1) {
        return;
    }

    this->_selStart = 0;
    this->_selEnd = len;
    this->_selBegin = this->_selEnd;
    this->_caretAtChar = len;
    this->adjustStartCharForCaretAtChar();

    this->updateCaretPos();

    if (redraw) {
        this->redraw();
    }
}

VOID CTextField::setSelection(ULONG start, ULONG length, BOOL redraw) {
    LONG str_len = static_cast<LONG>(this->_text.length());
    if (str_len < 1) {
        return;
    }

    this->_selStart = start;
    if (this->_selStart > str_len) {
        this->_selStart = str_len;
    }

    this->_selEnd = this->_selStart + length;
    if (this->_selEnd > str_len) {
        this->_selEnd = str_len;
    }

    this->_selBegin = this->_selEnd;

    if (redraw) {
        this->redraw();
    }
}

LONG CTextField::_calculateInnerWidth() {
    return this->width - this->_theme->textfield_frame_left->width() - this->_theme->textfield_frame_right->width();
}

VOID CTextField::insertText(CmzString s, BOOL redraw) {
    if (this->isSelection()) {
        this->replaceSelectionWith(s, FALSE);
    } else {
        s.replaceCharsWithCodeLessThan(32, L' ');
        this->_text.insertAt(this->_caretAtChar, s);
        this->_caretAtChar = static_cast<LONG>(this->_caretAtChar + s.length());
        this->adjustStartCharForCaretAtChar();
    }

    if (redraw) {
        this->redraw();
    }
}

VOID CTextField::replaceSelectionWith(CmzString replaceWith, BOOL redraw) {
    if (this->isSelection()) {
        replaceWith.replaceCharsWithCodeLessThan(32, L' ');
        ULONG start = this->_selStart;
        ULONG len = this->getSelectionLength();
        if ((ULONG)this->_startChar >= start) {
            this->_startChar = start;
        }
        this->_caretAtChar = start;
        if (this->_startChar + 5 > this->_caretAtChar) {
            for (LONG i = 0; i < 5; i++) {
                if (this->_startChar > 1) {
                    this->_startChar--;
                }
            }
        }
        this->_text.replace(start, len, replaceWith);
        this->_caretAtChar = static_cast<LONG>(this->_selStart + replaceWith.length());
        this->adjustStartCharForCaretAtChar();

        this->clearSelection(FALSE);
        this->updateCaretPos();
        if (redraw) {
            this->redraw();
        }
    }
}

VOID CTextField::removeSelectedText(BOOL redraw) {
    this->replaceSelectionWith(L"", redraw);
}

CmzString CTextField::getText() {
    return this->_text;
}

VOID CTextField::hide() {
    if (this->_caret) {
        this->_caret->active = FALSE;
        this->_caret->hide();
    }
    __super::hide();
}
