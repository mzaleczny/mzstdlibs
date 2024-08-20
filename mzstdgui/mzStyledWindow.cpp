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
#include "mzApplication.h"
#include "mzStyledWindow.h"
#include "mzstdguiGlobals.h"
#include "mzstd.h"
#include "mzstdwin32.h"
#include "mzstdgraphics.h"
#include "mzstdgui.h"
#include "mzFileUtils.h"
#include "mzTheme.h"
#include "mzTextField.h"
#include "mzPanel.h"
#include "mzScreen.h"
#include "mzMessages.h"
#include "mzMessageBox.h"
#include "mzLayoutFile.h"
#include "mzKeyboard.h"
using namespace mzstd;

VOID CStyledWindow::_commonInit(CApplication *app, CmzString name, CmzString layoutFilename, CWindowClass *wc, HWND hWnd) {
    this->setRealClassName(L"CStyledWindow");
    
    this->_app = app;
    
    CTheme* theme = this->_app->getTheme();
    BOOL isWindowSkinLoaded = theme ? theme->isWindowSkinLoaded() : FALSE;
    
    this->_name = name;
    this->_canvas = NULL;
    this->_spriteThatCapturedMouse = NULL;
    this->_activeControl = NULL;
    if (isWindowSkinLoaded) {
        this->_caret = new CCaret(this, 0, 0);
    } else {
        this->_caret = NULL;
    }
    this->_needUpdate = TRUE;
    this->_allowResizing = TRUE;
    this->_layoutFilename = layoutFilename;
    if (this->_layoutFilename != COMMON_EMPTY_STRING && !FileExists(this->_layoutFilename)) {
        this->_layoutFilename = this->_app->getStringsDir() + L"\\" + this->_layoutFilename;
        if (!FileExists(this->_layoutFilename)) {
            this->_layoutFilename = COMMON_EMPTY_STRING;
        }
    }

    if (!this->_app) {
        return;
    }
    
    if (!hWnd && isWindowSkinLoaded) {
        if (wc == NULL) {
            wc = new CWindowClass(app);
            wc->_wcex.style = 0;
        }
        this->_initWindowClass(wc);
        if (this->_wc) {
            this->_create(SW_HIDE, this->_exStyle, this->_style);
        }
    }
    
    if (theme && isWindowSkinLoaded) {
        // dodajemy do listy sprite'ów przycisk zamknięcia okna
        CGuiControl *sprite;
        sprite = new CGuiControl(NULL, this, L"wnd_close_button", theme->wnd_close_button, 52, 0);
        this->_sprites.append((LONGLONG)sprite);
        
        sprite = new CGuiControl(NULL, this, L"wnd_minimize_button", theme->wnd_minimize_button, 0, 0);
        if (!this->_allowResizing) sprite->hide();
        this->_sprites.append((LONGLONG)sprite);
        
        sprite = new CGuiControl(NULL, this, L"wnd_maximize_button", theme->wnd_maximize_button, 26, 0);
        if (!this->_allowResizing) sprite->hide();
        this->_sprites.append((LONGLONG)sprite);
        
        sprite = new CGuiControl(NULL, this, L"wnd_restore_button", theme->wnd_restore_button, 26, 0);
        sprite->hide();
        this->_sprites.append((LONGLONG)sprite);

        this->_setWindowParameters();
        this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
        if (!this->_canvas) {
            this->calculateAndApplyRegion();
        }
        this->invalidate();
        if (theme && theme->isWindowSkinLoaded()) {
            this->setWindowParametersBasedOnAppTheme();
        }

        // dodajemy główny pojemnik, do którego powinny być dodawane kolejne
        // kontrolki
        LONG panelWidth = this->rcClient.right - this->rcClient.left;
        LONG panelHeight = this->rcClient.bottom - this->rcClient.top;
        LONG panelX = 0, panelY = 0;
        if (theme && theme->isWindowSkinLoaded()) {
            panelWidth -= theme->wnd_frame_left->width() + theme->wnd_frame_right->width();
            panelHeight -= theme->wnd_caption_middle->height() + theme->wnd_frame_bottom->height();
            panelX = theme->wnd_frame_left->width();
            panelY = theme->wnd_caption_left->height();
        }
        
        CPanel *panel = new CPanel(NULL, this, "mainPanel", panelWidth, panelHeight, panelX, panelY);
        this->addChild(panel);
    }
    
    this->_delayAfterKeyDown = 300;
    this->_delayAfterKeyPress = 10;
    this->_currentDelay = this->_delayAfterKeyDown;
    this->_lastKeyEventTime = 0;
}

CStyledWindow::CStyledWindow(CApplication *app, CWindow* parentWindow, CmzString name, CWindowClass *wc, HWND hWnd)
    : CWindow(parentWindow)
{
    this->_commonInit(app, name, COMMON_EMPTY_STRING, wc, hWnd);
}

CStyledWindow::CStyledWindow(CApplication *app, CWindow* parentWindow, CmzString name, CmzString layoutFilename, CWindowClass *wc, HWND hWnd)
    : CWindow(parentWindow)
{
    this->_commonInit(app, name, layoutFilename, wc, hWnd);
}

CStyledWindow::CStyledWindow(CApplication *app, CWindow* parentWindow, CmzString name, CmzString layout, INT layoutContentType, BOOL returnEditedValues)
    : CWindow(parentWindow)
{
    this->_commonInit(app, name, COMMON_EMPTY_STRING, NULL, NULL);
    if (layoutContentType == RESOURCE_TYPE_JSON) {
        CPanel* mp = this->getPanel(L"mainPanel");
        if (mp) {
            this->loadGuiLayout(layout, layoutContentType,FALSE);
        }
    }
    if (returnEditedValues) {
        this->_returnEditedValues = returnEditedValues;
    }
}

CStyledWindow::~CStyledWindow() {
    if (this->_sprites && this->_allSprites) {
        this->clearGuiLayout();
        CmzLinkListNode *n = this->_sprites.first();
        while (n != NULL) {
            CSprite *sprite = (CSprite*)n->data;
            delete sprite;
            n = n->next;
        }
        this->_sprites.clear();
        this->_allSprites.clear();
    }
    
    if (this->_hrgn) {
        DeleteObject(this->_hrgn);
    }
    if (this->_canvas) {
        DeleteObject(this->_canvas);
    }

    if (this->_caret) {
        delete this->_caret;
        this->_caret = NULL;
    }
}

VOID CStyledWindow::addChild(CSprite *sprite) {
    if (sprite) {
        CGuiControl* gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            gc->parentWnd = this;
        }
        this->_sprites.append((LONGLONG)sprite);
    }
}

VOID WINAPI CStyledWindow::removeChild(CSprite *sprite) {
    this->_sprites.delFirst((LONGLONG)sprite);
}

VOID WINAPI CStyledWindow::removeChild(CmzString name) {
    CSprite* sprite = this->getSprite(name);
    if (sprite) {
        this->removeChild(sprite);
    }
}

