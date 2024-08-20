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
#include "mzImageList.h"
#include "mzstdguiGlobals.h"
#include "mzGraphicsUtils.h"
#include "mzStringList.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"
using namespace mzstd;

CmzImageList::CmzImageList() {
    this->setRealClassName(L"CmzImageList");
    this->hBitmaps = NULL;
    this->numBitmaps = 0;
    this->fnames = NULL;
    this->transColors = NULL;
    this->count = 0;
    this->imWidth = 0;
    this->imHeight = 0;
    this->dir = L"";
}

CmzImageList::~CmzImageList() {
    this->clear();
}

VOID CmzImageList::clear() {
    if (this->numBitmaps > 0) {
        for (LONG i = 0; i < this->numBitmaps; i++) {
            DeleteObject(this->hBitmaps[i]);
            this->hBitmaps[i] = NULL;
        }
        delete[] this->hBitmaps;
        delete this->fnames;
        if (this->transColors) {
            delete[] this->transColors;
        }
        this->hBitmaps = NULL;
        this->fnames = NULL;
        this->transColors = NULL;
        this->numBitmaps = 0;
        this->count = 0;
    }
    this->dir = L"";
}

VOID CmzImageList::load(CmzString dir, COLORREF transColor) {
    if (!DirectoryExists(dir)) {
        return;
    }

    // czyścimy dotychczasową zawartośc listy
    this->clear();

    this->dir = dir;
    // najpierw pobieramy wszystkie bitmapy w podanym katalogu
    CmzStringList *lstBitmaps = GetDirectoryFiles(dir, L"bmp");

    if (lstBitmaps) {
        if (lstBitmaps->size() > 0) {
            BITMAP bm;
            HDC hdc;
            HBITMAP hOrigBmp;
            LONG retval;
            LONG bmWidth, bmHeight;
            LONG curImage;
            // kolor transparentny dla obrazka
            COLORREF tclr;

            this->numBitmaps = 1;
            this->hBitmaps = new HBITMAP[this->numBitmaps];
            this->hBitmaps[0] = NULL;
            this->fnames = new CmzStringList();

            // teraz pobieramy rozmiar pierwszej z nich i dodajemy do listy następne o takim samym
            // rozmiarze
            HBITMAP hBitmap;
            curImage = 0;
            for (ULONG i = 0; i < lstBitmaps->size(); i++) {
                hBitmap = (HBITMAP)LoadImage(NULL, (this->dir + L'\\' + lstBitmaps->getAt(i)).getBuffer(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if (hBitmap) {
                    retval = GetObject(hBitmap, sizeof(bm), &bm);
                    if (retval) {
                        bmWidth = bm.bmWidth;
                        bmHeight = bm.bmHeight;
                        // jeśli wymiary bitmap nie były jeszcze zainicjowane, to je inicjujemy
                        if (this->imWidth == 0 && this->imHeight == 0) {
                            this->imWidth = bmWidth;
                            this->imHeight = bmHeight;
                            // tutaj dopiero tworzymy pierwszą bitmapę, bo dopiero tutaj
                            // znamy wymiary pierwszego obrazka
                            if (!this->hBitmaps[0]) {
                                hdc = GetDC(NULL);
                                this->hBitmaps[0] = CreateCompatibleBitmap(hdc, CmzImageList_BITMAP_CONTAINER_WIDTH, this->imHeight);
                                ReleaseDC(NULL, hdc);
                            }
                        }

                        // bierzemy pod uwagę tylko obrazki o rozmiarze takim jak pierwszy
                        if (this->imWidth == bmWidth && this->imHeight == bmHeight) {
                            this->addImage(hBitmap, lstBitmaps->getAt(i), curImage);
                            curImage++;
                        }
                    }
                    DeleteObject(hBitmap);
                }
            }

            this->count = static_cast<LONG>(this->fnames->size());
            // jeśli wczytano jakieś obrazki, to ustawiamy odpowiednie kolory transparentne
            if (this->count > 0) {
                this->transColors = new COLORREF[this->count];

                LONG x;
                LONG imagesPerContainer = CmzImageList_BITMAP_CONTAINER_WIDTH / this->imWidth;
                hdc = CreateCompatibleDC(NULL);
                hOrigBmp = (HBITMAP)SelectObject(hdc, this->hBitmaps[0]);

                // przechodzimy kolejno przez wszystkie pojemniki
                LONG counter = 0;
                for (LONG i = 0; i < this->numBitmaps; i++) {
                    if (i % imagesPerContainer == 0) {
                        SelectObject(hdc, this->hBitmaps[i]);
                    }

                    // przechodzimy kolejno przez wszystkie obrazki aktualnego pojemnika
                    for (LONG pos = 0; pos < imagesPerContainer && counter < this->count; pos++) {
                        x = pos * this->imWidth;
                        if (transColor != NULL) {
                            tclr = transColor;
                        } else {
                            tclr = GetPixel(hdc, x, 0);
                        }
                        this->transColors[counter] = tclr;
                        counter++;
                    }
                }

                SelectObject(hdc, hOrigBmp);
                DeleteDC(hdc);
            }
        }
        delete lstBitmaps;
    }
}

VOID CmzImageList::addImage(HBITMAP hBitmap, CmzString fname, ULONG pos) {
    LONG x;
    HDC hdcDst, hdcSrc;
    HBITMAP hDstOrigBmp, hSrcOrigBmp;

    // obliczamy aktualną współrzędną x rysowania
    x = pos * this->imWidth;
    // jeśli bitmapa się nie mieści na ostatnim pojemniku, o dodajemy nowy
    if (x + this->imWidth > CmzImageList_BITMAP_CONTAINER_WIDTH) {
        LONG newNumBitmaps = this->numBitmaps + 1;
        HBITMAP *hBitmaps = new HBITMAP[newNumBitmaps];
        memcpy(hBitmaps, this->hBitmaps, this->numBitmaps * sizeof(HBITMAP*));
        delete[] this->hBitmaps;

        this->hBitmaps = hBitmaps;
        this->numBitmaps = newNumBitmaps;

        hdcDst = GetDC(NULL);
        this->hBitmaps[newNumBitmaps - 1] = CreateCompatibleBitmap(hdcDst, CmzImageList_BITMAP_CONTAINER_WIDTH, this->imHeight);
        ReleaseDC(NULL, hdcDst);

        pos = 0;
        x = pos * this->imWidth;
    }

    hdcDst = CreateCompatibleDC(0);
    hdcSrc = CreateCompatibleDC(0);

    hDstOrigBmp = (HBITMAP)SelectObject(hdcDst, this->hBitmaps[this->numBitmaps - 1]);
    hSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, hBitmap);
    BitBlt(hdcDst, x, 0, this->imWidth, this->imHeight, hdcSrc, 0, 0, SRCCOPY);
    SelectObject(hdcDst, hDstOrigBmp);
    SelectObject(hdcSrc, hSrcOrigBmp);

    DeleteDC(hdcDst);
    DeleteDC(hdcSrc);
    this->fnames->append(fname);
}

VOID CmzImageList::bitblt(HDC hdc, LONG x, LONG y, LONG imageIndex) {
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HBITMAP hSrcBmp;
    HBITMAP hSrcOrigBmp;
    LONG imagesPerContainer = CmzImageList_BITMAP_CONTAINER_WIDTH / this->imWidth;
    LONG containerNumber = imageIndex / imagesPerContainer;
    LONG posInContainer = imageIndex % imagesPerContainer;

    hSrcBmp = this->hBitmaps[containerNumber];
    hSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, hSrcBmp);

    BitBlt(hdc, x, y, this->imWidth, this->imHeight, hdcSrc, posInContainer * this->imWidth, 0, SRCCOPY);

    SelectObject(hdcSrc, hSrcOrigBmp);
    DeleteDC(hdcSrc);
}

