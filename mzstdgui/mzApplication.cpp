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
#include "mzstdguiGlobals.h"
#include "mzResource.h"
#include "mzStyledWindow.h"
#include "mzSplashWindow.h"
#include "mzThread.h"
#include "mzMessageBox.h"
#include "mzFileUtils.h"
#include "mzMenu.h"
#include "mzPanel.h"
#include "mzStringList.h"
#include "mzLocale.h"
#include "mzStdObject.h"
#include "mzMessages.h"
#include "mzTheme.h"
#include "mzScreen.h"
#include "mzKeyboard.h"
#include "mzClipboard.h"
#include "mzFileUtils.h"
#include "mzTree.h"
#include "mzLayoutFile.h"
#include "mzLogger.h"
#include "mzObjectList.h"

using namespace mzstd;


CApplication::CApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, LONG nCmdShow, CmzString vendor, CmzString appname, CmzString globalResourcesDir)
	: _hInst(hInstance), _cmdLine(lpCmdLine), _hPrevInst(hPrevInstance), _cmdShow(nCmdShow)
{
    this->setRealClassName(L"CApplication");
    this->_vendor = vendor;
    this->_appname = appname;
    this->homeDir = GetCurrentDirectory();
    this->screen = new CScreen();
    this->kbd = new CKeyboard();
    this->resources = new CResource(this, globalResourcesDir);
    this->_theme = new CTheme(this->_hInst, this->resources, L"oldwaita-blue-submarine-1.0");
    if (this->_theme && this->_theme->getThemeDir() == COMMON_EMPTY_STRING) {
        delete this->_theme;
        this->_theme = NULL;
    }
    this->_exitMessageLoop = FALSE;
    this->cursor = new CmzCursor();
    this->clipboard = new CClipboard();
    this->_messageLoopInterval = 15;
    this->_lastTickCount = 0;
    this->_config = new CmzTree();
    ::application = this;
    this->_returnCode = 0;
    this->_modalStack = new CmzObjectList();
}

CApplication::~CApplication() {
    // teraz niszczymy wszystkie zarejestrowane okna
    CmzLinkListNode *n = this->_registeredWindows.first();
    CWindow *wnd;
    while (n != NULL) {
        wnd = (CWindow*)n->data;
        n->data = NULL;
        // NULL-ujemy wskaźnik na aplikację okna, żeby w destruktorze
        // nie próbowało się odrejestrować
        wnd->_app = NULL;
        delete wnd;
        n = n->next;
    }
    this->_registeredWindows.clear();

    CWindowClass *wc;
    n = this->_registeredClasses.first();
    while (n != NULL) {
        wc = (CWindowClass*)n->data;
        wc->_unregisterClass();
        delete wc;
        n->data = 0;
        n = n->next;
    }
    this->_registeredClasses.clear();

    // teraz niszczymy wszystkie dane o zdarzeniach
    n = this->_events.first();
    CmzStdObject* menuItemData;
    while (n != NULL) {
        menuItemData = (CmzStdObject*)n->data;
        n->data = NULL;
        delete menuItemData;
        n = n->next;
    }
    this->_events.clear();

    if (this->screen) {
        delete this->screen;
        this->screen = NULL;
    }

    if (this->_theme) {
        delete this->_theme;
        this->_theme = NULL;
    }

    if (this->resources) {
        delete this->resources;
        this->resources = NULL;
    }

    if (this->kbd) {
        delete this->kbd;
        this->kbd = NULL;
    }

    if (this->cursor) {
        delete this->cursor;
        this->cursor = NULL;
    }

    if (this->clipboard) {
        delete this->clipboard;
        this->clipboard = NULL;
    }

    if (this->_config) {
        delete this->_config;
        this->_config = NULL;
    }

    if (this->_modalStack) {
        this->_modalStack->clear();
        delete this->_modalStack;
        this->_modalStack = NULL;
    }
    
    // Sending WM_QUIT at this moment is already meaningless. Important is internal system cleanup done
    // by the function below.
    PostQuitMessage(this->_returnCode);
}