VOID CStyledWindow::setWindowParametersBasedOnAppTheme() {
    this->_borderTop = 0;
    this->_borderBottom = 0;
    this->_borderLeft = 0;
    this->_borderRight = 0;
    this->_captionHeight = 0;

    if (this->_app) {
        CTheme* theme = this->_app->getTheme();
        if (theme && theme->isWindowSkinLoaded()) {
            this->_borderTop = 2;
            this->_borderBottom = theme->wnd_frame_bottom->height();
            this->_borderLeft = theme->wnd_frame_left->width();
            this->_borderRight = theme->wnd_frame_right->width();
            this->_captionHeight = theme->wnd_caption_middle->height();
        }
    }
}

VOID CStyledWindow::onDraw(HDC hdc) {
    if (!this->_canvas) {
        return;
    }

    RECT updateRect;
    GetUpdateRect(this->_hWnd, &updateRect, FALSE);
    RECT bmpUpdateRect = this->_canvas->getUpdateRect();
    RECT rc;
    UnionRect(&rc, &updateRect, &bmpUpdateRect);

    if (this->_needUpdate) {
        this->_updateCanvas();
    }

    /*
    if (rc.left != 0 || rc.top != 0 || rc.right != 0 || rc.bottom != 0) {
        this->_canvas->beginPaint(hdc);
        this->_canvas->bitblt(hdc, 0, 0);
        //this->_canvas->bitblt(hdc, rc.left, rc.top, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
        this->_canvas->endPaint();
        this->_canvas->validateBitmap();
    }
    */
    this->_canvas->beginPaint(hdc);
    this->_canvas->bitblt(hdc, 0, 0);
    this->_canvas->endPaint();
}

VOID CStyledWindow::_updateCanvas() {
    if (!this->_app) {
        return;
    }
    
    CTheme *t = this->_app->getTheme();
    if (!this->_canvas || !this->isVisible() || this->isIconic() || !this->_app || !t || !t->isWindowSkinLoaded()) {
		return;
    }
    this->_needUpdate = FALSE;

    ULONG x = 0;
    ULONG y = 0;
    RECT rc;
    HDC hdc = GetDC(0);

    this->_canvas->beginPaint(hdc);

    // ================================================================
    // Rysujemy belkę tytułową
    // ================================================================
    CmzBitmap* wnd_caption_left = NULL;
    CmzBitmap* wnd_caption_middle = NULL;
    CmzBitmap* wnd_caption_right = NULL;
    CmzBitmap* wnd_frame_left = NULL;
    CmzBitmap* wnd_frame_right = NULL;
    CmzBitmap* wnd_frame_bottom = NULL;
    CmzBitmap* wnd_frame_bottom_left = NULL;
    CmzBitmap* wnd_frame_bottom_right = NULL;
    CmzBitmap* wnd_inner_bg = t->wnd_inner_bg;
    
    if (this->isActive() || this->_isBlur) {
        wnd_caption_left = t->wnd_caption_left;
        wnd_caption_right = t->wnd_caption_right;
        wnd_caption_middle = t->wnd_caption_middle;
        wnd_frame_left = t->wnd_frame_left;
        wnd_frame_right = t->wnd_frame_right;
        wnd_frame_bottom = t->wnd_frame_bottom;
        wnd_frame_bottom_left = t->wnd_frame_bottom_left;
        wnd_frame_bottom_right = t->wnd_frame_bottom_right;
    } else {
        wnd_caption_left = t->wnd_caption_inactive_left;
        wnd_caption_right = t->wnd_caption_inactive_right;
        wnd_caption_middle = t->wnd_caption_inactive_middle;
        wnd_frame_left = t->wnd_frame_inactive_left;
        wnd_frame_right = t->wnd_frame_inactive_right;
        wnd_frame_bottom = t->wnd_frame_inactive_bottom;
        wnd_frame_bottom_left = t->wnd_frame_inactive_bottom_left;
        wnd_frame_bottom_right = t->wnd_frame_inactive_bottom_right;
    }

    // Najpierw tlo
    this->_canvas->stretchBitmap(wnd_inner_bg, 0, 0, this->clientWidth(), this->clientHeight());

    this->_canvas->drawBitmap(wnd_caption_left, x, y);
    x += wnd_caption_left->width();
    ULONG caption_middle_width = this->clientWidth() - wnd_caption_left->width() - wnd_caption_right->width();
    this->_canvas->stretchBitmap(wnd_caption_middle,
        x, y,
        caption_middle_width,
        this->_captionHeight
    );
    x += caption_middle_width;
    this->_canvas->drawBitmap(wnd_caption_right, x, y);
    // ================================================================
    // Koniec rysowania belki tytułowej
    // ================================================================

    // ================================================================
    // Rysujemy obramowanie okna
    // ================================================================
    // Ramka lewa
    x = 0;
    y = this->_captionHeight;
    this->_canvas->stretchBitmap(wnd_frame_left,
        x, y,
        this->_borderLeft,
        this->regionHeight() - y - wnd_frame_bottom_left->height()
    );

    // Lewy dolny róg
    y = this->regionHeight() - wnd_frame_bottom_left->height();
    this->_canvas->drawBitmap(wnd_frame_bottom_left, x, y);

    // Ramka dolna
    x += wnd_frame_bottom_left->width();
    y = this->regionHeight() - this->_borderBottom;
    this->_canvas->stretchBitmap(wnd_frame_bottom,
        x, y,
        this->regionWidth() - x - wnd_frame_bottom_right->width(),
        this->_borderLeft
    );

    // Prawy dolny róg
    x = this->regionWidth() - wnd_frame_bottom_right->width();
    y = this->regionHeight() - wnd_frame_bottom_right->height();
    this->_canvas->drawBitmap(wnd_frame_bottom_right, x, y);

    // Ramka prawa
    x = this->regionWidth() - this->_borderRight;
    y = this->_captionHeight;
    this->_canvas->stretchBitmap(wnd_frame_right,
        x, y,
        this->_borderRight,
        this->regionHeight() - y - wnd_frame_bottom_right->height()
    );
    // ================================================================
    // Koniec rysowania obramowania okna
    // ================================================================


    // ================================================================
    // Nagłówek okienka
    // ================================================================
    CmzString caption = this->getText();
    rc.left = t->wnd_frame_left->width() + 4;
    rc.top = 4;
    rc.right = rc.left + caption_middle_width;
    rc.bottom = rc.top + this->_captionHeight - 4;
    if (t->wnd_leftmenu_button) {
        rc.left += t->wnd_leftmenu_button->width() + 4;
    }
    this->_canvas->setBkMode(TRANSPARENT);
    this->_canvas->drawText(t->windowCaptionFont, caption, rc);
    // ================================================================
    // Koniec rysowania nagłówka okienka
    // ================================================================



    // ================================================================
    // Rysowanie skolejkowanych sprite'ów
    // ================================================================
    CmzLinkListNode* n = this->_sprites.first();
    CSprite *sprite;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->isVisible()) {
            //this->_canvas->drawSprite(sprite);
            sprite->onDraw(this->_canvas);
        }
        n = n->next;
    }
    // ================================================================
    // Koniec rysowania sprite'ów
    // ================================================================

    if (this->_isBlur) {
        this->_canvas->setOpacity(0.8f);
    }
    
    if (this->_caret->isVisible()) {
        this->drawCaret(this->_canvas->getDC());
    }

    this->_canvas->endPaint();
    ReleaseDC(0, hdc);
}

