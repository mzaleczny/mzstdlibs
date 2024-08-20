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
#include "mzMenu.h"
#include "mzstdguiGlobals.h"
#include "mzBitmap.h"
#include "mzSprite.h"
#include "mzJsonParser.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"
#include "mzTheme.h"
#include "mzStyledWindow.h"
#include "mzPanel.h"
#include "mzMessages.h"
#include "mzApplication.h"
#include "mzImageList.h"
#include "mzStyledWindow.h"
using namespace mzstd;

mzstd::CMenu::CMenu(CSprite *parent, CmzString name, mzstd::CMenu *parentMenu, LONG type, CmzImageList* imageList)
    : CGuiControl(parent, name, NULL, 0, 0)
{
    this->setRealClassName(L"CMenu");
	this->_menuType = type;
    this->_menuData = NULL;
	this->_menuItems = NULL;
    this->_imageList = imageList;
    this->_lMouseButtonPressed = FALSE;

    if (type == CMENU_MENU_TYPE_MAINMENU) {
        this->_autosize = TRUE;
        if (this->parent) {
            this->setSizeRelativeToParent(parent->width, parent->height);
        } else {
            this->setSizeRelativeToParent(10, 10);
        }
    } else {
        this->_transparentDrawing = TRUE;
        this->_autosize = FALSE;
    }
    this->_opened = FALSE;
    this->_parentMenu = parentMenu;
    this->_frameTopHiliteWidth = 0;
    this->_openedSubmenu = NULL;
}

mzstd::CMenu::~CMenu() {
    if (this->_menuData) {
        delete this->_menuData;
    }
}

BOOL mzstd::CMenu::loadMenu(CmzString content, LONG contentType) {
    this->_opened = FALSE;
    if (contentType == RESOURCE_TYPE_JSON) {
        CmzJsonParser parser;

        // jeśli przechowujemy już wskaźnik na strukturę menu, to musimy go zwolnić
        if (this->_menuData) {
            delete this->_menuData;
            this->_menuItems = NULL;
        }

        this->_menuData = parser.parse(content);
        if (!this->_menuData) {
            if (!parser.isError()) {
                AlertBox(L"Error during loading menu. Resource does not contain json objects.");
            } else {
                ErrorBox(parser.getErrorMessage());
            }
            return FALSE;
        }

        CmzStdObjectProperty *root = this->_menuData->get(L"root");
        if (root->type == STDOBJ_PROPERTY_ARRAY) {
            this->_menuItems = (CmzArrayList*)root->aValue;
        } else {
            this->_menuItems = NULL;
        }

        // jeśli mamy _menuItems, to ustawiamy na każdym obiekcie tej tablicy
        // wartość informującą, że pozycja nie jest podkreślona
        if (this->_menuItems) {
            CmzStdObjectProperty *item;
            for (ULONG i = 0; i < this->_menuItems->size(); i++) {
                item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
                if (item) {
                    if (!item->extraData) {
                        item->extraData = new CmzStdObject();
                    }
                    item->extraData->set(L"hilited", 0LL);
                }
            }
        }

        this->_needUpdate = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL mzstd::CMenu::loadMenuFromFile(CmzString fileName, LONG contentType) {
    this->_opened = FALSE;
    if (FileExists(fileName)) {
        CmzString content = ReadFile(fileName);
        return this->loadMenu(content, contentType);
    }

    return FALSE;
}

VOID mzstd::CMenu::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
		return;
    }

	switch (this->_menuType) {
	case CMENU_MENU_TYPE_MAINMENU:
		this->_updateCanvasAsMainMenu();
		break;
	case CMENU_MENU_TYPE_POPUPMENU:
		this->_updateCanvasAsPopupMenu();
		break;
	}
}

