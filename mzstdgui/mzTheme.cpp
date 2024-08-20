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
#include "mzTheme.h"
#include "mzstdguiGlobals.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"
#include "mzAnimatedBitmap.h"
#include "mzGraphicsUtils.h"
#include "mzResource.h"
using namespace mzstd;

CTheme::CTheme(HINSTANCE hInst, CResource* res, CmzString name) 
    :   _name(name), _resources(res)
{
    this->setRealClassName(L"CTheme");
    this->_initAttributes(hInst);

    if (this->themeExists(res->applicationResourcesDir + L"\\themes\\" + name)) {
        this->_themesDir = res->applicationResourcesDir + L"\\themes";
    } else if (this->themeExists(res->globalResourcesDir + L"\\themes\\" + name)) {
        this->_themesDir = res->globalResourcesDir + L"\\themes";
    }
    
    this->loadDefaultFonts();
    if (this->_themesDir != COMMON_EMPTY_STRING) {
        this->load(name);
    }
}


CTheme::~CTheme() {
    this->unload();
}

VOID CTheme::load(CmzString name) {
    this->globalStandardFont = new CmzFont(NULL, RGB(255, 255, 255), "Verdana", 8);

    if (DirectoryExists(this->_themesDir + L"\\" + name)) {
        this->loadWindowSkinResources(name);
        this->loadPanelSkinResources(name);
        this->loadButtonSkinResources(name);
        this->loadCheckboxSkinResources(name);
        this->loadScrollbarSkinResources(name);
        this->loadLabelSkinResources(name);
        this->loadTextfieldSkinResources(name);
        this->loadToolbar16SkinResources(name);
        this->loadListboxSkinResources(name);
        this->loadGridSkinResources(name);
        this->loadMenuSkinResources(name);
        this->loadPopupmenuSkinResources(name);
    }
}

VOID CTheme::loadDefaultFonts() {
    this->globalStandardFont = new CmzFont(NULL, RGB(255, 255, 255), "Verdana", 8);
    this->windowCaptionFont = new CmzFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
    this->captionFont = new CmzFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
    this->commonTextControlFont = new CmzFont(NULL, this->commonTextControlNormalFontColor, "Verdana", 8);
    this->menuFont = this->globalStandardFont;
    this->commonLabelFont = this->globalStandardFont;
    this->commonButtonControlFont = this->globalStandardFont;
    this->commonCheckboxControlFont = this->globalStandardFont;
    this->commonListboxControlFont = this->globalStandardFont;
    this->commonGridControlFont = this->globalStandardFont;
}

VOID CTheme::loadWindowSkinResources(CmzString themeName) {
    CmzString windowsDir = this->_themesDir + L"\\" + themeName + L"\\windows\\";

    if (DirectoryExists(windowsDir)) {
        this->wnd_caption_left = new CmzBitmap(this->_hInst, windowsDir + L"caption_left.bmp");
        this->wnd_caption_middle = new CmzBitmap(this->_hInst, windowsDir + L"caption_middle.bmp");
        this->wnd_caption_right = new CmzBitmap(this->_hInst, windowsDir + L"caption_right.bmp");
        this->wnd_caption_inactive_left = new CmzBitmap(this->_hInst, windowsDir + L"caption_inactive_left.bmp");
        this->wnd_caption_inactive_middle = new CmzBitmap(this->_hInst, windowsDir + L"caption_inactive_middle.bmp");
        this->wnd_caption_inactive_right = new CmzBitmap(this->_hInst, windowsDir + L"caption_inactive_right.bmp");
        this->wnd_frame_left = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_left.bmp");
        this->wnd_frame_right = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_right.bmp");
        this->wnd_frame_bottom = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_bottom.bmp");
        this->wnd_frame_bottom_left = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_bottom_left.bmp");
        this->wnd_frame_bottom_right = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_bottom_right.bmp");
        this->wnd_frame_inactive_left = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_inactive_left.bmp");
        this->wnd_frame_inactive_right = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_inactive_right.bmp");
        this->wnd_frame_inactive_bottom = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_inactive_bottom.bmp");
        this->wnd_frame_inactive_bottom_left = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_inactive_bottom_left.bmp");
        this->wnd_frame_inactive_bottom_right = new CmzBitmap(this->_hInst, windowsDir + L"window_frame_inactive_bottom_right.bmp");
        this->wnd_inner_bg = new CmzBitmap(this->_hInst, windowsDir + L"inner_bg.bmp");

        this->wnd_close_button = new CmzAnimatedBitmap(this->_hInst, windowsDir + L"button_close");
        this->wnd_minimize_button = new CmzAnimatedBitmap(this->_hInst, windowsDir + L"button_minimize");
        this->wnd_maximize_button = new CmzAnimatedBitmap(this->_hInst, windowsDir + L"button_maximize");
        this->wnd_restore_button = new CmzAnimatedBitmap(this->_hInst, windowsDir + L"button_restore");
        this->wnd_leftmenu_button = NULL;

        // Fonty
        this->windowCaptionFont = new CmzFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
        this->captionFont = new CmzFont(NULL, RGB(255, 255, 255), SYSTEM_WINDOW_CAPTION_FONT);
    }
}

VOID CTheme::unloadWindowSkinResources() {
    // ****************************************************************
    // Elementy okna
    // ****************************************************************
    if (this->wnd_caption_left) {
        delete this->wnd_caption_left;
        this->wnd_caption_left = NULL;
    }
    if (this->wnd_caption_middle) {
        delete this->wnd_caption_middle;
        this->wnd_caption_middle = NULL;
    }
    if (this->wnd_caption_right) {
        delete this->wnd_caption_right;
        this->wnd_caption_right = NULL;
    }
    if (this->wnd_caption_inactive_left) {
        delete this->wnd_caption_inactive_left;
        this->wnd_caption_inactive_left = NULL;
    }
    if (this->wnd_caption_inactive_middle) {
        delete this->wnd_caption_inactive_middle;
        this->wnd_caption_inactive_middle = NULL;
    }
    if (this->wnd_caption_inactive_right) {
        delete this->wnd_caption_inactive_right;
        this->wnd_caption_inactive_right = NULL;
    }
    if (this->wnd_frame_left) {
        delete this->wnd_frame_left;
        this->wnd_frame_left = NULL;
    }
    if (this->wnd_frame_right) {
        delete this->wnd_frame_right;
        this->wnd_frame_right = NULL;
    }
    if (this->wnd_frame_bottom) {
        delete this->wnd_frame_bottom;
        this->wnd_frame_bottom = NULL;
    }
    if (this->wnd_frame_bottom_left) {
        delete this->wnd_frame_bottom_left;
        this->wnd_frame_bottom_left = NULL;
    }
    if (this->wnd_frame_bottom_right) {
        delete this->wnd_frame_bottom_right;
        this->wnd_frame_bottom_right = NULL;
    }
    if (this->wnd_frame_inactive_left) {
        delete this->wnd_frame_inactive_left;
        this->wnd_frame_inactive_left = NULL;
    }
    if (this->wnd_frame_inactive_right) {
        delete this->wnd_frame_inactive_right;
        this->wnd_frame_inactive_right = NULL;
    }
    if (this->wnd_frame_inactive_bottom) {
        delete this->wnd_frame_inactive_bottom;
        this->wnd_frame_inactive_bottom = NULL;
    }
    if (this->wnd_frame_inactive_bottom_left) {
        delete this->wnd_frame_inactive_bottom_left;
        this->wnd_frame_inactive_bottom_left = NULL;
    }
    if (this->wnd_frame_inactive_bottom_right) {
        delete this->wnd_frame_inactive_bottom_right;
        this->wnd_frame_inactive_bottom_right = NULL;
    }
    if (this->wnd_inner_bg) {
        delete this->wnd_inner_bg;
        this->wnd_inner_bg = NULL;
    }

    if (this->wnd_close_button) {
        delete this->wnd_close_button;
        this->wnd_close_button = NULL;
    }
    if (this->wnd_minimize_button) {
        delete this->wnd_minimize_button;
        this->wnd_minimize_button = NULL;
    }
    if (this->wnd_maximize_button) {
        delete this->wnd_maximize_button;
        this->wnd_maximize_button = NULL;
    }
    if (this->wnd_restore_button) {
        delete this->wnd_restore_button;
        this->wnd_restore_button = NULL;
    }
    if (this->wnd_leftmenu_button) {
        delete this->wnd_leftmenu_button;
        this->wnd_leftmenu_button = NULL;
    }

    // Fonty
    if (this->windowCaptionFont && this->windowCaptionFont != this->globalStandardFont) {
        delete this->windowCaptionFont;
        this->windowCaptionFont = NULL;
    }
    if (this->captionFont && this->captionFont != this->globalStandardFont) {
        delete this->captionFont;
        this->captionFont = NULL;
    }
}

