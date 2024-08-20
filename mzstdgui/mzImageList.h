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

#ifndef __MZSTDGUI_IMAGE_LIST_H__
#define __MZSTDGUI_IMAGE_LIST_H__

#include "mzstdguiCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzStringList.h"

#define CmzImageList_BITMAP_CONTAINER_WIDTH       4096L

// Ta klasa przechowuje obrazki o tym samym rozmiarze.
class DECLSPEC_MZSTDGUI CmzImageList : public mzstd::CmzObject {
public:
    // tablica bitmap przechowujących kolejne obrazki
    HBITMAP* hBitmaps;
    // ilość elementów w tablicy: hBitmaps
    LONG numBitmaps;
    // lista nazw plików obrazków kolejno ładowanych do elementów tablicy hBitmaps, tzn.
    // fnames->getAt(123456) = "image.bmp" oznacza, że obrazek o numerze 123455 (liczymy od 0)
    // znajdujący się w bitmapie hBitmaps[123456/imagesPerContainer] na pozycji
    // 123456 % imagesPerContainer został załadowany z pliku this->dir + "\\image.bmp"
    mzstd::CmzStringList *fnames;
    // Tablica kolorów transparentnych kolejnych wczytanych obrazków.
    // transColors[10] = RGB oznacza że RGB jest kolorem transparentnym dla obrazka
    // fnames->getAt(10)
    COLORREF *transColors;
    // ilość wszystkich załadowanych obrazków
    LONG count;

    // Wymiary każdego obrazka przechowywanego przez tą listę
    LONG imWidth;
    LONG imHeight;

    // Przechowuje ścieżkę do katalogu, z którego nastąpiło ostatnie załadowanie listy
    // obrazków.
    mzstd::CmzString dir;

    WINAPI CmzImageList();
    virtual WINAPI ~CmzImageList();
    // czyści listę i zwalnia wszystkie zaalokowane zasoby
    VOID WINAPI clear();
    // ładuje obrazki z podanego katalogu, jeśli podano kolor transparentny, to będzie
    // on wspólny dla wszystkich załadowanych bitmap, jeśli nie, to odpowiednie kolory
    // transparentne zostaną pobrane z każdego z wczytanych obrazków
    VOID WINAPI load(mzstd::CmzString dir, COLORREF transColor = NULL);

    // Dodaje w ostatniej bitmapie-pojemniku, na pozycji pos podaną bitmapę.
    // Jeśli bitmapa na tej pozycji w tym pojemniku się nie zmieści, to zwiększa liczbę
    // pojemników i dodaje tę bitmapę do nowego ostatnio utworzonego pojemnika.
    VOID WINAPI addImage(HBITMAP hBitmap, mzstd::CmzString fname, ULONG pos);

    VOID WINAPI bitblt(HDC hdc, LONG x = 0, LONG y = 0, LONG imageIndex = 0);
    VOID WINAPI bitblt(HDC hdc, LONG x, LONG y, mzstd::CmzString fname);
    VOID WINAPI transparentBitblt(HDC hdc, LONG x = 0, LONG y = 0, LONG imageIndex = 0);
    VOID WINAPI transparentBitblt(HDC hdc, LONG x, LONG y, mzstd::CmzString fname);

    // zapisuje wszystkie przechowywane obrazki do pliku o podanej nazwie. Jeśli pojemników
    // na obrazki jest więcej niż jeden to tworzone są pliki: fname_01.bmp, fname_02.bmp, ...
    // po jednym dla każdego pojemnika.
    VOID WINAPI saveToFile(mzstd::CmzString fname);

    LONG WINAPI getIndexForImageName(mzstd::CmzString fname);
    inline mzstd::CmzString WINAPI getImageNameForIndex(ULONG index) { return this->fnames->getAt(index); }

protected:
private:
    // Zwraca bitmapę zawierającą obszar zajmowany przez obrazki w danym pojemniku
    // Uwaga!!! Bitmapę tą należy zwolnić przy pomocy funkcji DeleteObject().
    HBITMAP WINAPI _getImagePartOfContainer(ULONG i);
};

typedef CmzImageList* PImageList;

#endif
