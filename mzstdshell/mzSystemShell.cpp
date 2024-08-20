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

#include "pch.h"
#include "mzSystemShell.h"
#include <shlobj.h>
#include <shellapi.h>
#include "mzFileUtils.h"
using namespace mzstd;

WINAPI CSystemShell::CSystemShell() {
    this->setRealClassName(L"CSystemShell");
}

WINAPI CSystemShell::~CSystemShell() {
}


CmzString CSystemShell::GetShellSystemFolder(DWORD foldertype, BOOL current, BOOL verify, BOOL create) {
    WCHAR path[MAX_PATH];
    DWORD folder_flag = 0;
    DWORD flag = 0;
    
    folder_flag |= CSIDL_FLAG_DONT_UNEXPAND;
    if (!verify) {
        folder_flag |= CSIDL_FLAG_DONT_VERIFY;
    }
    if (create) {
        folder_flag |= CSIDL_FLAG_CREATE;
    }
    
    if (current) {
        flag = SHGFP_TYPE_CURRENT;
    } else {
        flag = SHGFP_TYPE_DEFAULT;
    }
    
    if (SUCCEEDED(SHGetFolderPath(NULL, foldertype | folder_flag, NULL, flag, path))) {
        return path;
    }
    
    return COMMON_EMPTY_STRING;
}

CmzString CSystemShell::GetAdminToolsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_ADMINTOOLS, current, verify, create);
}

CmzString CSystemShell::GetAltStartupFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_ALTSTARTUP, current, verify, create);
}

CmzString CSystemShell::GetAppDataFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_APPDATA, current, verify, create);
}

CmzString CSystemShell::GetRecycleBinFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_BITBUCKET, current, verify, create);
}

CmzString CSystemShell::GetCDBurnAreaFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_CDBURN_AREA, current, verify, create);
}

CmzString CSystemShell::GetCommonAdminToolsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_ADMINTOOLS, current, verify, create);
}

CmzString CSystemShell::GetCommonAltStartupFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_ALTSTARTUP, current, verify, create);
}

CmzString CSystemShell::GetCommonAppDataFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_APPDATA, current, verify, create);
}

CmzString CSystemShell::GetCommonDesktopFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_DESKTOPDIRECTORY, current, verify, create);
}

CmzString CSystemShell::GetCommonDocumentsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_DOCUMENTS, current, verify, create);
}

CmzString CSystemShell::GetCommonFavoritesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_FAVORITES, current, verify, create);
}

CmzString CSystemShell::GetCommonMusicFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_MUSIC, current, verify, create);
}

CmzString CSystemShell::GetCommonOEMLinksFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_OEM_LINKS, current, verify, create);
}

CmzString CSystemShell::GetCommonPicturesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_PICTURES, current, verify, create);
}

CmzString CSystemShell::GetCommonProgramsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_PROGRAMS, current, verify, create);
}

CmzString CSystemShell::GetCommonStartMenuFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_STARTMENU, current, verify, create);
}

CmzString CSystemShell::GetCommonStartupFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_STARTUP, current, verify, create);
}

CmzString CSystemShell::GetCommonTemplatesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_TEMPLATES, current, verify, create);
}

CmzString CSystemShell::GetCommonVideoFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMMON_VIDEO, current, verify, create);
}

CmzString CSystemShell::GetNetworkFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COMPUTERSNEARME, current, verify, create);
}

CmzString CSystemShell::GetConnectionsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_CONNECTIONS, current, verify, create);
}

CmzString CSystemShell::GetControlPanelFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_CONTROLS, current, verify, create);
}

CmzString CSystemShell::GetCookiesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_COOKIES, current, verify, create);
}

CmzString CSystemShell::GetDesktopVirtualFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_DESKTOP, current, verify, create);
}

CmzString CSystemShell::GetDesktopFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_DESKTOPDIRECTORY, current, verify, create);
}

CmzString CSystemShell::GetComputerFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_DRIVES, current, verify, create);
}

CmzString CSystemShell::GetFavoritesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_FAVORITES, current, verify, create);
}