VOID CTheme::loadPanelSkinResources(CmzString themeName) {
    CmzString panelDir = this->_themesDir + L"\\" + themeName + L"\\panel\\";

    if (DirectoryExists(panelDir)) {
        this->panel_nocaption_left = new CmzBitmap(this->_hInst, panelDir + L"nocaption_left.bmp");
        this->panel_nocaption_middle = new CmzBitmap(this->_hInst, panelDir + L"nocaption_middle.bmp");
        this->panel_nocaption_right = new CmzBitmap(this->_hInst, panelDir + L"nocaption_right.bmp");
        this->panel_caption_left = new CmzBitmap(this->_hInst, panelDir + L"caption_left.bmp");
        this->panel_caption_middle = new CmzBitmap(this->_hInst, panelDir + L"caption_middle.bmp");
        this->panel_caption_right = new CmzBitmap(this->_hInst, panelDir + L"caption_right.bmp");
        this->panel_frame_left = new CmzBitmap(this->_hInst, panelDir + L"frame_left.bmp");
        this->panel_frame_right = new CmzBitmap(this->_hInst, panelDir + L"frame_right.bmp");
        this->panel_frame_bottom = new CmzBitmap(this->_hInst, panelDir + L"frame_bottom.bmp");
        this->panel_frame_bottom_left = new CmzBitmap(this->_hInst, panelDir + L"frame_bottom_left.bmp");
        this->panel_frame_bottom_right = new CmzBitmap(this->_hInst, panelDir + L"frame_bottom_right.bmp");
        this->panel_inner_bg = new CmzBitmap(this->_hInst, panelDir + L"inner_bg.bmp");
        // fulscreen bitmap for all panels to speedup panel redrawing
        if (this->panel_inner_bg && this->panel_inner_bg->hBitmap) {
            this->panel_inner_bg_fullscreen = this->_prepareScreenBg(this->panel_inner_bg);
        }
    }
}

VOID CTheme::unloadPanelSkinResources() {
    // ****************************************************************
    // Elementy panelu
    // ****************************************************************
    if (this->panel_nocaption_left) {
        delete this->panel_nocaption_left;
        this->panel_nocaption_left = NULL;
    }
    if (this->panel_nocaption_middle) {
        delete this->panel_nocaption_middle;
        this->panel_nocaption_middle = NULL;
    }
    if (this->panel_nocaption_right) {
        delete this->panel_nocaption_right;
        this->panel_nocaption_right = NULL;
    }
    if (this->panel_caption_left) {
        delete this->panel_caption_left;
        this->panel_caption_left = NULL;
    }
    if (this->panel_caption_middle) {
        delete this->panel_caption_middle;
        this->panel_caption_middle = NULL;
    }
    if (this->panel_caption_right) {
        delete this->panel_caption_right;
        this->panel_caption_right = NULL;
    }
    if (this->panel_frame_left) {
        delete this->panel_frame_left;
        this->panel_frame_left = NULL;
    }
    if (this->panel_frame_right) {
        delete this->panel_frame_right;
        this->panel_frame_right = NULL;
    }
    if (this->panel_frame_bottom) {
        delete this->panel_frame_bottom;
        this->panel_frame_bottom = NULL;
    }
    if (this->panel_frame_bottom_left) {
        delete this->panel_frame_bottom_left;
        this->panel_frame_bottom_left = NULL;
    }
    if (this->panel_frame_bottom_right) {
        delete this->panel_frame_bottom_right;
        this->panel_frame_bottom_right = NULL;
    }
    if (this->panel_inner_bg) {
        delete this->panel_inner_bg;
        this->panel_inner_bg = NULL;
    }
    if (this->panel_inner_bg_fullscreen) {
        delete this->panel_inner_bg_fullscreen;
        this->panel_inner_bg_fullscreen = NULL;
    }
}

VOID CTheme::loadMenuSkinResources(CmzString themeName) {
    CmzString menuDir = this->_themesDir + L"\\" + themeName + L"\\menu\\";

    if (DirectoryExists(menuDir)) {
        this->menu_bg_left = new CmzBitmap(this->_hInst, menuDir + L"bg_left.bmp");
        this->menu_bg_middle = new CmzBitmap(this->_hInst, menuDir + L"bg_middle.bmp");
        this->menu_bg_right = new CmzBitmap(this->_hInst, menuDir + L"bg_right.bmp");
        this->menu_selected_mainmenu_item_bg_left = new CmzBitmap(this->_hInst, menuDir + L"selected_mainmenu_item_bg_left.bmp");
        this->menu_selected_mainmenu_item_bg_middle = new CmzBitmap(this->_hInst, menuDir + L"selected_mainmenu_item_bg_middle.bmp");
        this->menu_selected_mainmenu_item_bg_right = new CmzBitmap(this->_hInst, menuDir + L"selected_mainmenu_item_bg_right.bmp");
        this->menu_opened_mainmenu_item_bg_left = new CmzBitmap(this->_hInst, menuDir + L"opened_mainmenu_item_bg_left.bmp");
        this->menu_opened_mainmenu_item_bg_middle = new CmzBitmap(this->_hInst, menuDir + L"opened_mainmenu_item_bg_middle.bmp");
        this->menu_opened_mainmenu_item_bg_right = new CmzBitmap(this->_hInst, menuDir + L"opened_mainmenu_item_bg_right.bmp");
        this->menu_shadow_bottom_left = new CmzBitmap(this->_hInst, menuDir + L"shadow_bottom_left.bmp");
        this->menu_shadow_bottom_middle = new CmzBitmap(this->_hInst, menuDir + L"shadow_bottom_middle.bmp");
        this->menu_shadow_bottom_right = new CmzBitmap(this->_hInst, menuDir + L"shadow_bottom_right.bmp");
        this->menu_shadow_right_middle = new CmzBitmap(this->_hInst, menuDir + L"shadow_right_middle.bmp");
        this->menu_shadow_right_top = new CmzBitmap(this->_hInst, menuDir + L"shadow_right_top.bmp");

        // Fonty
        if (!this->menuFont) {
            this->menuFont = this->globalStandardFont;
        }
    }
}

VOID CTheme::unloadMenuSkinResources() {
    // ****************************************************************
    // Elementy menu
    // ****************************************************************
	if (this->menu_bg_left) {
		delete this->menu_bg_left;
		this->menu_bg_left = NULL;
	}
	if (this->menu_bg_middle) {
		delete this->menu_bg_middle;
		this->menu_bg_middle = NULL;
	}
	if (this->menu_bg_right) {
		delete this->menu_bg_right;
		this->menu_bg_right = NULL;
	}
	if (this->menu_selected_mainmenu_item_bg_left) {
		delete this->menu_selected_mainmenu_item_bg_left;
		this->menu_selected_mainmenu_item_bg_left = NULL;
	}
	if (this->menu_selected_mainmenu_item_bg_middle) {
		delete this->menu_selected_mainmenu_item_bg_middle;
		this->menu_selected_mainmenu_item_bg_middle = NULL;
	}
	if (this->menu_selected_mainmenu_item_bg_right) {
		delete this->menu_selected_mainmenu_item_bg_right;
		this->menu_selected_mainmenu_item_bg_right = NULL;
	}
	if (this->menu_opened_mainmenu_item_bg_left) {
		delete this->menu_opened_mainmenu_item_bg_left;
		this->menu_opened_mainmenu_item_bg_left = NULL;
	}
	if (this->menu_opened_mainmenu_item_bg_middle) {
		delete this->menu_opened_mainmenu_item_bg_middle;
		this->menu_opened_mainmenu_item_bg_middle = NULL;
	}
	if (this->menu_opened_mainmenu_item_bg_right) {
		delete this->menu_opened_mainmenu_item_bg_right;
		this->menu_opened_mainmenu_item_bg_right = NULL;
	}
	if (this->menu_shadow_bottom_left) {
		delete this->menu_shadow_bottom_left;
		this->menu_shadow_bottom_left = NULL;
	}
	if (this->menu_shadow_bottom_middle) {
		delete this->menu_shadow_bottom_middle;
		this->menu_shadow_bottom_middle = NULL;
	}
	if (this->menu_shadow_bottom_right) {
		delete this->menu_shadow_bottom_right;
		this->menu_shadow_bottom_right = NULL;
	}
	if (this->menu_shadow_right_middle) {
		delete this->menu_shadow_right_middle;
		this->menu_shadow_right_middle = NULL;
	}
	if (this->menu_shadow_right_top) {
		delete this->menu_shadow_right_top;
		this->menu_shadow_right_top = NULL;
	}

    // ****************************************************************
    // Fonty
    // ****************************************************************
    if (this->menuFont && this->menuFont != this->globalStandardFont) {
        delete this->menuFont;
        this->menuFont = NULL;
    }
}

