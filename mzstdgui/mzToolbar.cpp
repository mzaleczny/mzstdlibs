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
#include "mzToolbar.h"
#include "mzstdguiGlobals.h"
#include "mzFileUtils.h"
#include "mzJsonParser.h"
#include "mzMessageBox.h"
#include "mzTheme.h"
#include "mzArrayList.h"
#include "mzImageList.h"
#include "mzStyledWindow.h"
#include "mzApplication.h"
#include "mzMessages.h"
using namespace mzstd;

CToolbar::CToolbar(CSprite *parent, CmzString name, CmzString itemsJsonFname, CmzImageList* imageList, LONG size, LONG x, LONG y)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_items = NULL;
    this->_commonInit(itemsJsonFname, imageList, size);
}

VOID CToolbar::_commonInit(CmzString itemsJsonFname, CmzImageList* imageList, LONG size) {
    this->setRealClassName(L"CToolbar");
    this->_autosize = TRUE;
    this->_lMouseButtonPressed = FALSE;
    if (this->_items) {
        delete this->_items;
        this->_items = NULL;
    }
    this->_imageList = imageList;
    // na razie jest dozwolony tylko toolbar dla ikon o rozmiarze 16x16
    if (size != 16) {
        size = 16;
    }
    this->_size = size;
    this->load(itemsJsonFname);

    if (this->parent) {
        this->setSizeRelativeToParent(parent->width, parent->height);
    } else {
        this->setSizeRelativeToParent(10, 10);
    }
}

CToolbar::~CToolbar() {
    this->cleanup();
}

VOID CToolbar::cleanup() {
    if (this->_items) {
        delete this->_items;
        this->_items = NULL;
    }
    this->_itemsJsonFname = L"";
}

VOID CToolbar::load(CmzString itemsJsonFname) {
    if (!FileExists(itemsJsonFname)) {
        return;
    }

    this->cleanup();
    this->_itemsJsonFname = itemsJsonFname;

    CmzJsonParser parser;
    CmzString Content = ReadFile(this->_itemsJsonFname);
    this->_items = parser.parse(Content);
    if (!this->_items) {
        if (!parser.isError()) {
            AlertBox(CmzString(L"Plik [") + this->_itemsJsonFname + L"] nie zawiera obiektów json");
        } else {
            ErrorBox(parser.getErrorMessage());
        }
        return;
    }

    // ustawiamy flagi podświetlenia
    CmzStdObjectProperty *root = this->_items->get(L"root");
	if (root && root->type == STDOBJ_PROPERTY_ARRAY) {
        CmzPointerList* al = root->aValue;
        if (al) {
            CmzStdObjectProperty *item;
            for (ULONG i = 0; i < al->size(); i++) {
                item = (CmzStdObjectProperty*)al->getAt(i);
                if (!item->extraData) {
                    item->extraData = new CmzStdObject();
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
            }
        }
    }
}

VOID CToolbar::setSizeRelativeToParent(LONG parentWidth, LONG parentHeight) {
	this->setSize(parentWidth, this->_theme->toolbar16_middle->height());
}

VOID CToolbar::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

BOOL CToolbar::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

	CmzStdObjectProperty *item;
	ULONG i;
	BOOL redrawMenu = FALSE;
    if (!this->_items) {
        return TRUE;
    }
    CmzStdObjectProperty *root = this->_items->get(L"root");
	if (!root || root->type != STDOBJ_PROPERTY_ARRAY) {
        return TRUE;
    }
    CmzPointerList* al = root->aValue;

	if (!this->pointIn(x, y)) {
        for (i = 0; i < al->size(); i++) {
            item = (CmzStdObjectProperty*)al->getAt(i);
            if (item) {
                if (item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL)) {
                    if (this->_lMouseButtonPressed) {
                        item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_HILITED));
                    }
                    redrawMenu = TRUE;
                }
                if (!this->_lMouseButtonPressed) {
                    item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
                }
            }
        }

		if (redrawMenu) {
			CStyledWindow *wnd = this->getParentWindow();
			if (wnd) {
				this->_needUpdate = TRUE;
				wnd->invalidate();
			}
		}

        return FALSE;
    }


    // tutaj sprawdzamy nad którą pozycją jest kursor i ją podświetlamy
    LONG item_x, item_width;
    for (i = 0; i < al->size(); i++) {
        item = (CmzStdObjectProperty*)al->getAt(i);
        if (item) {
            item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
            item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);
            if (item->oValue->get(L"type")->value == L"tbButton" && x >= this->x + item_x && x <= this->x + item_x + item_width) {
                // jesteśmy nad tym elementem
                if (this->_lMouseButtonPressed) {
                    if (item->extraData->get(L"state")->iValue == static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_HILITED)) {
                        item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_PRESSED));
                        redrawMenu = TRUE;
                    }
                } else {
                    if (item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_HILITED)) {
                        item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_HILITED));
                        redrawMenu = TRUE;
                    }
                }
            } else {
                // nie jesteśmy nad tym elementem
                if (!this->_lMouseButtonPressed) {
                    if (item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL)) {
                        redrawMenu = TRUE;
                    }
                    item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
                } else {
                    if (item->extraData->get(L"state")->iValue == static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_PRESSED)) {
                        redrawMenu = TRUE;
                        item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_HILITED));
                    }
                }                
            }
        }
    }

	if (redrawMenu) {
		CStyledWindow *wnd = this->getParentWindow();
		if (wnd) {
			this->_needUpdate = TRUE;
			wnd->invalidate();
		}
	}

    return TRUE;
}

