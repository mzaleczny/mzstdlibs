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
#include "mzClipboard.h"
#include "mzMessageBox.h"
#include "mzArrayList.h"
using namespace mzstd;

CClipboard::CClipboard() {
    this->setRealClassName(L"CClipboard");
    this->_availableFormats = new CmzArrayList(10);
}

CClipboard::~CClipboard() {
    if (this->_availableFormats) {
        delete this->_availableFormats;
    }
}

VOID CClipboard::getClipboardFormatName(CmzString& name, UINT format) {
    INT result;
    INT buf_len = 255;
    WCHAR* formatName = new WCHAR[buf_len];
    result = GetClipboardFormatName(format, formatName, buf_len);

    if (result == 0) {
        name = L"";
    } else {
        name = formatName;
    }

    delete[] formatName;
}

BOOL CClipboard::copyTextToClipboard(HWND hwnd, const CmzString& s) {
    if (!hwnd) {
        return FALSE;
    }

    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    size_t str_len = s.length();

    // jeśli mamy pusty tekst, to czyścimy i zamykamy schowek
    if (str_len < 1) {
        this->emptyClipboard();
        CloseClipboard();
        return TRUE;
    }

    HGLOBAL hglbCopyUnicode = GlobalAlloc(GMEM_MOVEABLE, (str_len + 1) * sizeof(WCHAR));
    if (hglbCopyUnicode == NULL) {
        CloseClipboard();
        return FALSE;
    }
    HGLOBAL hglbCopyAnsi = GlobalAlloc(GMEM_MOVEABLE, (str_len + 1) * sizeof(CHAR));
    if (hglbCopyAnsi == NULL) {
        GlobalFree(hglbCopyUnicode);
        CloseClipboard();
        return FALSE;
    }

    // najpierw kopiujemy dane UNICODE
    LPVOID mem = GlobalLock(hglbCopyUnicode);
    // kopiujemy tekst razem ze znkiem NULL kończącym go
    memcpy(mem, s.getBuffer(), (str_len+1) * sizeof(WCHAR));
    GlobalUnlock(hglbCopyUnicode);

    // a następnie konwerujemy i kopiujemy dane ANSI
    mem = GlobalLock(hglbCopyAnsi);
    s.toAnsi((CHAR*)mem, (str_len+1) * sizeof(WCHAR));
    GlobalUnlock(hglbCopyAnsi);

    this->emptyClipboard();
    // umieszczamy dane (a właściwie uchwyt HGLOBAL na przydzielony obszar pamięci) w schowku
    SetClipboardData(CF_TEXT, hglbCopyAnsi);
    SetClipboardData(CF_UNICODETEXT, hglbCopyUnicode);
    CloseClipboard();

    return TRUE;
}

CmzString CClipboard::getTextFromClipboard(HWND hwnd) {
    if (!hwnd) {
        return L"";
    }

    if (!OpenClipboard(hwnd)) {
        return L"";
    }

    ULONGLONG clFormat = EnumClipboardFormats(0);
    if (!IsClipboardFormatAvailable(CF_TEXT) && !IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        return L"";
    }

    CmzString retval = L"";
    // W pierwszej kolejności próbujemy pobrać tekst UNICODE
    HGLOBAL hglb;

    hglb = GetClipboardData(CF_TEXT);
    if (hglb != NULL) {
        CHAR* str = (CHAR*)GlobalLock(hglb);
        if (str != NULL) {
            retval = str;
            GlobalUnlock(hglb);
            CloseClipboard();

            return retval;
        }
    }

    hglb = GetClipboardData(CF_UNICODETEXT);
    if (hglb != NULL) {
        WCHAR* str = (WCHAR*)GlobalLock(hglb);
        if (str != NULL) {
            retval = str;
            GlobalUnlock(hglb);
            CloseClipboard();

            return retval;
        }
    }

    CloseClipboard();
    return L"";
}

BOOL CClipboard::_getAllAvailableFormats(HWND hwnd) {
    this->_availableFormats->clear();
    if (!hwnd) {
        return FALSE;
    }

    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    UINT format = EnumClipboardFormats(0);
    while (format != 0) {
        this->_availableFormats->append(format);
        format = EnumClipboardFormats(format);
    }

    if (GetLastError() != ERROR_SUCCESS) {
        ShowLastSystemError();
    }

    CloseClipboard();
    return TRUE;
}