VOID mzstd::CMenu::_updateCanvasAsMainMenu() {
	LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy belkę menu
    // ================================================================
    ULONG bg_middle_width = 0;
    ULONG bg_middle_height = 0;
    this->canvas->drawBitmap(t->menu_bg_left, x, y);
    x += t->menu_bg_left->width();
    bg_middle_width = this->canvas->width() - t->menu_bg_left->width() - t->menu_bg_right->width();
    bg_middle_height = t->menu_bg_middle->height();
    this->canvas->stretchBitmap(t->menu_bg_middle,
        x, y,
        bg_middle_width,
        bg_middle_height
    );
    x += bg_middle_width;
    this->canvas->drawBitmap(t->menu_bg_right, x, y);
    // ================================================================
    // Koniec rysowania belki menu
    // ================================================================


    // ================================================================
    // Rysujemy teksty pozycji menu
    // ================================================================
	this->canvas->setBkMode(TRANSPARENT);
	x = t->menu_bg_left->width();
	y = 0;
	if (this->_menuItems) {
		CmzStdObjectProperty *item;
		CmzString caption;
		RECT rc;
        HDC hdc = this->canvas->getDC();
        if (hdc == 0) {
            this->canvas->beginPaint();
        }
		SIZE size = t->menuFont->measureString(L"Sample text_yj", this->canvas->getDC());
        if (hdc == 0) {
            this->canvas->endPaint();
        }
		rc.top = (t->menu_bg_middle->height() - size.cy) / 2;
		rc.left = CMENU_HORIZONTAL_GAP;
		rc.bottom = rc.top + size.cy;

		for (ULONG i = 0; i < this->_menuItems->size(); i++) {
			item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
			if (item->type == STDOBJ_PROPERTY_VALUE || item->type == STDOBJ_PROPERTY_OBJECT) {
				if (!item->extraData) {
					item->extraData = new CmzStdObject();
				}
				if (item->type == STDOBJ_PROPERTY_VALUE) {
					caption = item->value;
				} else {
					caption = item->oValue->get(L"caption")->value;
				}
				
                LONG menuitem_width = 0;
				if (item->extraData->get(L"hilited")->iValue != 0) {
                    menuitem_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);
					bg_middle_width = 0;
                    if (this->_opened) {
                        bg_middle_height = t->menu_opened_mainmenu_item_bg_middle->height();
                    } else {
					    bg_middle_height = t->menu_selected_mainmenu_item_bg_middle->height();
                    }
					x = rc.left - CMENU_HORIZONTAL_GAP / 2;
					y = (t->menu_bg_middle->height() - bg_middle_height) / 2;

                    if (this->_opened) {
					    this->canvas->drawBitmap(t->menu_opened_mainmenu_item_bg_left, x, y);
                        x += t->menu_opened_mainmenu_item_bg_left->width();

                        bg_middle_width = menuitem_width - t->menu_opened_mainmenu_item_bg_left->width() - t->menu_selected_mainmenu_item_bg_right->width();
                        bg_middle_height = t->menu_opened_mainmenu_item_bg_middle->height();
					    this->canvas->stretchBitmap(t->menu_opened_mainmenu_item_bg_middle,
						    x, y,
						    bg_middle_width,
						    bg_middle_height
					    );
					    x += bg_middle_width;
					    this->canvas->drawBitmap(t->menu_opened_mainmenu_item_bg_right, x, y);
                    } else {
                        this->canvas->drawBitmap(t->menu_selected_mainmenu_item_bg_left, x, y);
                        x += t->menu_selected_mainmenu_item_bg_left->width();
                        bg_middle_width = menuitem_width - t->menu_selected_mainmenu_item_bg_left->width() - t->menu_selected_mainmenu_item_bg_right->width();
                        bg_middle_height = t->menu_selected_mainmenu_item_bg_middle->height();
                        this->canvas->stretchBitmap(t->menu_selected_mainmenu_item_bg_middle,
						    x, y,
						    bg_middle_width,
						    bg_middle_height
					    );
					    x += bg_middle_width;
					    this->canvas->drawBitmap(t->menu_selected_mainmenu_item_bg_right, x, y);
                    }
				}

				caption.remove(L'&');
                HDC hdc = this->canvas->getDC();
                if (hdc == 0) {
                    this->canvas->beginPaint();
                }
				size = t->menuFont->measureString(caption, this->canvas->getDC());
                if (hdc == 0) {
                    this->canvas->endPaint();
                }
				item->extraData->set(L"x", static_cast<LONGLONG>(rc.left - CMENU_HORIZONTAL_GAP / 2));
				item->extraData->set(L"y", static_cast<LONGLONG>(rc.top));
                item->extraData->set(L"popup_x", static_cast<LONGLONG>(this->x + x));
                item->extraData->set(L"popup_y", static_cast<LONGLONG>(this->y + (t->menu_bg_middle->height() - t->menu_opened_mainmenu_item_bg_middle->height()) / 2 + t->menu_opened_mainmenu_item_bg_middle->height()));
			    item->extraData->set(L"width", static_cast<LONGLONG>((LONG)size.cx + CMENU_HORIZONTAL_GAP));
			    item->extraData->set(L"height", static_cast<LONGLONG>((LONG)size.cy));
				rc.right = rc.left + size.cx;
				this->canvas->drawText(t->menuFont, caption, rc);
				rc.left += size.cx + CMENU_HORIZONTAL_GAP;
                if (menuitem_width == 0) {
                    menuitem_width = size.cx + CMENU_HORIZONTAL_GAP;
                }
                item->extraData->set(L"hilite_width", static_cast<LONGLONG>(menuitem_width - t->menu_opened_mainmenu_item_bg_left->width() - t->menu_selected_mainmenu_item_bg_right->width()));
			}
		}
	}
    // ================================================================
    // Koniec rysowania tekstów pozycji menu
    // ================================================================

	this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID mzstd::CMenu::_updateCanvasAsPopupMenu() {
    LONG x = 0;
    LONG y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;

    LONG width = this->width;
    this->canvas->beginPaint(hdc);
    // ================================================================
    // Rysujemy górną ramkę
    // ================================================================
    ULONG middle_width = width - t->popupmenu_frame_left_top->width() - t->popupmenu_frame_top_right->width() - t->popupmenu_frame_shadow_right->width();
    ULONG bg_middle_height = 0;
    // lewy górny róg
    this->canvas->drawBitmap(t->popupmenu_frame_left_top, x, y);
    x += t->popupmenu_frame_left_top->width();
    if (this->_frameTopHiliteWidth > 0) {
        // ramka podświetlona (tuż pod pozycją menu głównego)
        this->canvas->stretchBitmap(t->popupmenu_frame_top_hilited,
            x, y,
            this->_frameTopHiliteWidth,
            t->popupmenu_frame_top_hilited->height()
        );
        x += this->_frameTopHiliteWidth;
        this->canvas->drawBitmap(t->popupmenu_frame_top_hilited_end, x, y);
        x += t->popupmenu_frame_top_hilited_end->width();
    }
    // ramka normalna
    LONG normalTopFrameWidth = middle_width - this->_frameTopHiliteWidth - t->popupmenu_frame_top_hilited_end->width();
    this->canvas->stretchBitmap(t->popupmenu_frame_top,
        x, y,
        normalTopFrameWidth,
        t->popupmenu_frame_top->height()
    );
    x += normalTopFrameWidth;
    // prawy górny róg
    this->canvas->drawBitmap(t->popupmenu_frame_top_right, x, y);
    // ================================================================
    // Koniec rysowania górnej ramki
    // ================================================================

    // ================================================================
    // Rysujemy lewą ramkę
    // ================================================================
    x = 0;
    y = t->popupmenu_frame_left_top->height();
    this->canvas->stretchBitmap(t->popupmenu_frame_left,
        x, y,
        t->popupmenu_frame_left->width(),
        this->_summarizeItemHeight
    );
    // ================================================================
    // Koniec rysowania lewej ramki
    // ================================================================

    // ================================================================
    // Rysujemy dolną ramkę
    // ================================================================
    x = 0;
    y = this->height - t->popupmenu_frame_bottom->height() - t->popupmenu_frame_shadow_bottom->height();
    this->canvas->drawBitmap(t->popupmenu_frame_bottom_left, x, y);
    x += t->popupmenu_frame_bottom_left->width();
    this->canvas->stretchBitmap(t->popupmenu_frame_bottom,
        x, y,
        middle_width,
        t->popupmenu_frame_bottom->height()
    );
    x += middle_width;
    this->canvas->drawBitmap(t->popupmenu_frame_bottom_right, x, y);
    // ================================================================
    // Koniec rysowania dolnej ramki
    // ================================================================

    // ================================================================
    // Rysujemy prawą ramkę
    // ================================================================
    x = width - t->popupmenu_frame_right->width() - t->popupmenu_frame_shadow_right->width();
    y = t->popupmenu_frame_top_right->height();
    this->canvas->stretchBitmap(t->popupmenu_frame_right,
        x, y,
        t->popupmenu_frame_right->width(),
        this->_summarizeItemHeight
    );
    // ================================================================
    // Koniec prawej lewej ramki
    // ================================================================


    // ================================================================
    // Rysujemy prawy cień
    // ================================================================
    x = width - t->popupmenu_frame_shadow_right->width();
    y = CMENU_POPUPMENU_SHADOW_TOP_INDENT;
    this->canvas->drawBitmap(t->popupmenu_frame_shadow_top_right, x, y);
    y += t->popupmenu_frame_shadow_top_right->height();
    this->canvas->stretchBitmap(t->popupmenu_frame_shadow_right,
        x, y,
        t->popupmenu_frame_shadow_right->width(),
        this->height - CMENU_POPUPMENU_SHADOW_TOP_INDENT - t->popupmenu_frame_shadow_top_right->height() - t->popupmenu_frame_shadow_bottom_right->height()
    );
    // ================================================================
    // Koniec prawego cienia
    // ================================================================

    // ================================================================
    // Rysujemy dolny cień
    // ================================================================
    x = CMENU_POPUPMENU_SHADOW_LEFT_INDENT;
    y = this->height - t->popupmenu_frame_shadow_bottom->height();
    this->canvas->drawBitmap(t->popupmenu_frame_shadow_bottom_left, x, y);
    x += t->popupmenu_frame_shadow_bottom_left->width();
    LONG bottom_shadow_inner_width = this->width - CMENU_POPUPMENU_SHADOW_LEFT_INDENT - t->popupmenu_frame_shadow_bottom_left->width() - t->popupmenu_frame_shadow_bottom_right->width();
    this->canvas->stretchBitmap(t->popupmenu_frame_shadow_bottom,
        x, y,
        bottom_shadow_inner_width,
        t->popupmenu_frame_shadow_bottom->height()
    );
    x += bottom_shadow_inner_width;
    this->canvas->drawBitmap(t->popupmenu_frame_shadow_bottom_right, x, y);
    // ================================================================
    // Koniec dolnego cienia
    // ================================================================

    // ================================================================
    // Rysujemy lewy pasek na ewentualne ikonki
    LONG inner_menu_height = this->height - t->popupmenu_frame_top->height() - t->popupmenu_frame_bottom->height() - t->popupmenu_frame_shadow_bottom->height();
    LONG inner_menu_width = this->width - t->popupmenu_frame_left->width() - t->popupmenu_frame_right->width() - t->popupmenu_frame_shadow_right->width() - t->popupmenu_icon_area_left->width();
    // ================================================================
    x = t->popupmenu_frame_left_top->width();
    y = t->popupmenu_frame_top->height();
    this->canvas->stretchBitmap(t->popupmenu_icon_area_left,
        x, y,
        t->popupmenu_icon_area_left->width(),
        inner_menu_height
    );
    // ================================================================
    // Koniec rysowania lewego paska na ewentualne ikonki
    // ================================================================

    // ================================================================
    // Rysujemy tło popupmenu
    // ================================================================
    x = t->popupmenu_frame_left_top->width() + t->popupmenu_icon_area_left->width();
    y = t->popupmenu_frame_top->height();
    this->canvas->stretchBitmap(t->popupmenu_bg,
        x, y,
        inner_menu_width,
        inner_menu_height
    );
    // ================================================================
    // Koniec rysowania tła popupmenu
    // ================================================================

    // ================================================================
    // Wypisujemy pozycje popupmenu
    // ================================================================
	this->canvas->setBkMode(TRANSPARENT);
    x = t->popupmenu_frame_left_top->width() + t->popupmenu_icon_area_left->width() + CMENU_HORIZONTAL_GAP_BETWEEN_ICONBAR_AND_CAPTION;
    y = t->popupmenu_frame_top->height() + CMENU_VERTICAL_GAP / 2;
    CmzArrayList *menuItems = this->_menuItems;
	if (menuItems) {
		CmzStdObjectProperty *item, *icon;
		CmzString caption;
        CmzString shortcut;
        SIZE size, shortcutSize;
        LONG item_height = this->_theme->popupmenu_selected_item_bg_middle->height();
		for (ULONG i = 0; i < menuItems->size(); i++) {
			item = (CmzStdObjectProperty*)menuItems->getAt(i);
			if (item->type == STDOBJ_PROPERTY_OBJECT) {
				caption = item->oValue->get(L"caption")->value;
                if (caption == L"-") {
                    this->canvas->stretchBitmap(t->popupmenu_divider,
                        x - t->popupmenu_icon_area_left->width() + CMENU_HORIZONTAL_DIVIDER_INDENT, y + CMENU_VERTICAL_MARGIN_FOR_DIVIDER,
                        inner_menu_width + t->popupmenu_icon_area_left->width() - CMENU_HORIZONTAL_DIVIDER_INDENT - 2 * CMENU_HORIZONTAL_GAP_BETWEEN_ICONBAR_AND_CAPTION,
                        t->popupmenu_divider->height()
                    );
                    y += CMENU_VERTICAL_MARGIN_FOR_DIVIDER + t->popupmenu_divider->height() + CMENU_VERTICAL_MARGIN_FOR_DIVIDER;
                } else {
                    // jeśli pozycja jest podświetlona, to rysujemy podświetlenie
                    if (item->extraData->get(L"hilited")->iValue != 0) {
                        LONG w = width - t->popupmenu_frame_left->width() - t->popupmenu_frame_right->width() - t->popupmenu_frame_shadow_right->width() - t->popupmenu_selected_item_bg_left->width() - t->popupmenu_selected_item_bg_right->width();
                        LONG hx = t->popupmenu_frame_left->width();
                        LONG hy = y - CMENU_VERTICAL_GAP / 2;
                        this->canvas->drawBitmap(t->popupmenu_selected_item_bg_left, hx, hy);
                        hx += t->popupmenu_selected_item_bg_left->width();
                        this->canvas->stretchBitmap(t->popupmenu_selected_item_bg_middle,
                            hx, hy,
                            w,
                            t->popupmenu_selected_item_bg_middle->height()
                        );
                        hx += w;
                        this->canvas->drawBitmap(t->popupmenu_selected_item_bg_right, hx, hy);
                    }

                    shortcut = L"";
                    if (item->oValue->get(L"shortcut")) {
                        shortcut = item->oValue->get(L"shortcut")->value;
                    }
                    HDC hdc = this->canvas->getDC();
                    if (hdc == 0) {
                        this->canvas->beginPaint();
                    }
				    size = t->menuFont->measureString(caption, this->canvas->getDC());
                    this->canvas->drawText(t->menuFont, caption, x, y + (this->_itemHeight - size.cy) / 2);
				    item->extraData->set(L"x", static_cast<LONGLONG>(t->popupmenu_frame_left_top->width()));
				    item->extraData->set(L"y", static_cast<LONGLONG>(y));
                    item->extraData->set(L"popup_x", static_cast<LONGLONG>(this->x + this->width));
                    item->extraData->set(L"popup_y", static_cast<LONGLONG>(y));
			        item->extraData->set(L"width", static_cast<LONGLONG>(this->width - t->popupmenu_frame_left->width() - t->popupmenu_frame_right->width() - t->popupmenu_frame_shadow_right->width()));
                    item->extraData->set(L"height", static_cast<LONGLONG>(this->_itemHeight));
                    if (shortcut.length() > 0) {
				        shortcutSize = t->menuFont->measureString(shortcut, this->canvas->getDC());
						LONG x_shortcut = width - t->popupmenu_frame_right->width() - t->popupmenu_frame_shadow_right->width() - CMENU_HORIZONTAL_GAP_BETWEEN_SHORTCUT_AND_ARROW - this->_theme->popupmenu_submenu_arrow->width() - this->_largestShortcutWidth;
						this->canvas->drawText(t->menuFont, shortcut, x_shortcut, y + (this->_itemHeight - shortcutSize.cy) / 2);
                    }
                    if (hdc == 0) {
                        this->canvas->endPaint();
                    }
                    // i na koniec rysujemy ikonkę jeśli taka jest do menu przypisana
                    icon = item->oValue->get(L"icon");
                    if (icon && this->_imageList) {
                        // TUTAJ KOD rysujący ikonkę
                        //this->_imageList->Bitblt(this->canvas->getDC(), (t->popupmenu_icon_area_left->width() - this->_imageList->imWidth) / 2, y + (this->_itemHeight - this->_imageList->imHeight) / 2, icon->value);
                        this->_imageList->transparentBitblt(this->canvas->getDC(), (t->popupmenu_icon_area_left->width() - this->_imageList->imWidth) / 2, y + (this->_itemHeight - this->_imageList->imHeight) / 2, icon->value);
                    }
				    y += item_height + CMENU_VERTICAL_GAP;
                }
			}
		}
	}
    // ================================================================
    // Koniec wypisywania pozycji popupmenu
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

VOID mzstd::CMenu::onDraw(CmzBitmap *dest) {
	CSprite::onDraw(dest);
}

VOID mzstd::CMenu::setSizeRelativeToParent(LONG parentWidth, LONG parentHeight) {
	this->setSize(parentWidth, this->_theme->menu_bg_middle->height());
}

BOOL mzstd::CMenu::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    CmzStdObjectProperty *item;
	CmzStdObjectProperty *v;
	ULONG i;
	BOOL redrawMenu = FALSE;

	if (!this->pointIn(x, y)) {
        BOOL clearHilite = TRUE;

        // jeśli kursor jest w ewentualnie otwartym podmenu, to nie likwidujemy
        // zaznaczenia
        if (this->_opened && this->_openedSubmenu) {
            clearHilite = FALSE;
        }

        if (clearHilite) {
            redrawMenu = this->_clearHilite();
		    if (redrawMenu) {
                // zamykamy otwarte podmenu (o ile było takie - sprawdzenie jest
                // robione w poniższej funkcji)
                this->closeOpenedSubmenu();
                this->_opened = FALSE;

			    CStyledWindow *wnd = this->getParentWindow();
			    if (wnd) {
				    this->_needUpdate = TRUE;
				    wnd->invalidate();
			    }
		    }
        }
		return FALSE;
	}


	LONG item_x, item_y;
	LONG item_width, item_height;
	LONG menu_height;
    LONG selected_item_height;
    LONG tbMargin;
    BOOL isItemAlreadyHilited;

	if (this->_menuType == CMENU_MENU_TYPE_MAINMENU) {
        menu_height = this->height;
        selected_item_height = this->_theme->menu_selected_mainmenu_item_bg_middle->height();
        tbMargin = (menu_height - selected_item_height) / 2;
	} else {
        selected_item_height = this->_theme->popupmenu_selected_item_bg_middle->height();
        menu_height = selected_item_height;
        tbMargin = 0;
    }

    // musimy sprawdzić, czy w ogóle jesteśmy nad jakąś pozycją
    BOOL isMouseOverItem = FALSE;
	for (i = 0; i < this->_menuItems->size(); i++) {
		item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
		if (!item->extraData || !item->extraData->get(L"width")) {
			continue;
		}
		if (item->type == STDOBJ_PROPERTY_VALUE || item->type == STDOBJ_PROPERTY_OBJECT) {
			item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
			item_y = static_cast<LONG>(item->extraData->get(L"y")->iValue);
			item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);
            item_height = static_cast<LONG>(item->extraData->get(L"height")->iValue);
		}

		if (x >= this->x + item_x && x <= this->x + item_x + item_width && y >= this->y + item_y + tbMargin && y <= this->y + item_y + tbMargin + item_height) {
            isMouseOverItem = TRUE;
            break;
        }
    }

    isItemAlreadyHilited = FALSE;
    BOOL openedMenuWasClosed = FALSE;
	for (i = 0; i < this->_menuItems->size(); i++) {
		item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
		if (!item->extraData || !item->extraData->get(L"width")) {
			continue;
		}
		if (item->type == STDOBJ_PROPERTY_VALUE || item->type == STDOBJ_PROPERTY_OBJECT) {
			item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
			item_y = static_cast<LONG>(item->extraData->get(L"y")->iValue);
			item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);
            item_height = static_cast<LONG>(item->extraData->get(L"height")->iValue);
		}

		if (!isItemAlreadyHilited && (x >= this->x + item_x && x <= this->x + item_x + item_width && y >= this->y + item_y + tbMargin && y <= this->y + item_y + tbMargin + item_height)) {
			v = item->extraData->get(L"hilited");
			if (!v || v->iValue != 1) {
			    redrawMenu = TRUE;
                if (this->_opened) {
                    if (!openedMenuWasClosed) {
                        this->closeOpenedSubmenu();
                    }
                    openedMenuWasClosed = TRUE;
                    this->openPopupMenu(item);
                }
			}
			item->extraData->set(L"hilited", 1LL);
            isItemAlreadyHilited = TRUE;
		} else {
			v = item->extraData->get(L"hilited");
			if (!v || v->iValue != 0) {
                if (!isMouseOverItem && this->_opened && this->_openedSubmenu) {
                    continue;
                }
				redrawMenu = TRUE;
                // zamykamy otwarte podmenu (o ile było takie - sprawdzenie jest
                // robione w poniższej funkcji)
                if (!openedMenuWasClosed) {
                    this->closeOpenedSubmenu();
                }
                openedMenuWasClosed = TRUE;
			}
			item->extraData->set(L"hilited", 0LL);
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

BOOL mzstd::CMenu::closeOpenedSubmenu() {
    if (this->_opened && this->_openedSubmenu) {
        if (this->_openedSubmenu->parent) {
            CPanel *panel = dynamic_cast<CPanel*>(this->_openedSubmenu->parent);
            if (panel) {
                panel->removeChild(this->_openedSubmenu);
                delete this->_openedSubmenu;
                this->_openedSubmenu = NULL;
                panel->invalidate();
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL mzstd::CMenu::openPopupMenu(CmzStdObjectProperty *item) {
    if (this->parent) {
        CPanel *panel = dynamic_cast<CPanel*>(this->parent);
        CmzString caption;
        if (panel) {
	        caption = item->oValue->get(L"caption")->value;
            CmzStdObjectProperty *items = (CmzStdObjectProperty*)item->oValue->get(L"items");
            if (items->type == STDOBJ_PROPERTY_ARRAY) {
                mzstd::CMenu *popupMenu = new mzstd::CMenu(panel, CmzString(L"PopupMenu_") + caption, this, CMENU_MENU_TYPE_POPUPMENU, this->_imageList);
                popupMenu->_transparentDrawing = FALSE;
                CmzArrayList *aItems = (CmzArrayList*)items->aValue;
                popupMenu->setMenuItems(aItems);
                if (item->extraData->get(L"hilite_width")) {
                    popupMenu->setTopFrameHiliteWidth(static_cast<LONG>(item->extraData->get(L"hilite_width")->iValue));
                } else {
                    popupMenu->setTopFrameHiliteWidth(0);
                }
                LONG x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
                LONG y = static_cast<LONG>(item->extraData->get(L"popup_y")->iValue);
                this->_openedSubmenu = popupMenu;
                popupMenu->setPosition(x, y);
                panel->addChild(popupMenu);
                panel->invalidate();
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL mzstd::CMenu::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

	CmzStdObjectProperty *item;
	CmzStdObjectProperty *v;
	ULONG i;
	BOOL redrawMenu = FALSE;
    this->_lMouseButtonPressed = FALSE;

	if (!this->pointIn(x, y)) {
        CStyledWindow *wnd = this->getParentWindow();
        BOOL processed = FALSE;

        // jeśli menu jest otwarte, to przekazujemy zdarzenie do otwartego podmenu
        if (this->_openedSubmenu) {
            processed = this->_openedSubmenu->onMouseDown(x, y);
        }

        if (!processed) {
            redrawMenu = this->_clearHilite();
            this->closeOpenedSubmenu();
            this->_opened = FALSE;
        }

		if (redrawMenu) {
			if (wnd) {
				this->_needUpdate = TRUE;
				wnd->invalidate();
			}
		}

		return processed;
	}

    this->_lMouseButtonPressed = TRUE;
    if (this->_parentMenu == NULL) {
        // aktywną kontrolką staje się tylko menu, które nie jest podmenu dla innego menu
        this->getParentWindow()->setActiveControl(this);
    }

	LONG item_x, item_y;
	LONG item_width;
	LONG menu_height = this->height;
	LONG selected_item_height = this->_theme->menu_selected_mainmenu_item_bg_middle->height();
	LONG tbMargin = 0;
    if (this->_menuType == CMENU_MENU_TYPE_MAINMENU) {
        tbMargin = (menu_height - selected_item_height) / 2;
    }
    CmzString caption;

    // czy kliknięto wewnątrz bieżącej pozycji menu
    BOOL inItem;
	for (i = 0; i < this->_menuItems->size(); i++) {
		item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
        // domyślnie nie kliknięto tej pozycji
        inItem = FALSE;
		if (!item->extraData) {
			continue;
		}
        if (item->type != STDOBJ_PROPERTY_VALUE && item->type != STDOBJ_PROPERTY_OBJECT) {
            continue;
        }

        if (!item->extraData->get(L"x")) {
            continue;
        }

		item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
		item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);

        if (this->_menuType == CMENU_MENU_TYPE_MAINMENU) {
            if (x >= this->x + item_x && x <= this->x + item_x + item_width && y >= this->y + tbMargin && y <= this->y + selected_item_height) {
                inItem = TRUE;
            }
        } else {
            CmzString c = item->oValue->get(L"caption")->getAsString();
            item_y = static_cast<LONG>(item->extraData->get(L"y")->iValue);
            if (x >= this->x + item_x && x <= this->x + item_x + item_width && y >= this->y + item_y && y <= this->y + item_y + this->_itemHeight) {
                inItem = TRUE;
            }
        }

		if (inItem) {
            if (item->type == STDOBJ_PROPERTY_OBJECT) {
                // Kliknięto tę pozycję - jeśli ma ona podmenu, to je otwieramy bądź
                // zamykamy
                CmzStdObjectProperty *subItems = item->oValue->get(L"items");
                if (subItems != NULL) {
                    if (this->_opened) {
                        this->closeOpenedSubmenu();
                        this->_opened = FALSE;
                    } else {
                        if (item->oValue->get(L"items")) {
                            this->_opened = TRUE;
                        }
                    }
                    redrawMenu = TRUE;
                    // Kliknięto tę pozycję
			        v = item->extraData->get(L"hilited");
			        if (v) {
                        item->extraData->set(L"hilited", 1LL);
                        if (this->_opened) {
                            this->openPopupMenu(item);
                        }
			        }
                }
            }
		} else {
			v = item->extraData->get(L"hilited");
			if (v) {
                if (v->iValue == 1) {
                    redrawMenu = TRUE;
                    if (this->_opened) {
                        this->closeOpenedSubmenu();
                        this->_opened = FALSE;
                    }
                }
                item->extraData->set(L"hilited", 0LL);
			} else {
                item->extraData->set(L"hilited", 0LL);
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

BOOL mzstd::CMenu::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

	CmzStdObjectProperty *item;
	ULONG i;
	BOOL redrawMenu = FALSE;

	if (!this->pointIn(x, y)) {
        this->_lMouseButtonPressed = FALSE;
        CStyledWindow *wnd = this->getParentWindow();
        BOOL processed = FALSE;

        redrawMenu = this->_clearHilite();
        // jeśli menu jest otwarte, to przekazujemy zdarzenie do otwartego podmenu
        if (this->_openedSubmenu) {
            processed = this->_openedSubmenu->onMouseUp(x, y);
        }

        if (!processed) {
            this->closeOpenedSubmenu();
            this->_opened = FALSE;
            // jeśli to menu główne, to zerujemy aktywną kontrolkę w Parent formularzu
            if (this->_menuType == CMENU_MENU_TYPE_MAINMENU) {
                this->getParentWindow()->setActiveControl(NULL);
            }
        }

		if (redrawMenu) {
			if (wnd) {
				this->_needUpdate = TRUE;
				wnd->invalidate();
			}
		}

		return processed;
	}

    // jeśli zwolniono wcześniej wciśnięty lewy przycisk myszki i mamy otwarte submenu,
    // to nic nie robimy (oprócz zaznaczenia, że lewy przycisk myszki nie jest już wciśnięty).
    if (this->_lMouseButtonPressed && this->_opened && this->_openedSubmenu) {
        this->_lMouseButtonPressed = FALSE;
        return FALSE;
    }

    this->_lMouseButtonPressed = FALSE;
	LONG item_x, item_y;
	LONG item_width;
	LONG menu_height = this->height;
	LONG selected_item_height = this->_theme->menu_selected_mainmenu_item_bg_middle->height();
	LONG tbMargin = 0;
    if (this->_menuType == CMENU_MENU_TYPE_MAINMENU) {
        tbMargin = (menu_height - selected_item_height) / 2;
    }
    CmzString caption;

    // czy kliknięto wewnątrz bieżącej pozycji menu
    BOOL inItem;
	for (i = 0; i < this->_menuItems->size(); i++) {
		item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
        // domyślnie nie kliknięto tej pozycji
        inItem = FALSE;
		if (!item->extraData) {
			continue;
		}
        if (item->type != STDOBJ_PROPERTY_VALUE && item->type != STDOBJ_PROPERTY_OBJECT) {
            continue;
        }

        if (!item->extraData->get(L"x")) {
            continue;
        }

		item_x = static_cast<LONG>(item->extraData->get(L"x")->iValue);
		item_width = static_cast<LONG>(item->extraData->get(L"width")->iValue);

        if (this->_menuType == CMENU_MENU_TYPE_MAINMENU) {
            if (x >= this->x + item_x && x <= this->x + item_x + item_width && y >= this->y + tbMargin && y <= this->y + selected_item_height) {
                inItem = TRUE;
            }
        } else {
            item_y = static_cast<LONG>(item->extraData->get(L"y")->iValue);
            if (x >= this->x + item_x && x <= this->x + item_x + item_width && y >= this->y + item_y && y <= this->y + item_y + this->_itemHeight) {
                inItem = TRUE;
            }
        }

		if (inItem) {
            if (item->type == STDOBJ_PROPERTY_OBJECT) {
                redrawMenu = TRUE;
                // Kliknięto tę pozycję - jeśli nie ma ona podmenu, to wywołujemy funkcję
                // obsługi kliknięcia w pozycję menu
                CmzStdObjectProperty *subItems = item->oValue->get(L"items");
                if (subItems == NULL) {
                    CStyledWindow *wnd = this->getParentWindow();
                    if (wnd) {
                        CmzStdObject *menuInfo = new CmzStdObject();
                        menuInfo->set(L"name", item->oValue->get(L"name")->value);
                        menuInfo->set(L"caption", item->oValue->get(L"caption")->value);
                        menuInfo->set(L"id", item->oValue->get(L"id")->value);
                        menuInfo->set(L"icon", item->oValue->get(L"icon")->value);
                        menuInfo->set(L"menu_name", this->name);
                        CmzString root_menu_name = L"";
                        if (this->getParentMenu()) {
                            root_menu_name = this->getParentMenu()->name;
                        }
                        menuInfo->set(L"root_menu_name", root_menu_name);
                        menuInfo->set(L"window_name", wnd->getName());
                        if (item->oValue->get(L"shortcut")) {
                            menuInfo->set(L"shortcut", item->oValue->get(L"shortcut")->value);
                        } else {
                            menuInfo->set(L"shortcut", L"");
                        }

                        CApplication* app = wnd->getApplication();
                        menuInfo->set(L"event_type", static_cast<LONGLONG>(WM_MENUITEM_CLICKED));
                        app->pushClickedMenuItemData(menuInfo);

                        // zerujemy aktywną kontrolkę w Parent formularzu
                        this->getParentWindow()->setActiveControl(NULL);

                        PostMessage(wnd->getHwnd(), WM_MENUITEM_CLICKED, (WPARAM)app, NULL);
                    }

                    if (this->_parentMenu && this->_parentMenu->_opened) {
                        mzstd::CMenu *parentMenu = this->_parentMenu;
                        parentMenu->_needUpdate = TRUE;
                        parentMenu->closeOpenedSubmenu();
                        parentMenu->_opened = FALSE;
                        parentMenu->redraw();
                        return TRUE;
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

BOOL mzstd::CMenu::_clearHilite() {
	CmzStdObjectProperty *item;
	CmzStdObjectProperty *v;
    ULONG i;
    BOOL redrawMenu = FALSE;

    if (!this->_menuItems) {
        return FALSE;
    }
	// ustawiamy hilite wszystkich pozycji menu na 0
	for (i = 0; i < this->_menuItems->size(); i++) {
		item = (CmzStdObjectProperty*)this->_menuItems->getAt(i);
		if (!item->extraData) {
			continue;
		}
		if (item->type == STDOBJ_PROPERTY_VALUE || item->type == STDOBJ_PROPERTY_OBJECT) {
			v = item->extraData->get(L"hilited");
			if (!v || v->iValue != 0) {
				redrawMenu = TRUE;
			}
			item->extraData->set(L"hilited", 0LL);
		}
	}

    return redrawMenu;
}

VOID mzstd::CMenu::setMenuItems(CmzArrayList *menuItems) {
    // akcję w tej metodzie wykonujemy tylko w przypadku POPUP_MENU
    if (this->_menuType == CMENU_MENU_TYPE_POPUPMENU) {
        // teraz jeszcze musimy określić szerokość _largestItemWidth
        SIZE sizeCaption, sizeShortcut;
        CmzStdObjectProperty *item;
        CmzString caption;
        CmzString shortcut;
        CmzStdObjectProperty *subItems;
        LONG sumWidth;
        LONG maxHeight = 0;

        this->_menuItems = menuItems;

        if (menuItems && menuItems->size() > 0) {
            // na wszystich pozycjach tego menu ustawiamy wyzerowaną flagę hilited
            for (ULONG i = 0; i < menuItems->size(); i++) {
                item = (CmzStdObjectProperty*)menuItems->getAt(i);
                if (!item->extraData) {
                    item->extraData = new CmzStdObject();
                }
                item->extraData->set(L"hilited", 0LL);
            }
        }

        this->_itemHeight = 0;
        this->_summarizeItemHeight = 0;
        this->_largestItemWidth = 0;
        this->_largestShortcutWidth = 0;
        this->_largestSummarizeItemWidth = 0;
        for (ULONG i = 0; i < menuItems->size(); i++) {
            item = (CmzStdObjectProperty*)menuItems->getAt(i);
            if (item->type == STDOBJ_PROPERTY_VALUE) {
                caption = item->value;
                shortcut = L"";
                subItems = NULL;
            } else if (item->type == STDOBJ_PROPERTY_OBJECT) {
                caption = item->oValue->get(L"caption")->value;
                if (item->oValue->get(L"shortcut")) {
                    shortcut = item->oValue->get(L"shortcut")->value;
                } else {
                    shortcut = L"";
                }
                subItems = (CmzStdObjectProperty*)item->oValue->get(L"items");
            } else {
                caption = L"";
                shortcut = L"";
                subItems = NULL;
            }
            sizeCaption.cx = 0;
            sizeCaption.cy = 0;
            sizeShortcut.cx = 0;
            sizeShortcut.cy = 0;
            CmzBitmap* cnv = this->canvas;
            if (!cnv && this->_parentMenu) {
                cnv = this->_parentMenu->canvas;
            }
            if (!cnv && this->parent) {
                cnv = this->parent->canvas;
            }
            HDC hdc = cnv->getDC();
            if (hdc == 0) {
                cnv->beginPaint();
            }
            if (caption.length() > 0) {
                sizeCaption = this->_theme->menuFont->measureString(caption, cnv->getDC());
                if (sizeCaption.cx > this->_largestItemWidth) {
                    this->_largestItemWidth = sizeCaption.cx;
                }
            }
            if (shortcut.length() > 0) {
                sizeShortcut = this->_theme->menuFont->measureString(shortcut, cnv->getDC());
                if (sizeShortcut.cx > this->_largestShortcutWidth) {
                    this->_largestShortcutWidth = sizeShortcut.cx;
                }
            }
            if (hdc == 0) {
                cnv->endPaint();
            }
            sumWidth = CMENU_HORIZONTAL_GAP_BETWEEN_ICONBAR_AND_CAPTION + sizeCaption.cx + CMENU_HORIZONTAL_GAP_BETWEEN_ITEM_CONTENT_AND_RIGHT_BORDER + this->_theme->popupmenu_frame_left->width() + this->_theme->popupmenu_frame_right->width() + this->_theme->popupmenu_frame_shadow_right->width();
            if (sizeShortcut.cx) {
                sumWidth += CMENU_HORIZONTAL_GAP_BETWEEN_CAPTION_AND_SHORTCUT + sizeShortcut.cx;
            }
            if (subItems) {
                sumWidth += this->_theme->popupmenu_submenu_arrow->width() + CMENU_HORIZONTAL_GAP_BETWEEN_SHORTCUT_AND_ARROW;
            }

            sumWidth += 40;
            if (sumWidth > this->_largestSummarizeItemWidth) {
                this->_largestSummarizeItemWidth = sumWidth;
            }

            if (caption != L"-") {
                if (sizeCaption.cy > maxHeight) {
                    maxHeight = sizeCaption.cy;
                }
                if (sizeShortcut.cy > maxHeight) {
                    maxHeight = sizeShortcut.cy;
                }
                if (this->_theme->popupmenu_selected_item_bg_middle->height() > maxHeight) {
                    maxHeight = this->_theme->popupmenu_selected_item_bg_middle->height();
                }
                this->_summarizeItemHeight += maxHeight + CMENU_VERTICAL_GAP;
            } else {
                this->_summarizeItemHeight += CMENU_VERTICAL_MARGIN_FOR_DIVIDER + this->_theme->popupmenu_divider->height() + CMENU_VERTICAL_MARGIN_FOR_DIVIDER;
            }
        }

        this->_itemHeight = maxHeight + CMENU_VERTICAL_GAP;
    }

    // musimy jeszcze ustawić bitmapę sprite'a na odpowiednie rozmiary
    LONG width = this->_largestSummarizeItemWidth + this->_theme->popupmenu_frame_shadow_right->width() + this->_theme->popupmenu_icon_area_left->width();
    LONG height = this->_summarizeItemHeight + this->_theme->popupmenu_frame_left_top->height() + this->_theme->popupmenu_frame_bottom_left->height() + this->_theme->popupmenu_frame_shadow_bottom->height();
    this->setSize(width, height);
    this->canvas->fillWithColor(this->_transparentColor);
}

mzstd::CMenu* WINAPI mzstd::CMenu::getRootMenu() {
    if (this->_parentMenu == NULL) {
        return this;
    }

    mzstd::CMenu* rootMenu = this->_parentMenu;
    while (rootMenu->_parentMenu) {
        rootMenu = rootMenu->_parentMenu;
    }

    return rootMenu;
}
