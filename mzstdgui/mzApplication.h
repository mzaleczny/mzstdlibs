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

#ifndef __MZSTDGUI_APPLICATION_H__
#define __MZSTDGUI_APPLICATION_H__

#include "mzstdguiCommon.h"
#include "mzstdguiGlobals.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzLinkList.h"
#include "mzObjectList.h"
#include "mzStdObject.h"
#include "mzTree.h"
#include "mzCursor.h"

class CWindowClass;
class CWindow;
class CScreen;
class CTheme;
class CResource;
class CmzImageList;
class CStyledWindow;
class CKeyboard;
class CClipboard;
class CSplashWindow;

#define APPLICATION_ERROR_RESOURCE_DIR_NOT_EXISTS       1

class DECLSPEC_MZSTDGUI CApplication : public mzstd::CmzObject {
public:
    CScreen* screen;
    CResource* resources;
    CKeyboard* kbd;
    mzstd::CmzCursor* cursor;
    CClipboard* clipboard;
    mzstd::CmzString homeDir;

	WINAPI CApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, LONG nCmdShow, mzstd::CmzString vendor, mzstd::CmzString appname, mzstd::CmzString globalResourcesDir = COMMON_EMPTY_STRING) throw(...);
	virtual WINAPI ~CApplication();

    
    inline HINSTANCE WINAPI getInstance() { return this->_hInst; }
    inline HINSTANCE WINAPI getPrevInstance() { return this->_hPrevInst; }
    inline mzstd::CmzString WINAPI getCmdLine() { return this->_cmdLine; }
    inline LONG WINAPI getCmdShow() { return this->_cmdShow; }

    BOOL WINAPI registerClass(CWindowClass *wndClass);
    BOOL WINAPI unregisterClass(mzstd::CmzString className);
    BOOL WINAPI registerWindow(CWindow *wnd);
    BOOL WINAPI unregisterWindow(CWindow *wnd);
    CWindow* WINAPI getRegisteredWindow(HANDLE hWnd);
    CWindow* WINAPI getMainWindow();
    CWindow* WINAPI getActiveWindow();
    CWindow* WINAPI getWindowByName(mzstd::CmzString name);
    inline VOID WINAPI pushClickedMenuItemData(mzstd::CmzStdObject* menuItemInfo) {
        this->_events.append((LONGLONG)menuItemInfo);
    }
    inline VOID WINAPI pushEvent(mzstd::CmzStdObject* eventInfo) {
        this->_events.append((LONGLONG)eventInfo);
    }
    // zdejmuje z listy ostatnie wystąpienie zdarzenia kliknięcia menu i zwraca to zdarzenie
    mzstd::CmzStdObject* WINAPI popClickedMenuItemData();
    // zdejmuje z listy ostatnie wystąpienie zdarzenia podanego typu i zwraca to zdarzenie
    mzstd::CmzStdObject* WINAPI popEvent(LONG type);

    CWindowClass* WINAPI getRegisteredClass(mzstd::CmzString className);
    static VOID CALLBACK TimeProc(ULONG uID, ULONG uMsg, UINT_PTR dwUser, DWORD dw1, DWORD dw2);
    VOID WINAPI exit(LONG returnCode);

    // Metoda wywoływana w metodzie run po określeniu okna głównego i przed ewentualnym
    // utworzeniem podstawowych podkomponentów (np. menu głównego)
    virtual VOID WINAPI onInit() {};
    // Metoda wywoływana w metodzie run po utworzeniu wszystkich podstawowych
    // podkomponentów (np. menu głównego)
    virtual VOID WINAPI onCreationComplete() {};
    virtual CmzImageList* WINAPI getImageList(mzstd::CmzString name) {return NULL; };

    LONG WINAPI run();
    LONG WINAPI run(CWindow *wnd);
    // return TRUE if all messages in the queue was processed. FALSE otherwise.
    BOOL processMessages();
    inline VOID WINAPI setCurTickCount(ULONG tc) { this->_curTickCount = tc; }
    inline ULONG WINAPI getCurTickCount() { return this->_curTickCount; }
    inline ULONG WINAPI getLastTickCount() { return this->_lastTickCount; }

    mzstd::CmzString WINAPI getHomeDir();
    mzstd::CmzString WINAPI getResourcesDir();
    mzstd::CmzString WINAPI getApplicationResourcesDir();
    mzstd::CmzString WINAPI getGlobalResourcesDir();
    mzstd::CmzString WINAPI getStringsDir();
    mzstd::CmzString WINAPI getThemeDir();
    CTheme* WINAPI getTheme();
    
    VOID WINAPI setSplashBitmap(mzstd::CmzString splashBitmapFilename) { this->_splashBitmapFilename = splashBitmapFilename; }

    VOID WINAPI setSplashWindow(mzstd::CmzString splashBitmapFilename, LONG displayTime = 250) {
        this->_splashBitmapFilename = splashBitmapFilename;
        this->_splashWindowDisplayTime = displayTime;
    }
    
    inline mzstd::CmzString WINAPI getVendor() { return this->_vendor; }
    inline mzstd::CmzString WINAPI getAppname() { return this->_appname; }
    
    VOID WINAPI loadConfigFromRegistry(mzstd::CmzString registryPath = L"");
    VOID WINAPI loadConfigFromFile(mzstd::CmzString filePath = L"");

    CWindow* WINAPI getCurModalWindow();
    LONG WINAPI getModalWindowsCount();
    BOOL WINAPI doModal(CWindow* wnd);
    BOOL WINAPI unModal(CWindow* wnd);

protected:
	HINSTANCE _hInst;
	HINSTANCE _hPrevInst;
	mzstd::CmzString _cmdLine;
	LONG _cmdShow;
    mzstd::CmzLinkList _registeredClasses;
    mzstd::CmzLinkList _registeredWindows;
    mzstd::CmzLinkList _events;
    BOOL _exitMessageLoop;
    ULONG _curTickCount;
    ULONG _messageLoopInterval;
    ULONG _lastTickCount;
    MSG _msg;
    mzstd::CmzString _splashBitmapFilename;
    LONG _splashWindowDisplayTime;
    CTheme* _theme;
    mzstd::CmzTree* _config;
    mzstd::CmzString _vendor;
    mzstd::CmzString _appname;
    // Value returned by run method. Necessary to pass to PostQuitMessage in CApplication destructor.
    LONG _returnCode;
    mzstd::CmzObjectList* _modalStack;
    
    // returns FALSE if splash window should be closed and main window dsplayed
    BOOL WINAPI _processSplashWindow();

    BOOL WINAPI _isProperlyInitialised(CWindow* wnd);
    VOID WINAPI _blurAllWindowsExcept(CWindow* wnd);
    VOID WINAPI _unblurAllWindows();

private:
};

typedef CApplication* PApplication;

#endif
