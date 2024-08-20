/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdwin32
 * Version: 1.05
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdwin32 library
 * 
 * mzstdwin32 library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdwin32 library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */
#include "pch.h"
#include "mzStdDialogs.h"
#include "mzMessageBox.h"
#include <CdErr.h>
#include "mzStringList.h"
#include "mzLogger.h"
#include "mzMemoryView.h"

using namespace mzstd;

OPENFILENAME* WINAPI CreateOpenFileNameStruct(WCHAR* caption, CmzString initialDir, CmzString filter, CmzString defExt, DWORD filterIndex) {
    CmzStringList* filterList = new CmzStringList();
    if (!filterList) {
        return NULL;
    }

    if (filter != L"") {
        CmzStringList* lst = filter.explode(L';');
        if (lst) {
            CmzString item;
            CmzStringList* itemList = NULL;
            for (ULONGLONG i = 0; i < lst->size(); i++) {
                item = lst->getAt(i);
                item.trim();
                if (item.length() > 0) {
                    CmzString description = item;
                    item.replace(L" (", L'|');
                    item.remove(L')');
                    itemList = item.explode(L'|');
                    if (itemList) {
                        if (itemList->size() >= 2) {
                            filterList->append(description);
                            item = itemList->getAt(1); // filter pattern
                            filterList->append(item);
                        }
                        delete itemList;
                    }
                }
            }
            delete lst;
        }
    }
        
    CmzString filterStr;
    if (filterList->size() > 0) {
        filterList->joinIntoStringArray(&filterStr);
    } else {
        filterStr = L"";
        filterStr.appendAfterEos(L"All files");
        filterStr.appendAfterEos(L"*.*");
    }
    delete filterList;

    OPENFILENAME* ofn = new OPENFILENAME;
    if (ofn) {
        ZeroMemory(ofn, sizeof(*ofn));
        ofn->lStructSize = sizeof(*ofn);
        ofn->hwndOwner = GetActiveWindow();
        ofn->hInstance = AfxGetInstanceHandle();
        ofn->lpstrCustomFilter = NULL;
        ofn->lpstrFilter = new WCHAR[filterStr.getBufferSize()];
        if (!ofn->lpstrFilter) {
            delete ofn;
            return NULL;
        }
        CopyMemory((VOID*)ofn->lpstrFilter, (VOID*)filterStr.getBuffer(), filterStr.getBufferSize()*2);
        ofn->nMaxCustFilter = 0;
        ofn->nFilterIndex = filterIndex+1;
        ofn->lpstrFile = new WCHAR[MAX_PATH];
        if (!ofn->lpstrFile) {
            delete[] ofn->lpstrFilter;
            delete ofn;
            return NULL;
        }
        ofn->lpstrFile[0] = NULL;
        ofn->nMaxFile = MAX_PATH;
        ofn->lpstrFileTitle = NULL;
        ofn->nMaxFileTitle = 0;
        if (initialDir != COMMON_EMPTY_STRING) {
            ofn->lpstrInitialDir = new WCHAR[initialDir.length()+1];
            if (ofn->lpstrInitialDir) {
                wcscpy_s((WCHAR*)ofn->lpstrInitialDir, initialDir.length()+1, initialDir.getBuffer());
            }
        } else {
            ofn->lpstrInitialDir = NULL;
        }
        ofn->lpstrTitle = caption;
        ofn->Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
        ofn->nFileOffset = 0;
        ofn->nFileExtension = 0;
        if (defExt.length() > 0) {
            ofn->lpstrDefExt = new WCHAR[defExt.length()+1];
            if (ofn->lpstrDefExt) {
                wcscpy_s((WCHAR*)ofn->lpstrDefExt, defExt.length()+1, defExt.getBuffer());
            }
        }
        ofn->lCustData = NULL;
        ofn->lpfnHook = NULL;
        ofn->lpTemplateName = NULL;
    }

    return ofn;
}

VOID WINAPI DeleteOpenFileNameStruct(OPENFILENAME* ofn) {
    if (ofn) {
        if (ofn->lpstrFilter) {
            delete[] ofn->lpstrFilter;
        }
        if (ofn->lpstrFile) {
            delete[] ofn->lpstrFile;
        }
        if (ofn->lpstrInitialDir) {
            delete[] ofn->lpstrInitialDir;
        }
        if (ofn->lpstrDefExt) {
            delete[] ofn->lpstrDefExt;
        }
        delete ofn;
    }
}

CmzString WINAPI CheckIfDialogError() {
    switch (CommDlgExtendedError()) {
        case CDERR_DIALOGFAILURE:
            return L"CDERR_DIALOGFAILURE"; break;
        case CDERR_FINDRESFAILURE:
            return L"CDERR_FINDRESFAILURE"; break;
        case CDERR_NOHINSTANCE:
            return L"CDERR_NOHINSTANCE"; break;
        case CDERR_INITIALIZATION:
            return L"CDERR_INITIALIZATION"; break;
        case CDERR_NOHOOK:
            return L"CDERR_NOHOOK"; break;
        case CDERR_LOCKRESFAILURE:
            return L"CDERR_LOCKRESFAILURE"; break;
        case CDERR_NOTEMPLATE:
            return L"CDERR_NOTEMPLATE"; break;
        case CDERR_LOADRESFAILURE:
            return L"CDERR_LOADRESFAILURE"; break;
        case CDERR_STRUCTSIZE:
            return L"CDERR_STRUCTSIZE"; break;
        case CDERR_LOADSTRFAILURE:
            return L"CDERR_LOADSTRFAILURE"; break;
        case FNERR_BUFFERTOOSMALL:
            return L"FNERR_BUFFERTOOSMALL"; break;
        case CDERR_MEMALLOCFAILURE:
            return L"CDERR_MEMALLOCFAILURE"; break;
        case FNERR_INVALIDFILENAME:
            return L"FNERR_INVALIDFILENAME"; break;
        case CDERR_MEMLOCKFAILURE:
            return L"CDERR_MEMLOCKFAILURE"; break;
        case FNERR_SUBCLASSFAILURE:
            return L"FNERR_SUBCLASSFAILURE"; break;
        default:
            break;
    }

    return L"";
}

CmzString WINAPI FileOpenDlg(CmzString initialDir, CmzString filter, CmzString defExt, DWORD filterIndex) {
    OPENFILENAME* ofn = CreateOpenFileNameStruct(L"Open file", initialDir, filter, defExt, filterIndex);
    CmzString fname = COMMON_EMPTY_STRING;
    
    if (ofn) {
        if (GetOpenFileName(ofn)) {
            fname = ofn->lpstrFile;
        } else {
            CmzString msg = CheckIfDialogError();
            // if we have error
            if (msg != COMMON_EMPTY_STRING) {
                ErrorBox(msg);
            }
        }
        DeleteOpenFileNameStruct(ofn);
    }
    
    return fname;
}

CmzString WINAPI FileSaveDlg(CmzString initialDir, CmzString filter, CmzString defExt, DWORD filterIndex) {
    OPENFILENAME* ofn = CreateOpenFileNameStruct(L"Save file", initialDir, filter, defExt, filterIndex);
    CmzString fname = COMMON_EMPTY_STRING;
    
    if (ofn) {
        if (GetSaveFileName(ofn)) {
            fname = ofn->lpstrFile;
        } else {
            CmzString msg = CheckIfDialogError();
            // if we have error
            if (msg != COMMON_EMPTY_STRING) {
                ErrorBox(msg);
            }
        }
        DeleteOpenFileNameStruct(ofn);
    }
    
    return fname;
}
