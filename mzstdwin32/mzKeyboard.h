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

#ifndef __MZSTDWIN32_KEYBOARD_H__
#define __MZSTDWIN32_KEYBOARD_H__

#include "mzstdwin32Common.h"
#include "mzObject.h"

// oznacza, że klawisz jest nie wciśnięty (zawartość atrybutów jest następująca:
//  CApplication::kbdBufferLast[i] == 0 
#define CKEYBOARD_KEY_STATE_NORMAL     0
#define CKEYBOARD_KEY_STATE_DOWN       1
#define CKEYBOARD_KEY_STATE_PRESSED    2
#define CKEYBOARD_KEY_STATE_UP         3
#define CKEYBOARD_KEY_STATE_TOGGLED    4
#define CKEYBOARD_KEY_STATE_UNTOGGLED  5


class DECLSPEC_MZSTDWIN32 CKeyboard : public mzstd::CmzObject {
public:
    // czas co jaki stan klawiatury jest odświeżany, używany po to by zdarzenia naciśnięć klawiszy
    // nie działy się zbyt szybko
    LONGLONG refreshDelayTime;
    LONGLONG bufferSize;

    // Wskaźnik na bufor klawiatury, który przechowuje informacje o wciśniętych klawiszach
    // pobrany aktualnie. Przed pobraniem aktualnego stanu klawiatury wartość atrybutu
    // this->kbdBuffer jest kopiowana do atrybutu this->_kbdBufferLast.
    BYTE* kbdBuffer;
    // Wskaźnik na bufor przechowujący informację o aktualnym stanie danego klawisza wirtualnego.
    // Każda komórka może mieć jedną z następujących wartości:
    //      CKEYBOARD_KEY_STATE_NORMAL  - last time key was not pressed
    //      CKEYBOARD_KEY_STATE_DOWN    - key is pressed (immediately after this state key state is becoming CKEYBOARD_KEY_STATE_PRESSED)
    //      CKEYBOARD_KEY_STATE_PRESSED - key is being pressed
    //      CKEYBOARD_KEY_STATE_UP      - key is released (immediately after this state key state is becoming CKEYBOARD_KEY_STATE_NORMAL)
    BYTE* kbdKeyState;
    // Array containing BOOLS informing if apply delay for KeyPress events. First onKeyPress event is done
    // without delay to ensure that each onKeyDown has its onKeyPress.
    BYTE* kbdApplyKeyPressDelay;

    WINAPI CKeyboard();
	virtual WINAPI ~CKeyboard();
    // czy odświeżono bufor klawiatury
    VOID WINAPI updateState();
    VOID WINAPI reset();

    inline BYTE WINAPI getKeyState(UINT vkKey) { return this->kbdKeyState[vkKey]; }
    BOOL WINAPI isKeyPressed(UINT vkKey);
    UINT WINAPI toScanCode(UINT vkKey);
    UINT WINAPI toCharCode(UINT vkKey);
    CHAR WINAPI toChar(UINT vkKey);
    WCHAR WINAPI toWideChar(UINT vkKey);

    BOOL WINAPI isSystemKey(UINT vkKey);
    BOOL WINAPI isMouseButton(UINT vkKey);
    BOOL WINAPI isNumeric(UINT vkKey);
    BOOL WINAPI isAlpha(UINT vkKey);
    BOOL WINAPI isAlphanumeric(UINT vkKey);
    BOOL WINAPI isPunctKey(UINT vkKey);
    BOOL WINAPI isSpaceKey(UINT vkKey);
    BOOL WINAPI isCursorKey(UINT vkKey);
    BOOL WINAPI isDeleteKey(UINT vkKey);
    BOOL WINAPI isFKey(UINT vkKey);
    BOOL WINAPI isToggleKey(UINT vkKey);
    // Poniższa funkcja zwraca TRUE, jeśli podany klawisz powoduje zmianę położenia karetki
    // w polu tekstowym.
    BOOL WINAPI isCaretMovingKey(UINT vkKey);

    inline BYTE WINAPI isKeyToggled(ULONGLONG vkKey) { return this->kbdKeyState[vkKey] == CKEYBOARD_KEY_STATE_TOGGLED; }
    inline BYTE WINAPI isCapsLockToggled() { return this->kbdKeyState[VK_CAPITAL] == CKEYBOARD_KEY_STATE_TOGGLED; }
    inline BYTE WINAPI isNumLockToggled() { return this->kbdKeyState[VK_NUMLOCK] == CKEYBOARD_KEY_STATE_TOGGLED; }
    inline BYTE WINAPI isScrollLockToggled() { return this->kbdKeyState[VK_SCROLL] == CKEYBOARD_KEY_STATE_TOGGLED; }

    inline BYTE WINAPI isAltPressed() { return this->isKeyPressed(VK_LMENU) || this->isKeyPressed(VK_RMENU) || this->isKeyPressed(VK_MENU); }
    inline BYTE WINAPI isLAltPressed() { return this->isKeyPressed(VK_LMENU); }
    inline BYTE WINAPI isRAltPressed() { return this->isKeyPressed(VK_RMENU); }

    inline BYTE WINAPI isControlPressed() { return this->isKeyPressed(VK_LCONTROL) || this->isKeyPressed(VK_RCONTROL) || this->isKeyPressed(VK_CONTROL); }
    inline BYTE WINAPI isLControlPressed() { return this->isKeyPressed(VK_LCONTROL); }
    inline BYTE WINAPI isRControlPressed() { return this->isKeyPressed(VK_RCONTROL); }

    inline BYTE WINAPI isShiftPressed() { return this->isKeyPressed(VK_LSHIFT) || this->isKeyPressed(VK_RSHIFT) || this->isKeyPressed(VK_SHIFT); }
    inline BYTE WINAPI isLShiftPressed() { return this->isKeyPressed(VK_LSHIFT); }
    inline BYTE WINAPI isRShiftPressed() { return this->isKeyPressed(VK_RSHIFT); }

protected:
    // wskaźnik na bufor klawiatury, który przechowuje informacje o wciśniętych klawiszach
    // pobrany ostatnio (przed uaktualnieniem atrybutu this->kbdBuffer).
    BYTE* _kbdBufferLast;
    // czas ostatniego odświeżenia bufora klawiatury
    ULONGLONG _lastRefreshTime;
};
typedef CKeyboard* PKeyboard;

#endif;
