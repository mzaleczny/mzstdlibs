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
#include "mzLayoutFile.h"
#include "mzstd.h"
#include "mzstdguiGlobals.h"
#include "mzFileUtils.h"
#include "mzJsonParser.h"
#include "mzApplication.h"
#include "mzPanel.h"
#include "mzMenu.h"
#include "mzLabel.h"
#include "mzTextField.h"
#include "mzGrid.h"
#include "mzButton.h"
#include "mzCheckbox.h"
#include "mzListbox.h"
#include "mzMessageBox.h"
#include "mzScreen.h"
using namespace mzstd;

VOID CLayoutFile::_commonInit() {
    this->setRealClassName(L"CLayoutFile");
}

CLayoutFile::CLayoutFile() {
    this->_commonInit();
    this->_controls = new CmzArrayList();
}

CLayoutFile::~CLayoutFile() {
    this->_cleanup();
}

VOID CLayoutFile::_cleanup() {
    if (this->_controls) {
        delete this->_controls;
    }
}

CGuiControl* CLayoutFile::_getControl(WCHAR* name) {
    ULONGLONG size = this->_controls->size();
    CGuiControl* gc;
    for (ULONGLONG i = 0; i < size; i++) {
        gc = (CGuiControl*)this->_controls->getAt(i);
        if (gc && gc->name == name) {
            return gc;
        }
    }

    return NULL;
}

BOOL CLayoutFile::loadLayoutFromFile(CmzString fname, INT layoutContentType, CStyledWindow* wnd) {
    BOOL result = FALSE;

    if (!wnd) {
        return FALSE;
    }
    if (!FileExists(fname)) {
        return FALSE;
    }
    if (!this->_controls) {
        return FALSE;
    }

    if (layoutContentType == RESOURCE_TYPE_JSON) {
        CmzStdObject data;
        if (!data.loadFromFile(fname)) {
            return FALSE;
        }

        this->_controls->clear();
        this->_wnd = wnd;
        this->_filename = fname;
        this->_processData(&data);

        this->_controls->clear();
        return TRUE;
    }
    
    return FALSE;
}

BOOL CLayoutFile::loadLayout(CmzString layoutContent, INT layoutContentType, CStyledWindow* wnd) {
    BOOL result = FALSE;

    if (!wnd) {
        return FALSE;
    }
    if (!this->_controls) {
        return FALSE;
    }

    if (layoutContentType == RESOURCE_TYPE_JSON) {
        CmzJsonParser parser;
        CmzStdObject* data = parser.parse(layoutContent);
        if (!data) {
            if (!parser.isError()) {
                AlertBox(L"No json objects found.");
            } else {
                ErrorBox(parser.getErrorMessage());
            }
            return FALSE;
        }

        this->_controls->clear();
        this->_wnd = wnd;
        this->_filename = COMMON_EMPTY_STRING;
        this->_processData(data);

        this->_controls->clear();
        delete data;
        return TRUE;
    }
    
    return FALSE;
}

VOID CLayoutFile::_processData(CmzStdObject* data) {
    CmzStdObjectProperty* root = data->get(L"root");
    CmzArrayList* items;
    //CmzStdObject* item;

    if (!root) {
        return;
    }

    if (root->type == STDOBJ_PROPERTY_ARRAY) {
        items = (CmzArrayList*)root->aValue;
        if (items) {
            this->_processToplevelItems(items);
        }
    }
}

VOID CLayoutFile::_processToplevelItems(CmzArrayList* items) {
    if (!items) {
        return;
    }

    ULONGLONG size = items->size();
    CmzStdObjectProperty* item;
    CmzStdObject* oValue;
    for (ULONGLONG i = 0; i < size; i++) {
        item = (CmzStdObjectProperty*)items->getAt(i);
        if (item || item->type == STDOBJ_PROPERTY_OBJECT) {
            oValue = item->oValue;
            if (oValue) {
                if (oValue->getAsString(L"type") == L"window" && oValue->getAsString(L"name") == this->_wnd->getName()) {
                    this->_processWindowItem(oValue);
                    // we process only one window
                    break;
                }
            }
        }
    }
}