LRESULT CStyledWindow::onNChitTest(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LRESULT retval = 0;

    LONG xPos = LOWORD(lParam); 
    LONG yPos = HIWORD(lParam);

    LONG wndX = xPos - this->windowOrigin.x;
    LONG wndY = yPos - this->windowOrigin.y;

    LONG bitmapX = wndX - this->_rcRegion.left;
    LONG bitmapY = wndY - this->_rcRegion.top;

    //this->app->theme->wnd_close_button->play(FALSE);
    // jeśli jesteśmy na przycisku close button
    CSprite *sprite = this->getSprite("wnd_close_button");
    if (sprite) {
        sprite->play(FALSE);
        if (sprite->pointIn(bitmapX, bitmapY)) {
            sprite->play();
            retval = HTCLOSE;
        }
    }

    if (this->_allowResizing) {
        sprite = this->getSprite("wnd_maximize_button");
        if (sprite && sprite->isVisible()) {
            sprite->play(FALSE);
            if (sprite->pointIn(bitmapX, bitmapY)) {
                sprite->play();
                retval = HTGROWBOX;
            }
        }

        sprite = this->getSprite("wnd_restore_button");
        if (sprite && sprite->isVisible()) {
            sprite->play(FALSE);
            if (sprite->pointIn(bitmapX, bitmapY)) {
                sprite->play();
                retval = HTGROWBOX;
            }
        }

        sprite = this->getSprite("wnd_minimize_button");
        if (sprite) {
            sprite->play(FALSE);
            if (sprite->pointIn(bitmapX, bitmapY)) {
                sprite->play();
                retval = HTMINBUTTON;
            }
        }
    } // if (this->_allowResizing)

    if (retval != 0) {
        return retval;
    }

    // jeśli jesteśmy w nagłówku
    if (
        wndX >= this->_rcRegion.left + this->_borderLeft && wndX <= this->_rcRegion.right - this->_borderRight &&
        wndY >= this->_rcRegion.top + this->_borderTop && wndY <= this->_rcRegion.top + this->_captionHeight
       ) {
        return HTCAPTION;
    }

    if (this->_allowResizing) {
        LONG cornerSize = 5;
        // jeśli jesteśmy w lewym górnym rogu
        if (
            wndX >= this->_rcRegion.left && wndX <= this->_rcRegion.left + cornerSize &&
            wndY >= this->_rcRegion.top && wndY <= this->_rcRegion.top + cornerSize
           ) {
            return HTTOPLEFT;
        }
        // jeśli jesteśmy w prawym górnym rogu
        if (
            wndX >= this->_rcRegion.right - cornerSize && wndX <= this->_rcRegion.right &&
            wndY >= this->_rcRegion.top && wndY <= this->_rcRegion.top + cornerSize
           ) {
            return HTTOPRIGHT;
        }
        // jeśli jesteśmy w lewym dolnym rogu
        if (
            wndX >= this->_rcRegion.left && wndX <= this->_rcRegion.left + cornerSize &&
            wndY >= this->_rcRegion.bottom - cornerSize && wndY <= this->_rcRegion.bottom
           ) {
            return HTBOTTOMLEFT;
        }
        // jeśli jesteśmy w prawym dolnym rogu
        if (
            wndX >= this->_rcRegion.right - cornerSize && wndX <= this->_rcRegion.right &&
            wndY >= this->_rcRegion.bottom - cornerSize && wndY <= this->_rcRegion.bottom
           ) {
            return HTBOTTOMRIGHT;
        }

        // jeśli jesteśmy na górnej ramce
        if (
            wndY >= this->_rcRegion.top && wndY <= this->_rcRegion.top + this->_borderTop
           ) {
            return HTTOP;
        }
        // jeśli jesteśmy na lewej ramce
        if (
            wndX >= this->_rcRegion.left && wndX <= this->_rcRegion.left + this->_borderLeft
           ) {
            return HTLEFT;
        }
        // jeśli jesteśmy na prawej ramce
        if (
            wndX >= this->_rcRegion.right - this->_borderRight && wndX <= this->_rcRegion.right
           ) {
            return HTRIGHT;
        }
        // jeśli jesteśmy na dolnej ramce
        if (
            wndY >= this->_rcRegion.bottom - this->_borderBottom && wndY <= this->_rcRegion.bottom
           ) {
            return HTBOTTOM;
        }
    } // if (this->_allowResizing)

    return CWindow::onNChitTest(hWnd, message, wParam, lParam);
}

VOID CStyledWindow::calculateAndApplyRegion() {
    RECT rc;
    rc.left = this->clientOrigin.x - this->windowOrigin.x;
    rc.top = this->clientOrigin.y - this->windowOrigin.y;
    rc.right = rc.left + this->clientWidth();
    rc.bottom = rc.top + this->clientHeight();

    this->_rcRegion = rc;
    if (this->_canvas) {
        delete this->_canvas;
        this->_canvas = NULL;
    }
    this->_canvas = new CmzBitmap(this->regionWidth(), this->regionHeight(), RGB(255, 255, 255));
    this->_canvas->turnOnTrackingChanges();
    this->_hrgn = CreateRectRgn(this->_rcRegion.left, this->_rcRegion.top, this->_rcRegion.right, this->_rcRegion.bottom);
	SetWindowRgn(this->_hWnd, this->_hrgn, TRUE);
    rc = this->rcWindow;
    CSprite *btnClose = this->getSprite("wnd_close_button");
    if (btnClose) {
        btnClose->setPosition(this->regionWidth() - btnClose->width - 15, 0);
    }
    CSprite *btnMaximize = this->getSprite("wnd_maximize_button");
    if (btnMaximize) {
        btnMaximize->setPosition(this->regionWidth() - btnClose->width - btnMaximize->width - 15, 0);
    }
    CSprite *btnRestore = this->getSprite("wnd_restore_button");
    if (btnRestore) {
        btnRestore->setPosition(this->regionWidth() - btnClose->width - btnMaximize->width - 15, 0);
    }
    CSprite *btnMinimize = this->getSprite("wnd_minimize_button");
    if (btnMinimize) {
        LONG delta = 0;
        if (!btnRestore->isVisible() && !btnMaximize->isVisible()) {
            delta = 15;
        }
        btnMinimize->setPosition(this->regionWidth() - btnClose->width - btnMaximize->width - btnMinimize->width - 15 - delta, 0);
    }

    // skalujemy wszystkie wymagające tego komponenty
    this->_adjustChildrenSize();
    this->invalidate();
}

VOID CStyledWindow::_adjustChildrenSize() {
    CmzLinkListNode *n = this->_sprites.first();
    while (n != NULL) {
        CSprite *sprite = (CSprite*)n->data;
        if (sprite->name == "mainPanel") {
            LONG panelWidth = this->rcClient.right - this->rcClient.left;
            LONG panelHeight = this->rcClient.bottom - this->rcClient.top;
            LONG panelX = 0, panelY = 0;
            CTheme* theme = this->_app->getTheme();
            if (theme && theme->isWindowSkinLoaded()) {
                panelWidth -= theme->wnd_frame_left->width() + theme->wnd_frame_right->width();
                panelHeight -= theme->wnd_caption_middle->height() + theme->wnd_frame_bottom->height();
                panelX = theme->wnd_frame_left->width();
                panelY = theme->wnd_caption_left->height();
            }
            sprite->setSize(panelWidth, panelHeight);
            break;
        }
        n = n->next;
    }
}