VOID CTheme::loadPopupmenuSkinResources(CmzString themeName) {
    CmzString menuDir = this->_themesDir + L"\\" + themeName + L"\\menu\\";

    if (DirectoryExists(menuDir)) {
        this->popupmenu_bg = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_bg.bmp");
        this->popupmenu_frame_bottom = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_bottom.bmp");
        this->popupmenu_frame_bottom_left = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_bottom_left.bmp");
        this->popupmenu_frame_bottom_right = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_bottom_right.bmp");
        this->popupmenu_frame_left = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_left.bmp");
        this->popupmenu_frame_left_top = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_left_top.bmp");
        this->popupmenu_frame_right = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_right.bmp");
        this->popupmenu_frame_top = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_top.bmp");
        this->popupmenu_frame_top_hilited = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_top_hilited.bmp");
        this->popupmenu_frame_top_hilited_end = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_top_hilited_end.bmp");
        this->popupmenu_frame_top_right = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_top_right.bmp");
        this->popupmenu_icon_area_left = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_icon_area_left.bmp");
        this->popupmenu_submenu_arrow = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_submenu_arrow.bmp");
        this->popupmenu_frame_shadow_bottom = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_bottom.bmp");
        this->popupmenu_frame_shadow_bottom_left = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_bottom_left.bmp");
        this->popupmenu_frame_shadow_bottom_right = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_bottom_right.bmp");
        this->popupmenu_frame_shadow_right = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_right.bmp");
        this->popupmenu_frame_shadow_top_right = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_frame_shadow_top_right.bmp");
        this->popupmenu_divider = new CmzBitmap(this->_hInst, menuDir + L"popupmenu_divider.bmp");
        this->popupmenu_selected_item_bg_left = new CmzBitmap(this->_hInst, menuDir + L"selected_popupmenu_item_bg_left.bmp");
        this->popupmenu_selected_item_bg_middle = new CmzBitmap(this->_hInst, menuDir + L"selected_popupmenu_item_bg_middle.bmp");
        this->popupmenu_selected_item_bg_right = new CmzBitmap(this->_hInst, menuDir + L"selected_popupmenu_item_bg_right.bmp");
        this->popupmenu_opened_item_bg_left = new CmzBitmap(this->_hInst, menuDir + L"opened_popupmenu_item_bg_left.bmp");
        this->popupmenu_opened_item_bg_middle = new CmzBitmap(this->_hInst, menuDir + L"opened_popupmenu_item_bg_middle.bmp");
        this->popupmenu_opened_item_bg_right = new CmzBitmap(this->_hInst, menuDir + L"opened_popupmenu_item_bg_right.bmp");

        // Fonty
        if (!this->menuFont) {
            this->menuFont = this->globalStandardFont;
        }
    }
}

VOID CTheme::unloadPopupmenuSkinResources() {
    // ****************************************************************
    // Elementy popup menu
    // ****************************************************************
	if (this->popupmenu_bg) {
        delete this->popupmenu_bg;
        this->popupmenu_bg = NULL;
    }
	if (this->popupmenu_frame_bottom) {
        delete this->popupmenu_frame_bottom;
        this->popupmenu_frame_bottom = NULL;
    }
    if (popupmenu_frame_bottom_left) {
        delete this->popupmenu_frame_bottom_left;
        this->popupmenu_frame_bottom_left = NULL;
    }
    if (this->popupmenu_frame_bottom_right) {
        delete this->popupmenu_frame_bottom_right;
        this->popupmenu_frame_bottom_right = NULL;
    }
    if (this->popupmenu_frame_left) {
        delete this->popupmenu_frame_left;
        this->popupmenu_frame_left = NULL;
    }
    if (this->popupmenu_frame_left_top) {
        delete this->popupmenu_frame_left_top;
        this->popupmenu_frame_left_top = NULL;
    }
    if (this->popupmenu_frame_right) {
        delete this->popupmenu_frame_right;
        this->popupmenu_frame_right = NULL;
    }
    if (this->popupmenu_frame_top) {
        delete this->popupmenu_frame_top;
        this->popupmenu_frame_top = NULL;
    }
    if (this->popupmenu_frame_top_hilited) {
        delete this->popupmenu_frame_top_hilited;
        this->popupmenu_frame_top_hilited = NULL;
    }
    if (this->popupmenu_frame_top_hilited_end) {
        delete this->popupmenu_frame_top_hilited_end;
        this->popupmenu_frame_top_hilited_end = NULL;
    }
    if (this->popupmenu_frame_top_right) {
        delete this->popupmenu_frame_top_right;
        this->popupmenu_frame_top_right = NULL;
    }
    if (this->popupmenu_icon_area_left) {
        delete this->popupmenu_icon_area_left;
        this->popupmenu_icon_area_left = NULL;
    }
    if (this->popupmenu_submenu_arrow) {
        delete this->popupmenu_submenu_arrow;
        this->popupmenu_submenu_arrow = NULL;
    }
    if (this->popupmenu_frame_shadow_bottom) {
        delete this->popupmenu_frame_shadow_bottom;
        this->popupmenu_frame_shadow_bottom = NULL;
    }
    if (this->popupmenu_frame_shadow_bottom_left) {
        delete this->popupmenu_frame_shadow_bottom_left;
        this->popupmenu_frame_shadow_bottom_left = NULL;
    }
    if (this->popupmenu_frame_shadow_bottom_right) {
        delete this->popupmenu_frame_shadow_bottom_right;
        this->popupmenu_frame_shadow_bottom_right = NULL;
    }
    if (this->popupmenu_frame_shadow_right) {
        delete this->popupmenu_frame_shadow_right;
        this->popupmenu_frame_shadow_right = NULL;
    }
    if (this->popupmenu_frame_shadow_top_right) {
        delete this->popupmenu_frame_shadow_top_right;
        this->popupmenu_frame_shadow_top_right = NULL;
    }
    if (this->popupmenu_divider) {
        delete this->popupmenu_divider;
        this->popupmenu_divider = NULL;
    }
	if (this->popupmenu_selected_item_bg_left) {
        delete this->popupmenu_selected_item_bg_left;
        this->popupmenu_selected_item_bg_left = NULL;
    }
	if (this->popupmenu_selected_item_bg_middle) {
        delete this->popupmenu_selected_item_bg_middle;
        this->popupmenu_selected_item_bg_middle = NULL;
    }
	if (this->popupmenu_selected_item_bg_right) {
        delete this->popupmenu_selected_item_bg_right;
        this->popupmenu_selected_item_bg_right = NULL;
    }
	if (this->popupmenu_opened_item_bg_left) {
        delete this->popupmenu_opened_item_bg_left;
        this->popupmenu_opened_item_bg_left = NULL;
    }
	if (this->popupmenu_opened_item_bg_middle) {
        delete this->popupmenu_opened_item_bg_middle;
        this->popupmenu_opened_item_bg_middle = NULL;
    }
	if (this->popupmenu_opened_item_bg_right) {
        delete this->popupmenu_opened_item_bg_right;
        this->popupmenu_opened_item_bg_right = NULL;
    }

    // ****************************************************************
    // Fonty
    // ****************************************************************
    if (this->menuFont && this->menuFont != this->globalStandardFont) {
        delete this->menuFont;
        this->menuFont = NULL;
    }
}

VOID CTheme::loadToolbar16SkinResources(CmzString themeName) {
    CmzString toolbarDir = this->_themesDir + L"\\" + themeName + L"\\toolbar\\";

    if (DirectoryExists(toolbarDir)) {
        this->toolbar16_left = new CmzBitmap(this->_hInst, toolbarDir + L"toolbar16_left.bmp");
        this->toolbar16_middle = new CmzBitmap(this->_hInst, toolbarDir + L"toolbar16_middle.bmp");
        this->toolbar16_right = new CmzBitmap(this->_hInst, toolbarDir + L"toolbar16_right.bmp");
        this->toolbar16_button_hilited = new CmzBitmap(this->_hInst, toolbarDir + L"toolbar16_button_hilited.bmp");
        this->toolbar16_button_pressed = new CmzBitmap(this->_hInst, toolbarDir + L"toolbar16_button_pressed.bmp");
        this->toolbar16_separator = new CmzBitmap(this->_hInst, toolbarDir + L"toolbar16_separator.bmp");
    }
}

VOID CTheme::unloadToolbar16SkinResources() {
    // ****************************************************************
    // Elementy toolbar-ów
    // ****************************************************************
	if (this->toolbar16_left) {
		delete this->toolbar16_left;
		this->toolbar16_left = NULL;
	}
	if (this->toolbar16_middle) {
		delete this->toolbar16_middle;
		this->toolbar16_middle = NULL;
	}
	if (this->toolbar16_right) {
		delete this->toolbar16_right;
		this->toolbar16_right = NULL;
	}
	if (this->toolbar16_button_hilited) {
		delete this->toolbar16_button_hilited;
		this->toolbar16_button_hilited = NULL;
	}
	if (this->toolbar16_button_pressed) {
		delete this->toolbar16_button_pressed;
		this->toolbar16_button_pressed = NULL;
	}
	if (this->toolbar16_separator) {
		delete this->toolbar16_separator;
		this->toolbar16_separator = NULL;
	}
}

VOID CTheme::loadLabelSkinResources(CmzString themeName) {
    // Fonty
    this->commonLabelFont = this->globalStandardFont;
}

VOID CTheme::unloadLabelSkinResources() {
    // ****************************************************************
    // Fonty
    // ****************************************************************
    if (this->commonLabelFont && this->commonLabelFont != this->globalStandardFont) {
        delete this->commonLabelFont;
        this->commonLabelFont = NULL;
    }
}

