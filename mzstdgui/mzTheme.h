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

#ifndef __MZSTDGUI_THEME_H__
#define __MZSTDGUI_THEME_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzBitmap.h"
#include "mzAnimatedBitmap.h"
#include "mzIcon.h"
#include "mzFont.h"

class CResource;
class DECLSPEC_MZSTDGUI CTheme : public mzstd::CmzObject {
public:
    WINAPI CTheme(HINSTANCE hInst, CResource* res, mzstd::CmzString name);
    virtual WINAPI ~CTheme();

    // Bitmapy okna
    CmzBitmap* wnd_caption_left;
    CmzBitmap* wnd_caption_middle;
    CmzBitmap* wnd_caption_right;
    CmzBitmap* wnd_caption_inactive_left;
    CmzBitmap* wnd_caption_inactive_middle;
    CmzBitmap* wnd_caption_inactive_right;
    CmzBitmap* wnd_frame_left;
    CmzBitmap* wnd_frame_right;
    CmzBitmap* wnd_frame_bottom;
    CmzBitmap* wnd_frame_bottom_left;
    CmzBitmap* wnd_frame_bottom_right;
    CmzBitmap* wnd_frame_inactive_left;
    CmzBitmap* wnd_frame_inactive_right;
    CmzBitmap* wnd_frame_inactive_bottom;
    CmzBitmap* wnd_frame_inactive_bottom_left;
    CmzBitmap* wnd_frame_inactive_bottom_right;
    CmzBitmap* wnd_inner_bg;
    mzstd::CmzAnimatedBitmap* wnd_leftmenu_button;
    mzstd::CmzAnimatedBitmap* wnd_minimize_button;
    mzstd::CmzAnimatedBitmap* wnd_maximize_button;
    mzstd::CmzAnimatedBitmap* wnd_restore_button;
    mzstd::CmzAnimatedBitmap* wnd_close_button;

    // Bitmapy panelu
    CmzBitmap* panel_caption_left;
    CmzBitmap* panel_caption_middle;
    CmzBitmap* panel_caption_right;
    CmzBitmap* panel_nocaption_left;
    CmzBitmap* panel_nocaption_middle;
    CmzBitmap* panel_nocaption_right;
    CmzBitmap* panel_frame_left;
    CmzBitmap* panel_frame_right;
    CmzBitmap* panel_frame_bottom;
    CmzBitmap* panel_frame_bottom_left;
    CmzBitmap* panel_frame_bottom_right;
    CmzBitmap* panel_inner_bg;
    CmzBitmap* panel_inner_bg_fullscreen;

	// Bitmapy menu
	CmzBitmap* menu_bg_left;
	CmzBitmap* menu_bg_middle;
	CmzBitmap* menu_bg_right;
	CmzBitmap* menu_selected_mainmenu_item_bg_left;
	CmzBitmap* menu_selected_mainmenu_item_bg_middle;
	CmzBitmap* menu_selected_mainmenu_item_bg_right;
	CmzBitmap* menu_opened_mainmenu_item_bg_left;
	CmzBitmap* menu_opened_mainmenu_item_bg_middle;
	CmzBitmap* menu_opened_mainmenu_item_bg_right;
	CmzBitmap* menu_shadow_bottom_left;
	CmzBitmap* menu_shadow_bottom_middle;
	CmzBitmap* menu_shadow_bottom_right;
	CmzBitmap* menu_shadow_right_middle;
	CmzBitmap* menu_shadow_right_top;

    // Bitmapy popupmenu
    CmzBitmap* popupmenu_bg;
    CmzBitmap* popupmenu_frame_bottom;
    CmzBitmap* popupmenu_frame_bottom_left;
    CmzBitmap* popupmenu_frame_bottom_right;
    CmzBitmap* popupmenu_frame_left;
    CmzBitmap* popupmenu_frame_left_top;
    CmzBitmap* popupmenu_frame_right;
    CmzBitmap* popupmenu_frame_top;
    CmzBitmap* popupmenu_frame_top_hilited;
    CmzBitmap* popupmenu_frame_top_hilited_end;
    CmzBitmap* popupmenu_frame_top_right;
    CmzBitmap* popupmenu_icon_area_left;
    CmzBitmap* popupmenu_submenu_arrow;
    CmzBitmap* popupmenu_frame_shadow_bottom;
    CmzBitmap* popupmenu_frame_shadow_bottom_left;
    CmzBitmap* popupmenu_frame_shadow_bottom_right;
    CmzBitmap* popupmenu_frame_shadow_right;
    CmzBitmap* popupmenu_frame_shadow_top_right;
    CmzBitmap* popupmenu_divider;
	CmzBitmap* popupmenu_selected_item_bg_left;
	CmzBitmap* popupmenu_selected_item_bg_middle;
	CmzBitmap* popupmenu_selected_item_bg_right;
	CmzBitmap* popupmenu_opened_item_bg_left;
	CmzBitmap* popupmenu_opened_item_bg_middle;
	CmzBitmap* popupmenu_opened_item_bg_right;

