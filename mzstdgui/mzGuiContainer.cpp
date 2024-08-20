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
#include "mzGuiContainer.h"
#include "mzstdguiGlobals.h"
#include "mzTheme.h"
#include "mzBitmap.h"
using namespace mzstd;

VOID CGuiContainer::_commonInit() {
    this->setRealClassName(L"CGuiContainer");
    this->setAutoSize(TRUE);
}

CGuiContainer::CGuiContainer(CSprite *parent, CmzString name, LONG width, LONG height, LONG x, LONG y)
    : CGuiControl(parent, name, NULL, x, y)
{
    this->_commonInit();
}

CGuiContainer::CGuiContainer(CSprite* parent, CStyledWindow* parentWnd, CmzString name, CmzBitmap *canvas, LONG x, LONG y)
    : CGuiControl(parent, parentWnd, name, canvas, x, y)
{
    this->_commonInit();
}

CGuiContainer::~CGuiContainer() {
    this->cleanupSprites();
}

VOID CGuiContainer::setSize(LONG width, LONG height) {
    // resize bitmap
	__super::setSize(width, height);
	// and now we are resizing all children sprites that nned to be resized
	CmzLinkListNode* n = this->_sprites.first();
    while (n != NULL) {
        CSprite* sprite = (CSprite*)n->data;
		if (sprite->getAutosize()) {
			sprite->setSizeRelativeToParent(width, height);
        }
        n = n->next;
    }
}

VOID CGuiContainer::addChild(CSprite *child) {
    CGuiControl* gc = dynamic_cast<CGuiControl*>(child);
    if (gc) {
        gc->parentWnd = this->parentWnd;
    }
    this->_sprites.append((LONGLONG)child);
}