VOID CTheme::loadScrollbarSkinResources(CmzString themeName) {
    CmzString scrollbarDir = this->_themesDir + L"\\" + themeName + L"\\scrollbar\\";

    if (DirectoryExists(scrollbarDir)) {
        this->scrollbar_horizontal_arrow_left = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_arrow_left.bmp");
        this->scrollbar_horizontal_arrow_left_clicked = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_arrow_left_clicked.bmp");
        this->scrollbar_horizontal_arrow_right = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_arrow_right.bmp");
        this->scrollbar_horizontal_arrow_right_clicked = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_arrow_right_clicked.bmp");
        this->scrollbar_horizontal_thumb_left = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_thumb_left.bmp");
        this->scrollbar_horizontal_thumb_middle = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_thumb_middle.bmp");
        this->scrollbar_horizontal_thumb_right = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_thumb_right.bmp");
        this->scrollbar_horizontal_thumb_bg = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_thumb_bg.bmp");
        this->scrollbar_horizontal_bg = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_bg.bmp");
        this->scrollbar_horizontal_bg_clicked = new CmzBitmap(this->_hInst, scrollbarDir + L"horizontal_bg_clicked.bmp");

        this->scrollbar_vertical_arrow_up = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_arrow_up.bmp");
        this->scrollbar_vertical_arrow_up_clicked = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_arrow_up_clicked.bmp");
        this->scrollbar_vertical_arrow_down = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_arrow_down.bmp");
        this->scrollbar_vertical_arrow_down_clicked = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_arrow_down_clicked.bmp");
        this->scrollbar_vertical_thumb_top = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_thumb_top.bmp");
        this->scrollbar_vertical_thumb_middle = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_thumb_middle.bmp");
        this->scrollbar_vertical_thumb_bottom = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_thumb_bottom.bmp");
        this->scrollbar_vertical_thumb_bg = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_thumb_bg.bmp");
        this->scrollbar_vertical_bg = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_bg.bmp");
        this->scrollbar_vertical_bg_clicked = new CmzBitmap(this->_hInst, scrollbarDir + L"vertical_bg_clicked.bmp");
    }
}

VOID CTheme::unloadScrollbarSkinResources() {
    // ****************************************************************
    // Scrollbar
    // ****************************************************************
    if (this->scrollbar_horizontal_arrow_left) {
        delete this->scrollbar_horizontal_arrow_left;
        this->scrollbar_horizontal_arrow_left = NULL;
    }
    if (this->scrollbar_horizontal_arrow_left_clicked) {
        delete this->scrollbar_horizontal_arrow_left_clicked;
        this->scrollbar_horizontal_arrow_left_clicked = NULL;
    }
    if (this->scrollbar_horizontal_arrow_right) {
        delete this->scrollbar_horizontal_arrow_right;
        this->scrollbar_horizontal_arrow_right = NULL;
    }
    if (this->scrollbar_horizontal_arrow_right_clicked) {
        delete this->scrollbar_horizontal_arrow_right_clicked;
        this->scrollbar_horizontal_arrow_right_clicked = NULL;
    }
    if (this->scrollbar_horizontal_thumb_left) {
        delete this->scrollbar_horizontal_thumb_left;
        this->scrollbar_horizontal_thumb_left = NULL;
    }
    if (this->scrollbar_horizontal_thumb_middle) {
        delete this->scrollbar_horizontal_thumb_middle;
        this->scrollbar_horizontal_thumb_middle = NULL;
    }
    if (this->scrollbar_horizontal_thumb_right) {
        delete this->scrollbar_horizontal_thumb_right;
        this->scrollbar_horizontal_thumb_right = NULL;
    }
    if (this->scrollbar_horizontal_thumb_bg) {
        delete this->scrollbar_horizontal_thumb_bg;
        this->scrollbar_horizontal_thumb_bg = NULL;
    }
    if (this->scrollbar_horizontal_bg) {
        delete this->scrollbar_horizontal_bg;
        this->scrollbar_horizontal_bg = NULL;
    }
    if (this->scrollbar_horizontal_bg_clicked) {
        delete this->scrollbar_horizontal_bg_clicked;
        this->scrollbar_horizontal_bg_clicked = NULL;
    }
    if (this->scrollbar_vertical_arrow_up) {
        delete this->scrollbar_vertical_arrow_up;
        this->scrollbar_vertical_arrow_up = NULL;
    }
    if (this->scrollbar_vertical_arrow_up_clicked) {
        delete this->scrollbar_vertical_arrow_up_clicked;
        this->scrollbar_vertical_arrow_up_clicked = NULL;
    }
    if (this->scrollbar_vertical_arrow_down) {
        delete this->scrollbar_vertical_arrow_down;
        this->scrollbar_vertical_arrow_down = NULL;
    }
    if (this->scrollbar_vertical_arrow_down_clicked) {
        delete this->scrollbar_vertical_arrow_down_clicked;
        this->scrollbar_vertical_arrow_down_clicked = NULL;
    }
    if (this->scrollbar_vertical_thumb_top) {
        delete this->scrollbar_vertical_thumb_top;
        this->scrollbar_vertical_thumb_top = NULL;
    }
    if (this->scrollbar_vertical_thumb_middle) {
        delete this->scrollbar_vertical_thumb_middle;
        this->scrollbar_vertical_thumb_middle = NULL;
    }
    if (this->scrollbar_vertical_thumb_bottom) {
        delete this->scrollbar_vertical_thumb_bottom;
        this->scrollbar_vertical_thumb_bottom = NULL;
    }
    if (this->scrollbar_vertical_thumb_bg) {
        delete this->scrollbar_vertical_thumb_bg;
        this->scrollbar_vertical_thumb_bg = NULL;
    }
    if (this->scrollbar_vertical_bg) {
        delete this->scrollbar_vertical_bg;
        this->scrollbar_vertical_bg = NULL;
    }
    if (this->scrollbar_vertical_bg_clicked) {
        delete this->scrollbar_vertical_bg_clicked;
        this->scrollbar_vertical_bg_clicked = NULL;
    }
}

VOID CTheme::loadTextfieldSkinResources(CmzString themeName) {
    CmzString textfieldDir = this->_themesDir + L"\\" + themeName + L"\\textfield\\";

    if (DirectoryExists(textfieldDir)) {
        this->textfield_frame_left = new CmzBitmap(this->_hInst, textfieldDir + L"frame_left.bmp");
        this->textfield_frame_right = new CmzBitmap(this->_hInst, textfieldDir + L"frame_right.bmp");
        this->textfield_frame_middle = new CmzBitmap(this->_hInst, textfieldDir + L"frame_middle.bmp");
        this->textfield_frame_left_hover = new CmzBitmap(this->_hInst, textfieldDir + L"frame_left_hover.bmp");
        this->textfield_frame_right_hover = new CmzBitmap(this->_hInst, textfieldDir + L"frame_right_hover.bmp");
        this->textfield_frame_middle_hover = new CmzBitmap(this->_hInst, textfieldDir + L"frame_middle_hover.bmp");
        this->textfield_frame_left_active = new CmzBitmap(this->_hInst, textfieldDir + L"frame_left_active.bmp");
        this->textfield_frame_right_active = new CmzBitmap(this->_hInst, textfieldDir + L"frame_right_active.bmp");
        this->textfield_frame_middle_active = new CmzBitmap(this->_hInst, textfieldDir + L"frame_middle_active.bmp");
        this->textfield_selection = new CmzBitmap(this->_hInst, textfieldDir + L"selection.bmp");

        // Pozostałe parametry i zasoby
        this->commonTextControlNormalFontColor = RGB(0xff, 0xff, 0xff);
        this->commonTextControlSelectedFontColor = RGB(0xff, 0xff, 0x00);
        this->commonTextControlFont = new CmzFont(NULL, this->commonTextControlNormalFontColor, "Verdana", 8);
    }
}

VOID CTheme::unloadTextfieldSkinResources() {
    // ****************************************************************
    // TextField
    // ****************************************************************
    if (this->textfield_frame_left) {
        delete this->textfield_frame_left;
        this->textfield_frame_left = NULL;
    }
    if (this->textfield_frame_right) {
        delete this->textfield_frame_right;
        this->textfield_frame_right = NULL;
    }
    if (this->textfield_frame_middle) {
        delete this->textfield_frame_middle;
        this->textfield_frame_middle = NULL;
    }
    if (this->textfield_frame_left_hover) {
        delete this->textfield_frame_left_hover;
        this->textfield_frame_left_hover = NULL;
    }
    if (this->textfield_frame_right_hover) {
        delete this->textfield_frame_right_hover;
        this->textfield_frame_right_hover = NULL;
    }
    if (this->textfield_frame_middle_hover) {
        delete this->textfield_frame_middle_hover;
        this->textfield_frame_middle_hover = NULL;
    }
    if (this->textfield_frame_left_active) {
        delete this->textfield_frame_left_active;
        this->textfield_frame_left_active = NULL;
    }
    if (this->textfield_frame_right_active) {
        delete this->textfield_frame_right_active;
        this->textfield_frame_right_active = NULL;
    }
    if (this->textfield_frame_middle_active) {
        delete this->textfield_frame_middle_active;
        this->textfield_frame_middle_active = NULL;
    }
    if (this->textfield_selection) {
        delete this->textfield_selection;
        this->textfield_selection = NULL;
    }

    // ****************************************************************
    // Fonty
    // ****************************************************************
    if (this->commonTextControlFont && this->commonTextControlFont != this->globalStandardFont) {
        delete this->commonTextControlFont;
        this->commonTextControlFont = NULL;
    }
}

