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
#include "mzstdguiGlobals.h"
#include "mzApplication.h"
#include "mzMap.h"
#include "mzStyledWindow.h"
#include "mzFont.h"
#include "mzTheme.h"
#include "mzMessageBox.h"
#include "mzFileUtils.h"
#include "mzLogger.h"

using namespace mzstd;

CApplication* application = NULL;


VOID WINAPI initMzstdguiLibrary() {
}

VOID WINAPI cleanupMzstdguiLibrary() {
}

CApplication* WINAPI getApplication() {
    return ::application;
}

VOID WINAPI EditValues(WCHAR* windowTitle, CmzStringList* labels, CmzStringList* initialValues) {
    if (!windowTitle || !labels || labels->size() == 0) {
        return;
    }

    CWindow* activeWindow = ::application->getActiveWindow();
    CTheme* theme = ::application->getTheme();
    if (activeWindow && theme) {
        // find width of the LONGer label
        ULONGLONG fieldCount = labels->size();
        CmzFont* font = theme->commonLabelFont;
        HDC hdc = GetDC(0);
        LONG width = 0, height, x, y;
        LONG itemHeight = 25;
        LONG buttonFieldHeight = 30;
        LONG textFieldWidth = 300;
        CmzString s, value;
        CmzString jsonItem;
        CmzString lblName;
        SIZE size;
        for (ULONGLONG i = 0; i < fieldCount; i++) {
            s = labels->getAt(i);
            size = font->measureString(s, hdc);
            if (width < size.cx) {
                width = size.cx;
            }
            height = size.cy;
        }
        size = font->measureString(L": ", hdc);
        width += size.cx;
        ReleaseDC(0, hdc);
        
        width += 10;
        height = itemHeight;
        
        CmzString wndName = L"wndEditWindow";
        CmzString layout = FormatMessage(L"[\n"
                                         L"    {\n"
                                         L"        name: \"%s\",\n"
                                         L"        type: \"window\",\n"
                                         L"        caption: \"%s\",\n"
                                         L"        x: \"center\",\n"
                                         L"        y: \"center\",\n"
                                         L"        width: \"%d\",\n"
                                         L"        height: \"%d\",\n"
                                         L"        active-control: \"FIRST\",\n"
                                         L"        allow-resizing: \"false\",\n"
                                         L"        items: [\n",
                                         wndName.getBuffer(), windowTitle, width + textFieldWidth + 30, fieldCount * height + buttonFieldHeight + 40);

        x = 4;
        y = 4;
        for (LONG i = 0; i < fieldCount; i++) {
            s = labels->getAt(i) + L": ";
            lblName = CmzString(L"lbl") + s;
            jsonItem = FormatMessage(L"{\n"
                                     L"    name: \"%s\",\n"
                                     L"    type: \"label\",\n"
                                     L"    caption: \"%s\",\n"
                                     L"    x: \"%d\",\n"
                                     L"    y: \"%d\",\n"
                                     L"    width: \"%d\",\n"
                                     L"    height: \"%d\",\n"
                                     L"},",
                                     lblName.getBuffer(), s.getBuffer(), x, y, width, height);
            jsonItem.prependEachLine(L"            ");
            layout += jsonItem + L"\n";
            
            if (initialValues && initialValues->size() > i) {
                value = initialValues->getAt(i);
            } else {
                value = COMMON_EMPTY_STRING;
            }
            jsonItem = FormatMessage(L"{\n"
                                     L"    name: \"%s\",\n"
                                     L"    type: \"textfield\",\n"
                                     L"    text: \"%s \",\n"
                                     L"    width: \"%d\",\n"
                                     L"    position-relative-to: \"%s\",\n"
                                     L"    position: \"right\",\n"
                                     L"    enter-as-tab: \"true\",\n"
                                     L"},",
                                     s.getBuffer(), value.getBuffer(), textFieldWidth, lblName.getBuffer());
            jsonItem.prependEachLine(L"            ");
            layout += jsonItem + L"\n";
            y += height;
        }
        
        // adding buttons
        y += 8;
        // OK
        jsonItem = FormatMessage(L"{\n"
                                 L"    name: \"DEFAULT_BTN_OK\",\n"
                                 L"    type: \"button\",\n"
                                 L"    text: \"OK\",\n"
                                 L"    width: \"75\",\n"
                                 L"    x: \"%d\",\n"
                                 L"    y: \"%d\",\n"
                                 L"},",
                                 x, y);
        jsonItem.prependEachLine(L"        ");
        layout += jsonItem + L"\n";
        
        jsonItem = FormatMessage(L"{\n"
                                 L"    name: \"DEFAULT_BTN_CANCEL\",\n"
                                 L"    type: \"button\",\n"
                                 L"    text: \"Anuluj\",\n"
                                 L"    width: \"75\",\n"
                                 L"    position-relative-to: \"DEFAULT_BTN_OK\",\n"
                                 L"    position: \"right\",\n"
                                 L"}");
        jsonItem.prependEachLine(L"        ");
        layout += jsonItem + L"\n";
        
        layout += L"        ]\n        }\n]";
        
        LOG(layout.getBuffer());
        CStyledWindow* wnd = new CStyledWindow(::application, activeWindow, wndName, layout, RESOURCE_TYPE_JSON, TRUE);
        if (wnd) {
            wnd->showModal();
        }
    }

    return;
}