CSprite *CStyledWindow::getSprite(CmzString name) {
    return this->getSpriteByName(name);
}

CSprite* CStyledWindow::getSpriteByName(CmzString name) {
    // szukamy sprite'a o podanej nazwie w liście wszystkich sprite'ów na tym oknie
    CSprite* sprite;
    CmzLinkListNode *n = this->_allSprites.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->name == name) {
            return sprite;
        }
        n = n->next;
    }

    // if we have added Panel 'mainPanel' control, then we search sprite in its children
    CPanel* mp = this->getPanel(L"mainPanel");
    if (mp) {
        sprite = mp->getChild(name);
        if (sprite) {
            return sprite;
        }
    }
    
    // nie jest to żaden z występujących tu sprite'ów, więc zwracamy NULL
    return NULL;
}

CmzString CStyledWindow::getSpriteValue(CmzString spriteName) {
    CSprite* sprite = this->getSpriteByName(spriteName);
    if (sprite) {
        return sprite->getText();
    }
    
    return COMMON_EMPTY_STRING;
}

VOID CStyledWindow::setSpriteValue(CmzString spriteName, CmzString value) {
    CSprite* sprite = this->getSpriteByName(spriteName);
    if (sprite) {
        return sprite->setText(value);
    }
}

CSprite* CStyledWindow::getChild(CmzString name) {
    return this->getSpriteByName(name);
}

CPanel* CStyledWindow::getPanel(CmzString name) {
    CmzLinkListNode *n = this->_sprites.first();
    CSprite *sprite;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->name == name) {
            CPanel* panel = dynamic_cast<CPanel*>(sprite);
            if (panel) {
                return panel;
            }
        }
        n = n->next;
    }
    return NULL;
}

CPanel* CStyledWindow::getContainer() {
    return this->getPanel(L"mainPanel");
}

// Poniższa zmienna jest na potrzeby pozycjonowania okienka w pionie.
// Jeślibyśmy nie zapamiętywali współrzędnej y położenia okienka, to
// po przesunięciu okna powyżej górnej krawędzi ekranu spadałoby ono
// do piątego piksela.
LONG lastY;
LRESULT CStyledWindow::onMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    CWindow::onMove(hWnd, message, wParam, lParam);
    LONG xPos = (LONG)(lParam & 0xffff);
    if (xPos & 0x8000) {
        xPos -= 65535;
    }
    LONG yPos = (LONG)(lParam >> 16);

    if (yPos != 5) {
        lastY = yPos;
    }
    return 0;
}

LRESULT CStyledWindow::onExitSizeMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    RECT rc;
    GetWindowRect(this->_hWnd, &rc);
    SetWindowPos(this->_hWnd, NULL, rc.left, lastY - this->hiddenHeight, 0, 0, SWP_NOSIZE);
	this->invalidate();
    return 0;
}

LRESULT CStyledWindow::onSize(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    CWindow::onSize(hWnd, message, wParam, lParam);
    CSprite *btnRestore = this->getSprite("wnd_restore_button");
    CSprite *btnMaximize = this->getSprite("wnd_maximize_button");
    if (this->_allowResizing) {
        if (IsZoomed(this->_hWnd)) {
            btnRestore->show();
            btnMaximize->hide();
        } else {
            btnRestore->hide();
            btnMaximize->show();
        }
    }
    this->calculateAndApplyRegion();
    return 0;
}

LRESULT CStyledWindow::onSizing(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    static SHORT counter = 0;
    
    counter = (counter+1)%10;
    if (counter == 0) {
        CWindow::onSizing(hWnd, message, wParam, lParam);
        this->calculateAndApplyRegion();
    }
    return TRUE;
}

LRESULT CStyledWindow::onGetMinMaxInfo(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LONG hiddenHeight = this->hiddenHeight;
    LONG YBorder = GetSystemMetrics(SM_CYFRAME);
    LONG XBorder = GetSystemMetrics(SM_CXFRAME);

    MINMAXINFO *p = (MINMAXINFO*)lParam;
    p->ptMinTrackSize.x = 150;
    p->ptMinTrackSize.y = 150;
    p->ptMaxPosition.x = -XBorder;
    p->ptMaxPosition.y = -hiddenHeight;
    p->ptMaxSize.x = this->_app->screen->width + 2 * XBorder;
    p->ptMaxSize.y = this->_app->screen->height + hiddenHeight;
    p->ptMaxTrackSize.x = this->_app->screen->width + 2 * XBorder;
    p->ptMaxTrackSize.y = this->_app->screen->height + hiddenHeight;
    return 0;
}

