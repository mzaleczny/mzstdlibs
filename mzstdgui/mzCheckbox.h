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

#ifndef __MZSTDGUI_CHECKBOX_H__
#define __MZSTDGUI_CHECKBOX_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzFont.h"


#define CHECKBOX_DEFAULT_SPACING                4
#define CHECKBOX_TEXT_PLACEMENT_LEFT            1
#define CHECKBOX_TEXT_PLACEMENT_RIGHT           2

class DECLSPEC_MZSTDGUI CmzCheckbox : public CGuiControl {
public:
	WINAPI CmzCheckbox(CSprite *parent, mzstd::CmzString name, mzstd::CmzString text, BOOL checked, LONG x, LONG y);
	WINAPI CmzCheckbox(CSprite *parent, mzstd::CmzString name, mzstd::CmzString checkedStateText, mzstd::CmzString uncheckedStateText, BOOL checked, LONG x = 0, LONG y = 0, LONG width = 75, LONG textPlacement = CHECKBOX_TEXT_PLACEMENT_RIGHT, LONG textSpacing = CHECKBOX_DEFAULT_SPACING);
	virtual WINAPI ~CmzCheckbox();

    virtual VOID WINAPI setText(mzstd::CmzString checkedStateText, mzstd::CmzString uncheckedStateText, BOOL redraw = TRUE);
    virtual VOID WINAPI setText(mzstd::CmzString text, BOOL redraw = TRUE);
    
    // zwraca przechowywany w kontrolce tekst
    virtual mzstd::CmzString WINAPI getText();
    inline BOOL WINAPI checked() { return this->_checked; }
    VOID WINAPI setChecked();
    VOID WINAPI setUnchecked();
    VOID WINAPI toggleCheckedState();
    
    // zwraca aktualnie używany w kontrolce font
    CmzFont* WINAPI getFont();

    // Umieszcza focus na tej kontrolce a dokładnie:
    //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
    //      ParentWnd->setActiveControl(this) tego okna.
    //      Przełącza stan kontrolki w tryb CmzButton_STATE_FOCUSED.
    virtual VOID WINAPI focus();
    // Usuwa focus z tej kontrolki a dokładnie:
    //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
    //      Przełącza kontrolkę w stan CmzButton_STATE_HOVER lub CmzButton_STATE_NORMAL
    //      w zależności od aktualnego położenia myszy.
    virtual VOID WINAPI looseFocus();
    // funkcja obsługi kliknięcia
    virtual VOID WINAPI onClick();

    // Funkcje obsługi zdarzeń
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
    virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

    virtual BOOL WINAPI onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual BOOL WINAPI onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual BOOL WINAPI onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);

protected:
    mzstd::CmzString _text;
    mzstd::CmzString _checkedStateText;
    mzstd::CmzString _uncheckedStateText;
    CmzFont* _font;
    LONG   _textSpacing;
    LONG   _textPlacement;
    SIZE   _textSize;
    BOOL   _checked;

    virtual VOID WINAPI _commonInit(mzstd::CmzString checkedStateText, mzstd::CmzString uncheckedStateText, BOOL checked, LONG width, LONG textPlacement, LONG textSpacing);
    // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
    virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw);
        
    virtual VOID WINAPI _updateCanvas();
};
typedef CmzCheckbox* PCheckbox;

#endif