VOID CTheme::loadButtonSkinResources(CmzString themeName) {
    CmzString buttonDir = this->_themesDir + L"\\" + themeName + L"\\button\\";

    if (DirectoryExists(buttonDir)) {
        this->button_left = new CmzBitmap(this->_hInst, buttonDir + L"button_left.bmp");
        this->button_right = new CmzBitmap(this->_hInst, buttonDir + L"button_right.bmp");
        this->button_middle = new CmzBitmap(this->_hInst, buttonDir + L"button_middle.bmp");
        this->button_left_disabled = new CmzBitmap(this->_hInst, buttonDir + L"button_left_disabled.bmp");
        this->button_right_disabled = new CmzBitmap(this->_hInst, buttonDir + L"button_right_disabled.bmp");
        this->button_middle_disabled = new CmzBitmap(this->_hInst, buttonDir + L"button_middle_disabled.bmp");
        this->button_left_hover = new CmzBitmap(this->_hInst, buttonDir + L"button_left_hover.bmp");
        this->button_right_hover = new CmzBitmap(this->_hInst, buttonDir + L"button_right_hover.bmp");
        this->button_middle_hover = new CmzBitmap(this->_hInst, buttonDir + L"button_middle_hover.bmp");
        this->button_left_pushed = new CmzBitmap(this->_hInst, buttonDir + L"button_left_pushed.bmp");
        this->button_right_pushed = new CmzBitmap(this->_hInst, buttonDir + L"button_right_pushed.bmp");
        this->button_middle_pushed = new CmzBitmap(this->_hInst, buttonDir + L"button_middle_pushed.bmp");

        // Fonty
        this->commonButtonControlFont = this->globalStandardFont;
    }
}

VOID CTheme::unloadButtonSkinResources() {
    // ****************************************************************
    // Button
    // ****************************************************************
    if (this->button_left) {
        delete this->button_left;
        this->button_left = NULL;
    }
    if (this->button_right) {
        delete this->button_right;
        this->button_right = NULL;
    }
    if (this->button_middle) {
        delete this->button_middle;
        this->button_middle = NULL;
    }
    if (this->button_left_disabled) {
        delete this->button_left_disabled;
        this->button_left_disabled = NULL;
    }
    if (this->button_right_disabled) {
        delete this->button_right_disabled;
        this->button_right_disabled = NULL;
    }
    if (this->button_middle_disabled) {
        delete this->button_middle_disabled;
        this->button_middle_disabled = NULL;
    }
    if (this->button_left_hover) {
        delete this->button_left_hover;
        this->button_left_hover = NULL;
    }
    if (this->button_right_hover) {
        delete this->button_right_hover;
        this->button_right_hover = NULL;
    }
    if (this->button_middle_hover) {
        delete this->button_middle_hover;
        this->button_middle_hover = NULL;
    }
    if (this->button_left_pushed) {
        delete this->button_left_pushed;
        this->button_left_pushed = NULL;
    }
    if (this->button_right_pushed) {
        delete this->button_right_pushed;
        this->button_right_pushed = NULL;
    }
    if (this->button_middle_pushed) {
        delete this->button_middle_pushed;
        this->button_middle_pushed = NULL;
    }

    // ****************************************************************
    // Fonty
    // ****************************************************************
    if (this->commonButtonControlFont && this->commonButtonControlFont != this->globalStandardFont) {
        delete this->commonButtonControlFont;
        this->commonButtonControlFont = NULL;
    }
}

VOID CTheme::loadCheckboxSkinResources(CmzString themeName) {
    CmzString checkboxDir = this->_themesDir + L"\\" + themeName + L"\\checkbox\\";

    if (DirectoryExists(checkboxDir)) {
        this->checkbox = new CmzBitmap(this->_hInst, checkboxDir + L"checkbox.bmp");
        this->checkbox_disabled = new CmzBitmap(this->_hInst, checkboxDir + L"checkbox_disabled.bmp");
        this->checkbox_hover = new CmzBitmap(this->_hInst, checkboxDir + L"checkbox_hover.bmp");
        this->checkbox_checked = new CmzBitmap(this->_hInst, checkboxDir + L"checkbox_checked.bmp");
        this->checkbox_checked_disabled = new CmzBitmap(this->_hInst, checkboxDir + L"checkbox_checked_disabled.bmp");
        this->checkbox_checked_hover = new CmzBitmap(this->_hInst, checkboxDir + L"checkbox_checked_hover.bmp");

        // Fonty
        this->commonCheckboxControlFont = this->globalStandardFont;
    }
}

VOID CTheme::unloadCheckboxSkinResources() {
    // ****************************************************************
    // Checkbox
    // ****************************************************************
    if (this->checkbox) {
        delete this->checkbox;
        this->checkbox = NULL;
    }
    if (this->checkbox_disabled) {
        delete this->checkbox_disabled;
        this->checkbox_disabled = NULL;
    }
    if (this->checkbox_hover) {
        delete this->checkbox_hover;
        this->checkbox_hover = NULL;
    }
    if (this->checkbox_checked) {
        delete this->checkbox_checked;
        this->checkbox_checked = NULL;
    }
    if (this->checkbox_checked_disabled) {
        delete this->checkbox_checked_disabled;
        this->checkbox_checked_disabled = NULL;
    }
    if (this->checkbox_checked_hover) {
        delete this->checkbox_checked_hover;
        this->checkbox_checked_hover = NULL;
    }
    // ****************************************************************
    // Fonts
    // ****************************************************************
    if (this->commonCheckboxControlFont && this->commonCheckboxControlFont != this->globalStandardFont) {
        delete this->commonCheckboxControlFont;
        this->commonCheckboxControlFont = NULL;
    }
}

VOID CTheme::loadListboxSkinResources(CmzString themeName) {
    CmzString listboxDir = this->_themesDir + L"\\" + themeName + L"\\listbox\\";

    if (DirectoryExists(listboxDir)) {
        this->listbox_bg = new CmzBitmap(this->_hInst, listboxDir + L"bg.bmp");
        this->listbox_bg_selected = new CmzBitmap(this->_hInst, listboxDir + L"bg_selected.bmp");
        this->listbox_bg_fullscreen = this->_prepareScreenBg(this->listbox_bg);
        this->listbox_frame_top_left = new CmzBitmap(this->_hInst, listboxDir + L"frame_top_left.bmp");
        this->listbox_frame_top = new CmzBitmap(this->_hInst, listboxDir + L"frame_top.bmp");
        this->listbox_frame_top_right = new CmzBitmap(this->_hInst, listboxDir + L"frame_top_right.bmp");
        this->listbox_frame_bottom_left = new CmzBitmap(this->_hInst, listboxDir + L"frame_bottom_left.bmp");
        this->listbox_frame_bottom = new CmzBitmap(this->_hInst, listboxDir + L"frame_bottom.bmp");
        this->listbox_frame_bottom_right = new CmzBitmap(this->_hInst, listboxDir + L"frame_bottom_right.bmp");
        this->listbox_frame_left = new CmzBitmap(this->_hInst, listboxDir + L"frame_left.bmp");
        this->listbox_frame_right = new CmzBitmap(this->_hInst, listboxDir + L"frame_right.bmp");

        // Fonty
        this->commonListboxControlFont = this->globalStandardFont;
    }
}

VOID CTheme::unloadListboxSkinResources() {
    // ****************************************************************
    // Listbox
    // ****************************************************************
    if (this->listbox_bg_fullscreen) {
        delete this->listbox_bg_fullscreen;
        this->listbox_bg_fullscreen = NULL;
    }
    if (this->listbox_bg) {
        delete this->listbox_bg;
        this->listbox_bg = NULL;
    }
    if (this->listbox_bg_selected) {
        delete this->listbox_bg_selected;
        this->listbox_bg_selected = NULL;
    }
    if (this->listbox_frame_top_left) {
        delete this->listbox_frame_top_left;
        this->listbox_frame_top_left = NULL;
    }
    if (this->listbox_frame_top) {
        delete this->listbox_frame_top;
        this->listbox_frame_top = NULL;
    }
    if (this->listbox_frame_top_right) {
        delete this->listbox_frame_top_right;
        this->listbox_frame_top_right = NULL;
    }
    if (this->listbox_frame_bottom_left) {
        delete this->listbox_frame_bottom_left;
        this->listbox_frame_bottom_left = NULL;
    }
    if (this->listbox_frame_bottom) {
        delete this->listbox_frame_bottom;
        this->listbox_frame_bottom = NULL;
    }
    if (this->listbox_frame_bottom_right) {
        delete this->listbox_frame_bottom_right;
        this->listbox_frame_bottom_right = NULL;
    }
    if (this->listbox_frame_left) {
        delete this->listbox_frame_left;
        this->listbox_frame_left = NULL;
    }
    if (this->listbox_frame_right) {
        delete this->listbox_frame_right;
        this->listbox_frame_right = NULL;
    }
    // ****************************************************************
    // Fonts
    // ****************************************************************
    if (this->commonListboxControlFont && this->commonListboxControlFont != this->globalStandardFont) {
        delete this->commonListboxControlFont;
        this->commonListboxControlFont = NULL;
    }
}