BOOL CClipboard::isClipboardFormatAvailable(HWND hwnd, UINT format) {
    this->_availableFormats->clear();
    if (!hwnd) {
        return FALSE;
    }

    if (!OpenClipboard(hwnd)) {
        return FALSE;
    }

    BOOL retval = IsClipboardFormatAvailable(format);

    CloseClipboard();

    return retval;
}

CmzString CClipboard::getAllAvailableFormats(HWND hwnd, CmzString separator) {
    CmzString retval = L" ";

    this->_getAllAvailableFormats(hwnd);
    LONGLONG formatsCount = this->_availableFormats->size();
    ULONGLONG format;
    if (formatsCount > 0) {
        for (LONGLONG i = 0; i < formatsCount; i++) {
            format = this->_availableFormats->getAt(i);
            retval += intToHex(format);
            if (i < formatsCount - 1) {
                retval += separator;
            }
        }
    }

    return retval;
}

CmzString CClipboard::getAllAvailableFormatsDefines(HWND hwnd, CmzString separator, BOOL withNumbers) {
    CmzString retval = L" ";

    this->_getAllAvailableFormats(hwnd);
    LONGLONG formatsCount = this->_availableFormats->size();
    ULONGLONG format;
    if (formatsCount > 0) {
        for (LONGLONG i = 0; i < formatsCount; i++) {
            format = this->_availableFormats->getAt(i);

            switch (format) {
#ifndef NOCLIPBOARD
            case CF_TEXT:
                retval += L"CF_TEXT";
                break;
            case CF_BITMAP:
                retval += L"CF_BITMAP";
                break;
            case CF_METAFILEPICT:
                retval += L"CF_METAFILEPICT";
                break;
            case CF_SYLK:
                retval += L"CF_SYLK";
                break;
            case  CF_DIF:
                retval += L"CF_DIF";
                break;
            case  CF_TIFF:
                retval += L"CF_TIFF";
                break;
            case  CF_OEMTEXT:
                retval += L"CF_OEMTEXT";
                break;
            case  CF_DIB:
                retval += L"CF_DIB";
                break;
            case  CF_PALETTE:
                retval += L"CF_PALETTE";
                break;
            case  CF_PENDATA:
                retval += L"CF_PENDATA";
                break;
            case  CF_RIFF:
                retval += L"CF_RIFF";
                break;
            case  CF_WAVE:
                retval += L"CF_WAVE";
                break;
            case  CF_UNICODETEXT:
                retval += L"CF_UNICODETEXT";
                break;
            case  CF_ENHMETAFILE:
                retval += L"CF_ENHMETAFILE";
                break;
#if defined CF_HDROP
            case  CF_HDROP:
                retval += L"CF_HDRP";
                break;
#endif /* if defined CF_HDROP */
#if defined CF_LOCALE
            case  CF_LOCALE:
                retval += L"CF_LOCALE";
                break;
#endif /* if defined CF_LOCALE */
#if defined CF_DIBV5
            case  CF_DIBV5:
                retval += L"CF_DIBV5";
                break;
#endif /* if defined CF_DIBV5 */
            case CF_MAX:
                retval += L"CF_MAX";
                break;
            case CF_OWNERDISPLAY:
                retval += L"CF_OWNERDISPLAY";
                break;
            case CF_DSPTEXT:
                retval += L"CF_DSPTEXT";
                break;
            case CF_DSPBITMAP:
                retval += L"CF_DSPBITMAP";
                break;
            case CF_DSPMETAFILEPICT:
                retval += L"CF_DSPMETAFILEPICT";
                break;
            case CF_DSPENHMETAFILE:
                retval += L"CF_DSPENHMETAFILE";
                break;
            case CF_PRIVATEFIRST:
                retval += L"Nieznany";
                break;
            case CF_PRIVATELAST:
                retval += L"CF_PRIVATELAST";
                break;
            case CF_GDIOBJFIRST:
                retval += L"CF_GDIOBJFIRST";
                break;
            case CF_GDIOBJLAST:
                retval += L"CF_GDIOBJLAST";
                break;
#endif /* ifndef NOCLIPBOARD */
            default:
                retval += L"Nieznany";
                break;
            }

            if (withNumbers) {
                retval += CmzString(L" [") + intToHex(format) + L"]";
            }

            if (i < formatsCount - 1) {
                retval += separator;
            }
        }
    }

    return retval;
}
