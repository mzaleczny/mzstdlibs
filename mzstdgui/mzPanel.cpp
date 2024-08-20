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
#include "mzPanel.h"
#include "mzstdguiGlobals.h"
#include "mzSprite.h"
#include "mzBitmap.h"
#include "mzTheme.h"
#include "mzMenu.h"
using namespace mzstd;

CPanel::CPanel(CSprite *parent, CmzString name, LONG width, LONG height, LONG x, LONG y, BOOLEAN transparent)
    : CGuiContainer(parent, name, NULL, x, y)
{
	this->_commonInit(width, height, transparent);
}

CPanel::CPanel(CSprite *parent, CStyledWindow* const parentWnd, CmzString name, LONG width, LONG height, LONG x, LONG y, BOOLEAN transparent)
    : CGuiContainer(parent, parentWnd, name, NULL, x, y)
{
	this->_commonInit(width, height, transparent);
}

VOID CPanel::_commonInit(LONG width, LONG height, BOOLEAN transparent) {
    this->_transparent = transparent;
    this->setRealClassName(L"CPanel");
    this->_needUpdate = TRUE;

    if (width == -1) {
        width = DEFAULT_PANEL_WIDTH;
    }
    if (height == -1) {
        height = DEFAULT_PANEL_HEIGHT;
    }

    this->setCanvas(new CmzBitmap(width, height, RGB(0xff, 0xff, 0xff)));
}

CPanel::~CPanel() {
}

VOID CPanel::_updateCanvas() {
    this->_needUpdate = FALSE;
    if (!this->canvas) {
        return;
    }

    LONG x = 0;
    LONG y = 0;
    RECT rc;
    CTheme *t = this->_theme;
    HDC hdc = GetDC(0);
    ULONG caption_middle_width = 0;
    ULONG caption_middle_height = 0;

    this->canvas->beginPaint(hdc);
    if (!this->_transparent) {
        // ================================================================
        // Rysujemy belkę tytułową (o ile jest podany nagłówek)
        // ================================================================
        if (this->_caption.length() > 0) {
            this->canvas->drawBitmap(t->panel_caption_left, x, y);
            x += t->panel_caption_left->width();
            caption_middle_width = this->canvas->width() - t->panel_caption_left->width() - t->panel_caption_right->width();
            caption_middle_height = t->panel_caption_middle->height();
            this->canvas->stretchBitmap(t->panel_caption_middle,
                x, y,
                caption_middle_width,
                caption_middle_height
            );
            x += caption_middle_width;
            this->canvas->drawBitmap(t->panel_caption_right, x, y);
        } else {
            this->canvas->drawBitmap(t->panel_nocaption_left, x, y);
            x += t->panel_nocaption_left->width();
            caption_middle_width = this->canvas->width() - t->panel_nocaption_left->width() - t->panel_nocaption_right->width();
            caption_middle_height = t->panel_nocaption_middle->height();
            this->canvas->stretchBitmap(t->panel_nocaption_middle,
                x, y,
                caption_middle_width,
                t->panel_nocaption_middle->height()
            );
            x += caption_middle_width;
            this->canvas->drawBitmap(t->panel_nocaption_right, x, y);
        }
        // ================================================================
        // Koniec rysowania belki tytułowej
        // ================================================================

        // ================================================================
        // Rysujemy obramowanie panelu
        // ================================================================
        // Ramka lewa
        x = 0;
        y = caption_middle_height;
        this->canvas->stretchBitmap(t->panel_frame_left,
            x, y,
            t->panel_frame_left->width(),
            this->canvas->height() - y - t->panel_frame_bottom_left->height()
        );

        // Lewy dolny róg
        y = this->canvas->height() - t->panel_frame_bottom_left->height();
        this->canvas->drawBitmap(t->panel_frame_bottom_left, x, y);

        // Ramka dolna
        x += t->panel_frame_bottom_left->width();
        y = this->canvas->height() - t->panel_frame_bottom_left->height();
        this->canvas->stretchBitmap(t->panel_frame_bottom,
            x, y,
            this->canvas->width() - x - t->panel_frame_bottom_right->width(),
            t->panel_frame_bottom->height()
        );

        // Prawy dolny róg
        x = this->canvas->width() - t->panel_frame_bottom_right->width();
        y = this->canvas->height() - t->panel_frame_bottom_right->height();
        this->canvas->drawBitmap(t->panel_frame_bottom_right, x, y);

        // Ramka prawa
        x = this->canvas->width() - t->panel_frame_right->width();
        y = caption_middle_height;
        this->canvas->stretchBitmap(t->panel_frame_right,
            x, y,
            t->panel_frame_right->width(),
            this->canvas->height() - y - t->panel_frame_bottom_right->height()
        );
        // ================================================================
        // Koniec rysowania obramowania panelu
        // ================================================================


        // ================================================================
        // Nagłówek panelu
        // ================================================================
        if (this->_caption.length() > 0) {
            rc.left = t->panel_frame_left->width() + 4;
            rc.top = 4;
            rc.right = rc.left + caption_middle_width;
            rc.bottom = rc.top + caption_middle_height - 4;
            this->canvas->setBkMode(TRANSPARENT);
            this->canvas->drawText(t->captionFont, this->_caption, rc);
        }
        // ================================================================
        // Koniec rysowania nagłówka panelu
        // ================================================================

        LONG clientWidth = this->width - t->panel_frame_left->width() - t->panel_frame_right->width();
        LONG clientHeight = this->height - caption_middle_height - t->panel_frame_bottom->height();
        // ================================================================
        // Rysujemy tło panelu
        // ================================================================

        x = t->panel_frame_left->width();
        y = caption_middle_height;
        this->canvas->drawBitmapPart(t->panel_inner_bg_fullscreen, x, y, 0, 0, clientWidth, clientHeight);
        // ================================================================
        // Koniec rysowania tła panelu
        // ================================================================
    }
    else
    {
        // ================================================================
        // Rysujemy tło panelu na całym jego obszarze
        // ================================================================
        this->canvas->drawBitmapPart(t->panel_inner_bg_fullscreen, 0, 0, 0, 0, this->width, this->height);
        // ================================================================
        // Koniec rysowania tła panelu
        // ================================================================
    }


    this->canvas->endPaint();

    ReleaseDC(0, hdc);
}

