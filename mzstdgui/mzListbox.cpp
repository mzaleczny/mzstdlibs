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
#include "mzListbox.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
#include "mzBitmap.h"
#include "mzKeyboard.h"
#include "mzStyledWindow.h"
#include "mzStdObject.h"
#include "mzMessages.h"
#include "mzApplication.h"
using namespace mzstd;

VOID CListbox::_commonInit(LONG width, LONG height) {
    this->setRealClassName(L"CListbox");
    this->_font = NULL;
    this->_items = new CmzObjectList();
    this->_topItemIndex = -1;

    this->setSize(width, height);
    this->_tabStop = TRUE;
    this->_padding = 2; // padding of the listbox
    this->MeasuredTextSize.cx = 0;
    this->MeasuredTextSize.cy = 0;

    this->_isMultiselect = FALSE;
}

CListbox::CListbox(CSprite *parent, CmzString name, CmzString items, LONG x, LONG y, LONG width, LONG height)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit(width, height);
    this->setItems(items);
}

CListbox::CListbox(CSprite *parent, CmzString name, CmzStringList* items, LONG x, LONG y, LONG width, LONG height)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit(width, height);
    this->setItems(items);
}

CListbox::~CListbox() {
    if (this->_items) {
        delete this->_items;
        this->_items = NULL;
    }
}

VOID CListbox::setItems(CmzString items, BOOL redraw) {
    CmzStringList* lst = items.explode(L'\n');
    if (lst && lst->size() > 0) {
        this->setItems(lst, redraw);
        delete lst;
    }
}

VOID CListbox::setItems(CmzStringList* items, BOOL redraw) {
    if (items) {
            if (this->_items) {
            CTheme *t = this->_theme;
            CmzFont* font = this->getFont();

            SIZE size = this->_getInnerSize();
            LONG allowedTextAreaWidth = size.cx;
            LONG allowedTextAreaHeight = size.cy;

            CGuiControlItem* item;
            LONG maxItemWidth = 0;
            LONG maxItemHeight = 0;
            LONG summaryItemHeight = 0;
            LONG visibleItems = 0;
            HDC hdc = this->canvas->getDC();

            this->_items->clear();
            this->_fullVisibleItems = 0;
            this->_visibleItems = 0;
            for (ULONG i = 0; i < items->size(); i++) {
                item = new CGuiControlItem(items->getAt(i));
                if (item) {
                    SIZE size = {0, 0};
                    if (font && hdc) {
                        size = font->measureString(item->value, hdc);
                        if (size.cx > maxItemWidth) {
                            maxItemWidth += size.cx;
                        }
                        if (size.cy > maxItemHeight) {
                            maxItemHeight += size.cy;
                        }
                        if (summaryItemHeight <= allowedTextAreaHeight) {
                            this->_fullVisibleItems += 1;
                        }
                        summaryItemHeight += size.cy;
                        if (summaryItemHeight <= allowedTextAreaHeight) {
                            this->_visibleItems += 1;
                        }
                    }
                    item->size = size;
                    this->_items->append(item);
                }
            }
            this->_topItemIndex = 0;
            this->removeVerticalScrollbar();
            this->removeHorizontalScrollbar();
            BOOL addVScr = allowedTextAreaHeight < summaryItemHeight;
            BOOL addHScr = allowedTextAreaWidth < maxItemWidth;
            this->addScrollbars(addVScr, addHScr, 0, static_cast<LONG>(items->size()) - this->_visibleItems, 0, maxItemWidth - allowedTextAreaWidth);

            if (redraw) {
                this->redraw();
            }
        }
    }
}

CmzString CListbox::getText() {
    CmzString result;

    if (this->_items) {
        CGuiControlItem* item;
        for (LONG i = 0; i < this->_items->size(); i++) {
            item = (CGuiControlItem*)this->_items->getAt(i);
            if (item) {
                result += item->value + L"\n";
            }
        }
    }

    return result;
}

CmzString CListbox::getTextExt(LONG selected, LONG checked, LONG imageIndex, LONG data) {
    CmzString result;

    if (selected == -1 && checked == -1 && imageIndex == -1 && data == 0) {
        return COMMON_EMPTY_STRING;
    }

    if (this->_items) {
        CGuiControlItem* item;
        BOOL includeItem;

        for (LONG i = 0; i < this->_items->size(); i++) {
            item = (CGuiControlItem*)this->_items->getAt(i);
            if (item) {
                includeItem = TRUE;

                if (includeItem && selected > 0 && (BOOL)selected != item->selected) {
                    includeItem = FALSE;
                }
                if (includeItem && checked > 0 && (BOOL)checked != item->checked) {
                    includeItem = FALSE;
                }
                if (includeItem && imageIndex >= 0 && imageIndex != item->imageIndex) {
                    includeItem = FALSE;
                }
                if (includeItem && data >= 0 && data != item->data) {
                    includeItem = FALSE;
                }

                if (includeItem) {
                    result += item->value + L"\n";
                }
            }
        }
    }

    return result;
}

