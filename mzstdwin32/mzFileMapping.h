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

#ifndef __MZSTDWIN32_FILE_MAPPING_H__
#define __MZSTDWIN32_FILE_MAPPING_H__

#include "mzstdwin32Common.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzArrayList.h"
#include "mzStringList.h"


// domyślny rozmiar obiektu mapującego to 512kB
#define CFILEMAPPING_DEFAULT_VIEW_MAPPING_SIZE              524288


class DECLSPEC_MZSTDWIN32 CmzMappedFile : public mzstd::CmzObject {
public:
    CmzMappedFile(HANDLE handle, mzstd::CmzString &filename)
        : handle(handle), filename(filename)
    {
        this->setRealClassName(L"CmzMappedFile");
    };

    HANDLE handle;
    mzstd::CmzString filename;
};
typedef CmzMappedFile* PMappedFile;


class DECLSPEC_MZSTDWIN32 CFileMappingView : mzstd::CmzObject {
public:
    CFileMappingView(LPVOID mapAddress, LPVOID requestedData, DWORD mappedViewSize, DWORD requestedMappedViewSize, DWORD mapOffset, DWORD requestedOffset)
        : mapAddress(mapAddress), requestedData(requestedData), mappedViewSize(mappedViewSize), requestedMappedViewSize(requestedMappedViewSize), mapOffset(mapOffset), requestedOffset(requestedOffset)
    {
        this->setRealClassName(L"CFileMappingView");
    };

    // Wskaźnik na obszar pamięci zmapowanego fragmentu obiektu FileMapping.
    LPVOID mapAddress;
    // Wskaźnik na obszar pamięci zmapowanego fragmentu obiektu FileMapping, o który zażądano.
    // Różnica pomiędzy tym wskaźnikiem a wskaźnikiem mapAddress wynika z tego, że mapowane
    // obszary pliku muszą się zaczynać od offsetów będących wielokrotnością parametru systemowego:
    // System Allocation Granularity.
    // Wskaźnik ten jest równy:
    //  this->mapAddress + (this->requestedOffset - this->mapOffset)
    LPVOID requestedData;

    // Rozmiar zaalokowanego bloku pamięci przydzielonej na zmapowany fragment pliku.
    DWORD  mappedViewSize;
    // Żądany rozmiar zaalokowanego bloku pamięci przydzielonej na zmapowany fragment pliku.
    DWORD  requestedMappedViewSize;

    // Offset względem początku pliku dla zmapowanego fragmentu pliku.
    DWORD mapOffset;
    // Żądany offset względem początku pliku dla zmapowanego fragmentu pliku.
    DWORD requestedOffset;
};
typedef CFileMappingView* PFileMappingView;


class DECLSPEC_MZSTDWIN32 CFileMapping : mzstd::CmzObject {
public:
    WINAPI CFileMapping(mzstd::CmzString fname, mzstd::CmzString name = L"",
        DWORD dwDesiredAccess = GENERIC_READ,
        LPSECURITY_ATTRIBUTES lpAttributes = NULL,
        DWORD dwCreationDisposition = OPEN_EXISTING,
        DWORD mappingSizeHigh = 0,
        DWORD mappingSizeLow = 0);

    WINAPI ~CFileMapping();
    VOID WINAPI close();

    // ===========================================================================================
    // Poniższa tablica ma zagwarantować, że metoda CreateFile otwierająca fizycznie plik
    // nie zostanie wywołana dwa razy na tym samym pliku

    // Statyczna tablica zawierająca informacje o wszystkich otwartych plikach
    static mzstd::CmzArrayList* mappedFiles;
    // ===========================================================================================
    static VOID WINAPI initMappedFilesArray();
    static VOID WINAPI destroyMappedFilesArray();

    // zwraca uchwyt do odpowiedniego pliku
    HANDLE WINAPI getFileHandle(mzstd::CmzString fname);
    inline BOOL WINAPI opened() { return this->_fpMappingHandle != NULL; }
    LPVOID WINAPI mapFileFragmentToMemory(DWORD offset, DWORD size);

private:
    // blokujemy możliwość użycia konstruktora domyślnego
    WINAPI CFileMapping() {}

    // nazwa pliku otwartego przy tworzeniu obiektu tej klasy
    mzstd::CmzString _fname;
    mzstd::CmzString _name;

    // Uchwyt do pliku o nazwie podanej przy tworzeniu obiektu tej klasy
    HANDLE _fp;
    // uchwyt do obiektu FileMapping dla pliku o uchwycie w atrybucie: this->_fp
    HANDLE _fpMappingHandle;
    DWORD _desiredAccess;
    DWORD _creationDisposition;
    LPSECURITY_ATTRIBUTES _securityAttributes;
    // określa starsze DWORD rozmiaru obiektu FileMapping
    DWORD _mappingSizeHigh;
    // określa młodsze DWORD rozmiaru obiektu FileMapping
    DWORD _mappingSizeLow;
    // System allocation granularity
    DWORD _allocationGranularity;
    // Tablica wskaźników na zmapowane fragmenty plików.
    mzstd::CmzArrayList* _mapViews;

    // tworzy obiekt CFileMapping, zwraca TRUE, jeśli obiekt ten został poprawnie stworzony
    BOOL WINAPI _create(mzstd::CmzString fname, mzstd::CmzString name, DWORD dwDesiredAccess, LPSECURITY_ATTRIBUTES lpAttributes, DWORD dwCreationDisposition, DWORD mappingSizeHigh, DWORD mappingSizeLow);
    // zamyka wszystkie zmapowane widoki i zwalnia przydzieloną im pamięć
    VOID WINAPI _closeAllOpenedMappedViews();
    // zwraca TRUE, jeśli podany plik został już gdzieś otwarty w tej aplikacji
    inline BOOL WINAPI _isFileOpened(mzstd::CmzString fname) { return this->getFileHandle(fname) != NULL; }
};
typedef CFileMapping* PFileMapping;


#endif