VOID CPanel::onDraw(CmzBitmap *dest) {
    if (!this->_visible) return;
	if (!this->canvas) return;

    if (this->_needUpdate) {
        this->invalidateChildSpritesCanvases();
        this->_updateCanvas();
    }

    // ================================================================
    // drawing child-sprites
    // ================================================================
	HDC hdc = GetDC(0);
	this->canvas->beginPaint(hdc);
    this->canvas->setBkMode(TRANSPARENT);
    this->drawChildSprites(this->canvas);
	this->canvas->endPaint();
	ReleaseDC(0, hdc);
    // ================================================================
    // end of drawing child-sprites
    // ================================================================

    dest->drawBitmap(this->canvas, this->x, this->y, this->_currentFrame);
}

BOOL CPanel::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
	// obliczamy współrzędne w obrębie tego sprite'a i przekazujemy je do sprite'ów
	// zawartych w tym pojemniku
	LONG coord_relative_to_this_x;
	LONG coord_relative_to_this_y;
    CSprite *sprite;

    if (!this->pointIn(x, y)) {
	    coord_relative_to_this_x = -1;
	    coord_relative_to_this_y = -1;
	} else {
	    coord_relative_to_this_x = x - this->x;
	    coord_relative_to_this_y = y - this->y;
    }

    // Najpierw obsługujemy ewentualnie otwarte pozycje menu, gdyż one mogą przykrywać inne
    // sprite'y a powinny być obsłużone jako pierwsze
	CmzLinkListNode *n = this->_sprites.first();
    mzstd::CMenu* menu;
    BOOL mouseOverMenu = FALSE; // czy zdażenie WM_MOUSE zostało obsłużone przez któreś z menu
    CGuiControl* gc;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        menu = dynamic_cast<mzstd::CMenu*>(sprite);
        if (menu && menu->getMenuItems() && menu->getMenuItems()->size() > 0) {
		    if (!gc->onMouseMove(coord_relative_to_this_x, coord_relative_to_this_y)) {
			    if (gc->pointIn(coord_relative_to_this_x, coord_relative_to_this_y)) {
				    gc->play();
			    } else {
				    gc->play(FALSE);
			    }
		    } else {
                // zaznaczamy, że myszka jest nad menu
                mouseOverMenu = TRUE;
            }
        }
        n = n->next;
    }

    // jeśli menu obsłużyło ruch myszki, to do pozostałych kontrolek wysyłamy zdarzenie ze
    // współrzędnymi gwarantującymi, że nie rozpoznają myszki nad sobą (czyli wykonają potrzebne
    // akcje czyszczące, np. usunięcie zaznaczenia/podświetlenia itp. itd.);
    if (mouseOverMenu) {
        coord_relative_to_this_x = MININT;
        coord_relative_to_this_y = MININT;
    }

    // a teraz pozostałe sprite'y
	n = this->_sprites.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        menu = dynamic_cast<mzstd::CMenu*>(sprite);
        if (!menu && gc) {
		    if (!gc->onMouseMove(coord_relative_to_this_x, coord_relative_to_this_y)) {
			    if (gc->pointIn(coord_relative_to_this_x, coord_relative_to_this_y)) {
				    gc->play();
			    } else {
				    gc->play(FALSE);
			    }
		    }
        }
        n = n->next;
    }

	return TRUE;
}