VOID CLayoutFile::_processItems(CmzPointerList* items, CGuiContainer* parent) {
    if (!items) {
        return;
    }

    ULONGLONG size = items->size();
    CmzStdObjectProperty* item;
    CmzStdObject* oValue;
    for (ULONGLONG i = 0; i < size; i++) {
        item = (CmzStdObjectProperty*)items->getAt(i);
        if (item || item->type == STDOBJ_PROPERTY_OBJECT) {
            oValue = item->oValue;
            if (oValue) {
                CmzString type = oValue->getAsString(L"type");
                if (type == L"textfield") {
                    this->_processTextfieldItem(oValue, parent);
                    continue;
                }
                if (type == L"button") {
                    this->_processButtonItem(oValue, parent);
                    continue;
                }
                if (type == L"label") {
                    this->_processLabelItem(oValue, parent);
                    continue;
                }
                if (type == L"checkbox") {
                    this->_processCheckboxItem(oValue, parent);
                    continue;
                }
                if (type == L"listbox") {
                    this->_processListboxItem(oValue, parent);
                    continue;
                }
                if (type == L"grid") {
                    this->_processGridItem(oValue, parent);
                    continue;
                }
                if (type == L"menu") {
                    this->_processMenuItem(oValue, parent);
                    continue;
                }
                this->_processGuiControlFromOtherDllItem(oValue, parent);
            }
        }
    }
}

VOID CLayoutFile::_processWindowItem(CmzStdObject* item) {
    LONG x, y, width, height;
    CmzStdObjectProperty* property;
    CmzString activeControl;
    CmzString caption;
    BOOL allowResizing = TRUE;

    this->getDimensionProperties(item, &x, &y, &width, &height);
    property = item->get(L"active-control");
    if (property && property->type == STDOBJ_PROPERTY_VALUE) {
        activeControl = property->value;
    }
    allowResizing = item->getAsString(L"allow-resizing") != L"false";
    caption = item->getAsString(L"caption");
    
    property = item->get(L"items");
    if (property && property->type == STDOBJ_PROPERTY_ARRAY) {
        CmzPointerList* items = property->aValue;
        if (items) {
            this->_processItems(items, this->_wnd->getContainer());
            // now activate default active control
            CGuiControl* gc = this->_getControl(activeControl.getBuffer());
            if (gc && gc->isVisible() && gc->isEnabled() && gc->canTabStop()) {
                this->_wnd->setActiveControl(gc);
            }
        }
    }
    
    if (width != MAXINT && height != MAXINT) {
        this->_wnd->setSize(width, height);
    }
    
    if (x != MAXINT && y != MAXINT) {
        this->_wnd->setPosition(x, y);
    }
    
    this->_wnd->allowResizing(allowResizing);
    if (caption != COMMON_EMPTY_STRING) {
        this->_wnd->setText(caption);
    }
}

VOID CLayoutFile::_processMenuItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString menuItemsFile = item->getAsString(L"menu-items-file");
        if (menuItemsFile != COMMON_EMPTY_STRING) {
            if (!FileExists(menuItemsFile)) {
                CmzString resDir = ::application->getResourcesDir();
                if (resDir != COMMON_EMPTY_STRING) {
                    menuItemsFile = resDir + L"\\strings\\" + menuItemsFile;
                    if (FileExists(menuItemsFile)) {
                        mzstd::CMenu *gc = new mzstd::CMenu(parent, name);
                        if (gc) {
                            gc->loadMenuFromFile(menuItemsFile);
                            this->addControl(gc, parent);
                        }
                    }
                }
            }
        }
    }
}

VOID CLayoutFile::_processLabelItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString caption, relativeControl, relativePosition;
        LONG x, y, width, height;
        BOOL autosize = FALSE;
        CmzStdObjectProperty* property;

        this->getDimensionProperties(item, &x, &y, &width, &height);
        this->getRelativePositionProperties(item, &relativeControl, &relativePosition);
        caption = item->getAsString(L"caption");
        
        property = item->get(L"autosize");
        if (property && property->type == STDOBJ_PROPERTY_VALUE && property->value == L"true") {
            autosize = TRUE;
        }

        CLabel* gc = new CLabel(parent, name, x, y, caption, width, height, autosize);
        if (gc) {
            this->doRelativePositioning(gc, relativeControl.getBuffer(), relativePosition.getBuffer());
            this->addControl(gc, parent);
        }
    }
}