VOID CTheme::loadGridSkinResources(CmzString themeName) {
    CmzString gridDir = this->_themesDir + L"\\" + themeName + L"\\grid\\";

    if (DirectoryExists(gridDir)) {
        this->grid_cell_inner_bg_normal = new CmzBitmap(this->_hInst, gridDir + L"cell_inner_bg_normal.bmp");
        this->grid_cell_inner_bg_selected = new CmzBitmap(this->_hInst, gridDir + L"cell_inner_bg_selected.bmp");
        this->grid_cell_inner_bg_selected_active = new CmzBitmap(this->_hInst, gridDir + L"cell_inner_bg_selected_active.bmp");
        this->grid_left_header_inner_bg_normal_cell = new CmzBitmap(this->_hInst, gridDir + L"left_header_inner_bg_normal_cell.bmp");
        this->grid_left_header_inner_bg_selected_cell = new CmzBitmap(this->_hInst, gridDir + L"left_header_inner_bg_selected_cell.bmp");
        this->grid_left_top_header_inner_bg = new CmzBitmap(this->_hInst, gridDir + L"left_top_header_inner_bg.bmp");
        this->grid_left_top_header_mark = new CmzBitmap(this->_hInst, gridDir + L"left_top_header_mark.bmp");
        this->grid_top_header_inner_bg_normal_cell = new CmzBitmap(this->_hInst, gridDir + L"top_header_inner_bg_normal_cell.bmp");
        this->grid_top_header_inner_bg_selected_cell = new CmzBitmap(this->_hInst, gridDir + L"top_header_inner_bg_selected_cell.bmp");

        // Fonty i pozostałe parametry
        this->commonGridControlFont = this->globalStandardFont;

        CmzString settings_fname = gridDir + L"\\_settings.ini";

        this->commonGridControlTopHeaderNormalFontColor = this->_getIniColorValue(settings_fname, L"top_header_font_color_normal");
        this->commonGridControlTopHeaderSelectedFontColor = this->_getIniColorValue(settings_fname, L"top_header_font_color_selected");
        this->commonGridControlLeftHeaderNormalFontColor = this->_getIniColorValue(settings_fname, L"left_header_font_color_normal");
        this->commonGridControlLeftHeaderSelectedFontColor = this->_getIniColorValue(settings_fname, L"left_header_font_color_selected");
        this->commonGridControlTopHeaderCellBorderColor_Normal = this->_getIniColorValue(settings_fname, L"top_header_cell_border_color_normal");
        this->commonGridControlTopHeaderCellBorderColor_Selected = this->_getIniColorValue(settings_fname, L"top_header_cell_border_color_selected");
        this->commonGridControlLeftTopHeaderCellBorderColor = this->_getIniColorValue(settings_fname, L"left_top_header_cell_border_color");
        this->commonGridControlLeftHeaderCellBorderColor_Normal = this->_getIniColorValue(settings_fname, L"left_header_cell_border_color_normal");
        this->commonGridControlLeftHeaderCellBorderColor_Selected = this->_getIniColorValue(settings_fname, L"left_header_cell_border_color_selected");
        this->commonGridControlCellBorderColor_Normal = this->_getIniColorValue(settings_fname, L"cell_border_color_normal");
        this->commonGridControlCellBorderColor_Selected = this->_getIniColorValue(settings_fname, L"cell_border_color_selected");
        this->commonGridControlCellBorderColor_Active = this->_getIniColorValue(settings_fname, L"cell_border_color_active");
        this->commonGridControlSelectionBorderColor = this->_getIniColorValue(settings_fname, L"border_color_selection");
        this->commonGridControlFocusedGridBorderColor = this->_getIniColorValue(settings_fname, L"border_color_focused_grid");
    }
}

VOID CTheme::unloadGridSkinResources() {
    if (this->grid_cell_inner_bg_normal) {
        delete this->grid_cell_inner_bg_normal;
        this->grid_cell_inner_bg_normal = NULL;
    }
    if (this->grid_cell_inner_bg_selected) {
        delete this->grid_cell_inner_bg_selected;
        this->grid_cell_inner_bg_selected = NULL;
    }
    if (this->grid_cell_inner_bg_selected_active) {
        delete this->grid_cell_inner_bg_selected_active;
        this->grid_cell_inner_bg_selected_active = NULL;
    }
    if (this->grid_left_header_inner_bg_normal_cell) {
        delete this->grid_left_header_inner_bg_normal_cell;
        this->grid_left_header_inner_bg_normal_cell = NULL;
    }
    if (this->grid_left_header_inner_bg_selected_cell) {
        delete this->grid_left_header_inner_bg_selected_cell;
        this->grid_left_header_inner_bg_selected_cell = NULL;
    }
    if (this->grid_left_top_header_inner_bg) {
        delete this->grid_left_top_header_inner_bg;
        this->grid_left_top_header_inner_bg = NULL;
    }
    if (this->grid_left_top_header_mark) {
        delete this->grid_left_top_header_mark;
        this->grid_left_top_header_mark = NULL;
    }
    if (this->grid_top_header_inner_bg_normal_cell) {
        delete this->grid_top_header_inner_bg_normal_cell;
        this->grid_top_header_inner_bg_normal_cell = NULL;
    }
    if (this->grid_top_header_inner_bg_selected_cell) {
        delete this->grid_top_header_inner_bg_selected_cell;
        this->grid_top_header_inner_bg_selected_cell = NULL;
    }

    // ****************************************************************
    // Fonty
    // ****************************************************************
    if (this->commonGridControlFont && this->commonGridControlFont != this->globalStandardFont) {
        delete this->commonGridControlFont;
        this->commonGridControlFont = NULL;
    }
}

VOID CTheme::unload() {
    this->unloadWindowSkinResources();
    this->unloadPanelSkinResources();
    this->unloadMenuSkinResources();
    this->unloadPopupmenuSkinResources();
    this->unloadToolbar16SkinResources();
    this->unloadLabelSkinResources();
    this->unloadScrollbarSkinResources();
    this->unloadTextfieldSkinResources();
    this->unloadButtonSkinResources();
    this->unloadCheckboxSkinResources();
    this->unloadListboxSkinResources();
    this->unloadGridSkinResources();

    if (this->globalStandardFont) {
        delete this->globalStandardFont;
        this->globalStandardFont = NULL;
    }
}

CmzBitmap* CTheme::_prepareScreenBg(CmzBitmap *sourceBmp) {
    LONG screenWidth = GetSystemMetrics(SM_CXSCREEN);
    LONG screenHeight = GetSystemMetrics(SM_CYSCREEN);
    CmzBitmap *screenBmp = NULL;
    // bitmapa mająca przyspieszyć wypełnienie powyższej bitmapy
    CmzBitmap *tmpBmp = NULL;
    LONG tmpWidth, tmpHeight;

    screenBmp = new CmzBitmap(screenWidth, screenHeight, RGB(0xff, 0xff, 0xff));
    tmpWidth = 0;
    while (tmpWidth < 100) {
        tmpWidth += sourceBmp->width();
    }
    tmpHeight = 0;
    while (tmpHeight < 100) {
        tmpHeight += sourceBmp->height();
    }
    tmpBmp = new CmzBitmap(tmpWidth, tmpHeight, RGB(0xff, 0xff, 0xff));

    LONG x = 0;
    LONG y = 0;
    LONG bgW = sourceBmp->width();
    LONG bgH = sourceBmp->height();
    LONG tilesCountX = tmpWidth / bgW;
    LONG tilesCountY = tmpHeight / bgH;
    LONG reminderX = tmpWidth - tilesCountX * bgW;
    LONG reminderY = tmpHeight - tilesCountY * bgH;
    HDC hdc = GetDC(0);

    tmpBmp->beginPaint(hdc);
    for (LONG j = 0; j < tilesCountY; j++) {
        for (LONG i = 0; i < tilesCountX; i++) {
            tmpBmp->drawBitmap(sourceBmp, x + i * bgW, y + j * bgH);
        }
    }
    tmpBmp->endPaint();

    // *******************************************************************
    // a teraz wypełniamy właściwą bitmapę pełnoekranową
    bgW = tmpBmp->width();
    bgH = tmpBmp->height();
    tilesCountX = screenWidth / bgW;
    tilesCountY = screenHeight / bgH;
    reminderX = screenWidth - tilesCountX * bgW;
    reminderY = screenHeight - tilesCountY * bgH;

    screenBmp->beginPaint(hdc);
    for (LONG j = 0; j < tilesCountY; j++) {
        for (LONG i = 0; i < tilesCountX; i++) {
            screenBmp->drawBitmap(tmpBmp, x + i * bgW, y + j * bgH);
        }
    }
    // rysujemy ewentualne zakończenie tła na osi x
    for (LONG j = 0; j < tilesCountY; j++) {
        screenBmp->drawBitmapPart(tmpBmp, x + tilesCountX * bgW, y + j * bgH, 0, 0, reminderX, bgH);
    }
    // rysujemy ewentualne zakończenie tła na osi y
    for (LONG i = 0; i < tilesCountX; i++) {
        screenBmp->drawBitmapPart(tmpBmp, x + i * bgW, y + tilesCountY * bgH, 0, 0, bgW, reminderY);
    }
    // wypełniamy prostokąt w prawym dolnym rogu
    screenBmp->drawBitmapPart(tmpBmp, x + tilesCountX * bgW, y + tilesCountY * bgH, 0, 0, reminderX, reminderY);
    screenBmp->endPaint();

    ReleaseDC(0, hdc);
    delete tmpBmp;
    return screenBmp;
}

