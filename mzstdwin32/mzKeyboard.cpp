/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdwin32
 * Version: 1.05
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdwin32 library
 * 
 * mzstdwin32 library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdwin32 library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */
#include "pch.h"
#include "mzstdwin32Common.h"
#include "mzKeyboard.h"
#include "mzString.h"
using namespace mzstd;

CKeyboard::CKeyboard() {
    this->setRealClassName(L"CKeyboard");
    this->bufferSize = 256;
    this->_lastRefreshTime = 0;
    this->refreshDelayTime = 400;

    this->_kbdBufferLast = new BYTE[this->bufferSize];
    this->kbdBuffer = new BYTE[this->bufferSize];
    this->kbdKeyState = new BYTE[this->bufferSize];
    this->kbdApplyKeyPressDelay = new BYTE[this->bufferSize];

    this->reset();
    
    this->updateState();
}

CKeyboard::~CKeyboard() {
    if (this->_kbdBufferLast) {
        delete[] this->_kbdBufferLast;
        this->_kbdBufferLast = NULL;
    }

    if (this->kbdBuffer) {
        delete[] this->kbdBuffer;
        this->kbdBuffer = NULL;
    }

    if (this->kbdKeyState) {
        delete[] this->kbdKeyState;
        this->kbdKeyState = NULL;
    }
    
    if (this->kbdApplyKeyPressDelay) {
        delete[] this->kbdApplyKeyPressDelay;
        this->kbdApplyKeyPressDelay = NULL;
    }
}

VOID CKeyboard::reset() {
    // We reset buffers
    ZeroMemory(this->kbdBuffer, this->bufferSize);
    ZeroMemory(this->_kbdBufferLast, this->bufferSize);
    ZeroMemory(this->kbdKeyState, this->bufferSize);
    ZeroMemory(this->kbdApplyKeyPressDelay, this->bufferSize);
    
    // now we silently flushes keyboard messages
    MSG msg;
    while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE)) {
    }
    
    // And set keyboard buffer for the current thread
    SetKeyboardState(this->kbdBuffer);
}

VOID CKeyboard::updateState() {
    CopyMemory(this->_kbdBufferLast, this->kbdBuffer, this->bufferSize);
    if (GetKeyboardState(this->kbdBuffer)) {
        BYTE hiBit = 0x80; // = 128 = 10000000b
        BYTE loBit = 0x01; // = 128 = 10000000b
        BYTE* cur = this->kbdBuffer;
        BYTE* last = this->_kbdBufferLast;
        BYTE* state = this->kbdKeyState;
        BYTE* applyKeyPressDelay = this->kbdApplyKeyPressDelay;
        BYTE curKey, lastKey;

        for (LONGLONG i = 0; i < this->bufferSize; i++) {
            curKey = cur[i];
            lastKey = last[i];
            // jeśli stan klawisza się nie zmienił
            if (curKey == lastKey) {
                // jeśli klawisz jest wciśnięty, to ustawiamy mu stan: CKEYBOARD_KEY_STATE_PRESSED
                if (curKey & hiBit) {
                    if (state[i] != CKEYBOARD_KEY_STATE_PRESSED) {
                        applyKeyPressDelay[i] = FALSE;
                    }
                    state[i] = CKEYBOARD_KEY_STATE_PRESSED;
                } else {
                    if (state[i] == CKEYBOARD_KEY_STATE_PRESSED) {
                        state[i] = CKEYBOARD_KEY_STATE_UP;
                    } else {
                        state[i] = CKEYBOARD_KEY_STATE_NORMAL;
                    }
                }

                // przechodzimy do kolejnej iteracji pętli
                continue;
            }

            // jeśli aktualnie klawisz jest wciśnięty
            if (curKey & hiBit) {
                // to ustawiamy stan: CKEYBOARD_KEY_STATE_DOWN
                state[i] = CKEYBOARD_KEY_STATE_DOWN;
                continue;
            }

            // jeśli poprzedni stan tego klawisza był: CKEYBOARD_KEY_STATE_UP, to przechodzimy w stan:
            // CKEYBOARD_KEY_STATE_NORMAL
            if (state[i] == CKEYBOARD_KEY_STATE_UP) {
                state[i] = CKEYBOARD_KEY_STATE_NORMAL;
                continue;
            }

            // jeśli poprzedni stan tego klawisza był: CKEYBOARD_KEY_STATE_DOWN, to ustawiamy stan: CKEYBOARD_KEY_STATE_UP
            if (state[i] == CKEYBOARD_KEY_STATE_DOWN) {
                state[i] = CKEYBOARD_KEY_STATE_PRESSED;
                applyKeyPressDelay[i] = FALSE;
                continue;
            }
            
            // CKEYBOARD_KEY_STATE_PRESSED, to ustawiamy stan: CKEYBOARD_KEY_STATE_UP
            if (state[i] == CKEYBOARD_KEY_STATE_PRESSED) {
                state[i] = CKEYBOARD_KEY_STATE_UP;
                continue;
            }
        }

        // na koniec ustawiamy poprawne stany dla klawiszy typu toggle
        state[VK_NUMLOCK] = cur[VK_NUMLOCK] & loBit;
        state[VK_SCROLL] = cur[VK_SCROLL] & loBit;
        state[VK_CAPITAL] = cur[VK_CAPITAL] & loBit;
    }
}

