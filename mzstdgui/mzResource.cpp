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
#include "mzResource.h"
#include "mzstdguiGlobals.h"
#include "mzFileUtils.h"
#include "mzApplication.h"
using namespace mzstd;

CResource::CResource(CApplication *app, CmzString globalResourcesDir)
    : _app(app)
{
    this->setRealClassName(L"CResource");
    this->appSmallIcon = NULL;
    this->appIcon = NULL;
    
    
    if (globalResourcesDir == ".") {
        this->globalResourcesDir = GetCurrentDirectory() + L"\\resources";
        globalResourcesDir = COMMON_EMPTY_STRING;
    }
    
    this->applicationResourcesDir = GetCurrentDirectory() + L"\\resources";
    if (!DirectoryExists(this->applicationResourcesDir)) {
        this->applicationResourcesDir = applicationResourcesDir;
    }
    if (DirectoryExists(globalResourcesDir)) {
        this->globalResourcesDir = globalResourcesDir;
    } else {
        if (this->globalResourcesDir == COMMON_EMPTY_STRING) {
            this->globalResourcesDir = GLOBAL_RESOURCES_DIR_DEFAULT;
        }
    }
}

CResource::~CResource() {
    this->unload();
}

VOID CResource::load() {
    CmzString resDir = this->applicationResourcesDir;
    CmzString iconsDir = resDir + "\\icons\\";
    CmzString icons16x16Dir = resDir + "\\icons\\16x16\\";
    
    if (DirectoryExists(resDir)) {

        if (DirectoryExists(iconsDir)) {
            this->appSmallIcon = new CmzIcon(this->_app->getInstance(), iconsDir + "default_icon_small.ico");
            this->appIcon = new CmzIcon(this->_app->getInstance(), iconsDir + "default_icon_normal.ico");
        }
    }
    if (!this->appSmallIcon || !this->appIcon) {
        resDir = this->globalResourcesDir;
        if (DirectoryExists(resDir)) {
            iconsDir = resDir + "\\icons\\";
            icons16x16Dir = resDir + "\\icons\\16x16\\";

            if (DirectoryExists(iconsDir)) {
                if (!this->appSmallIcon) {
                    this->appSmallIcon = new CmzIcon(this->_app->getInstance(), iconsDir + "default_icon_small.ico");
                }
                if (!this->appIcon) {
                    this->appIcon = new CmzIcon(this->_app->getInstance(), iconsDir + "default_icon_normal.ico");
                }
            }
        }
    }
}

VOID CResource::unload() {
    if (this->appIcon) {
        delete this->appIcon;
        this->appIcon = NULL;
    }
    if (this->appSmallIcon) {
        delete this->appSmallIcon;
        this->appSmallIcon = NULL;
    }
}