VOID CLayoutFile::_processTextfieldItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString text, relativeControl, relativePosition;
        LONG x, y, width, height;
        BOOL enterAsTab = TRUE, canTabStop = TRUE, disableTabkey = FALSE;

        this->getDimensionProperties(item, &x, &y, &width, &height);
        this->getRelativePositionProperties(item, &relativeControl, &relativePosition);
        text = item->getAsString(L"text");
        enterAsTab = item->getAsString(L"enter-as-tab") != L"false";
        canTabStop = item->getAsString(L"can-tab-stop") != L"false";
        disableTabkey = item->getAsString(L"disable-tab-key") == L"true";

        CTextField* gc = new CTextField(parent, name, x, y, width);
        if (gc) {
            this->doRelativePositioning(gc, relativeControl.getBuffer(), relativePosition.getBuffer());
            gc->setTabStop(canTabStop);
            if (disableTabkey) {
                gc->disableTabkey();
            }
            gc->doEnterAsTabkey(enterAsTab);
            
            this->addControl(gc, parent);
        }
    }
}

VOID CLayoutFile::_processGridItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString relativeControl, relativePosition;
        LONG x, y, width, height;
        LONG columnCount = 10, rowCount = 10;
        BOOL enterAsTab = TRUE, canTabStop = TRUE, disableTabkey = FALSE;
        BOOL entireRowSelect = FALSE, entireColumnSelect = FALSE;
        BOOL showTopHeader = TRUE, showLeftHeader = TRUE;
        BOOL drawVerticalLines = TRUE, drawHorizontalLines = TRUE;
        LONG leftHeaderWidth = 0;
        
        this->getDimensionProperties(item, &x, &y, &width, &height);
        this->getRelativePositionProperties(item, &relativeControl, &relativePosition);
        columnCount = static_cast<LONG>(item->getAsInt(L"columnCount"));
        rowCount = static_cast<LONG>(item->getAsInt(L"rowCount"));
        canTabStop = item->getAsString(L"can-tab-stop") != L"false";
        disableTabkey = item->getAsString(L"disable-tab-key") == L"true";
        entireRowSelect = item->getAsString(L"entire-row-select") == L"true";
        entireColumnSelect = item->getAsString(L"entire-column-select") == L"true";
        showTopHeader = item->getAsString(L"show-top-header") != L"false";
        showLeftHeader = item->getAsString(L"show-left-header") != L"false";
        drawVerticalLines = item->getAsString(L"draw-vertical-lines") != L"false";
        drawHorizontalLines = item->getAsString(L"draw-horizontal-lines") != L"false";
        leftHeaderWidth = static_cast<LONG>(item->getAsInt(L"left-header-width"));

        if (!columnCount) {
            columnCount = 10;
        }
        if (!rowCount) {
            rowCount = 10;
        }
        
        CGrid* gc = new CGrid(parent, name, columnCount, rowCount, x, y, width, height, TRUE,
                              showLeftHeader, showTopHeader, entireRowSelect, entireColumnSelect,
                              drawVerticalLines, drawHorizontalLines);
        if (gc) {
            CmzStringList* captions;
            
            captions = this->_getAsStringList(item, L"top-header-captions");
            if (captions && captions->size() > 0) {
                gc->setTopHeaderCaptions(captions);
                delete captions;
            }
            
            captions = this->_getAsStringList(item, L"left-header-captions");
            if (captions && captions->size() > 0) {
                gc->setLeftHeaderCaptions(captions);
                delete captions;
            }
            
            if (leftHeaderWidth > 0) {
                gc->setLeftHeaderWidth(leftHeaderWidth);
            }
            this->doRelativePositioning(gc, relativeControl.getBuffer(), relativePosition.getBuffer());            
            this->addControl(gc, parent);
        }
    }
}

