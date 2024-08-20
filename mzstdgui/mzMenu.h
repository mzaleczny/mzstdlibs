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

#ifndef __MZSTDGUI_MENU_H__
#define __MZSTDGUI_MENU_H__

#include "mzstdguiCommon.h"
#include "mzStdObject.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzLinkList.h"
#include "mzArrayList.h"
#include "mzResource.h"

#define CMENU_HORIZONTAL_GAP		                                10L
#define CMENU_VERTICAL_GAP	                                        0L
#define CMENU_VERTICAL_MARGIN_FOR_DIVIDER	                        2L
#define CMENU_HORIZONTAL_GAP_BETWEEN_CAPTION_AND_SHORTCUT		    20L
#define CMENU_HORIZONTAL_GAP_BETWEEN_SHORTCUT_AND_ARROW		        5L
#define CMENU_HORIZONTAL_GAP_BETWEEN_ICONBAR_AND_CAPTION	        5L
#define CMENU_HORIZONTAL_GAP_BETWEEN_ITEM_CONTENT_AND_RIGHT_BORDER	5L
#define CMENU_HORIZONTAL_DIVIDER_INDENT                             0L

#define CMENU_MENU_TYPE_MAINMENU	1L
#define CMENU_MENU_TYPE_POPUPMENU	2L

// ilość wolnej przestrzeni od góry dla prawego cienia popupmenu
#define CMENU_POPUPMENU_SHADOW_TOP_INDENT    2L
// ilość wolnej przestrzeni od lewej dla dolnego cienia popupmenu
#define CMENU_POPUPMENU_SHADOW_LEFT_INDENT    5L

class CmzImageList;

namespace mzstd {

class DECLSPEC_MZSTDGUI CMenu : public CGuiControl {
public:
	WINAPI CMenu(CSprite *parent, CmzString name, CMenu *parentMenu = NULL, LONG type = CMENU_MENU_TYPE_MAINMENU, CmzImageList* imageList = NULL);
	virtual WINAPI ~CMenu();

	BOOL WINAPI loadMenu(CmzString content, LONG contentType = RESOURCE_TYPE_JSON);
	BOOL WINAPI loadMenuFromFile(CmzString fileName, LONG contentType = RESOURCE_TYPE_JSON);
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
	virtual VOID WINAPI setSizeRelativeToParent(LONG parentWidth, LONG parentHeight);
	virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
	virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
	virtual BOOL WINAPI onMouseUp(LONG x, LONG y);
    VOID WINAPI setMenuItems(mzstd::CmzArrayList *menuItems);
    VOID WINAPI setTopFrameHiliteWidth(LONG w) { this->_frameTopHiliteWidth = w; }
    BOOL WINAPI closeOpenedSubmenu();
    BOOL WINAPI openPopupMenu(CmzStdObjectProperty *item);
    inline CMenu* WINAPI getParentMenu() { return this->_parentMenu; }
    // Zwraca menu najwyższego poziomu, z któego zostało rozwinięte to menu. Jeśli to menu
    // jest menu najwyższego poziomu, to zwracany jest wskaźnik na to menu.
    CMenu* WINAPI getRootMenu();
    inline VOID WINAPI setImageList(CmzImageList* imageList) { this->_imageList = imageList; }
    inline CmzImageList* WINAPI getImageList() { return this->_imageList; }
    inline LONG WINAPI getType() { return this->_menuType; }
    inline mzstd::CmzArrayList* WINAPI getMenuItems() { return this->_menuItems; }

private:
	LONG _menuType;
    // informuje czy menu jest otwarte
    BOOL _opened;
    // wskaźnik na menu, które spowodowało rozwinięcie się tego menu. Jest ono różne
    // od wskaźnika parent, bo popupa wyświetlamy i dodajemy do pojemnika a nie do
    // innego menu.
    CMenu *_parentMenu;
    // poniższy atrybut ma znaczenie tylko dla popupmenu otwartego dla pozycji z menu
    // głównego i jest równe szerokości (width) tej pozycji. Określa szerokość
    // jaśniejszego paska górnego obramowania (tuż pod otwartą pozycją menu głównego).
    LONG _frameTopHiliteWidth;
    // atrybut mający znaczenie tylko dla popup menu. Określa długość (width) tekstu
    // nagłówka (wyrysowanego, w pikselach) najdłuższej pozycji menu. Wyznaczany
    // w metodzie setMenuItems.
    LONG _largestItemWidth;
    // atrybut mający znaczenie tylko dla popup menu. Określa długość (width) tekstu
    // (wyrysowanego, w pikselach) najdłuższego skrótu w tym popupmenu. Wyznaczany
    // w metodzie setMenuItems.
    LONG _largestShortcutWidth;
    // atrybut mający znaczenie tylko dla popup menu. Określa długość (width)
    // najdłuższej pozycji menu (jest to zsumowanie długości wyrysowanego tekstu
    // nagłówka, długości jej shortcutu, ewentualnej strzałki wskazującej że pozycja
    // ma podmenu, oraz długości wszystkich odstępów pomiędzy wymienionymi wcześniej
    // elementami).
    LONG _largestSummarizeItemWidth;
    // atrybut mający znaczenie tylko dla popup menu. Określa łączną wysokość (height)
    // wyrysowanych wszystkich pozycji popupmenu.
    LONG _summarizeItemHeight;
    // atrybut mający znaczenie tylko w przypadku popupmenu. Określa wysokość pozycji
    // menu.
    LONG _itemHeight;
    // Wskaźnik na listę obrazków zawierającą ikonki dla tego menu. Wskaźnika tego
    // nie zwalniamy. Jest on przekazywany każdemu nowemu podmenu otwieranemu przez to
    // menu.
    CmzImageList* _imageList;

    // zawiera pełną definicję menu (całą strukturę) wczytaną z pliku json.
    // do każdej pozycji dodawane są następujące atrybuty:
    //   hilite - dostępne wartości:
    //      0 - menu nie jest podświetlone
    //      1 - kursor myszy jest nad pozycją menu i jest ono podświetlone
    //      2 - kliknięto menu i zostało ono otworzone
    //   width - szerokośc danej pozycji
    CmzStdObject *_menuData;
	// zawiera wskaźnik na tablicę pozycji wyświetlanych w tym menu. Ten wskaźnik
	// nie powinien być zwalniany, bo wskazuje on na pewną część obiektu _menuData
	// przechowywanego w tym menu lub w jednym z jego rodziców.
	mzstd::CmzArrayList *_menuItems;
	// wskaźnik na rozwinięte podmenu dla którejś pozycji z tego menu. Tego wskaźnika
    // nie zwalniamy, gdyż robi to pojemnik, w którym umieszczone jest to podmenu.
	CMenu *_openedSubmenu;

    virtual VOID WINAPI _updateCanvas();
	VOID WINAPI _updateCanvasAsMainMenu();
	VOID WINAPI _updateCanvasAsPopupMenu();
	//BOOL WINAPI onMouseDownForMainMenu(LONG x, LONG y);
	//BOOL WINAPI onMouseDownForPopupMenu(LONG x, LONG y);
	//BOOL WINAPI onMouseMoveForMainMenu(LONG x, LONG y);
	//BOOL WINAPI onMouseMoveForPopupMenu(LONG x, LONG y);
    // czyści ewentualne zaznaczenie pozycji (bądź to podświetlonej bądź otwartej).
    // Zwraca TRUE, jeśli jakaś pozycja została wyczyszczona i należy odmalować menu.
    BOOL WINAPI _clearHilite();
};

typedef CMenu* PMenu;

}

#endif
