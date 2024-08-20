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

#ifndef __MZSTDGUI_GUI_CONTAINER_H__
#define __MZSTDGUI_GUI_CONTAINER_H__

#include "mzstdguiCommon.h"
#include "mzString.h"
#include "mzGuiControl.h"
#include "mzLinkList.h"

class DECLSPEC_MZSTDGUI CGuiContainer : public CGuiControl {
public:
    WINAPI CGuiContainer(CSprite *parent, mzstd::CmzString name, LONG width = -1, LONG height = -1, LONG x = 0, LONG y = 0);
    WINAPI CGuiContainer(CSprite* parent, CStyledWindow* parentWnd, mzstd::CmzString name, CmzBitmap *canvas, LONG x, LONG y);
    virtual WINAPI ~CGuiContainer();
    
	virtual VOID WINAPI setSize(LONG width, LONG height);
	virtual VOID WINAPI addChild(CSprite *child);
    virtual inline VOID WINAPI prependChild(CSprite *child) { this->_sprites.addAsFirst((LONGLONG)child); }

protected:    
	VOID WINAPI _commonInit();

private:
};
typedef CGuiContainer* PGuiContainer;

#endif
