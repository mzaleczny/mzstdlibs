/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdshell
 * Version: 1.04
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdshell library
 * 
 * mzstdshell library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdshell library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZSTDSHELL_SHELL_H__
#define __MZSTDSHELL_SHELL_H__

#include "mzstdshellCommon.h"
#include "mzObject.h"
#include "mzString.h"

class DECLSPEC_MZSTDSHELL CSystemShell : public mzstd::CmzObject {
public:
	WINAPI CSystemShell();
    WINAPI ~CSystemShell();
    
    static HINSTANCE Execute(HWND hwnd, mzstd::CmzString filename, mzstd::CmzString parameters, mzstd::CmzString homeDir, LONG showCmd = SW_SHOW);
    
    static mzstd::CmzString GetAdminToolsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetAltStartupFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetAppDataFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetRecycleBinFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCDBurnAreaFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonAdminToolsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonAltStartupFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonAppDataFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonDesktopFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonDocumentsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonFavoritesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonMusicFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonOEMLinksFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonPicturesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonProgramsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonStartMenuFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonStartupFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonTemplatesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCommonVideoFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetNetworkFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetConnectionsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetControlPanelFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetCookiesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetDesktopVirtualFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetDesktopFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetComputerFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetFavoritesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetFontsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetHistoryFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetInternetFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetInternetCacheFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetLocalAppDataFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetMyDocumentsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetMyMusicFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetMyPicturesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetMyVideoFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetNetHoodFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetNetworkNeighborhoodFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetPrintersFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetPrintHoodFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetProfileFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetProgramFilesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetProgramFilesX86Folder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetProgramFilesCommonFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetProgramFilesCommonX86Folder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetProgramsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetRecentFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetResourcesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetLocalizedResourcesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetSendtoFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetStartMenuFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetStartupFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetSystemFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetSystemX86Folder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetTemplatesFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetWindowsFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    static mzstd::CmzString GetTempFolder(BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    
    static mzstd::CmzString BrowseForFolder(mzstd::CmzString rootFolder);
    
private:
    static mzstd::CmzString GetShellSystemFolder(DWORD foldertype, BOOL current = TRUE, BOOL verify = FALSE, BOOL create = FALSE);
    
};
typedef CSystemShell* PSystemShell;

#endif