BOOL CKeyboard::isKeyPressed(UINT vkKey) {
    BYTE state = this->kbdKeyState[vkKey];
    return (state == CKEYBOARD_KEY_STATE_DOWN) || (state == CKEYBOARD_KEY_STATE_PRESSED);
}

UINT CKeyboard::toScanCode(UINT vkKey) {
    return MapVirtualKey(vkKey, MAPVK_VK_TO_VSC);
}

UINT CKeyboard::toCharCode(UINT vkKey) {
    return MapVirtualKey(vkKey, MAPVK_VK_TO_CHAR);
}

BOOL CKeyboard::isSystemKey(UINT vkKey) {
    return vkKey == VK_MENU || vkKey == VK_LMENU || vkKey == VK_RMENU ||
            vkKey == VK_SHIFT || vkKey == VK_LSHIFT || vkKey == VK_RSHIFT ||
            vkKey == VK_CONTROL || vkKey == VK_LCONTROL || vkKey == VK_RCONTROL;
}

BOOL CKeyboard::isMouseButton(UINT vkKey) {
    return vkKey == VK_LBUTTON || vkKey == VK_RBUTTON || vkKey == VK_CANCEL ||
            vkKey == VK_MBUTTON || vkKey == VK_XBUTTON1 || vkKey == VK_XBUTTON2;
}

BOOL CKeyboard::isNumeric(UINT vkKey) {
    WCHAR ch = this->toWideChar(vkKey);
    return (((WORD)ch >= 0x30) && ((WORD)ch <= 0x39));
}

BOOL CKeyboard::isAlpha(UINT vkKey) {
    WCHAR ch = this->toWideChar(vkKey);
    return isCharAlpha(ch);
}

BOOL CKeyboard::isAlphanumeric(UINT vkKey) {
    WCHAR ch = this->toWideChar(vkKey);
    return (((WORD)ch >= 0x30) && ((WORD)ch <= 0x39)) || isCharAlpha(ch);
}

BOOL CKeyboard::isCursorKey(UINT vkKey) {
    return (vkKey == VK_LEFT) || (vkKey == VK_RIGHT) || (vkKey == VK_UP) || (vkKey == VK_DOWN);
}

BOOL CKeyboard::isDeleteKey(UINT vkKey) {
    return (vkKey == VK_BACK) || (vkKey == VK_DELETE);
}

BOOL CKeyboard::isFKey(UINT vkKey) {
    return (vkKey >= VK_F1) && (vkKey <= VK_F24);
}

BOOL CKeyboard::isToggleKey(UINT vkKey) {
    return vkKey == VK_NUMLOCK || vkKey == VK_CAPITAL || vkKey == VK_SCROLL;
}

BOOL CKeyboard::isCaretMovingKey(UINT vkKey) {
    return (vkKey == VK_LEFT) || (vkKey == VK_RIGHT) || (vkKey == VK_UP) || (vkKey == VK_DOWN) ||
            (vkKey == VK_HOME) || (vkKey == VK_END) || (vkKey == VK_PRIOR) || (vkKey == VK_NEXT);
}

BOOL CKeyboard::isPunctKey(UINT vkKey) {
    return isCharPunct(this->toWideChar(vkKey));
}

BOOL CKeyboard::isSpaceKey(UINT vkKey) {
    return isCharSpace(this->toWideChar(vkKey));
}

CHAR CKeyboard::toChar(UINT vkKey) {
    WORD ch;
    LONGLONG result = ToAscii(vkKey, this->toScanCode(vkKey), this->kbdBuffer, &ch, 0);
    if (result == 1) {
        return (CHAR)ch;
    } else {
        return '.';
    }
}

WCHAR CKeyboard::toWideChar(UINT vkKey) {
    WCHAR buf[10];
    LONGLONG result = ToUnicode(vkKey, this->toScanCode(vkKey), this->kbdBuffer, buf, 10, 1);
    if (result > 0) {
        return buf[0];
    } else {
        return L'.';
    }
}
