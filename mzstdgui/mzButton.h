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

#ifndef __MZSTDGUI_BUTTON_H__
#define __MZSTDGUI_BUTTON_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzFont.h"
#include "mzImageList.h"



class DECLSPEC_MZSTDGUI CmzButton : public CGuiControl {
public:
	WINAPI CmzButton(CSprite *parent, mzstd::CmzString name, mzstd::CmzString text, LONG x = 0, LONG y = 0, LONG width = 75);
	virtual WINAPI ~CmzButton();

    // ustawia w kontrolce podany tekst oraz przed wyświetleniem wycina z niego wszystkie znaki
    // o kodzie < 32
    virtual VOID WINAPI setText(mzstd::CmzString text, BOOL redraw = TRUE);
    virtual VOID WINAPI setImage(CmzImageList* imageList, mzstd::CmzString normalStateImageName,
                                    mzstd::CmzString hoverStateImageName = COMMON_EMPTY_STRING,
                                    mzstd::CmzString pushedStateImageName = COMMON_EMPTY_STRING,
                                    mzstd::CmzString focusedStateImageName = COMMON_EMPTY_STRING,
                                    mzstd::CmzString disabledStateImageName = COMMON_EMPTY_STRING);
    virtual VOID WINAPI setImage(CmzImageList* imageList, LONG normalStateImageIndex,
                                    LONG hoverStateImageIndex = -1, LONG pushedStateImageIndex = -1,
                                    LONG focusedStateImageIndex = -1, LONG disabledStateImageIndex = -1);
    virtual VOID WINAPI removeImage();

    // czyści tekst kontrolki (atrybut this->_text) ze znaków o kodzie < 32.
    virtual VOID WINAPI cleanupText();
    // zwraca przechowywany w kontrolce tekst
    inline mzstd::CmzString WINAPI getText() { return this->_text; }

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
    CmzFont* _font;
    // Przechowuje wskaźnik na listę obrazków z której pobierane są obrazki, tgo wskaźnika nie
    // zwalniamy
    CmzImageList* _imageList;
    LONG _normalStateImageIndex;
    LONG _hoverStateImageIndex;
    LONG _pushedStateImageIndex;
    LONG _focusedStateImageIndex;
    LONG _disabledStateImageIndex;

    virtual VOID WINAPI _commonInit(mzstd::CmzString text, LONG width);
    // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
    virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw);
        
    virtual VOID WINAPI _updateCanvas();
};
typedef CmzButton* PButton;

#endif