CGuiControlItem* CListbox::getItem(LONG item) {
    CGuiControlItem* result = NULL;

    if (this->_items) {
        result = (CGuiControlItem*)this->_items->getAt(item);
    }

    return result;
}

CmzString CListbox::getItemValue(LONG item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->value;
    }
    return COMMON_EMPTY_STRING;
}

BOOL CListbox::isItemSelected(LONG item) {
    if (this->_isMultiselect) {
        CGuiControlItem* result = this->getItem(item);
        if (result) {
            return result->selected;
        }
    } else {
        return this->_selectedItem == item;
    }

    return FALSE;
}

BOOL CListbox::isItemChecked(LONG item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->checked;
    }
    return FALSE;
}

LONG CListbox::getItemImageIndex(LONG item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->imageIndex;
    }
    return 0;
}

LONG CListbox::getItemData(LONG item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->data;
    }
    return 0;
}

CmzObjectList* CListbox::getSelectedItems() {
    // create object list that does not deletes its items in destructor
    CmzObjectList* lst = new CmzObjectList(50, FALSE);
    if (!lst) {
        return NULL;
    }

    if (this->_items) {
        CGuiControlItem* item;
        for (LONG i = 0; i < this->_items->size(); i++) {
            item = (CGuiControlItem*)this->_items->getAt(i);
            if (item && item->selected) {
                lst->append(item);
            }
        }
    }

    return lst;
}

LONG CListbox::getSelectedIndex() {
    if (!this->_isMultiselect) {
        return this->_selectedItem;
    }

    return -1;
}

CmzString CListbox::getSelectedItemValue() {
    if (this->_isMultiselect) {
        return this->getTextExt(1);
    }
    return this->getItemValue(this->_selectedItem);
}

BOOL CListbox::getSelectedItemCheckedState() {
    if (!this->_isMultiselect) {
        if (this->_selectedItem >= 0) {
            return this->isItemChecked(this->_selectedItem);
        }
    }

    return FALSE;
}

LONG CListbox::getSelectedItemImageIndex() {
    if (!this->_isMultiselect) {
        if (this->_selectedItem >= 0) {
            return this->getItemImageIndex(this->_selectedItem);
        }
    }

    return -1;
}

LONG CListbox::getSelectedItemData() {
    if (!this->_isMultiselect) {
        if (this->_selectedItem >= 0) {
            return this->getItemData(this->_selectedItem);
        }
    }

    return 0;
}

VOID CListbox::selectItem(LONG item) {
    if (this->_isMultiselect) {
        CGuiControlItem* gcItem = this->getItem(item);
        if (gcItem) {
            gcItem->selected = TRUE;
        }
    } else {
        this->_selectedItem = item;
    }

    this->redraw();
}

VOID CListbox::selectItem(CmzString item) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (LONG i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->selected = TRUE;
                if (!this->_isMultiselect) {
                    break;
                }
            }
        }
    }

    this->redraw();
}

VOID CListbox::setItemChecked(LONG item) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->checked = TRUE;
    }

    this->redraw();
}

VOID CListbox::setItemChecked(CmzString item) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (LONG i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->checked = TRUE;
            }
        }
    }

    this->redraw();
}

VOID CListbox::setItemData(LONG item, LONG data) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->data = data;
    }

    this->redraw();
}

VOID CListbox::setItemData(CmzString item, LONG data) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (LONG i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->data = data;
            }
        }
    }

    this->redraw();
}

VOID CListbox::setItemImageIndex(LONG item, LONG imageIndex) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->imageIndex = imageIndex;
    }

    this->redraw();
}

VOID CListbox::setItemImageIndex(CmzString item, LONG imageIndex) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (LONG i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->imageIndex = imageIndex;
            }
        }
    }

    this->redraw();
}

VOID CListbox::setItemAttributes(LONG item, BOOL selected, BOOL checked, LONG imageIndex, LONG data) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->selected = selected;
        gcItem->checked = checked;
        gcItem->data = data;
        gcItem->imageIndex = imageIndex;
    }

    this->redraw();
}

VOID CListbox::setItemAttributes(CmzString item, BOOL selected, BOOL checked, LONG imageIndex, LONG data) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (LONG i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->selected = selected;
                gcItem->checked = checked;
                gcItem->imageIndex = imageIndex;
                gcItem->data = data;
            }
        }
    }

    this->redraw();
}


CmzFont* CListbox::getFont() {
    CmzFont* font = this->_theme->commonCheckboxControlFont;
    if (this->_font) {
        font = this->_font;
    }
    return font;
}

LONG CListbox::_getInnerTopLeftX() {
	CTheme *t = this->_theme;
    LONG x = this->_padding;
    if (t && t->listbox_frame_top_left) {
        x += t->listbox_frame_top_left->width();
    }
    return x;
}

