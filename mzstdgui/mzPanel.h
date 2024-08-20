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

#ifndef __MZSTDGUI_PANEL_H__
#define __MZSTDGUI_PANEL_H__

#include "mzstdguiCommon.h"
#include "mzString.h"
#include "mzGuiContainer.h"

#define     DEFAULT_PANEL_WIDTH     500L
#define     DEFAULT_PANEL_HEIGHT    300L

class CStyledWindow;

class DECLSPEC_MZSTDGUI CPanel : public CGuiContainer {
public:
	WINAPI CPanel(CSprite *parent, mzstd::CmzString name, LONG width = -1, LONG height = -1, LONG x = 0, LONG y = 0, BOOLEAN transparent = TRUE);
	WINAPI CPanel(CSprite *parent, CStyledWindow* const parentWnd, mzstd::CmzString name, LONG width = -1, LONG height = -1, LONG x = 0, LONG y = 0, BOOLEAN transparent = TRUE);
	virtual WINAPI ~CPanel();

    inline mzstd::CmzString WINAPI getCaption() { return this->_caption; }
    inline VOID WINAPI setCaption(mzstd::CmzString caption) { this->_caption = caption; this->_needUpdate = TRUE; }
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
	virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

protected:
    mzstd::CmzString _caption;
    BOOLEAN _transparent;
    
	VOID WINAPI _commonInit(LONG width, LONG height, BOOLEAN transparent = TRUE);
    virtual VOID WINAPI _updateCanvas();
};
typedef CPanel* PPanel;

#endif