BOOL CApplication::registerClass(CWindowClass *wndClass) {
    if (!wndClass) {
        return FALSE;
    }

    // jeśli klasa o takiej nazwie jest już zarejestrowana, to nic nie robimy
    if (this->getRegisteredClass(wndClass->className)) {
        return FALSE;
    }

    if (wndClass->_registerClass()) {
        this->_registeredClasses.append((LONGLONG)wndClass);
        return TRUE;
    }

    return FALSE;
}

BOOL CApplication::unregisterClass(CmzString className) {
    CWindowClass *wc = this->getRegisteredClass(className);
    if (!wc) return FALSE;
    if (wc->_unregisterClass()) {
        LONGLONG data = (LONGLONG)wc;
        delete wc;
        return this->_registeredClasses.del(data) > 0;
    }
    return FALSE;
}

CWindowClass *CApplication::getRegisteredClass(CmzString className) {
    CWindowClass *wc;
    CmzLinkListNode *n = this->_registeredClasses.first();
    while (n != NULL) {
        wc = (CWindowClass*)n->data;
        if (wc->className == className) {
            return wc;
        }
        n = n->next;
    }

    return NULL;
}

BOOL CApplication::registerWindow(CWindow *wnd) {
    if (this->_registeredWindows.contains((LONGLONG)wnd)) {
        return FALSE;
    } else {
        this->_registeredWindows.append((LONGLONG)wnd);
    }
    return TRUE;
}

BOOL CApplication::unregisterWindow(CWindow *wnd) {
    if (wnd->isMainWindow) {
        this->_exitMessageLoop = TRUE;
    }
    return this->_registeredWindows.del((LONGLONG)wnd) > 0;
}

CWindow* CApplication::getRegisteredWindow(HANDLE hWnd) {
    CWindow *wnd;
    CmzLinkListNode *n = this->_registeredWindows.first();
    while (n != NULL) {
        wnd = (CWindow*)n->data;
        if (wnd && wnd->_hWnd == hWnd) {
            return wnd;
        }
        n = n->next;
    }

    return NULL;
}

CWindow* CApplication::getMainWindow() {
    CWindow *wnd, *firstWindow = NULL;
    CmzLinkListNode *n = this->_registeredWindows.first();
    while (n != NULL) {
        wnd = (CWindow*)n->data;
        if (!firstWindow) {
            firstWindow = wnd;
        }
        if (wnd->isMainWindow) {
            return wnd;
        }

        n = n->next;
    }

    return firstWindow;
}

CWindow* CApplication::getActiveWindow() {
    // pobieramy uchwyt aktywnego w systemie okna
    HWND activeWindow = GetActiveWindow();

    // teraz sprawdzamy, czy jest to jedno z zarejestrowanych okien
    CWindow *wnd;
    CmzLinkListNode *n = this->_registeredWindows.first();
    while (n != NULL) {
        wnd = (CWindow*)n->data;
        if (wnd->_hWnd == activeWindow) {
            return wnd;
        }
        n = n->next;
    }

    // nie jest to żadne z zarejestrowanych okien, więc zwracamy NULL
    return NULL;
}

CWindow* CApplication::getWindowByName(CmzString name) {
    // szukamy okna o podanej nazwie w liście zarejestrowanych okien
    CWindow *wnd;
    CmzLinkListNode *n = this->_registeredWindows.first();
    while (n != NULL) {
        wnd = (CWindow*)n->data;
        if (wnd->getName() == name) {
            return wnd;
        }
        n = n->next;
    }

    // nie jest to żadne z zarejestrowanych okien, więc zwracamy NULL
    return NULL;
}