BOOL CPanel::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
	// obliczamy współrzędne w obrębie tego sprite'a i przekazujemy je do sprite'ów
	// zawartych w tym pojemniku
	LONG coord_relative_to_this_x;
	LONG coord_relative_to_this_y;
    //CSprite *sprite;

    if (!this->pointIn(x, y)) {
	    coord_relative_to_this_x = -1;
	    coord_relative_to_this_y = -1;
	} else {
	    coord_relative_to_this_x = x - this->x;
	    coord_relative_to_this_y = y - this->y;
    }

    BOOL result = this->doChildOnMouseDown(coord_relative_to_this_x, coord_relative_to_this_y);
    if (result) {
        return result;
    }
    
    if (coord_relative_to_this_x == -1 && coord_relative_to_this_y == -1) {
        return FALSE;
    } else {
	    return TRUE;
    }
}

BOOL CPanel::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
	// obliczamy współrzędne w obrębie tego sprite'a i przekazujemy je do sprite'ów
	// zawartych w tym pojemniku
	LONG coord_relative_to_this_x;
	LONG coord_relative_to_this_y;
    CSprite *sprite;

    if (!this->pointIn(x, y)) {
	    coord_relative_to_this_x = -1;
	    coord_relative_to_this_y = -1;
	} else {
	    coord_relative_to_this_x = x - this->x;
	    coord_relative_to_this_y = y - this->y;
    }

	CmzLinkListNode *n = this->_sprites.first();
    mzstd::CMenu *menu;
    CGuiControl* gc;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        menu = dynamic_cast<mzstd::CMenu*>(sprite);
        if (menu && menu->getMenuItems() && menu->getMenuItems()->size() > 0) {
            // jeśli mamy menu, to zdarzenie wysyłamy tylko do menu najwyższego poziomu
            // bo ono dba o rozpropagowanie zdarzenia w obrębie wszystkich otwartych
            // podmenu
            if (menu->getParentMenu() == NULL) {
                if (gc->onMouseUp(coord_relative_to_this_x, coord_relative_to_this_y)) {
                    return TRUE;
                }
            }
        } else {
		    if (gc && gc->onMouseUp(coord_relative_to_this_x, coord_relative_to_this_y)) {
                return TRUE;
            }
        }
        n = n->next;
    }

    if (coord_relative_to_this_x == -1 && coord_relative_to_this_y == -1) {
	    return FALSE;
    } else {
        return TRUE;
    }
}