CmzString CSystemShell::GetFontsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_FONTS, current, verify, create);
}

CmzString CSystemShell::GetHistoryFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_HISTORY, current, verify, create);
}

CmzString CSystemShell::GetInternetFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_INTERNET, current, verify, create);
}

CmzString CSystemShell::GetInternetCacheFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_INTERNET_CACHE, current, verify, create);
}

CmzString CSystemShell::GetLocalAppDataFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_LOCAL_APPDATA, current, verify, create);
}

CmzString CSystemShell::GetMyDocumentsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_MYDOCUMENTS, current, verify, create);
}

CmzString CSystemShell::GetMyMusicFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_MYMUSIC, current, verify, create);
}

CmzString CSystemShell::GetMyPicturesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_MYPICTURES, current, verify, create);
}

CmzString CSystemShell::GetMyVideoFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_MYVIDEO, current, verify, create);
}

CmzString CSystemShell::GetNetHoodFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_NETHOOD, current, verify, create);
}

CmzString CSystemShell::GetNetworkNeighborhoodFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_NETWORK, current, verify, create);
}

CmzString CSystemShell::GetPrintersFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PRINTERS, current, verify, create);
}

CmzString CSystemShell::GetPrintHoodFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PRINTHOOD, current, verify, create);
}

CmzString CSystemShell::GetProfileFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PROFILE, current, verify, create);
}

CmzString CSystemShell::GetProgramFilesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PROGRAM_FILES, current, verify, create);
}

CmzString CSystemShell::GetProgramFilesX86Folder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PROGRAM_FILESX86, current, verify, create);
}

CmzString CSystemShell::GetProgramFilesCommonFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PROGRAM_FILES_COMMON, current, verify, create);
}

CmzString CSystemShell::GetProgramFilesCommonX86Folder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PROGRAM_FILES_COMMONX86, current, verify, create);
}

CmzString CSystemShell::GetProgramsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_PROGRAMS, current, verify, create);
}

CmzString CSystemShell::GetRecentFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_RECENT, current, verify, create);
}

CmzString CSystemShell::GetResourcesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_RESOURCES, current, verify, create);
}

CmzString CSystemShell::GetLocalizedResourcesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_RESOURCES_LOCALIZED, current, verify, create);
}

CmzString CSystemShell::GetSendtoFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_SENDTO, current, verify, create);
}

CmzString CSystemShell::GetStartMenuFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_STARTMENU, current, verify, create);
}

CmzString CSystemShell::GetStartupFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_STARTUP, current, verify, create);
}

CmzString CSystemShell::GetSystemFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_SYSTEM, current, verify, create);
}

CmzString CSystemShell::GetSystemX86Folder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_SYSTEMX86, current, verify, create);
}

CmzString CSystemShell::GetTemplatesFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_TEMPLATES, current, verify, create);
}

CmzString CSystemShell::GetWindowsFolder(BOOL current, BOOL verify, BOOL create) {
    return CSystemShell::GetShellSystemFolder(CSIDL_WINDOWS, current, verify, create);
}

CmzString CSystemShell::GetTempFolder(BOOL current, BOOL verify, BOOL create) {
    CmzString tmpDir = GetParentDir(CSystemShell::GetLocalAppDataFolder(current, verify, create)) + L"\\Temp";
    if (create && !DirectoryExists(tmpDir)) {
        CreateDirectory(tmpDir, TRUE);
    }
    return tmpDir;
}

HINSTANCE CSystemShell::Execute(HWND hwnd, CmzString filename, CmzString parameters, CmzString homeDir, LONG showCmd) {
    return ShellExecute(hwnd, L"open", filename.getBuffer(), parameters.getBuffer(), homeDir.getBuffer(), showCmd);
}

CmzString CSystemShell::BrowseForFolder(CmzString rootFolder) {
    BROWSEINFO binfo;
    ZeroMemory(&binfo, sizeof(binfo));
    binfo.hwndOwner = GetActiveWindow();
    binfo.pszDisplayName = L"Display name";
    binfo.lpszTitle = L"Title";
    SHBrowseForFolder(&binfo);
    
    return L"";
}
