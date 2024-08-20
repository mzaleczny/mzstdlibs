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

#ifndef __MZSTDGUI_LISTBOX_H__
#define __MZSTDGUI_LISTBOX_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzObjectList.h"
#include "mzImageList.h"
#include "mzFont.h"
#include "mzScrollbar.h"

class DECLSPEC_MZSTDGUI CListbox : public CGuiControl {
public:
	WINAPI CListbox(CSprite *parent, mzstd::CmzString name, mzstd::CmzString items, LONG x, LONG y, LONG width = 200, LONG height = 200);
	WINAPI CListbox(CSprite *parent, mzstd::CmzString name, mzstd::CmzStringList* items, LONG x, LONG y, LONG width = 200, LONG height = 200);
	virtual WINAPI ~CListbox();

    virtual VOID WINAPI setItems(mzstd::CmzString items, BOOL redraw = TRUE);
    virtual VOID WINAPI setItems(mzstd::CmzStringList* items, BOOL redraw = TRUE);
    
    // zwraca przechowywany w kontrolce tekst
    virtual mzstd::CmzString WINAPI getText();
    mzstd::CmzString WINAPI getTextExt(LONG selected = -1, LONG checked = -1, LONG imageIndex = -1, LONG data = 0);
    CGuiControlItem* WINAPI getItem(LONG item);
    mzstd::CmzString WINAPI getItemValue(LONG item);
    BOOL WINAPI isItemSelected(LONG item);
    BOOL WINAPI isItemChecked(LONG item);
    LONG WINAPI getItemImageIndex(LONG item);
    LONG WINAPI getItemData(LONG item);
    
    mzstd::CmzString WINAPI getSelectedItemValue();
    BOOL WINAPI getSelectedItemCheckedState();
    LONG WINAPI getSelectedItemImageIndex();
    LONG WINAPI getSelectedItemData();
    
    mzstd::CmzObjectList* WINAPI getSelectedItems();
    LONG WINAPI getSelectedIndex();
    inline ULONGLONG WINAPI getItemsCount() {
        if (this->_items) {
            return this->_items->size();
        }
        return 0;
    };
    
    VOID WINAPI selectItem(LONG item);
    VOID WINAPI selectItem(mzstd::CmzString item);
    VOID WINAPI setItemChecked(LONG item);
    VOID WINAPI setItemChecked(mzstd::CmzString item);
    VOID WINAPI setItemData(LONG item, LONG data);
    VOID WINAPI setItemData(mzstd::CmzString item, LONG data);
    VOID WINAPI setItemImageIndex(LONG item, LONG imageIndex);
    VOID WINAPI setItemImageIndex(mzstd::CmzString item, LONG imageIndex);
    VOID WINAPI setItemAttributes(LONG item, BOOL selected, BOOL checked, LONG imageIndex, LONG data);
    VOID WINAPI setItemAttributes(mzstd::CmzString item, BOOL selected, BOOL checked, LONG imageIndex, LONG data);
    
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
    mzstd::CmzObjectList* _items;
    CmzFont* _font;
    CmzImageList* _imageList;
    BOOL _isCheckedList; //if checkbox near items are drawed and item.checked attribute is applied
    BOOL _isMultiselect;
    LONG _selectedItem; // used if this->_isMultiselect is FALSE
    LONG _topItemIndex; // index item displayed at top
    LONG _fullVisibleItems; // count of fully visible items
    LONG _visibleItems;     // count of all visible items (including the last one)
    LONG _padding;
    SIZE MeasuredTextSize;

    virtual VOID WINAPI _commonInit(LONG width, LONG height);
    // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
    virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw);
        
    virtual VOID WINAPI _updateCanvas();
    LONG WINAPI _getInnerTopLeftX();
    LONG WINAPI _getInnerTopLeftY();
    SIZE WINAPI _getInnerSize();
};
typedef CListbox* PListbox;

#endif