VOID CLayoutFile::_processButtonItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString text, relativeControl, relativePosition;
        LONG x, y, width, height;
        BOOL canTabStop, disableTabkey, disabled;
        BOOL transparentDrawing;
        
        this->getCommonProperties(item, &transparentDrawing);
        this->getDimensionProperties(item, &x, &y, &width, &height);
        this->getRelativePositionProperties(item, &relativeControl, &relativePosition);
        text = item->getAsString(L"text");
        canTabStop = item->getAsString(L"can-tab-stop") != L"false";
        disableTabkey = item->getAsString(L"disable-tab-key") == L"true";
        disabled = item->getAsString(L"disabled") == L"true";

        CmzButton* gc = new CmzButton(parent, name, text, x, y, width);
        if (gc) {
            gc->setTransparentDrawing(transparentDrawing);
            this->doRelativePositioning(gc, relativeControl.getBuffer(), relativePosition.getBuffer());            
            if (disabled) {
                gc->disable();
            }
            this->addControl(gc, parent);
        }
    }
}

VOID CLayoutFile::_processCheckboxItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString text, relativeControl, relativePosition, checkedStateText, uncheckedStateText, textPlacementStr;
        LONG x, y, width, height, textSpacing, textPlacement;
        BOOL canTabStop, disableTabkey, disabled, checked;
        BOOL transparentDrawing;
        
        this->getCommonProperties(item, &transparentDrawing);
        this->getDimensionProperties(item, &x, &y, &width, &height);
        this->getRelativePositionProperties(item, &relativeControl, &relativePosition);
        text = item->getAsString(L"text");
        checkedStateText = item->getAsString(L"checked-state-text");
        uncheckedStateText = item->getAsString(L"unchecked-state-text");
        canTabStop = item->getAsString(L"can-tab-stop") != L"false";
        disableTabkey = item->getAsString(L"disable-tab-key") == L"true";
        disabled = item->getAsString(L"disabled") == L"true";
        checked = item->getAsString(L"checked") == L"true";
        textSpacing = static_cast<LONG>(item->getAsInt(L"text-spacing"));
        textPlacementStr = item->getAsString(L"text-placement");

        if (text != COMMON_EMPTY_STRING) {
            checkedStateText = text;
            uncheckedStateText = text;
        }
        if (textPlacementStr == L"left") {
            textPlacement = CHECKBOX_TEXT_PLACEMENT_LEFT;
        } else {
            textPlacement = CHECKBOX_TEXT_PLACEMENT_RIGHT;
        }
        if (textSpacing == 0) {
            textSpacing = CHECKBOX_DEFAULT_SPACING;
        }
        
        CmzCheckbox* gc = new CmzCheckbox(parent, name, checkedStateText, uncheckedStateText, checked, x, y, width, textPlacement, textSpacing);
        if (gc) {
            gc->setTransparentDrawing(transparentDrawing);
            this->doRelativePositioning(gc, relativeControl.getBuffer(), relativePosition.getBuffer());            
            if (disabled) {
                gc->disable();
            }
            this->addControl(gc, parent);
        }
    }
}

VOID CLayoutFile::_processListboxItem(CmzStdObject* item, CGuiContainer* parent) {
    if (!item || !parent) {
        return;
    }

    CmzString name = item->getAsString(L"name");
    if (name != COMMON_EMPTY_STRING) {
        CmzString text, relativeControl, relativePosition;
        LONG x, y, width, height;
        BOOL canTabStop, disableTabkey, disabled;
        BOOL transparentDrawing;
        
        this->getCommonProperties(item, &transparentDrawing);
        this->getDimensionProperties(item, &x, &y, &width, &height);
        this->getRelativePositionProperties(item, &relativeControl, &relativePosition);
        canTabStop = item->getAsString(L"can-tab-stop") != L"false";
        disableTabkey = item->getAsString(L"disable-tab-key") == L"true";
        disabled = item->getAsString(L"disabled") == L"true";

        CListbox* gc = new CListbox(parent, name, L"", x, y, width, height);
        if (gc) {
            CmzStringList* lst = this->_getAsStringList(item, L"items");
            if (lst && lst->size() > 0) {
                gc->setItems(lst);
                delete lst;
            }
            gc->setTransparentDrawing(transparentDrawing);
            this->doRelativePositioning(gc, relativeControl.getBuffer(), relativePosition.getBuffer());            
            if (disabled) {
                gc->disable();
            }
            this->addControl(gc, parent);
        }
    }
}