VOID CmzImageList::bitblt(HDC hdc, LONG x, LONG y, CmzString fname) {
    // określamy pozycję obrazka o tej nazwie pliku
    for (LONG pos = 0; pos < this->count; pos++) {
        if (this->fnames->getAt(pos) == fname) {
            this->bitblt(hdc, x, y, pos);
        }
    }
}

VOID CmzImageList::transparentBitblt(HDC hdc, LONG x, LONG y, LONG imageIndex) {
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HBITMAP hSrcBmp;
    HBITMAP hSrcOrigBmp;
    LONG imagesPerContainer = CmzImageList_BITMAP_CONTAINER_WIDTH / this->imWidth;
    LONG containerNumber = imageIndex / imagesPerContainer;
    LONG posInContainer = imageIndex % imagesPerContainer;

    hSrcBmp = this->hBitmaps[containerNumber];
    hSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, hSrcBmp);

    COLORREF clrTrans = this->transColors[imageIndex];
    HBITMAP hMaskBmp = CreateBitmap(this->imWidth, this->imHeight, 1, 1, NULL);
    HDC hdcMask = CreateCompatibleDC(NULL);
    HBITMAP hdcMaskOrigBmp = (HBITMAP)SelectObject(hdcMask, hMaskBmp);
    HBITMAP hTmpSrcBmp = CreateCompatibleBitmap(hdcSrc, this->imWidth, this->imHeight);
    HDC hdcTmpSrc = CreateCompatibleDC(NULL);
    HBITMAP hdcTmpSrcOrigBmp = (HBITMAP)SelectObject(hdcTmpSrc, hTmpSrcBmp);

    BitBlt(hdcTmpSrc, 0, 0, this->imWidth, this->imHeight, hdcSrc, posInContainer * this->imWidth, 0, SRCCOPY);
    SelectObject(hdcSrc, hSrcOrigBmp);
    DeleteDC(hdcSrc);

    // Tworzymy maskę bitmapy źródłowej - na bitmapie maski każdy kolor bitmapy
    // źródłowej różny od koloru tła będzie czarny a kolor tła będzie biały.
    // Zapewniają to dwie poniższe funkcje.
    COLORREF origTmpSrcBkColor = SetBkColor(hdcTmpSrc, clrTrans);
    BitBlt(hdcMask, 0, 0, this->imWidth, this->imHeight, hdcTmpSrc, 0, 0, SRCCOPY);

    COLORREF origTmpSrcTextColor = SetTextColor(hdcTmpSrc, RGB(255,255,255));
    SetBkColor(hdcTmpSrc, RGB(0,0,0));
    BitBlt(hdcTmpSrc, 0, 0, this->imWidth, this->imHeight, hdcMask, 0, 0, SRCAND);
    SetTextColor(hdcTmpSrc, origTmpSrcTextColor);
    SetBkColor(hdcTmpSrc, origTmpSrcBkColor);

    BitBlt(hdc, x, y, this->imWidth, this->imHeight, hdcMask, 0, 0, SRCAND);
    BitBlt(hdc, x, y, this->imWidth, this->imHeight, hdcTmpSrc, 0, 0, SRCPAINT);

    SelectObject(hdcMask, hdcMaskOrigBmp);
    SelectObject(hdcTmpSrc, hdcTmpSrcOrigBmp);
    DeleteDC(hdcMask);
    DeleteDC(hdcTmpSrc);
    DeleteObject(hMaskBmp);
    DeleteObject(hTmpSrcBmp);
}

