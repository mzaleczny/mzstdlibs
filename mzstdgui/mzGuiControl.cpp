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
#include "mzGuiControl.h"
#include "mzstdguiGlobals.h"
#include "mzstd.h"
#include "mzAnimatedBitmap.h"
#include "mzBitmap.h"
#include "mzStyledWindow.h"
#include "mzApplication.h"
#include "mzCursor.h"
#include "mzMenu.h"
#include "mzScrollbarVertical.h"
#include "mzScrollbarHorizontal.h"
#include "mzTextField.h"
#include "mzMessageBox.h"
#include "mzMessages.h"
using namespace mzstd;

CGuiControlItem::CGuiControlItem(CmzString value, BOOL selected, BOOL checked, LONG imageIndex, LONG data)
    : value(value), selected(selected), checked(checked), imageIndex(imageIndex), data(data)
{
    this->setRealClassName(L"CGuiControlItem");
}


CGuiControl::CGuiControl(CSprite* parent, CmzString name, CmzBitmap *canvas, LONG x, LONG y, BOOL editable)
    : parentWnd(NULL), CSprite(parent, name, canvas, x, y)
{
	this->_commonInit(editable);
}

CGuiControl::CGuiControl(CSprite* parent, CStyledWindow* parentWnd, CmzString name, CmzBitmap *canvas, LONG x, LONG y, BOOL editable)
    : CSprite(parent, name, canvas, x, y), parentWnd(parentWnd)
{
	this->_commonInit(editable);
}

VOID CGuiControl::_commonInit(BOOL editable) {
    this->setRealClassName(L"CGuiControl");
    this->_hscrollbar = NULL;
    this->_vscrollbar = NULL;
    this->_editor = NULL;
    this->_theme = this->getTheme();
    this->_cursor = this->getCursor();
    this->_lMouseButtonPressed = FALSE;
    this->_editable = FALSE;
    this->_isEditor = FALSE;
    CStyledWindow* wnd = this->getParentWindow();
    wnd->registerSprite(this);
    if (editable) {
        this->addEditor();
        if (this->_editor) {
            this->_editable = TRUE;
        }
    }
}

CGuiControl::~CGuiControl() {
    CStyledWindow* wnd = this->getParentWindow();
    wnd->unregisterSprite(this);
}

VOID CGuiControl::cleanupSprites() {
    if (this->_sprites.size() < 1) {
        return;
    }

	CmzLinkListNode* n = this->_sprites.first();
    CSprite* sprite;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite) {
            delete sprite;
        }
        n = n->next;
    }
    this->_sprites.clear();
    
    return;
}

VOID CGuiControl::onDraw(CmzBitmap *dest) {
	if (!this->canvas) return;
    if (!this->_visible) return;

    if (this->_needUpdate) {
        this->_theme = this->getTheme();
        this->_updateCanvas();
    }
    if (this->_transparentDrawing) {
        //dest->drawTransparentBitmap(this->canvas, this->x, this->y, this->_currentFrame, this->_transparentColor);
    } else {
        dest->drawBitmap(this->canvas, this->x, this->y, this->_currentFrame);
    }
}

BOOL CGuiControl::update() {
    CmzAnimatedBitmap *b = dynamic_cast<CmzAnimatedBitmap*>(this->canvas);
    if (b) {
        if (this->isPlaying()) {
            if (this->_step == 1) {
                this->forward();
            } else if (this->_step == -1) {
                this->backward();
            }
            return TRUE;
        }
    }
    
    CSprite* sprite;
    BOOL result = FALSE;
    BOOL doRedraw = FALSE;
    CmzLinkListNode *n = this->_sprites.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite) {
            doRedraw = sprite->update();
            if (doRedraw) {
                sprite->redraw();
                result = TRUE;
            }
        }
        n = n->next;
    }

    return result;
}

CStyledWindow* CGuiControl::getParentWindow() {
	if (this->parentWnd) {
		return this->parentWnd;
	}

	CSprite *parent = this->parent;
    CGuiControl* gc;
	while (parent) {
        gc = dynamic_cast<CGuiControl*>(parent);
		if (gc && gc->parentWnd) {
			return gc->parentWnd;
		}
		parent = parent->parent;
	}

	return NULL;
}

