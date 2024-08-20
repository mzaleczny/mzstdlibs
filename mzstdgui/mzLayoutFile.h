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

#ifndef __MZSTDGUI_LAYOUT_FILE_H__
#define __MZSTDGUI_LAYOUT_FILE_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzStyledWindow.h"
#include "mzGuiContainer.h"
#include "mzResource.h"
#include "mzStringList.h"

class CLayoutFile;
typedef VOID (WINAPI *ADDGUICONTROLFUNC)(CLayoutFile* loader, mzstd::CmzStdObject* item, CGuiContainer* parent);


class DECLSPEC_MZSTDGUI CLayoutFile : public mzstd::CmzObject {
public:
    WINAPI CLayoutFile();
    virtual WINAPI ~CLayoutFile();
    
    BOOL WINAPI loadLayoutFromFile(mzstd::CmzString fname, INT layoutContentType, CStyledWindow* wnd);    
    BOOL WINAPI loadLayout(mzstd::CmzString layoutContent, INT layoutContentType, CStyledWindow* wnd);    
    VOID WINAPI addControl(CGuiControl* gc, CGuiContainer* parent);
    VOID WINAPI getCommonProperties(mzstd::CmzStdObject* item, BOOL* transparentDrawing);
    VOID WINAPI getDimensionProperties(mzstd::CmzStdObject* item, LONG* x, LONG* y, LONG* width, LONG* height);
    VOID WINAPI getRelativePositionProperties(mzstd::CmzStdObject* item, mzstd::CmzString* relativeControl, mzstd::CmzString* relativePosition);    
    VOID WINAPI doRelativePositioning(CGuiControl* gc, WCHAR* positionRelativeTo, WCHAR* position);

protected:
    // auxiliary array of controls to add to window
    mzstd::CmzArrayList* _controls;
    CGuiControl* WINAPI _getControl(WCHAR* name);
    VOID WINAPI _commonInit();
    virtual VOID WINAPI _cleanup();

private:
    // Variable valid only during call to drawJsonFile method.
    // It is set to canvas pointer and NULL-ed just before drawJsonFile method return.
    CStyledWindow* _wnd;
    mzstd::CmzString _filename;

    mzstd::CmzStringList* WINAPI _getAsStringList(mzstd::CmzStdObject* item, WCHAR* name);
    VOID WINAPI _processData(mzstd::CmzStdObject* data);
    VOID WINAPI _processToplevelItems(mzstd::CmzArrayList* items);
    VOID WINAPI _processItems(mzstd::CmzPointerList* items, CGuiContainer* parent);
    VOID WINAPI _processWindowItem(mzstd::CmzStdObject* item);
    VOID WINAPI _processMenuItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processLabelItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processTextfieldItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processGridItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processButtonItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processCheckboxItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processListboxItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
    VOID WINAPI _processGuiControlFromOtherDllItem(mzstd::CmzStdObject* item, CGuiContainer* parent);
};
typedef CLayoutFile* PLayoutFile;

#endif