VOID CLayoutFile::_processGuiControlFromOtherDllItem(CmzStdObject* item, CGuiContainer* parent) {
    return;
    CmzString type = item->getAsString(L"type");
    
    if (type == L"chart") {
        HMODULE hDLL = LoadLibrary(L"mzstdcharts.dll");
        if (hDLL) {
            ADDGUICONTROLFUNC addGuiControl = (ADDGUICONTROLFUNC)GetProcAddress(::application->getInstance(), "_addGuiControl");
            ShowLastSystemError();
            if (addGuiControl) {
                addGuiControl(this, item, parent);
            }
        }
    }
}

VOID WINAPI CLayoutFile::addControl(CGuiControl* gc, CGuiContainer* parent) {
    parent->addChild(gc);
    this->_controls->append((LONGLONG)gc);
}

VOID CLayoutFile::getCommonProperties(CmzStdObject* item, BOOL* transparentDrawing) {
    // we set MAXLONG values
    *transparentDrawing = FALSE;
    CmzString transparent = item->getAsString(L"transparent-drawing");
    if (transparent.toLowercase() == L"true") {
        *transparentDrawing = TRUE;
    }
}

VOID CLayoutFile::getDimensionProperties(CmzStdObject* item, LONG* x, LONG* y, LONG* width, LONG* height) {
    // we set MAXLONG values
    *x = MAXINT;
    *y = MAXINT;
    *width = MAXINT;
    *height = MAXINT;
    //LONG iValue;
    CmzString sValue;
    CmzStdObjectProperty* property;

    property = item->get(L"width");
    if (property) {
        if (property->type == STDOBJ_PROPERTY_VALUE_INT || property->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
            *width = static_cast<LONG>(item->getAsInt(L"width"));
        }
    }

    property = item->get(L"height");
    if (property) {
        if (property->type == STDOBJ_PROPERTY_VALUE_INT || property->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
            *height = static_cast<LONG>(item->getAsInt(L"height"));
        }
    }

    property = item->get(L"x");
    if (property) {
        if (property->type == STDOBJ_PROPERTY_VALUE_INT || property->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
            *x = static_cast<LONG>(item->getAsInt(L"x"));
        } else if (property->type == STDOBJ_PROPERTY_VALUE) {
            sValue = property->value;
            if (sValue == L"center" && ::application && ::application->screen) {
                *x = (::application->screen->width - *width) / 2;
            }
        }
    }

    property = item->get(L"y");
    if (property) {
        if (property->type == STDOBJ_PROPERTY_VALUE_INT || property->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
            *y = static_cast<LONG>(item->getAsInt(L"y"));
        } else if (property->type == STDOBJ_PROPERTY_VALUE) {
            sValue = property->value;
            if (sValue == L"center" && ::application && ::application->screen) {
                *y = (::application->screen->height - *height) / 2;
            }
        }
    }
}

VOID CLayoutFile::getRelativePositionProperties(CmzStdObject* item, CmzString* relativeControl, CmzString* relativePosition) {
    *relativeControl = item->getAsString(L"position-relative-to");
    *relativePosition = item->getAsString(L"position");
}

VOID CLayoutFile::doRelativePositioning(CGuiControl* gc, WCHAR* positionRelativeTo, WCHAR* position) {
    CGuiControl* rel = this->_getControl(positionRelativeTo);
    if (rel) {
        if (wcscmp(position, L"below") == 0) {
            gc->setPosition(rel->x, rel->y + rel->height + 4);
        } else if (wcscmp(position, L"right") == 0) {
            gc->setPosition(rel->x + rel->width + 4, rel->y);
        }
    }
}

CmzStringList* CLayoutFile::_getAsStringList(CmzStdObject* item, WCHAR* name) {
    CmzStringList* retval = NULL;
    if (item) {
        CmzStdObjectProperty* prop = item->get(name);
        if (prop && prop->type == STDOBJ_PROPERTY_ARRAY) {
            CmzPointerList* lst = prop->aValue;
            if (lst && lst->size() > 0) {
                retval = new CmzStringList(lst->size());
                if (retval) {
                    for (ULONG i = 0; i < lst->size(); i++) {
                        prop = (CmzStdObjectProperty*)lst->getAt(i);
                        if (prop && (prop->type == STDOBJ_PROPERTY_VALUE || prop->type == STDOBJ_PROPERTY_VALUE_INT || prop->type == STDOBJ_PROPERTY_VALUE_DOUBLE)) {
                            retval->append(prop->getAsString());
                        }
                    }
                }
            }
        }
    }
    
    return retval;
}