VOID CGuiControl::redraw() {
    // jeśli sprite, nie ma przypisanego rodzica oraz okna lub jest niewidoczny, to go nie
    // odrysowujemy bo jeszcze nie jest wyświetlany
    if (!this->_visible || (!this->parent && !this->parentWnd)) {
        return;
    }
    this->_needUpdate = TRUE;
    if (this->parent) {
        this->parent->redraw();
    }

    CStyledWindow *wnd = this->getParentWindow();
	if (wnd) {
		wnd->invalidate();
	}
}

VOID WINAPI CGuiControl::getCurrentMousePosition(LONG& x, LONG& y) {
    x = 0;
    y = 0;
    CStyledWindow* wnd = this->getParentWindow();
    if (wnd) {
        wnd->getCurrentMousePosition(x, y);
        x -= this->x;
        y -= this->y;
        CSprite* parent = this->parent;
        while (parent) {
            x -= parent->x;
            y -= parent->y;
            parent = parent->parent;
        }
    }
}

VOID CGuiControl::invalidate() {
    this->_needUpdate = TRUE;
    this->getParentWindow()->invalidate();
}

CTheme* CGuiControl::getTheme() {
    CStyledWindow* wnd = this->getParentWindow();
    if (!wnd) {
        return NULL;
    }
    return wnd->getApplication()->getTheme();
}

CmzCursor* CGuiControl::getCursor() {
    CStyledWindow* wnd = this->getParentWindow();
    if (!wnd) {
        return NULL;
    }
    return wnd->getApplication()->cursor;
}

CKeyboard* CGuiControl::getKbd() {
    CStyledWindow* wnd = this->getParentWindow();
    if (!wnd) {
        return NULL;
    }
    return wnd->getApplication()->kbd;
}

VOID CGuiControl::setState(LONG state, BOOL redraw) {
    if (this->_state != state) {
        this->_state = state;
        if (redraw) {
            this->redraw();
        }
    }
}

VOID CGuiControl::addState(LONG state, BOOL redraw) {
    LONG old_state = this->_state;
    this->_state |= state;
    if (redraw && old_state != this->_state) {
        this->redraw();
    }
}

VOID CGuiControl::removeState(LONG state, BOOL redraw) {
    LONG old_state = this->_state;
    this->_state &= ~state;
    if (redraw && old_state != this->_state) {
        this->redraw();
    }
}

BOOL CGuiControl::setTextForSprite(CmzString value, CmzString spriteName, CmzString wndName, BOOL redraw) {
    CStyledWindow* wnd = NULL;

    if (wndName.length() < 1) {
        wnd = this->getParentWindow();
    }

    // jeśli okno nie jest jeszcze ustawione, to szukamy go w liście okien zarejestrowanych
    // w obiekcie CApplication.
    if (!wnd) {
        CApplication* app = wnd->getApplication();
        app->getWindowByName(wndName);
    }

    // jeśli nie znaleziono okna, to zwracamy FALSE
    if (!wnd) {
        return FALSE;
    }

    CSprite* sprite = wnd->getSpriteByName(spriteName);
    if (!sprite) {
        return FALSE;
    }

    CGuiControl* gc = dynamic_cast<CGuiControl*>(sprite);
    if (gc) {
        gc->setText(value, redraw);
    }
    
    return TRUE;
}

VOID CGuiControl::addScrollbars(BOOL vertical, BOOL horizontal, LONG vmin, LONG vmax, LONG hmin, LONG hmax) {
    LONG size;
    
    if (vertical) {
        size = 0;
        if (!horizontal) {
            size = this->height;
        }
        this->addVerticalScrollbar(vmin, vmax, size);
    }
    
    if (horizontal) {
        size = 0;
        if (!vertical) {
            size = this->width;
        }
        this->addHorizontalScrollbar(hmin, hmax, size);
    }
}

VOID CGuiControl::addVerticalScrollbar(LONG min, LONG max, LONG size) {
    if (::application) {
        CTheme* theme = ::application->getTheme();
        LONG vscr_w = theme->scrollbar_vertical_arrow_up->width();
        LONG hscr_h = theme->scrollbar_horizontal_arrow_right->height();
        CScrollbar* scrbar = new CScrollbarVertical(this, this->name + L"_VSB", this->width - vscr_w, 0, min, max, min, TRUE);
        if (scrbar) {
            if (!size) {
                size = this->height - hscr_h;
            }
            scrbar->setSizeRelativeToParent(this->width, size);
            scrbar->registerPositionNotificationForSprite(this);
            (dynamic_cast<CScrollbarVertical*>(scrbar))->setThumbSize(24);
            this->_vscrollbar = scrbar;
            this->_sprites.append((LONGLONG)scrbar);
        }
    }
}