LRESULT CStyledWindow::onLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LONG x = LOWORD(lParam);
    LONG y = HIWORD(lParam);
    //LONG xx, yy;
    //this->getCurrentMousePosition(xx, yy);
    //this->setText(CmzString("") + x + "x" + y + " :: " + xx + "x" + yy);

    CSprite *sprite;
    CmzLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            if (gc->onMouseDown(x, y)) {
                break;
            }
        }
        n = n->next;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CStyledWindow::onLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LONG x = LOWORD(lParam);
    LONG y = HIWORD(lParam);

    CSprite *sprite;
    CmzLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            if (gc && gc->onMouseUp(x, y)) {
                break;
            }
        }
        n = n->next;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CStyledWindow::onNCLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    // wysyłamy do wszystkich sprite'ów odpowiednie zdarzenie
    CSprite *sprite;
    CmzLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            gc->onMouseDown(-1, -1);
        }
        n = n->next;
    }

    if (wParam == HTCLOSE) {
        return 0;
    }
    if (wParam == HTMINBUTTON) {
        CSprite *sprite = this->getSprite("wnd_minimize_button");
        if (sprite) {
            sprite->play(FALSE);
        }
        ShowWindow(this->_hWnd, SW_MINIMIZE);
        return 0;
    }
    if (wParam == HTGROWBOX) {
        CSprite *btnRestore = this->getSprite("wnd_restore_button");
        CSprite *btnMaximize = this->getSprite("wnd_maximize_button");
        if (btnRestore && btnRestore->isVisible()) {
            if (btnRestore) btnRestore->hide();
            if (btnMaximize) btnMaximize->show();
            ShowWindow(this->_hWnd, SW_RESTORE);
        } else {
            if (btnRestore) btnRestore->show();
            if (btnMaximize) btnMaximize->hide();
            ShowWindow(this->_hWnd, SW_MAXIMIZE);
        }
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CStyledWindow::onNCLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {  
    if (wParam == HTCLOSE) {
        DestroyWindow(this->_hWnd);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


LRESULT CStyledWindow::onNCLButtonDblClk(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    if (!this->_allowResizing) {
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CStyledWindow::onMouseMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    if (!this->_isActive) {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    
    LONG x = LOWORD(lParam);
    LONG y = HIWORD(lParam);

    // ustawiamy domyślny kursor - jeśli taki już jest ustawiony, to funkcja nic nie robi
    this->_app->cursor->setNormalCursor();

    CSprite *sprite;
    CmzLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            if (!gc->onMouseMove(x, y)) {
                if (gc->pointIn(x, y)) {
                    gc->play();
                } else {
                    gc->play(FALSE);
                }
            }
        }
        n = n->next;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WINAPI CStyledWindow::onWmUserEvent(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    CApplication* app = (CApplication*)wParam;
    if (app) {
        CmzStdObject* eventInfo = NULL;
        
        switch (message) {
        case WM_MENUITEM_CLICKED: {
            eventInfo = app->popClickedMenuItemData();
            if (eventInfo) {
                this->onMenuItemClicked(eventInfo);
            }
            break;
        }// case WM_MENUITEM_CLICKED

        case WM_TOOLBARITEM_CLICKED: {
            eventInfo = app->popEvent(WM_TOOLBARITEM_CLICKED);
            if (eventInfo) {
                this->onToolbarItemClicked(eventInfo);
            }
            break;
        }// case WM_TOOLBARITEM_CLICKED

        case WM_BUTTON_CLICKED: {
            eventInfo = app->popEvent(WM_BUTTON_CLICKED);
            if (eventInfo) {
                // if we have dafault button such as: DEFAULT_BTN_OK, DEFAULT_BTN_CANCEL
                CmzString btn = eventInfo->getAsString(L"name");
                if (btn == L"DEFAULT_BTN_OK") {
                    this->_app->unModal(this);
                    this->hide();
                    this->_internalProcessOnGetEditedValues(eventInfo);
                    this->close();
                } else if (btn == L"DEFAULT_BTN_CANCEL") {
                    if (this->_parentWindow) {
                        this->_app->unModal(this);
                        this->hide();
                        this->_parentWindow->onGetEditedValues(NULL);
                        this->close();
                    }
                } else {
                    this->onButtonClicked(eventInfo);
                }
            }
            break;
        }// case WM_BUTTON_CLICKED

        case WM_GUICONTROL_CLICKED: {
            eventInfo = app->popEvent(WM_GUICONTROL_CLICKED);
            if (eventInfo) {
                this->onGuiControlClicked(eventInfo);
            }
            break;
        }// case WM_BUTTON_CLICKED

        }// switch

        if (eventInfo) {
            delete eventInfo;
        }
    }
    return 0;
    //return DefWindowProc(hWnd, message, wParam, lParam);
}

// Funkcja potrzebna do odgrywania animacji sprite'ów (zmiany kolejnych
// ich klatek)
VOID CStyledWindow::onTimer(ULONG uID, ULONG curTickCount) {
    if (!this->_isActive) {
        return;
    }
    
    BOOL redrawWnd = FALSE;

    CSprite *sprite;
    //CGuiContainer* container;
    CmzLinkListNode *n = this->_sprites.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->update()) {
            redrawWnd = TRUE;
        }
        n = n->next;
    }

    this->drawCaret();

    if (redrawWnd) {
		this->invalidate();
    }
}

VOID CStyledWindow::onActivation() {
}

VOID CStyledWindow::onDeactivation() {
    this->onMouseMove(this->_hWnd, 0, 0, 0);
}

VOID CStyledWindow::drawCaret(HDC hdc) {
    // jeśli aktywną kontrolką jest kontrolka dziedzicząca po CTextField, to obsługujemy miganie
    // karetki w tym polu
    if (this->_activeControl) {
        CTextField* tf = dynamic_cast<CTextField*>(this->_activeControl);
        if (tf) {
            BOOL releaseDC = FALSE;
            if (!hdc) {
                hdc = GetDC(this->_hWnd);
                releaseDC = TRUE;
            }
            this->_caret->update(this->_app->getCurTickCount());
            this->_caret->draw(hdc);
            if (releaseDC) {
                ReleaseDC(this->_hWnd, hdc);
            }
        }
    }
}

VOID CStyledWindow::clearGuiLayout() {
    if (this->_sprites) {
        CmzLinkListNode* n = this->_sprites.first();
        CGuiControl* gc;
        CGuiControl* mainPanel = this->getContainer();
        while (n != NULL) {
            CSprite *sprite = (CSprite*)n->data;
            n = n->next;
            gc = dynamic_cast<CGuiControl*>(sprite);
            if (gc) {
                if (gc != mainPanel && wcscmp(gc->getRealClassName(), L"CGuiControl") != 0) {
                    delete gc;
                    this->_sprites.del((LONGLONG)gc);
                }
            }
        }
   }
}

VOID CStyledWindow::loadGuiLayout(CmzString content, INT layoutContentType, BOOL clearActualContent) {
    CLayoutFile* layout = new CLayoutFile();
    if (layout) {
        if (clearActualContent) {
            this->clearGuiLayout();
        }
        layout->loadLayout(content, layoutContentType, this);
        delete layout;
    }
}

VOID CStyledWindow::loadGuiLayoutFromFile(CmzString fname, INT layoutContentType, BOOL clearActualContent) {
    this->_layoutFilename = fname;
    if (this->_layoutFilename != COMMON_EMPTY_STRING && !FileExists(this->_layoutFilename)) {
        this->_layoutFilename = this->_app->getStringsDir() + L"\\" + this->_layoutFilename;
        if (!FileExists(this->_layoutFilename)) {
            this->_layoutFilename = COMMON_EMPTY_STRING;
            return;
        }
    }
    CLayoutFile* layout = new CLayoutFile();
    if (layout) {
        if (clearActualContent) {
            this->clearGuiLayout();
        }
        layout->loadLayoutFromFile(this->_layoutFilename, layoutContentType, this);
        delete layout;
    }
}

VOID CStyledWindow::createGuiLayout() {
    CWindow::createGuiLayout();
    if (this->_layoutFilename != COMMON_EMPTY_STRING && FileExists(this->_layoutFilename)) {
        this->loadGuiLayoutFromFile(this->_layoutFilename, RESOURCE_TYPE_JSON);
    } else {
        // jeśli okno główne ma panel o nazwie mainPanel i w odpowiednim katalogu jest
        // zdefiniowane menu główne, to je dodajemy do okienka
        if (this->isMainWindow) {
            CPanel *mp = (CPanel*)this->getSprite(L"mainPanel");
            if (mp && mp->getChild(L"mainMenu") == NULL) {
                if (FileExists(this->_app->getResourcesDir() + L"\\strings\\mainmenu.json")) {
                    mzstd::CMenu *menu = new mzstd::CMenu(mp, L"mainMenu");
                    menu->loadMenuFromFile(this->_app->getResourcesDir() + L"\\strings\\mainmenu.json");
                    mp->addChild(menu);
                }
            }
        }
    }
}

VOID WINAPI CStyledWindow::getCurrentMousePosition(LONG& x, LONG& y) {
    POINT p;
    if (GetCursorPos(&p)) {
        x = p.x - this->clientOrigin.x;
        y = p.y - this->clientOrigin.y;
    } else {
        x = 0;
        y = 0;
    }
}

VOID CStyledWindow::setActiveControl(CGuiControl* sprite) {
    if (this->_activeControl == sprite) {
        return;
    }

    // domyślnie ukrywamy karetkę
    this->_caret->active = FALSE;

    if (this->_activeControl) {
        this->_activeControl->looseFocus();
    }
    
    this->_activeControl = sprite;
    if (sprite) {
        // jeśli mamy pole tekstowe, to pokazujemy karetkę
        CTextField* tf = dynamic_cast<CTextField*>(sprite);
        if (tf) {
            this->_caret->active = TRUE;
            this->_caret->show();
        }

        sprite->focus();
    }
}

VOID CStyledWindow::getSpritePosition(CSprite* sprite, LONG& x, LONG& y) {
    x = MININT;
    y = MININT;

    if (sprite) {
        CSprite* p;

        x = sprite->x;
        y = sprite->y;

        p = sprite->parent;
        while (p != NULL) {
            x += p->x;
            y += p->y;

            p = p->parent;
        }
    }
}


BOOL CStyledWindow::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& updateCaretPos, BOOL& redraw) {
    updateCaretPos = FALSE;
    redraw = FALSE;
    return FALSE;
}

BOOL CStyledWindow::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
    BOOL vkLAlt, BOOL vkRAlt,
    BOOL vkLShift, BOOL vkRShift,
    BOOL vkLControl, BOOL vkRControl,
    BOOL systemKey,
    ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    BOOL updateCaretPos = FALSE;
    BOOL redraw = FALSE;
    
    return this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt,
        vkLShift, vkRShift, vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, updateCaretPos, redraw);
}