    // Bitmapy toolbara przeznaczonego na ikony o rozmiarze 16x16
    CmzBitmap* toolbar16_left;
    CmzBitmap* toolbar16_middle;
    CmzBitmap* toolbar16_right;
    CmzBitmap* toolbar16_button_hilited;
    CmzBitmap* toolbar16_button_pressed;
    CmzBitmap* toolbar16_separator;

    // Bitmapy scrollbara
    CmzBitmap* scrollbar_horizontal_arrow_left;
    CmzBitmap* scrollbar_horizontal_arrow_left_clicked;
    CmzBitmap* scrollbar_horizontal_arrow_right;
    CmzBitmap* scrollbar_horizontal_arrow_right_clicked;
    CmzBitmap* scrollbar_horizontal_thumb_left;
    CmzBitmap* scrollbar_horizontal_thumb_middle;
    CmzBitmap* scrollbar_horizontal_thumb_right;
    CmzBitmap* scrollbar_horizontal_thumb_bg;
    CmzBitmap* scrollbar_horizontal_bg;
    CmzBitmap* scrollbar_horizontal_bg_clicked;
    CmzBitmap* scrollbar_vertical_arrow_up;
    CmzBitmap* scrollbar_vertical_arrow_up_clicked;
    CmzBitmap* scrollbar_vertical_arrow_down;
    CmzBitmap* scrollbar_vertical_arrow_down_clicked;
    CmzBitmap* scrollbar_vertical_thumb_top;
    CmzBitmap* scrollbar_vertical_thumb_middle;
    CmzBitmap* scrollbar_vertical_thumb_bottom;
    CmzBitmap* scrollbar_vertical_thumb_bg;
    CmzBitmap* scrollbar_vertical_bg;
    CmzBitmap* scrollbar_vertical_bg_clicked;

    // Bitmapy textfield
    CmzBitmap* textfield_frame_left;
    CmzBitmap* textfield_frame_right;
    CmzBitmap* textfield_frame_middle;
    CmzBitmap* textfield_frame_left_hover;
    CmzBitmap* textfield_frame_right_hover;
    CmzBitmap* textfield_frame_middle_hover;
    CmzBitmap* textfield_frame_left_active;
    CmzBitmap* textfield_frame_right_active;
    CmzBitmap* textfield_frame_middle_active;
    CmzBitmap* textfield_selection;

    // Bitmapy button
    CmzBitmap* button_left;
    CmzBitmap* button_right;
    CmzBitmap* button_middle;
    CmzBitmap* button_left_disabled;
    CmzBitmap* button_right_disabled;
    CmzBitmap* button_middle_disabled;
    CmzBitmap* button_left_hover;
    CmzBitmap* button_right_hover;
    CmzBitmap* button_middle_hover;
    CmzBitmap* button_left_pushed;
    CmzBitmap* button_right_pushed;
    CmzBitmap* button_middle_pushed;

    // checkbox bitmaps
    CmzBitmap* checkbox;
    CmzBitmap* checkbox_disabled;
    CmzBitmap* checkbox_hover;
    CmzBitmap* checkbox_checked;
    CmzBitmap* checkbox_checked_disabled;
    CmzBitmap* checkbox_checked_hover;

    // listbox bitmaps
    CmzBitmap* listbox_bg_fullscreen;
    CmzBitmap* listbox_bg;
    CmzBitmap* listbox_bg_selected;
    CmzBitmap* listbox_frame_top_left;
    CmzBitmap* listbox_frame_top;
    CmzBitmap* listbox_frame_top_right;
    CmzBitmap* listbox_frame_bottom_left;
    CmzBitmap* listbox_frame_bottom;
    CmzBitmap* listbox_frame_bottom_right;
    CmzBitmap* listbox_frame_left;
    CmzBitmap* listbox_frame_right;

    // Bitmapy grid
    CmzBitmap* grid_cell_inner_bg_normal;
    CmzBitmap* grid_cell_inner_bg_selected;
    CmzBitmap* grid_cell_inner_bg_selected_active;
    CmzBitmap* grid_left_header_inner_bg_normal_cell;
    CmzBitmap* grid_left_header_inner_bg_selected_cell;
    CmzBitmap* grid_left_top_header_inner_bg;
    CmzBitmap* grid_left_top_header_mark;
    CmzBitmap* grid_top_header_inner_bg_normal_cell;
    CmzBitmap* grid_top_header_inner_bg_selected_cell;