BOOL CGuiControl::removeVerticalScrollbar() {
    if (this->_vscrollbar) {
        return this->removeChild(this->_vscrollbar);
    }
    return FALSE;
}

VOID CGuiControl::addHorizontalScrollbar(LONG min, LONG max, LONG size) {
    if (::application) {
        CTheme* theme = ::application->getTheme();
        LONG vscr_w = theme->scrollbar_vertical_arrow_up->width();
        LONG hscr_h = theme->scrollbar_horizontal_arrow_right->height();
        CScrollbar* scrbar = new CScrollbarHorizontal(this, this->name + L"_HSB", 0, this->height - hscr_h, min, max, min, TRUE);
        if (scrbar) {
            if (!size) {
                size = this->width - vscr_w;
            }
            scrbar->setSizeRelativeToParent(size, this->height);
            scrbar->registerPositionNotificationForSprite(this);
            (dynamic_cast<CScrollbarHorizontal*>(scrbar))->setThumbSize(24);
            this->_hscrollbar = scrbar;
            this->_sprites.append((LONGLONG)scrbar);
        }
    }
}

BOOL CGuiControl::removeHorizontalScrollbar() {
    if (this->_hscrollbar) {
        return this->removeChild(this->_hscrollbar);
    }
    return FALSE;
}

VOID CGuiControl::addEditor() {
    CTextField* editor = new CTextField(this, this->name + L"_EDITOR", 0, 0, 200);
    if (editor) {
        //editor->setSizeRelativeToParent(this->width - vscr_w, this->height);
        //scrbar->registerPositionNotificationForSprite(this);
        editor->_isEditor = TRUE;
        editor->disableTabkey();
        editor->hide();
        this->_editor = editor;
        this->_sprites.append((LONGLONG)editor);
        this->_editable = TRUE;
    }
}

BOOL CGuiControl::removeEditor() {
    if (this->_editor) {
        return this->removeChild(this->_editor);
    }
    return FALSE;
}

VOID CGuiControl::setEditable(BOOL value) {
    if (value && this->_editor) {
        return;
    }
    if (!value) {
        this->removeEditor();
        this->_editable = FALSE;
    }
    this->addEditor();
}

VOID CGuiControl::resetEditor() {
    if (!this->_editable || !this->_editor) {
        return;
    }
    CTextField* tf = dynamic_cast<CTextField*>(this->_editor);
    if (tf) {
        tf->setText(COMMON_EMPTY_STRING);
    }
}

VOID CGuiControl::setEditorValue(CmzStdObject* value) {
    if (this->_editor) {
        CTextField* tf = dynamic_cast<CTextField*>(this->_editor);
        if (tf) {
            CmzString v = value->getAsString(L"text");
            this->_editor->setText(v);
        }
    }
}

VOID CGuiControl::edit() {
    if (!this->_editable || !this->_editor) {
        return;
    }
    this->resetEditor();
    CmzStdObject value = this->getValue();
    this->setEditorValue(&value);
    this->onEditorShow();
    this->_editor->show();
    this->parentWnd->setActiveControl((CGuiControl*)this->_editor);
    this->_editor->redraw(); // forces this control to redraw also
}

VOID CGuiControl::endEdit(BOOL acceptChanges) {
    if (!this->_editable || !this->_editor || !this->parentWnd || (this->parentWnd->getActiveControl() != this->_editor)) {
        return;
    }
    
    if (acceptChanges) {
        CTextField* tf = dynamic_cast<CTextField*>(this->_editor);
        if (tf) {
            CmzStdObject value;
            value.set(L"text", tf->getText());
            this->onApplyEditorChanges(&value);
        }
    }
    
    this->_editor->hide();
    this->parentWnd->setActiveControl(this);
    this->redraw();
}