VOID CTheme::_initAttributes(HINSTANCE hInst) {
    this->_hInst = hInst;
    
    this->_iniSectionColors = L"colors";

    this->wnd_caption_left = NULL;
    this->wnd_caption_middle = NULL;
    this->wnd_caption_right = NULL;
    this->wnd_frame_left = NULL;
    this->wnd_frame_right = NULL;
    this->wnd_frame_bottom = NULL;
    this->wnd_frame_bottom_left = NULL;
    this->wnd_frame_bottom_right = NULL;
    this->wnd_leftmenu_button = NULL;
    this->wnd_minimize_button = NULL;
    this->wnd_maximize_button = NULL;
    this->wnd_restore_button = NULL;
    this->wnd_close_button = NULL;

    // Bitmapy panelu
    this->panel_caption_left = NULL;
    this->panel_caption_middle = NULL;
    this->panel_caption_right = NULL;
    this->panel_nocaption_left = NULL;
    this->panel_nocaption_middle = NULL;
    this->panel_nocaption_right = NULL;
    this->panel_frame_left = NULL;
    this->panel_frame_right = NULL;
    this->panel_frame_bottom = NULL;
    this->panel_frame_bottom_left = NULL;
    this->panel_frame_bottom_right = NULL;
    this->panel_inner_bg = NULL;
    this->panel_inner_bg_fullscreen = NULL;

	// Bitmapy menu
	this->menu_bg_left = NULL;
	this->menu_bg_middle = NULL;
	this->menu_bg_right = NULL;
	this->menu_selected_mainmenu_item_bg_left = NULL;
	this->menu_selected_mainmenu_item_bg_middle = NULL;
	this->menu_selected_mainmenu_item_bg_right = NULL;
	this->menu_opened_mainmenu_item_bg_left = NULL;
	this->menu_opened_mainmenu_item_bg_middle = NULL;
	this->menu_opened_mainmenu_item_bg_right = NULL;
	this->menu_shadow_bottom_left = NULL;
	this->menu_shadow_bottom_middle = NULL;
	this->menu_shadow_bottom_right = NULL;
	this->menu_shadow_right_middle = NULL;
	this->menu_shadow_right_top = NULL;

    // Bitmapy popupmenu
    this->popupmenu_bg = NULL;
    this->popupmenu_frame_bottom = NULL;
    this->popupmenu_frame_bottom_left = NULL;
    this->popupmenu_frame_bottom_right = NULL;
    this->popupmenu_frame_left = NULL;
    this->popupmenu_frame_left_top = NULL;
    this->popupmenu_frame_right = NULL;
    this->popupmenu_frame_top = NULL;
    this->popupmenu_frame_top_hilited = NULL;
    this->popupmenu_frame_top_hilited_end = NULL;
    this->popupmenu_frame_top_right = NULL;
    this->popupmenu_icon_area_left = NULL;
    this->popupmenu_submenu_arrow = NULL;
    this->popupmenu_frame_shadow_bottom = NULL;
    this->popupmenu_frame_shadow_bottom_left = NULL;
    this->popupmenu_frame_shadow_bottom_right = NULL;
    this->popupmenu_frame_shadow_right = NULL;
    this->popupmenu_frame_shadow_top_right = NULL;
    this->popupmenu_divider = NULL;
	this->popupmenu_selected_item_bg_left = NULL;
	this->popupmenu_selected_item_bg_middle = NULL;
	this->popupmenu_selected_item_bg_right = NULL;
	this->popupmenu_opened_item_bg_left = NULL;
	this->popupmenu_opened_item_bg_middle = NULL;
	this->popupmenu_opened_item_bg_right = NULL;

    // Bitmapy toolbara przeznaczonego na ikony o rozmiarze 16x16
    this->toolbar16_left = NULL;
    this->toolbar16_middle = NULL;
    this->toolbar16_right = NULL;
    this->toolbar16_button_hilited = NULL;
    this->toolbar16_button_pressed = NULL;
    this->toolbar16_separator = NULL;

    // Bitmapy scrollbara
    this->scrollbar_horizontal_arrow_left = NULL;
    this->scrollbar_horizontal_arrow_left_clicked = NULL;
    this->scrollbar_horizontal_arrow_right = NULL;
    this->scrollbar_horizontal_arrow_right_clicked = NULL;
    this->scrollbar_horizontal_thumb_left = NULL;
    this->scrollbar_horizontal_thumb_middle = NULL;
    this->scrollbar_horizontal_thumb_right = NULL;
    this->scrollbar_horizontal_thumb_bg = NULL;
    this->scrollbar_horizontal_bg = NULL;
    this->scrollbar_horizontal_bg_clicked = NULL;
    this->scrollbar_vertical_arrow_up = NULL;
    this->scrollbar_vertical_arrow_up_clicked = NULL;
    this->scrollbar_vertical_arrow_down = NULL;
    this->scrollbar_vertical_arrow_down_clicked = NULL;
    this->scrollbar_vertical_thumb_top = NULL;
    this->scrollbar_vertical_thumb_middle = NULL;
    this->scrollbar_vertical_thumb_bottom = NULL;
    this->scrollbar_vertical_thumb_bg = NULL;
    this->scrollbar_vertical_bg = NULL;
    this->scrollbar_vertical_bg_clicked = NULL;

    // Bitmapy Textfield
    this->textfield_frame_left = NULL;
    this->textfield_frame_right = NULL;
    this->textfield_frame_middle = NULL;
    this->textfield_frame_left_hover = NULL;
    this->textfield_frame_right_hover = NULL;
    this->textfield_frame_middle_hover = NULL;
    this->textfield_frame_left_active = NULL;
    this->textfield_frame_right_active = NULL;
    this->textfield_frame_middle_active = NULL;
    this->textfield_selection = NULL;

    // Bitmapy Button
    this->button_left = NULL;
    this->button_right = NULL;
    this->button_middle = NULL;
    this->button_left_disabled = NULL;
    this->button_right_disabled = NULL;
    this->button_middle_disabled = NULL;
    this->button_left_hover = NULL;
    this->button_right_hover = NULL;
    this->button_middle_hover = NULL;
    this->button_left_pushed = NULL;
    this->button_right_pushed = NULL;
    this->button_middle_pushed = NULL;

    // Checkbox bitmaps
    this->checkbox = NULL;
    this->checkbox_disabled = NULL;
    this->checkbox_hover = NULL;
    this->checkbox_checked = NULL;
    this->checkbox_checked_disabled = NULL;
    this->checkbox_checked_hover = NULL;

    // Listbox bitmaps
    this->listbox_bg_fullscreen = NULL;
    this->listbox_bg = NULL;
    this->listbox_frame_top_left = NULL;
    this->listbox_frame_top = NULL;
    this->listbox_frame_top_right = NULL;
    this->listbox_frame_bottom_left = NULL;
    this->listbox_frame_bottom = NULL;
    this->listbox_frame_bottom_right = NULL;
    this->listbox_frame_left = NULL;
    this->listbox_frame_right = NULL;

    // Bitmapy Grid
    this->grid_cell_inner_bg_normal = NULL;
    this->grid_cell_inner_bg_selected = NULL;
    this->grid_cell_inner_bg_selected_active = NULL;
    this->grid_left_header_inner_bg_normal_cell = NULL;
    this->grid_left_header_inner_bg_selected_cell = NULL;
    this->grid_left_top_header_mark = NULL;
    this->grid_left_top_header_inner_bg = NULL;
    this->grid_top_header_inner_bg_normal_cell = NULL;
    this->grid_top_header_inner_bg_selected_cell = NULL;

    // Fonty
    this->globalStandardFont = NULL;
    this->windowCaptionFont = NULL;
    this->captionFont = NULL;
    this->menuFont = NULL;
    this->commonLabelFont = NULL;
    this->commonTextControlFont = NULL;
    this->commonButtonControlFont = NULL;
    this->commonGridControlFont = NULL;
}

COLORREF CTheme::_getIniColorValue(CmzString settings_fname, WCHAR *ini_varname) {
    DWORD buf_size = 16;
    WCHAR input[16];
    WCHAR *colors_section = this->_iniSectionColors.getBuffer();

    GetPrivateProfileString(colors_section, ini_varname, L"#000000", input, buf_size, settings_fname.getBuffer());
    return HexToRGB(input);
}


BOOL CTheme::isLoaded() {
    return this->isWindowSkinLoaded() &&
           this->isPanelSkinLoaded() &&
           this->isMenuSkinLoaded() &&
           this->isPopupmenuSkinLoaded() &&
           this->isToolbar16SkinLoaded() &&
           this->isLabelSkinLoaded() &&
           this->isScrollbarSkinLoaded() &&
           this->isScrollbarHorizontalSkinLoaded() &&
           this->isScrollbarVerticalSkinLoaded() &&
           this->isTextfieldSkinLoaded() &&
           this->isButtonSkinLoaded() &&
           this->isCheckboxSkinLoaded() &&
           this->isListboxSkinLoaded() &&
           this->isGridSkinLoaded();
}