BOOL CToolbar::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

	CmzStdObjectProperty *item;
	ULONG i;
	BOOL redrawMenu = FALSE;
    if (!this->_items) {
        return TRUE;
    }
    CmzStdObjectProperty *root = this->_items->get(L"root");
	if (!root || root->type != STDOBJ_PROPERTY_ARRAY) {
        return FALSE;
    }
    CmzPointerList* al = root->aValue;
    this->_lMouseButtonPressed = FALSE;

	if (!this->pointIn(x, y)) {
        for (i = 0; i < al->size(); i++) {
            item = (CmzStdObjectProperty*)al->getAt(i);
            if (item) {
                if (item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL)) {
                    redrawMenu = TRUE;
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
            }
        }

		if (redrawMenu) {
			CStyledWindow *wnd = this->getParentWindow();
			if (wnd) {
				this->_needUpdate = TRUE;
				wnd->invalidate();
			}
		}

        return FALSE;
    }

    this->_lMouseButtonPressed = TRUE;
    this->getParentWindow()->captureMouse(this);
    // tutaj sprawdzamy nad którą pozycją jest kursor i zaznaczamy że została wciśnięta
    LONG item_x, item_width;
    for (i = 0; i < al->size(); i++) {
        item = (CmzStdObjectProperty*)al->getAt(i);
        if (item) {
            item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
            item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);
            if (item->oValue->get(L"type")->value == L"tbButton" && x >= this->x + item_x && x <= this->x + item_x + item_width) {
                // jesteśmy nad tym elementem
                if (!redrawMenu && item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_PRESSED)) {
                    redrawMenu = TRUE;
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_PRESSED));
            } else {
                // nie jesteśmy nad tym elementem
                if (!redrawMenu && item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL)) {
                    redrawMenu = TRUE;
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
            }
        }
    }

	if (redrawMenu) {
		CStyledWindow *wnd = this->getParentWindow();
		if (wnd) {
			this->_needUpdate = TRUE;
			wnd->invalidate();
		}
	}

    return TRUE;
}

BOOL CToolbar::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    this->getParentWindow()->captureMouse(NULL);
	CmzStdObjectProperty *item;
	ULONG i;
	BOOL redrawMenu = FALSE;
    if (!this->_items) {
        return TRUE;
    }
    CmzStdObjectProperty *root = this->_items->get(L"root");
	if (!root || root->type != STDOBJ_PROPERTY_ARRAY) {
        return TRUE;
    }
    CmzPointerList* al = root->aValue;
    this->_lMouseButtonPressed = FALSE;

	if (!this->pointIn(x, y)) {
        for (i = 0; i < al->size(); i++) {
            item = (CmzStdObjectProperty*)al->getAt(i);
            if (item) {
                if (item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL)) {
                    redrawMenu = TRUE;
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
            }
        }

		if (redrawMenu) {
			CStyledWindow *wnd = this->getParentWindow();
			if (wnd) {
				this->_needUpdate = TRUE;
				wnd->invalidate();
			}
		}

        return FALSE;
    }


    // tutaj sprawdzamy nad którą pozycją jest kursor i zaznaczamy że została wciśnięta
    LONG item_x, item_width;
    for (i = 0; i < al->size(); i++) {
        item = (CmzStdObjectProperty*)al->getAt(i);
        if (item) {
            item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
            item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);
            if (item->oValue->get(L"type")->value == L"tbButton" && x >= this->x + item_x && x <= this->x + item_x + item_width) {
                CStyledWindow *wnd = this->getParentWindow();
                if (wnd) {
                    // tutaj kliknięto dany przycisk, więc wysyłamy odpowiednie zdarzenie
                    CmzStdObject *eventInfo = new CmzStdObject();
                    eventInfo->set(L"tooltip", item->oValue->get(L"tooltip")->value);
                    eventInfo->set(L"id", item->oValue->get(L"id")->value);
                    eventInfo->set(L"icon", item->oValue->get(L"icon")->value);
                    eventInfo->set(L"type", item->oValue->get(L"type")->value);
                    eventInfo->set(L"toolbar_name", this->name);
                    eventInfo->set(L"form_name", wnd->getName());

                    CApplication* app = wnd->getApplication();
                    eventInfo->set(L"event_type", static_cast<LONGLONG>(WM_TOOLBARITEM_CLICKED));
                    app->pushClickedMenuItemData(eventInfo);
                    PostMessage(wnd->getHwnd(), WM_TOOLBARITEM_CLICKED, (WPARAM)app, NULL);
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_HILITED));
                redrawMenu = TRUE;
            } else {
                // nie jesteśmy nad tym elementem
                if (item->extraData->get(L"state")->iValue != static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL)) {
                    redrawMenu = TRUE;
                }
                item->extraData->set(L"state", static_cast<LONGLONG>(CTOOLBAR_ITEM_STATE_NORMAL));
            }
        }
    }

	if (redrawMenu) {
		CStyledWindow *wnd = this->getParentWindow();
		if (wnd) {
			this->_needUpdate = TRUE;
			wnd->invalidate();
		}
	}

    return TRUE;
}