VOID CmzImageList::transparentBitblt(HDC hdc, LONG x, LONG y, CmzString fname) {
    // określamy pozycję obrazka o tej nazwie pliku
    for (LONG pos = 0; pos < this->count; pos++) {
        if (this->fnames->getAt(pos) == fname) {
            this->transparentBitblt(hdc, x, y, pos);
        }
    }
}

HBITMAP CmzImageList::_getImagePartOfContainer(ULONG i) {
    if (i < (ULONG)this->numBitmaps) {
        LONG imagesPerContainer = CmzImageList_BITMAP_CONTAINER_WIDTH / this->imWidth;
        LONG imageCount;
        if ((LONG)i < this->numBitmaps - 1) {
            imageCount = imagesPerContainer;
        } else {
            imageCount = this->count % imagesPerContainer;
        }
        HBITMAP hSrcBmp = this->hBitmaps[i];
        HBITMAP hSrcOrigBmp;
        HBITMAP hDstOrigBmp;
        HDC hdcSrc = CreateCompatibleDC(NULL);
        HDC hdcDst = CreateCompatibleDC(NULL);
        HDC hdcScreenDC = GetDC(0);
        HBITMAP hDstBmp = CreateCompatibleBitmap(hdcScreenDC, imageCount * this->imWidth, this->imHeight);
        ReleaseDC(0, hdcScreenDC);

        hSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, hSrcBmp);
        hDstOrigBmp = (HBITMAP)SelectObject(hdcDst, hDstBmp);

        BitBlt(hdcDst, 0, 0, imageCount * this->imWidth, this->imHeight, hdcSrc, 0, 0, SRCCOPY);

        SelectObject(hdcSrc, hSrcOrigBmp);
        SelectObject(hdcDst, hDstOrigBmp);
        DeleteDC(hdcSrc);
        DeleteDC(hdcDst);

        return hDstBmp;
    }
    return NULL;
}

VOID CmzImageList::saveToFile(CmzString fname) {
    HBITMAP hTmpBmp;

    if (this->numBitmaps == 1) {
        hTmpBmp = this->_getImagePartOfContainer(0);
        ::SaveBitmapToFile(fname, hTmpBmp);
        DeleteObject(hTmpBmp);
    } else {
        CmzString num;
        CmzString baseFileName = fname;
        CmzString ext = fname.strAfterLast(L'.');
        baseFileName.cutAtLast(L'.');

        for (LONG i = 0; i < this->numBitmaps; i++) {
            if (i < 9) {
                num = CmzString(L"0") + i;
            } else {
                num = i;
            }

            hTmpBmp = this->_getImagePartOfContainer(i);
            ::SaveBitmapToFile(baseFileName + L"_" + num + L"." + ext, hTmpBmp);
            DeleteObject(hTmpBmp);
        }
    }
}

LONG CmzImageList::getIndexForImageName(CmzString fname) {
    // określamy pozycję obrazka o tej nazwie pliku
    for (LONG pos = 0; pos < this->count; pos++) {
        if (this->fnames->getAt(pos) == fname) {
            return pos;
        }
    }

    return -1;
}