BOOL CStyledWindow::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
    BOOL vkLAlt, BOOL vkRAlt,
    BOOL vkLShift, BOOL vkRShift,
    BOOL vkLControl, BOOL vkRControl,
    BOOL systemKey,
    ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    BOOL updateCaretPos = FALSE;
    BOOL redraw = FALSE;
    
    if (virtualCode == VK_TAB) {
        if (!this->_activeControl || this->_activeControl->isTabkeyEnabled()) {
            if (vkShift) {
                this->moveFocusBackward();
            } else {
                this->moveFocusForward();
            }
            redraw = TRUE;
            return TRUE;
        }
    }
    if (virtualCode == VK_RETURN) {
        if (this->_activeControl && this->_activeControl->isEnterAsTabkey() && this->_activeControl->isTabkeyEnabled()) {
            this->moveFocusForward();
            redraw = TRUE;
            return TRUE;
        }
    }
    
    return this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt,
        vkLShift, vkRShift, vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, updateCaretPos, redraw);
}

BOOL CStyledWindow::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
    BOOL vkLAlt, BOOL vkRAlt,
    BOOL vkLShift, BOOL vkRShift,
    BOOL vkLControl, BOOL vkRControl,
    BOOL systemKey,
    ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    
    return FALSE;
}

VOID CStyledWindow::dispatchKeyboardEvents() {   
    HWND h = GetActiveWindow();
    if (isMessageBoxOpened()) {
        return;
    }
    
    BYTE state;
    CKeyboard* kbd = this->_app->kbd;
    WORD charType;

    BOOL eventProcessed = FALSE;
    BOOL isAlphaNumKey;
    BOOL isCaretMovingKey;
    BOOL isDeleteKey;
    BOOL isSpaceKey;
    BOOL isPunctuation;
    //ULONG lastKeyEvent = 0; // 0 - None, 1 - KeyDown, 1 - KeyUp, 2 - KeyPress
    BOOL keyupEventHappened = FALSE;
    BOOL keydownEventHappened = FALSE;

    // Ustawiamy parametry czasowe do obsługi zdarzeń klawiatury
    ULONG curTime = this->_app->getCurTickCount();
    LONG delta = curTime - this->_lastKeyEventTime;
    BOOL sendMessageToActiveControl;
    
    for (LONG i = 0; i < this->_app->kbd->bufferSize; i++) {
        // if it is mouse button then we skip it
        if (i <= 6) {
            continue;
        }
        // if it is toggle key we skip it too
        if (kbd->isToggleKey(i)) {
            continue;
        }

        charType = getCharType(kbd->toWideChar(i));
        isAlphaNumKey = (charType & C1_ALPHA) || (charType & C1_DIGIT);
        isCaretMovingKey = kbd->isCaretMovingKey(i);
        isDeleteKey = kbd->isDeleteKey(i);
        isSpaceKey = (BOOL)(charType & C1_SPACE);
        isPunctuation = kbd->isPunctKey(i);
        //if (
        //    !isAlphaNumKey && !isCaretMovingKey && !isDeleteKey && !isSpaceKey && !isPunctuation) {
        //    continue;
        //}
        
        state = kbd->kbdKeyState[i];
        switch (state) {
        case CKEYBOARD_KEY_STATE_DOWN:
            eventProcessed = FALSE;
            sendMessageToActiveControl = TRUE;
            if (!eventProcessed && (i == VK_RETURN)) {
                eventProcessed = this->onEnterDown();
                if (!eventProcessed && this->_activeControl) {
                    eventProcessed = this->_activeControl->onEnterDown();
                }
            }
            if (!eventProcessed && (i == VK_ESCAPE)) {
                eventProcessed = this->onEscDown();
                if (!eventProcessed && this->_activeControl) {
                    eventProcessed = this->_activeControl->onEscDown();
                }
            }
            if (!eventProcessed && i == VK_TAB) {
                eventProcessed = this->onTabDown();
                if (!eventProcessed) {
                    if (this->_activeControl && this->_activeControl->isTabkeyEnabled()) {
                        eventProcessed = this->_activeControl->onTabDown();
                    } else {
                        sendMessageToActiveControl = FALSE;
                    }
                }
            }
            eventProcessed = this->onKeyDown(kbd->isKeyPressed(VK_MENU),
                kbd->isKeyPressed(VK_SHIFT), kbd->isKeyPressed(VK_CONTROL),
                kbd->isKeyPressed(VK_LMENU), kbd->isKeyPressed(VK_RMENU),
                kbd->isKeyPressed(VK_LSHIFT), kbd->isKeyPressed(VK_RSHIFT),
                kbd->isKeyPressed(VK_LCONTROL), kbd->isKeyPressed(VK_RCONTROL),
                kbd->isSystemKey(i), i, kbd->toScanCode(i), kbd->toWideChar(i));
            if (!eventProcessed && this->_activeControl && sendMessageToActiveControl) {
                eventProcessed = this->_activeControl->onKeyDown(kbd->isKeyPressed(VK_MENU),
                    kbd->isKeyPressed(VK_SHIFT), kbd->isKeyPressed(VK_CONTROL),
                    kbd->isKeyPressed(VK_LMENU), kbd->isKeyPressed(VK_RMENU),
                    kbd->isKeyPressed(VK_LSHIFT), kbd->isKeyPressed(VK_RSHIFT),
                    kbd->isKeyPressed(VK_LCONTROL), kbd->isKeyPressed(VK_RCONTROL),
                    kbd->isSystemKey(i), i, kbd->toScanCode(i), kbd->toWideChar(i));
            }

            if (eventProcessed && !kbd->isSystemKey(i)) {
                // Parametry czasu, dotyczące opóźnienia w dodawaniu kolejnych znaków w przypadku
                // wciśniętego i trzymanego przycisku uaktualniamy tylko w przypadku obsłużenia tego
                // znaku
                this->_lastKeyEventTime = curTime;
                this->_currentDelay = this->_delayAfterKeyDown;
                keydownEventHappened = TRUE;
            }
            break;
        case CKEYBOARD_KEY_STATE_UP:
            eventProcessed = FALSE;
            sendMessageToActiveControl = TRUE;
            if (!eventProcessed && i == VK_RETURN) {
                eventProcessed = this->onEnterUp();
                if (!eventProcessed && this->_activeControl) {
                    eventProcessed = this->_activeControl->onEnterUp();
                }
            }
            if (!eventProcessed && i == VK_ESCAPE) {
                eventProcessed = this->onEscUp();
                if (!eventProcessed && this->_activeControl) {
                    eventProcessed = this->_activeControl->onEscUp();
                }
            }
            if (!eventProcessed && i == VK_TAB) {
                eventProcessed = this->onTabUp();
                if (!eventProcessed) {
                    if (this->_activeControl && this->_activeControl->isTabkeyEnabled()) {
                        eventProcessed = this->_activeControl->onTabUp();
                    } else {
                        sendMessageToActiveControl = FALSE;
                    }
                }
            }

            eventProcessed = this->onKeyUp(kbd->isKeyPressed(VK_MENU),
                kbd->isKeyPressed(VK_SHIFT), kbd->isKeyPressed(VK_CONTROL),
                kbd->isKeyPressed(VK_LMENU), kbd->isKeyPressed(VK_RMENU),
                kbd->isKeyPressed(VK_LSHIFT), kbd->isKeyPressed(VK_RSHIFT),
                kbd->isKeyPressed(VK_LCONTROL), kbd->isKeyPressed(VK_RCONTROL),
                kbd->isSystemKey(i), i, kbd->toScanCode(i), kbd->toWideChar(i));
            if (!eventProcessed && this->_activeControl && sendMessageToActiveControl) {
                eventProcessed = this->_activeControl->onKeyUp(kbd->isKeyPressed(VK_MENU),
                    kbd->isKeyPressed(VK_SHIFT), kbd->isKeyPressed(VK_CONTROL),
                    kbd->isKeyPressed(VK_LMENU), kbd->isKeyPressed(VK_RMENU),
                    kbd->isKeyPressed(VK_LSHIFT), kbd->isKeyPressed(VK_RSHIFT),
                    kbd->isKeyPressed(VK_LCONTROL), kbd->isKeyPressed(VK_RCONTROL),
                    kbd->isSystemKey(i), i, kbd->toScanCode(i), kbd->toWideChar(i));
            }
            
            if (!kbd->isSystemKey(i)) {
                keyupEventHappened = TRUE;
            }
            break;
        case CKEYBOARD_KEY_STATE_PRESSED:
            sendMessageToActiveControl = TRUE;
            if (!kbd->kbdApplyKeyPressDelay[i] || (delta > this->_currentDelay || delta < 0)) {
                BOOL updateDelay = kbd->kbdApplyKeyPressDelay[i] == TRUE;
                kbd->kbdApplyKeyPressDelay[i] = TRUE;
                eventProcessed = FALSE;
                if (!eventProcessed && i == VK_RETURN) {
                    eventProcessed = this->onEnterPressed();
                    if (!eventProcessed && this->_activeControl) {
                        eventProcessed = this->_activeControl->onEnterPressed();
                    }
                }
                if (!eventProcessed && i == VK_ESCAPE) {
                    eventProcessed = this->onEscPressed();
                    if (!eventProcessed && this->_activeControl) {
                        eventProcessed = this->_activeControl->onEscPressed();
                    }
                }
                if (!eventProcessed && i == VK_TAB) {
                    eventProcessed = this->onTabPressed();
                    if (!eventProcessed) {
                        if (this->_activeControl && this->_activeControl->isTabkeyEnabled()) {
                            eventProcessed = this->_activeControl->onTabPressed();
                        } else {
                            sendMessageToActiveControl = FALSE;
                        }
                    }
                }
                eventProcessed = this->onKeyPressed(kbd->isKeyPressed(VK_MENU),
                    kbd->isKeyPressed(VK_SHIFT), kbd->isKeyPressed(VK_CONTROL),
                    kbd->isKeyPressed(VK_LMENU), kbd->isKeyPressed(VK_RMENU),
                    kbd->isKeyPressed(VK_LSHIFT), kbd->isKeyPressed(VK_RSHIFT),
                    kbd->isKeyPressed(VK_LCONTROL), kbd->isKeyPressed(VK_RCONTROL),
                    kbd->isSystemKey(i), i, kbd->toScanCode(i), kbd->toWideChar(i));
                if (!eventProcessed && this->_activeControl && sendMessageToActiveControl) {
                    eventProcessed = this->_activeControl->onKeyPressed(kbd->isKeyPressed(VK_MENU),
                        kbd->isKeyPressed(VK_SHIFT), kbd->isKeyPressed(VK_CONTROL),
                        kbd->isKeyPressed(VK_LMENU), kbd->isKeyPressed(VK_RMENU),
                        kbd->isKeyPressed(VK_LSHIFT), kbd->isKeyPressed(VK_RSHIFT),
                        kbd->isKeyPressed(VK_LCONTROL), kbd->isKeyPressed(VK_RCONTROL),
                        kbd->isSystemKey(i), i, kbd->toScanCode(i), kbd->toWideChar(i));
                }

                if (eventProcessed && !kbd->isSystemKey(i)) {
                    // Parametry czasu, dotyczące opóźnienia w dodawaniu kolejnych znaków w przypadku
                    // wciśniętego i trzymanego przycisku uaktualniamy tylko w przypadku obsłużenia tego
                    // znaku
                    this->_lastKeyEventTime = curTime;
                    if (updateDelay) {
                        this->_currentDelay = this->_delayAfterKeyPress;
                    }
                }
            }
            break;
        }
    }
    
    if (keyupEventHappened) {
        this->_lastKeyEventTime = curTime;
        this->_currentDelay = static_cast<LONG>(1.5f * this->_delayAfterKeyDown);
        return;
    }
    
    if (keydownEventHappened) {
        this->_lastKeyEventTime = curTime;
        this->_currentDelay = this->_delayAfterKeyDown;
    }
}