BOOL CGuiControl::removeChild(CSprite *child) {
    if (this->_sprites.delFirst((LONGLONG)child)) {
        if (this->_vscrollbar == child) {
            this->_vscrollbar = NULL;
        } else if (this->_hscrollbar == child) {
            this->_hscrollbar = NULL;
        } else if (this->_editor == child) {
            this->_editor = NULL;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CGuiControl::removeChild(CmzString childName) {
    CSprite* sprite = this->getChild(childName);
    if (sprite) {
        if (this->removeChild(sprite)) {
            delete sprite;
            return TRUE;
        }
    }
    return FALSE;
}

VOID CGuiControl::drawChildSprites(CmzBitmap* canvas) {
    // ================================================================
    // drawing child-sprites
    // ================================================================
	CmzLinkListNode *n = this->_sprites.first();
    CSprite *sprite;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->isVisible()) {
			//canvas->drawSprite(sprite);
            sprite->onDraw(canvas);
        }
        n = n->next;
    }
    // ================================================================
    // end of drawing child-sprites
    // ================================================================
}

VOID CGuiControl::invalidateChildSpritesCanvases() {
    // ================================================================
    // updating canvases of child-sprites
    // ================================================================
	CmzLinkListNode *n = this->_sprites.first();
    CSprite *sprite;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite && sprite->isVisible()) {
			sprite->invalidate();
        }
        n = n->next;
    }
    // ================================================================
    // end of updating canvases of child-sprites
    // ================================================================
}


VOID CGuiControl::onClick() {
    CStyledWindow *wnd = this->getParentWindow();
    if (wnd) {
        // tutaj kliknięto dany przycisk, więc wysyłamy odpowiednie zdarzenie
        CmzStdObject *eventInfo = new CmzStdObject();
        eventInfo->set(L"name", this->name);
        eventInfo->set(L"form_name", wnd->getName());
        CApplication* app = wnd->getApplication();
        eventInfo->set(L"event_type", static_cast<LONGLONG>(WM_GUICONTROL_CLICKED));
        app->pushEvent(eventInfo);
        PostMessage(wnd->getHwnd(), WM_GUICONTROL_CLICKED, (WPARAM)app, NULL);
    }
}

BOOL CGuiControl::doChildOnMouseEvent(LONG mouseEventType, LONG x, LONG y) {
    if (this->_sprites.size() < 1) {
        return FALSE;
    }
    
    // We traverse child sprites list
    CmzLinkListNode *n = this->_sprites.first();
    CSprite* sprite;
    CGuiControl* gc;
    mzstd::CMenu* menu;
    BOOL result;
    
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        menu = dynamic_cast<mzstd::CMenu*>(sprite);
        if (menu) {
            // jeśli mamy menu, to zdarzenie wysyłamy tylko do menu najwyższego poziomu
            // bo ono dba o rozpropagowanie zdarzenia w obrębie wszystkich otwartych
            // podmenu
            if (menu->getParentMenu() == NULL) {
                result = FALSE;
                switch (mouseEventType) {
                    case MOUSE_EVENT_ONMOUSEDOWN:
                        result = menu->onMouseDown(x, y);
                        break;
                    case MOUSE_EVENT_ONMOUSEMOVE:
                        result = menu->onMouseMove(x, y);
                        break;
                    case MOUSE_EVENT_ONMOUSEUP:
                        result = menu->onMouseUp(x, y);
                        break;
                }
                
                if (result) {
                    return TRUE;
                }
            }
        } else {
            if (gc) {
                result = FALSE;
                switch (mouseEventType) {
                    case MOUSE_EVENT_ONMOUSEDOWN:
                        result = gc->onMouseDown(x, y);
                        break;
                    case MOUSE_EVENT_ONMOUSEMOVE:
                        result = gc->onMouseMove(x, y);
                        break;
                    case MOUSE_EVENT_ONMOUSEUP:
                        result = gc->onMouseUp(x, y);
                        break;
                }

                if (result) {
                    return TRUE;
                }
            }
        }
        n = n->next;
    }
    
    return FALSE;
}

BOOL CGuiControl::doChildOnMouseMove(LONG x, LONG y) {
    return this->doChildOnMouseEvent(MOUSE_EVENT_ONMOUSEMOVE, x, y);
}

BOOL CGuiControl::doChildOnMouseDown(LONG x, LONG y) {
    return this->doChildOnMouseEvent(MOUSE_EVENT_ONMOUSEDOWN, x, y);
}

BOOL CGuiControl::doChildOnMouseUp(LONG x, LONG y) {
    return this->doChildOnMouseEvent(MOUSE_EVENT_ONMOUSEUP, x, y);
}