BOOL CTheme::isWindowSkinLoaded() {
    return  this->wnd_caption_left != NULL &&
            this->wnd_caption_middle != NULL &&
            this->wnd_caption_right != NULL &&
            this->wnd_caption_inactive_left != NULL &&
            this->wnd_caption_inactive_middle != NULL &&
            this->wnd_caption_inactive_right != NULL &&
            this->wnd_frame_left != NULL &&
            this->wnd_frame_right != NULL &&
            this->wnd_frame_bottom != NULL &&
            this->wnd_frame_bottom_left != NULL &&
            this->wnd_frame_bottom_right != NULL &&
            this->wnd_frame_inactive_left != NULL &&
            this->wnd_frame_inactive_right != NULL &&
            this->wnd_frame_inactive_bottom != NULL &&
            this->wnd_frame_inactive_bottom_left != NULL &&
            this->wnd_frame_inactive_bottom_right != NULL &&
            //this->wnd_leftmenu_button != NULL &&
            this->wnd_minimize_button != NULL &&
            this->wnd_maximize_button != NULL &&
            this->wnd_restore_button != NULL &&
            this->wnd_close_button != NULL;
}

BOOL CTheme::isPanelSkinLoaded() {
    return  this->panel_caption_left != NULL &&
            this->panel_caption_middle != NULL &&
            this->panel_caption_right != NULL &&
            this->panel_nocaption_left != NULL &&
            this->panel_nocaption_middle != NULL &&
            this->panel_nocaption_right != NULL &&
            this->panel_frame_left != NULL &&
            this->panel_frame_right != NULL &&
            this->panel_frame_bottom != NULL &&
            this->panel_frame_bottom_left != NULL &&
            this->panel_frame_bottom_right != NULL &&
            this->panel_inner_bg != NULL &&
            this->panel_inner_bg_fullscreen != NULL;
}

BOOL CTheme::isMenuSkinLoaded() {
	return  this->menu_bg_left != NULL &&
            this->menu_bg_middle != NULL &&
            this->menu_bg_right != NULL &&
            this->menu_selected_mainmenu_item_bg_left != NULL &&
            this->menu_selected_mainmenu_item_bg_middle != NULL &&
            this->menu_selected_mainmenu_item_bg_right != NULL &&
            this->menu_opened_mainmenu_item_bg_left != NULL &&
            this->menu_opened_mainmenu_item_bg_middle != NULL &&
            this->menu_opened_mainmenu_item_bg_right != NULL &&
            this->menu_shadow_bottom_left != NULL &&
            this->menu_shadow_bottom_middle != NULL &&
            this->menu_shadow_bottom_right != NULL &&
            this->menu_shadow_right_middle != NULL &&
            this->menu_shadow_right_top != NULL;
}

BOOL CTheme::isPopupmenuSkinLoaded() {
    return  this->popupmenu_bg != NULL &&
            this->popupmenu_frame_bottom != NULL &&
            this->popupmenu_frame_bottom_left != NULL &&
            this->popupmenu_frame_bottom_right != NULL &&
            this->popupmenu_frame_left != NULL &&
            this->popupmenu_frame_left_top != NULL &&
            this->popupmenu_frame_right != NULL &&
            this->popupmenu_frame_top != NULL &&
            this->popupmenu_frame_top_hilited != NULL &&
            this->popupmenu_frame_top_hilited_end != NULL &&
            this->popupmenu_frame_top_right != NULL &&
            this->popupmenu_icon_area_left != NULL &&
            this->popupmenu_submenu_arrow != NULL &&
            this->popupmenu_frame_shadow_bottom != NULL &&
            this->popupmenu_frame_shadow_bottom_left != NULL &&
            this->popupmenu_frame_shadow_bottom_right != NULL &&
            this->popupmenu_frame_shadow_right != NULL &&
            this->popupmenu_frame_shadow_top_right != NULL &&
            this->popupmenu_divider != NULL &&
            this->popupmenu_selected_item_bg_left != NULL &&
            this->popupmenu_selected_item_bg_middle != NULL &&
            this->popupmenu_selected_item_bg_right != NULL &&
            this->popupmenu_opened_item_bg_left != NULL &&
            this->popupmenu_opened_item_bg_middle != NULL &&
            this->popupmenu_opened_item_bg_right != NULL;
}

BOOL CTheme::isToolbar16SkinLoaded() {
    return  this->toolbar16_left != NULL &&
            this->toolbar16_middle != NULL &&
            this->toolbar16_right != NULL &&
            this->toolbar16_button_hilited != NULL &&
            this->toolbar16_button_pressed != NULL &&
            this->toolbar16_separator != NULL;
}

BOOL CTheme::isLabelSkinLoaded() {
    return TRUE;
}

BOOL CTheme::isScrollbarSkinLoaded() {
    return this->isScrollbarHorizontalSkinLoaded() && this->isScrollbarVerticalSkinLoaded();
}

BOOL CTheme::isScrollbarHorizontalSkinLoaded() {
    return  this->scrollbar_horizontal_arrow_left != NULL &&
            this->scrollbar_horizontal_arrow_left_clicked != NULL &&
            this->scrollbar_horizontal_arrow_right != NULL &&
            this->scrollbar_horizontal_arrow_right_clicked != NULL &&
            this->scrollbar_horizontal_thumb_left != NULL &&
            this->scrollbar_horizontal_thumb_middle != NULL &&
            this->scrollbar_horizontal_thumb_right != NULL &&
            this->scrollbar_horizontal_thumb_bg != NULL &&
            this->scrollbar_horizontal_bg != NULL &&
            this->scrollbar_horizontal_bg_clicked != NULL;
}

BOOL CTheme::isScrollbarVerticalSkinLoaded() {
    return  this->scrollbar_vertical_arrow_up != NULL &&
            this->scrollbar_vertical_arrow_up_clicked != NULL &&
            this->scrollbar_vertical_arrow_down != NULL &&
            this->scrollbar_vertical_arrow_down_clicked != NULL &&
            this->scrollbar_vertical_thumb_top != NULL &&
            this->scrollbar_vertical_thumb_middle != NULL &&
            this->scrollbar_vertical_thumb_bottom != NULL &&
            this->scrollbar_vertical_thumb_bg != NULL &&
            this->scrollbar_vertical_bg != NULL &&
            this->scrollbar_vertical_bg_clicked != NULL;
}

BOOL CTheme::isTextfieldSkinLoaded() {
    return  this->textfield_frame_left != NULL &&
            this->textfield_frame_right != NULL &&
            this->textfield_frame_middle != NULL &&
            this->textfield_frame_left_hover != NULL &&
            this->textfield_frame_right_hover != NULL &&
            this->textfield_frame_middle_hover != NULL &&
            this->textfield_frame_left_active != NULL &&
            this->textfield_frame_right_active != NULL &&
            this->textfield_frame_middle_active != NULL &&
            this->textfield_selection != NULL;
}

BOOL CTheme::isButtonSkinLoaded() {
    return  this->button_left != NULL &&
            this->button_right != NULL &&
            this->button_middle != NULL &&
            this->button_left_disabled != NULL &&
            this->button_right_disabled != NULL &&
            this->button_middle_disabled != NULL &&
            this->button_left_hover != NULL &&
            this->button_right_hover != NULL &&
            this->button_middle_hover != NULL &&
            this->button_left_pushed != NULL &&
            this->button_right_pushed != NULL &&
            this->button_middle_pushed != NULL;
}

BOOL CTheme::isCheckboxSkinLoaded() {
    return  this->checkbox != NULL &&
            this->checkbox_disabled != NULL &&
            this->checkbox_hover != NULL &&
            this->checkbox_checked != NULL &&
            this->checkbox_checked_disabled != NULL &&
            this->checkbox_checked_hover != NULL;
}

BOOL CTheme::isListboxSkinLoaded() {
    return  this->listbox_bg_fullscreen != NULL &&
            this->listbox_bg != NULL &&
            this->listbox_frame_top_left != NULL &&
            this->listbox_frame_top != NULL &&
            this->listbox_frame_top_right != NULL &&
            this->listbox_frame_bottom_left != NULL &&
            this->listbox_frame_bottom != NULL &&
            this->listbox_frame_bottom_right != NULL &&
            this->listbox_frame_left != NULL &&
            this->listbox_frame_right != NULL;
}

BOOL CTheme::isGridSkinLoaded() {
    return  this->grid_cell_inner_bg_normal != NULL &&
            this->grid_cell_inner_bg_selected != NULL &&
            this->grid_cell_inner_bg_selected_active != NULL &&
            this->grid_left_header_inner_bg_normal_cell != NULL &&
            this->grid_left_header_inner_bg_selected_cell != NULL &&
            this->grid_left_top_header_inner_bg != NULL &&
            this->grid_left_top_header_mark != NULL &&
            this->grid_top_header_inner_bg_normal_cell != NULL &&
            this->grid_top_header_inner_bg_selected_cell != NULL;
}

BOOL CTheme::themeExists(CmzString dir) {
    if (DirectoryExists(dir) &&
        DirectoryExists(dir + L"\\button") &&
        DirectoryExists(dir + L"\\checkbox") &&
        DirectoryExists(dir + L"\\listbox") &&
        DirectoryExists(dir + L"\\grid") &&
        DirectoryExists(dir + L"\\menu") &&
        DirectoryExists(dir + L"\\panel") &&
        DirectoryExists(dir + L"\\scrollbar") &&
        DirectoryExists(dir + L"\\textfield") &&
        DirectoryExists(dir + L"\\toolbar") &&
        DirectoryExists(dir + L"\\windows")) {
        return TRUE;
    }
    
    return FALSE;
}