VOID CStyledWindow::getAllFocusableSprites(CmzArrayList* lst) {
    if (!lst) {
        return;
    }

    lst->clear();

    // szukamy wszystkich sprite'ów, które mogą przyjąć focus
    CSprite* sprite;
    CmzLinkListNode *n = this->_allSprites.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        if (sprite->canTabStop()) {
            lst->append(n->data);
        }
        n = n->next;
    }
}

VOID CStyledWindow::getAllFocusableGuiControls(CmzArrayList* lst) {
    if (!lst) {
        return;
    }

    lst->clear();

    // szukamy wszystkich sprite'ów, które mogą przyjąć focus
    CSprite* sprite;
    CGuiControl* gc;
    CmzLinkListNode *n = this->_allSprites.first();
    while (n != NULL) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc && gc->canTabStop()) {
            lst->append(n->data);
        }
        n = n->next;
    }
}

VOID CStyledWindow::moveFocusForward() {
    if (this->_allSprites.size() < 1) {
        return;
    }

    CGuiControl* gc;
    CmzArrayList* lst = new CmzArrayList(this->_allSprites.size());
    this->getAllFocusableGuiControls(lst);

    ULONGLONG focusableSpritesCount = lst->size();
    ULONG activeSpriteNumber = MAXINT;

    if (focusableSpritesCount == 0) {
        return;
    }

    if (focusableSpritesCount == 1) {
        gc = (CGuiControl*)lst->getAt(0);
        if (gc) {
            // jeśli na liście jest tylko jeden sprite, który nie jest aktywną kontrolką, to
            // nadajemy mu focus
            if (gc != this->_activeControl) {
                this->setActiveControl(gc);
            }
        }
        return;
    }

    // jeśli sprite'ów na liście jest więcej i aktualnie żadna kontrolka nie jest aktywna, to
    // nadajemy focus pierwszej
    if (!this->_activeControl) {
        
        this->setActiveControl((CGuiControl*)lst->getAt(0));
        return;
    }

    // jeśli mamy teraz aktywną kontrolkę, to szukamy jej pozycji na liście znalezionych sprite'ów
    for (ULONGLONG i = 0; i < focusableSpritesCount; i++) {
        gc = (CGuiControl*)lst->getAt(i);
        if (gc == this->_activeControl) {
            activeSpriteNumber = static_cast<ULONG>(i);
            break;
        }
    }

    // jeśli activeSpriteNumber == MAXINT, to aktywujemy pierwszą dostępną kontrolkę
    if (activeSpriteNumber == MAXINT) {
        this->setActiveControl((CGuiControl*)lst->getAt(0));
        return;
    }

    // teraz aktywujemy pierwszą kontrolkę po bieżącej kontrolce
    ULONG pos = activeSpriteNumber + 1;
    if (pos == lst->size()) {
        pos = 0;
    }

    this->setActiveControl((CGuiControl*)lst->getAt(pos));
}

