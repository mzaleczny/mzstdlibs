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
#include "mzstdwin32Common.h"
#include "mzFileMapping.h"
#include "mzFileUtils.h"
#include "mzSystemParameters.h"
#include "mzMessageBox.h"
using namespace mzstd;
// ================================================================================================
// Atrybuty statyczne
mzstd::CmzArrayList* CFileMapping::mappedFiles = NULL;

// Metody statyczne klasy
VOID CFileMapping::initMappedFilesArray() {
    CFileMapping::mappedFiles = new CmzArrayList(10);
}

VOID CFileMapping::destroyMappedFilesArray() {
    CmzMappedFile* mf;

    for (ULONGLONG i = 0; i < CFileMapping::mappedFiles->size(); i++) {
        mf = (CmzMappedFile*)CFileMapping::mappedFiles->getAt(i);
        if (mf) {
            if (mf->handle) {
                CloseHandle(mf->handle);
            }
            delete mf;
        }
    }
    delete CFileMapping::mappedFiles;
    CFileMapping::mappedFiles = NULL;
}
// ================================================================================================

CFileMapping::CFileMapping(CmzString fname, CmzString name, DWORD dwDesiredAccess, LPSECURITY_ATTRIBUTES lpAttributes, DWORD dwCreationDisposition, DWORD mappingSizeHigh, DWORD mappingSizeLow) {
    this->setRealClassName(L"CFileMapping");
    this->_fp = NULL;
    this->_fpMappingHandle = NULL;
    this->_fname = L"";
    this->_desiredAccess = 0;
    this->_creationDisposition = 0;
    this->_securityAttributes = NULL;
    this->_name = L"";
    this->_mappingSizeHigh = 0;
    this->_mappingSizeLow = 0;
    this->_mapViews = new CmzArrayList();
    this->_allocationGranularity = GetAllocationGranularity();

    if (!CFileMapping::mappedFiles) {
        CFileMapping::initMappedFilesArray();
    }

    // jeśli rozmiar pliku == 0, to nic nie robimy bo mapowanie się nie uda
    if (GetFileSize(fname) < 1) {
        return;
    }

    if (FileExists(fname)) {
        this->_create(fname, name, dwDesiredAccess, lpAttributes, dwCreationDisposition, mappingSizeHigh, mappingSizeLow);
    }
}

CFileMapping::~CFileMapping() {
    this->close();
    if (this->_mapViews) {
        delete this->_mapViews;
        this->_mapViews = NULL;
    }
}