VOID CToolbar::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy belkę toolbara
    // ================================================================
    ULONG bg_middle_width = 0;
    ULONG bg_middle_height = 0;
    this->canvas->drawBitmap(t->toolbar16_left, x, y);
    x += t->toolbar16_left->width();
    bg_middle_width = this->canvas->width() - t->toolbar16_left->width() - t->toolbar16_right->width();
    bg_middle_height = t->toolbar16_middle->height();
    this->canvas->stretchBitmap(t->toolbar16_middle,
        x, y,
        bg_middle_width,
        bg_middle_height
    );
    x += bg_middle_width;
    this->canvas->drawBitmap(t->toolbar16_right, x, y);
    // ================================================================
    // Koniec rysowania belki menu
    // ================================================================

    // ================================================================
    // Rysujemy itemy toolbara
    // ================================================================
    y = (t->toolbar16_middle->height() - this->_size) / 2;
    x = CTOOLBAR_MARGIN;
    LONG hilited_button_y = 0;
    LONG hilited_button_width = 0;
    LONG hilited_button_height = 0;
    LONG separator_y = 0;
    LONG separator_width = 0;
    LONG separator_height = 0;
    switch (this->_size) {
    case 16:
    default:
        hilited_button_y = (t->toolbar16_middle->height() - t->toolbar16_button_hilited->height()) / 2;
        hilited_button_width = t->toolbar16_button_hilited->width();
        hilited_button_height = t->toolbar16_button_hilited->height();
        separator_y = (t->toolbar16_middle->height() - t->toolbar16_separator->height()) / 2;
        separator_width = t->toolbar16_separator->width();
        separator_height = t->toolbar16_separator->height();
        break;
    }

    if (this->_items) {
        CmzStdObjectProperty *root = this->_items->get(L"root");
	    if (root && root->type == STDOBJ_PROPERTY_ARRAY) {
            CmzPointerList* al = root->aValue;
            if (al) {
                CmzStdObjectProperty* item;
                CmzStdObjectProperty* attrib;
                CmzStdObjectProperty* state;
                CmzString itemType, itemIcon;
                for (ULONG i = 0; i < al->size(); i++) {
                    item = (CmzStdObjectProperty*)al->getAt(i);
                    if (item && item->type == STDOBJ_PROPERTY_OBJECT) {
                        attrib = item->oValue->get(L"type");
                        if (attrib) {
                            if (attrib->value == L"tbButton") {
                                state = item->extraData->get(L"state");
                                if (state->iValue == CTOOLBAR_ITEM_STATE_HILITED) {
                                    this->canvas->drawBitmap(t->toolbar16_button_hilited, x, hilited_button_y);
                                }
                                if (state->iValue == CTOOLBAR_ITEM_STATE_PRESSED) {
                                    this->canvas->drawBitmap(t->toolbar16_button_pressed, x, hilited_button_y);
                                }
                                if (this->_imageList) {
                                    item->extraData->set(L"x", static_cast<LONGLONG>(x));
                                    item->extraData->set(L"width", static_cast<LONGLONG>(hilited_button_width));
                                    attrib = item->oValue->get(L"icon");
                                    this->_imageList->transparentBitblt(this->canvas->getDC(), x + (hilited_button_width - this->_size) / 2, y, attrib->value);
                                    x += hilited_button_width + CTOOLBAR_BUTTON_GAP;
                                }
                            } else if (attrib->value == L"tbSeparator") {
                                item->extraData->set(L"x", static_cast<LONGLONG>(x));
                                item->extraData->set(L"width", static_cast<LONGLONG>(separator_width + 2 * CTOOLBAR_SEPARATOR_MARGIN));
                                x += CTOOLBAR_SEPARATOR_MARGIN;
                                this->canvas->drawBitmap(t->toolbar16_separator, x, separator_y);
                                x += separator_width + CTOOLBAR_SEPARATOR_MARGIN;
                            }
                        }
                    }
                }
            }
        }
    }
    // ================================================================
    // Koniec rysowania itemow toolbara
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}