VOID CStyledWindow::moveFocusBackward() {
    if (this->_allSprites.size() < 1) {
        return;
    }

    //CSprite* sprite;
    CGuiControl* gc;
    CmzArrayList* lst = new CmzArrayList(this->_allSprites.size());
    this->getAllFocusableGuiControls(lst);

    ULONGLONG focusableSpritesCount = lst->size();
    ULONG activeSpriteNumber = MAXINT;

    if (focusableSpritesCount == 0) {
        return;
    }

    if (focusableSpritesCount == 1) {
        gc = (CGuiControl*)lst->getAt(0);
        // jeśli na liście jest tylko jeden sprite, który nie jest aktywną kontrolką, to
        // nadajemy mu focus
        if (gc != this->_activeControl) {
            this->setActiveControl(gc);
        }
        return;
    }

    // jeśli sprite'ów na liście jest więcej i aktualnie żadna kontrolka nie jest aktywna, to
    // nadajemy focus pierwszej
    if (!this->_activeControl) {
        this->setActiveControl((CGuiControl*)lst->getAt(0));
        return;
    }

    // jeśli mamy teraz aktywną kontrolkę, to szukamy jej pozycji na liście znalezionych sprite'ów
    for (ULONGLONG i = 0; i < focusableSpritesCount; i++) {
        gc = (CGuiControl*)lst->getAt(i);
        if (gc == this->_activeControl) {
            activeSpriteNumber = static_cast<ULONG>(i);
            break;
        }
    }

    // jeśli activeSpriteNumber == MAXINT, to aktywujemy pierwszą dostępną kontrolkę
    if (activeSpriteNumber == MAXINT) {
        this->setActiveControl((CGuiControl*)lst->getAt(0));
        return;
    }

    // teraz aktywujemy pierwszą kontrolkę po bieżącej kontrolce
    ULONGLONG pos = activeSpriteNumber;
    if (pos > 0) {
        pos -= 1;
    } else {
        pos = lst->size() - 1;
    }

    this->setActiveControl((CGuiControl*)lst->getAt(pos));
}

VOID CStyledWindow::allowResizing(BOOL value) {
    BOOL oldValue = this->_allowResizing;
    CWindow::allowResizing(value);
    if (value != oldValue) {
        CSprite* sprite;
        if (value) {
            sprite = this->getSprite(L"wnd_minimize_button");
            sprite->show();
            if (IsZoomed(this->_hWnd)) {
                sprite = this->getSprite(L"wnd_restore_button");
                sprite->show();
            } else {
                sprite = this->getSprite(L"wnd_maximize_button");
                sprite->show();
            }
        } else {
            sprite = this->getSprite(L"wnd_minimize_button");
            sprite->hide();
            sprite = this->getSprite(L"wnd_maximize_button");
            sprite->hide();
            sprite = this->getSprite(L"wnd_restore_button");
            sprite->hide();
        }
        
        this->invalidate();
    }    
}

VOID WINAPI CStyledWindow::centerOnScreen() {
    CScreen* screen = this->_app->screen;
    RECT rc;
    
    GetWindowRect(this->_hWnd, &rc);
    LONG ww = rc.right - rc.left;
    LONG wh = rc.bottom - rc.top;
    LONG dw = ww - this->regionWidth();
    LONG dh = wh - this->regionHeight();
    
    SetWindowPos(this->_hWnd, NULL,
        (screen->width - ww) / 2 - dw/2, (screen->height - wh) / 2 - dh/2,
        0, 0, SWP_NOSIZE);
    this->_setWindowParameters();
    this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
	this->invalidate();    
}

VOID WINAPI CStyledWindow::setSize(LONG width, LONG height) {
    RECT rc;    
    GetWindowRect(this->_hWnd, &rc);
    
    width = width + ((rc.right - rc.left) - this->regionWidth());
    height = height + ((rc.bottom - rc.top) - this->regionHeight());
    
    SetWindowPos(this->_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    this->_setWindowParameters();
    this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
}

VOID CStyledWindow::setPosition(LONG x, LONG y) {
    //RECT rc;
    
    LONG dx = this->clientOrigin.x - this->windowOrigin.x;
    LONG dy = this->hiddenHeight;
    
    x -= dx;
    y -= dy;
    
    SetWindowPos(this->_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
    this->_setWindowParameters();
    this->hiddenHeight = this->clientOrigin.y - this->windowOrigin.y;
}

VOID CStyledWindow::_internalProcessOnGetEditedValues(CmzStdObject* eventInfo) {
    if (this->_parentWindow) {
        CPanel *panel = this->getContainer();
        if (panel) {
            CmzLinkList* spriteList = panel->getChildren();
            if (spriteList) {
                CmzLinkListNode* n = spriteList->first();
                CmzMap* map = new CmzMap();
                if (map) {
                    CSprite* sprite;
                    CTextField* tf;
                    while (n) {
                        sprite = (CSprite*)n->data;
                        tf = dynamic_cast<CTextField*>(sprite);
                        if (tf) {
                            CmzString name = tf->name;
                            CmzString value = tf->getText();
                            if (name != COMMON_EMPTY_STRING) {
                                map->set(name, value);
                            }
                        }
                        n = n->next;
                    }
                    // Here we call appropriate handler function of the parent window of this window
                    this->_parentWindow->onGetEditedValues(map);
                }
            }
        }
    }
}
