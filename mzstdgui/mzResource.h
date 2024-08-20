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

#ifndef __MZSTDGUI_RESOURCE_H__
#define __MZSTDGUI_RESOURCE_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzBitmap.h"
#include "mzAnimatedBitmap.h"
#include "mzIcon.h"


// types of layout content
#define RESOURCE_TYPE_JSON                  1
#define GLOBAL_RESOURCES_DIR_DEFAULT        L"c:\\devel\\resources"

class CApplication;
class DECLSPEC_MZSTDGUI CResource : public mzstd::CmzObject {
public:
    mzstd::CmzString globalResourcesDir;
    mzstd::CmzString applicationResourcesDir;
    
    WINAPI CResource(CApplication *app, mzstd::CmzString globalResourcesDir = GLOBAL_RESOURCES_DIR_DEFAULT);
    virtual WINAPI ~CResource();
    VOID WINAPI load();
    VOID WINAPI unload();

    mzstd::CmzIcon *appSmallIcon;
    mzstd::CmzIcon *appIcon;

private:
    CApplication *_app;
};

typedef CResource* PResource;

#endif