BOOL CApplication::_isProperlyInitialised(CWindow* wnd) {
    if (wnd) {
        CWindowClass* wclass = wnd->getWindowClass();
        if (wclass) {
            WNDCLASSEX* wcex = wclass->getClass();
            if (wcex && wcex->cbSize == sizeof(WNDCLASSEX)) {
                if ((this->getRegisteredClass(wclass->className) != NULL) && (this->getRegisteredWindow(wnd->getHwnd()) != NULL)) {
                    CTheme* theme = this->getTheme();
                    if (theme && theme->isWindowSkinLoaded() /* && theme->isMenuSkinLoaded() */ && theme->isPanelSkinLoaded()) {
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

LONG CApplication::run() {
    CWindow* wnd = new CStyledWindow(this, NULL, L"mzstd based application");
    if (wnd) {
        this->_returnCode = this->run(wnd);
        delete wnd;
        return this->_returnCode;
    }

    ErrorBox(L"Application initialisation failed. Terminating.");
    return -1;
}

LONG CApplication::run(CWindow *wnd) {
    CSplashWindow* splashWnd = NULL;

    if (!this->_isProperlyInitialised(wnd)) {
        ErrorBox(L"Application initialisation failed. Terminating.");
        this->_returnCode = -1;
        return this->_returnCode;
    }

    wnd->isMainWindow = TRUE;
    this->onInit();

    if (this->_splashBitmapFilename != COMMON_EMPTY_STRING && FileExists(this->_splashBitmapFilename)) {
        splashWnd = new CSplashWindow(this, wnd, L"Splash Window", this->_splashBitmapFilename);
        splashWnd->isMainWindow = FALSE;
    }

    this->onCreationComplete();
    if (!splashWnd && wnd) {
        wnd->show();
    }
    if (splashWnd) {
        splashWnd->show();
    }

	// Main message loop:
    this->_msg.wParam = 0;
    this->_msg.message = 0;

    while (!this->_exitMessageLoop && this->_registeredWindows.size() > 0) {
        while (!this->_exitMessageLoop && this->_msg.message != WM_QUIT) {
            if (this->processMessages()) {
                Wait(this->_messageLoopInterval);
                if (splashWnd) {
                    if (!this->_processSplashWindow()) {
                        delete splashWnd;
                        splashWnd = NULL;
                        wnd->show();
                    }
                }
            }
        }
    }

    this->_returnCode = (LONG)this->_msg.wParam;
    return this->_returnCode;
}

BOOL CApplication::_processSplashWindow() {
    this->_splashWindowDisplayTime--;
    if (this->_splashWindowDisplayTime <= 0) {
        return FALSE;
    }

    return TRUE;
}

BOOL CApplication::processMessages() {
    ULONG curTickCount = 0;
    SYSTEMTIME st;
    CWindow* activeWnd;
    CStyledWindow* activeStyledWnd;

    //curTickCount = GetTickCount();
    GetSystemTime(&st);
    curTickCount = st.wMilliseconds + st.wSecond * 1000 +
        st.wMinute * 60 * 1000 + st.wHour * 60 * 60 * 1000 +
        st.wDay * 24 * 60 * 60 * 1000;
    if (curTickCount < this->_lastTickCount) {
        this->_lastTickCount = 0;
        curTickCount = 1;
    }
    if (curTickCount - this->_lastTickCount > this->_messageLoopInterval) {
        this->_lastTickCount = curTickCount;
        this->_curTickCount = curTickCount;
        CApplication::TimeProc(0, 0, (UINT_PTR)this, 0, 0);
    }
    
    activeWnd = this->getRegisteredWindow(GetActiveWindow());
    if (activeWnd) {
        activeStyledWnd = dynamic_cast<CStyledWindow*>(activeWnd);
        if (activeStyledWnd) {
            this->kbd->updateState();
            activeStyledWnd->dispatchKeyboardEvents();
        }
    }

    LONG maxMessagesCount = 10;
    while (PeekMessage(&this->_msg, NULL, 0, 0, PM_REMOVE)) {
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&this->_msg);
			DispatchMessage(&this->_msg);
		//}
        maxMessagesCount--;
        if (maxMessagesCount < 1) {
            return FALSE;
        }
	}
    return TRUE;
}

VOID CALLBACK CApplication::TimeProc(ULONG uID, ULONG uMsg, UINT_PTR dwUser, DWORD dw1, DWORD dw2) {
    static BOOL processing = FALSE;
    if (processing) {
        return;
    }

    processing = TRUE;
    CApplication *app = (CApplication*)dwUser;
    if (app) {
        HWND hwnd = GetActiveWindow();

        // dla każdego zarejestrowanego w aplikacji okna wywołujemy
        // funkcję onTimer
        CWindow *wnd;
        CmzLinkListNode *n = app->_registeredWindows.first();
        while (n != NULL) {
            wnd = (CWindow*)n->data;
            wnd->onTimer(uID, app->getCurTickCount());
            n = n->next;
        }
    }
    processing = FALSE;
}

CmzStdObject* CApplication::popClickedMenuItemData() {
    return this->popEvent(WM_MENUITEM_CLICKED);
}

CmzStdObject* CApplication::popEvent(LONG type) {
    if (this->_events.size() < 1) {
        return NULL;
    }
    CmzStdObject* eventInfo = NULL;
    LONG foundPos = -1;
    CmzStdObject* ev;
    CmzLinkListNode *n = this->_events.first();

    ULONGLONG pos = 0;
    while (n != NULL) {
        ev = (CmzStdObject*)n->data;
        if (ev->get(L"event_type")->iValue == type) {
            eventInfo = ev;
            foundPos = (LONG)pos;
        }
        n = n->next;
        pos++;
    }

    if (foundPos >= 0 && eventInfo) {
        this->_events.del((ULONGLONG)foundPos);
    }

    return eventInfo;
}

CmzString CApplication::getHomeDir() {
    return this->homeDir;
}

CmzString CApplication::getResourcesDir() {
    return this->getApplicationResourcesDir();
}

CmzString CApplication::getApplicationResourcesDir() {
    if (!this->resources) {
        return COMMON_EMPTY_STRING;
    }
    return this->resources->applicationResourcesDir;
}

CmzString CApplication::getGlobalResourcesDir() {
    if (!this->resources) {
        return COMMON_EMPTY_STRING;
    }
    return this->resources->globalResourcesDir;
}

CmzString CApplication::getStringsDir() {
    if (!this->resources) {
        return COMMON_EMPTY_STRING;
    }
    return this->getApplicationResourcesDir() + L"\\strings";
}

CmzString CApplication::getThemeDir() {
    CTheme* theme = this->getTheme();
    if (theme) {
        return theme->getThemeDir();
    }
    return COMMON_EMPTY_STRING;
}

CTheme* CApplication::getTheme() {
    return this->_theme;
}

VOID CApplication::exit(LONG returnCode) {
    delete this;
    ExitProcess(returnCode);
}

VOID CApplication::loadConfigFromRegistry(CmzString registryPath) {

}

VOID CApplication::loadConfigFromFile(CmzString filePath) {
}

CWindow* CApplication::getCurModalWindow() {
    if (!this->_modalStack || this->_modalStack->size() < 1) {
        return NULL;
    }
    
    CWindow* wnd = (CWindow*)this->_modalStack->getLast();
    return wnd;
}

LONG CApplication::getModalWindowsCount() {
    if (!this->_modalStack || this->_modalStack->size() < 1) {
        return 0;
    }
    
    return static_cast<LONG>(this->_modalStack->size());
}

VOID CApplication::_blurAllWindowsExcept(CWindow* wnd) {
    CmzLinkListNode* n = this->_registeredWindows.first();
    CWindow* w;
    
    while (n != NULL) {
        w = (CWindow*)n->data;
        if (w && (w != wnd)) {
            if (!w->isBlur()) {
                w->setBlur(TRUE);
            }
        }
        n = n->next;
    }
}

VOID CApplication::_unblurAllWindows() {
    CmzLinkListNode* n = this->_registeredWindows.first();
    CWindow* w;
    while (n != NULL) {
        w = (CWindow*)n->data;
        if (w) {
            w->setBlur(FALSE);
        }
        n = n->next;
    }
}

BOOL CApplication::doModal(CWindow* wnd) {
    if (!this->_modalStack || !wnd) {
        return FALSE;
    }
    
    CWindow* w = this->getCurModalWindow();
    if (w != wnd) {
        this->_modalStack->append(wnd);
        this->_blurAllWindowsExcept(wnd);
        return TRUE;
    }
    
    return FALSE;
}

BOOL CApplication::unModal(CWindow* wnd) {
    if (!this->_modalStack || !wnd) {
        return FALSE;
    }
    
    // allow poping from modal stack only in case it is last showed modal window
    CWindow* w = (CWindow*)this->_modalStack->getLast();
    if (w != wnd) {
        return FALSE;
    }
    
    w->setIsModal(FALSE);
    this->_modalStack->pop();
    w = (CWindow*)this->_modalStack->getLast();
    if (!w) {
        this->_unblurAllWindows();
    } else {
        this->_blurAllWindowsExcept(w);
    }
    
    return TRUE;
}