    // Dostępne fonty
    CmzFont* globalStandardFont;
    CmzFont* windowCaptionFont;
    CmzFont* captionFont;
    CmzFont* menuFont;
    CmzFont* commonLabelFont;
    CmzFont* commonTextControlFont;
    CmzFont* commonButtonControlFont;
    CmzFont* commonGridControlFont;
    CmzFont* commonCheckboxControlFont;
    CmzFont* commonListboxControlFont;


    // kolory fontu this->commonTextControlFont wyświetlonego w trybie normalnym i zaznaczonym
    COLORREF commonTextControlNormalFontColor;
    COLORREF commonTextControlSelectedFontColor;

    COLORREF commonGridControlTopHeaderNormalFontColor;
    COLORREF commonGridControlTopHeaderSelectedFontColor;
    COLORREF commonGridControlLeftHeaderNormalFontColor;
    COLORREF commonGridControlLeftHeaderSelectedFontColor;
    COLORREF commonGridControlTopHeaderCellBorderColor_Normal;
    COLORREF commonGridControlTopHeaderCellBorderColor_Selected;
    COLORREF commonGridControlLeftTopHeaderCellBorderColor;
    COLORREF commonGridControlLeftHeaderCellBorderColor_Normal;
    COLORREF commonGridControlLeftHeaderCellBorderColor_Selected;
    COLORREF commonGridControlCellBorderColor_Normal;
    COLORREF commonGridControlCellBorderColor_Selected;
    COLORREF commonGridControlCellBorderColor_Active;
    COLORREF commonGridControlSelectionBorderColor;
    COLORREF commonGridControlFocusedGridBorderColor;

    VOID WINAPI load(mzstd::CmzString name);
    VOID WINAPI loadDefaultFonts();
    VOID WINAPI unload();

    VOID WINAPI loadWindowSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadWindowSkinResources();
    VOID WINAPI loadPanelSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadPanelSkinResources();
    VOID WINAPI loadMenuSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadMenuSkinResources();
    VOID WINAPI loadPopupmenuSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadPopupmenuSkinResources();
    VOID WINAPI loadToolbar16SkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadToolbar16SkinResources();
    VOID WINAPI loadLabelSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadLabelSkinResources();
    VOID WINAPI loadScrollbarSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadScrollbarSkinResources();
    VOID WINAPI loadTextfieldSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadTextfieldSkinResources();
    VOID WINAPI loadButtonSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadButtonSkinResources();
    VOID WINAPI loadCheckboxSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadCheckboxSkinResources();
    VOID WINAPI loadListboxSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadListboxSkinResources();
    VOID WINAPI loadGridSkinResources(mzstd::CmzString themeName);
    VOID WINAPI unloadGridSkinResources();

    BOOL WINAPI isLoaded();
    BOOL WINAPI isWindowSkinLoaded();
    BOOL WINAPI isPanelSkinLoaded();
    BOOL WINAPI isMenuSkinLoaded();
    BOOL WINAPI isPopupmenuSkinLoaded();
    BOOL WINAPI isToolbar16SkinLoaded();
    BOOL WINAPI isLabelSkinLoaded();
    BOOL WINAPI isScrollbarSkinLoaded();
    BOOL WINAPI isScrollbarHorizontalSkinLoaded();
    BOOL WINAPI isScrollbarVerticalSkinLoaded();
    BOOL WINAPI isTextfieldSkinLoaded();
    BOOL WINAPI isButtonSkinLoaded();
    BOOL WINAPI isCheckboxSkinLoaded();
    BOOL WINAPI isListboxSkinLoaded();
    BOOL WINAPI isGridSkinLoaded();
    
    inline mzstd::CmzString WINAPI getThemesDir() { return this->_themesDir; }
    inline mzstd::CmzString WINAPI getThemeDir() { return this->_themesDir + L"\\" + this->_name; }

    BOOL WINAPI themeExists(mzstd::CmzString dir);

protected:
    mzstd::CmzString _name;
    CResource* _resources;
    mzstd::CmzString _themesDir;
    HINSTANCE _hInst;
private:
    mzstd::CmzString _iniSectionColors;

    CmzBitmap* WINAPI _prepareScreenBg(CmzBitmap *source);
    VOID WINAPI _initAttributes(HINSTANCE hInst);
    COLORREF WINAPI _getIniColorValue(mzstd::CmzString settings_fname, WCHAR *ini_varname);
};

typedef CTheme* PTheme;

#endif
