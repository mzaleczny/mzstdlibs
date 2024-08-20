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

#ifndef __MZSTDGUI_TOOLBAR_H__
#define __MZSTDGUI_TOOLBAR_H__

#include "mzstdguiCommon.h"
#include "mzStdObject.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzLinkList.h"

class CmzImageList;

#define CTOOLBAR_ITEM_STATE_NORMAL      0L
#define CTOOLBAR_ITEM_STATE_HILITED     1L
#define CTOOLBAR_ITEM_STATE_PRESSED     2L

#define CTOOLBAR_MARGIN             4L
#define CTOOLBAR_SEPARATOR_MARGIN   4L
#define CTOOLBAR_BUTTON_GAP         1L

class DECLSPEC_MZSTDGUI CToolbar : public CGuiControl {
public:
	WINAPI CToolbar(CSprite *parent, mzstd::CmzString name, mzstd::CmzString itemsJsonFname = L"", CmzImageList* imageList = NULL, LONG size = 16, LONG x = 0, LONG y = 0);
	virtual WINAPI ~CToolbar();

	virtual VOID WINAPI setSizeRelativeToParent(LONG parentWidth, LONG parentHeight);
    VOID WINAPI load(mzstd::CmzString itemsJsonFname);
	virtual VOID WINAPI cleanup();
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
	virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

private:
    mzstd::CmzString _itemsJsonFname;
    mzstd::CmzLinkList _sprites;
    // Poniższego wskaźnika nie zwalniamy, bo to powinna być globalna lista obrazków dostępna
    // dla całego GUI
    CmzImageList* _imageList;
    LONG _size;

    // Każda pozycja typu "tbButton" tablicy itemów ma m.in dodatkowy atrybut "state",
    // który ma następujące wartości:
    //   0 - przycisk nie został podświelony ani wciśnięty
    //   1 - przycisk został podświetlony (kursor myszki jest nad nim)
    //   2 - przycisk został wciśnięty
    mzstd::CmzStdObject *_items;
    VOID WINAPI _commonInit(mzstd::CmzString itemsJsonFname, CmzImageList* imageList, LONG size);
    virtual VOID WINAPI _updateCanvas();
};

typedef CToolbar* PToolbar;

#endif