CSprite* CGuiControl::getChild(CmzString name) {
    if (this->_sprites.size() < 1) {
        return NULL;
    }

	CmzLinkListNode *n = this->_sprites.first();
    CSprite *sprite;
    CGuiControl* subsprite;
    
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->name == name) {
            return sprite;
        }
        subsprite = dynamic_cast<CGuiControl*>(sprite);
        if (subsprite) {
            sprite = subsprite->getChild(name);
            if (sprite) {
                return sprite;
            }
        }
        n = n->next;
    }

    return NULL;
}


BOOL CGuiControl::onEnterPressed() {
    if (this->_isEditor && this->parent) {
        CGuiControl* gc = dynamic_cast<CGuiControl*>(this->parent);
        if (gc) {
            gc->onChildNotification(this, VK_RETURN);
            return TRUE;
        }
    }
    
    if (this->_editable) {
        if (this->_editor && !this->_editor->isVisible()) {
            this->edit();
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CGuiControl::onEscPressed() {
    if (this->_isEditor && this->parent) {
        CGuiControl* gc = dynamic_cast<CGuiControl*>(this->parent);
        if (gc) {
            gc->onChildNotification(this, VK_ESCAPE);
            return TRUE;
        }
    }
    
    return FALSE;
}

LONG CGuiControl::onChildNotification(CGuiControl* child, LONG data) {
    if (child == this->_editor && this->_editable && this->_editor && this->_editor->isVisible()) {
        if (data == VK_RETURN || data == VK_ESCAPE) {
            this->endEdit(data == VK_RETURN);
        }
    }
    
    return 1;
}

VOID CGuiControl::enable() {
    __super::enable();
    this->setState(CONTROL_STATE_NORMAL);
}

VOID CGuiControl::disable() {
    __super::disable();
    this->setState(CONTROL_STATE_DISABLED);
}

VOID CGuiControl::draw(LONG x, LONG y,
                    CmzBitmap* frame_top_left, CmzBitmap* frame_top, CmzBitmap* frame_top_right,
                    CmzBitmap* frame_bottom_left, CmzBitmap* frame_bottom, CmzBitmap* frame_bottom_right,
                    CmzBitmap* frame_left, CmzBitmap* frame_right, CmzBitmap* bg) {
    LONG curX = x;
    LONG curY = y;
    LONG innerX = x;
    LONG innerY = y;
    LONG innerW, innerH;
    
    // Top left corner
    if (frame_top_left) {
        this->canvas->drawBitmap(frame_top_left, curX, curY);
        curX += frame_top_left->width();
        innerX = curX;
    }

    // Top frame
    if (frame_top) {
        innerW = this->canvas->width() - curX;
        if (frame_top_right) {
            innerW -= frame_top_right->width();
        }        
        this->canvas->stretchBitmap(frame_top, curX, curY, innerW, frame_top->height());
        curX += innerW;
    }
    
    // Top right corner
    if (frame_top_right) {
        this->canvas->drawBitmap(frame_top_right, curX, curY);
        curX = x;
        curY += frame_top_right->height();
        innerY = curY;
    }

    // Left frame
    if (frame_left) {
        innerH = this->canvas->height() - curY;
        if (frame_bottom_left) {
            innerH -= frame_bottom_left->height();
        }
        this->canvas->stretchBitmap(frame_left, curX, curY, frame_left->width(), innerH);
    }

    // Right frame
    if (frame_right) {
        curX = innerX + innerW;
        curY = innerY;
        this->canvas->stretchBitmap(frame_right, curX, curY, frame_right->width(), innerH);
    }
    
    // Bottom left corner
    if (frame_bottom_left) {
        curX = x;
        curY = innerY + innerH;
        this->canvas->drawBitmap(frame_bottom_left, curX, curY);
    }
    
    // Bottom frame
    if (frame_bottom) {
        curX = innerX;
        curY = innerY + innerH;
        this->canvas->stretchBitmap(frame_bottom, curX, curY, innerW, frame_bottom->height());
    }
    
    // Bottom right corner
    if (frame_bottom_right) {
        curX = innerX + innerW;
        curY = innerY + innerH;
        this->canvas->drawBitmap(frame_bottom_right, curX, curY);
    }
    
    // Background
    if (bg) {
        this->canvas->stretchBitmap(bg, innerX, innerY, innerW, innerH);
    }
}