LONG CListbox::_getInnerTopLeftY() {
	CTheme *t = this->_theme;
    LONG y = this->_padding;
    if (t && t->listbox_frame_top_left) {
        y += t->listbox_frame_top_left->height();
    }
    return y;
}

SIZE CListbox::_getInnerSize() {
    SIZE size;
    size.cx = this->width;
    size.cy = this->height;
	CTheme *t = this->_theme;
    if (t) {
        if (t && t->listbox_frame_top_left) {
            size.cx -= t->listbox_frame_top_left->width();
            size.cy -= t->listbox_frame_top_left->height();
        }
        if (t && t->listbox_frame_top_right) {
            size.cx -= t->listbox_frame_top_right->width();
        }
        if (t && t->listbox_frame_bottom_left) {
            size.cy -= t->listbox_frame_bottom_left->height();
        }
    }
    return size;
}

VOID CListbox::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    CmzBitmap* bg = t->listbox_bg_fullscreen;
    CmzBitmap* frame_tl = t->listbox_frame_top_left;
    CmzBitmap* frame_t = t->listbox_frame_top;
    CmzBitmap* frame_tr = t->listbox_frame_top_right;

    CmzBitmap* frame_bl = t->listbox_frame_bottom_left;
    CmzBitmap* frame_b = t->listbox_frame_bottom;
    CmzBitmap* frame_br = t->listbox_frame_bottom_right;

    CmzBitmap* frame_l = t->listbox_frame_left;
    CmzBitmap* frame_r = t->listbox_frame_right;
    CmzFont* font = this->getFont();

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Draw listbox
    // ================================================================
    this->draw(x, y, frame_tl, frame_t, frame_tr, frame_bl, frame_b, frame_br, frame_l, frame_r, bg);
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw focus if applied
    // ================================================================
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw items
    // ================================================================
    if (this->_items && this->_items->size() > 0) {
        CGuiControlItem* item;
        CmzFont* font = this->getFont();
        if (font) {
            SIZE innerSize = this->_getInnerSize();
            if (MeasuredTextSize.cx == 0)
            {
                MeasuredTextSize = font->measureString(COMMON_MEASURE_STRING, this->canvas->getDC());
            }
            x = this->_getInnerTopLeftX();
            y = this->_getInnerTopLeftY();
            RECT itemRect = {x, y, x + innerSize.cx, y + MeasuredTextSize.cy};
            for (LONG i = 0; i < this->_items->size(); i++) {
                if (itemRect.top >= this->height - this->_padding) {
                    break;
                }
                if (itemRect.bottom >= this->height - this->_padding) {
                    itemRect.bottom = this->height - this->_padding - 1;
                }
                if (isItemSelected(i))
                {
                    int x = itemRect.left;
                    int w = this->width - t->listbox_frame_left->width() - t->listbox_frame_right->width() - 2*this->_padding;
                    this->canvas->stretchBitmap(this->_theme->listbox_bg_selected,
                        x, itemRect.top,
                        w,
                        itemRect.bottom - itemRect.top
                    );
                }
                item = (CGuiControlItem*)this->_items->getAt(i);
                if (item) {
                    this->canvas->drawText(font, item->value, itemRect, TRUE, DT_LEFT | DT_NOPREFIX);
                    itemRect.top += MeasuredTextSize.cy;
                    itemRect.bottom += MeasuredTextSize.cy;
                }
            }
        }
    }
    // ================================================================
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID CListbox::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID CListbox::focus() {
    CStyledWindow* swnd = this->getParentWindow();
    if (swnd && swnd->getActiveControl() != this) {
        swnd->setActiveControl(this);
        return;
    }
    this->addState(CONTROL_STATE_FOCUSED);
}

VOID CListbox::looseFocus() {
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

BOOL CListbox::onMouseMove(LONG x, LONG y) {
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

BOOL CListbox::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        this->_lMouseButtonPressed = TRUE;
        CStyledWindow* wnd = this->getParentWindow();
        wnd->captureMouse(this);

        CTheme* t = this->_theme;
        int ClickedItem = (y - this->y - t->listbox_frame_top->height()) / MeasuredTextSize.cy;
        this->selectItem(ClickedItem);

        if (wnd->getActiveControl() != this) {
            wnd->setActiveControl(this);
        }

        return TRUE;
    }

    return FALSE;
}

BOOL CListbox::onMouseUp(LONG x, LONG y) {
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

VOID CListbox::onClick() {
    __super::onClick();
}

BOOL CListbox::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw) {
    redraw = FALSE;

    CKeyboard* kbd = this->getKbd();

    return TRUE;
}

BOOL CListbox::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
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
                }
            }
        }
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL CListbox::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
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

BOOL CListbox::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    return TRUE;
}