BOOL CFileMapping::_create(CmzString fname, CmzString name, DWORD dwDesiredAccess, LPSECURITY_ATTRIBUTES lpAttributes, DWORD dwCreationDisposition, DWORD mappingSizeHigh, DWORD mappingSizeLow) {
    DWORD shareModeExclusive = 0;
    HANDLE fp = this->getFileHandle(fname);
    BOOL fileOpenedHere = FALSE;

    if (!fp) {
        fp = CreateFile(fname.getBuffer(), dwDesiredAccess, shareModeExclusive, lpAttributes, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	    if (fp == INVALID_HANDLE_VALUE) {
            ShowLastSystemError();
            return FALSE;
	    }
        fileOpenedHere = TRUE;
    }

    // teraz tworzymy obiekt FileMapping
    DWORD flProtect;
    if ((dwDesiredAccess & GENERIC_READ) == GENERIC_READ) {
        flProtect = PAGE_READONLY;
    } else if ((dwDesiredAccess & GENERIC_WRITE) == GENERIC_WRITE) {
        flProtect = PAGE_WRITECOPY;
    } else {
        flProtect = PAGE_READWRITE;
    }

    WCHAR* fileMappingObjectName;
    if (name.length() > 0) {
        fileMappingObjectName = name.getBuffer();
    } else {
        fileMappingObjectName = NULL;
    }
    HANDLE fpMappingHandle = CreateFileMapping(fp, lpAttributes, flProtect, mappingSizeHigh, mappingSizeLow, fileMappingObjectName);
    if (!fpMappingHandle) {
        if (fileOpenedHere) {
            CloseHandle(fp);
        }
        ShowLastSystemError();
        return FALSE;
    }

    CmzMappedFile* mf = new CmzMappedFile(fp, fname);
    CFileMapping::mappedFiles->push((LONGLONG)mf);

    this->_fname = fname;
    this->_name = name;
    this->_fp = fp;
    this->_fpMappingHandle = fpMappingHandle;
    this->_desiredAccess = dwDesiredAccess;
    this->_securityAttributes = lpAttributes;
    this->_creationDisposition = dwCreationDisposition;
    this->_mappingSizeHigh = mappingSizeHigh;
    this->_mappingSizeLow = mappingSizeLow;

    return TRUE;
}

HANDLE CFileMapping::getFileHandle(CmzString fname) {
    CmzMappedFile* mf;
    for (ULONGLONG i = 0; i < CFileMapping::mappedFiles->size(); i++) {
        mf = (CmzMappedFile*)CFileMapping::mappedFiles->getAt(i);
        if (mf && mf->filename == fname) {
            return mf->handle;
        }
    }

    return NULL;
}

VOID CFileMapping::_closeAllOpenedMappedViews() {
    CFileMappingView* fmv;

    // Najpierw zamykamy/zwalniamy wszystkie zmapowane widoki
    for (ULONGLONG i = 0; i < this->_mapViews->size(); i++) {
        fmv = (CFileMappingView*)this->_mapViews->getAt(i);
        if (fmv) {
            UnmapViewOfFile(fmv->mapAddress);
            delete fmv;
        }
    }
    this->_mapViews->clear();
}

VOID CFileMapping::close() {
    this->_closeAllOpenedMappedViews();

    // teraz zamykamy uchwyt do obiektu FileMapping i File
    this->_fname = L"";
    if (this->_fp) {
        BOOL bFlag = CloseHandle(this->_fpMappingHandle);
        this->_fpMappingHandle = NULL;
        // jeśli obiekt FileMapping został poprawnie zamknięty, to zamykamy też skojarzony z nim plik
        if (bFlag != 0) {
            bFlag = CloseHandle(this->_fp);
            // jeśli poprawnie zamknięto plik, to usuwamy go z globalnej tablicy otwartych plików
            if (bFlag != 0) {
                CmzMappedFile* mf;
                for (ULONGLONG i = 0; i < CFileMapping::mappedFiles->size(); i++) {
                    mf = (CmzMappedFile*)CFileMapping::mappedFiles->getAt(i);
                    if (mf && mf->handle == this->_fp) {
                        delete mf;
                        CFileMapping::mappedFiles->delAt(i);
                        break;
                    }
                }
            }
            this->_fp = NULL;
        }
    }
}

LPVOID CFileMapping::mapFileFragmentToMemory(DWORD offset, DWORD size) {
    // wyznaczamy najbliższy dozwolony adres mapowania
    DWORD fileMapStart = (offset / this->_allocationGranularity) * this->_allocationGranularity;
    DWORD mapViewSize = (offset % this->_allocationGranularity) + size;
    DWORD deltaOffset = offset - fileMapStart;
    DWORD desiredAccess;
    if ((this->_desiredAccess & GENERIC_READ) == GENERIC_READ) {
        desiredAccess = FILE_MAP_READ;
    } else if ((this->_desiredAccess & GENERIC_WRITE) == GENERIC_WRITE) {
        desiredAccess = FILE_MAP_WRITE;
    } else {
        desiredAccess = FILE_MAP_ALL_ACCESS;
    }
    LPVOID mapAddress = MapViewOfFile(this->_fpMappingHandle, desiredAccess, 0, fileMapStart, mapViewSize);
    if (mapAddress) {
        LPVOID requestedData = (CHAR*)mapAddress + deltaOffset;
        CFileMappingView* fmv = new CFileMappingView(mapAddress, requestedData, mapViewSize, size, fileMapStart, offset); 
        this->_mapViews->append((LONGLONG)fmv);
        return requestedData;
    }

    return NULL;
}
