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

#ifndef __MZSTDWIN32_CLIPBOARD_H__
#define __MZSTDWIN32_CLIPBOARD_H__

#include "mzstdwin32Common.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzArrayList.h"


class DECLSPEC_MZSTDWIN32 CClipboard : public mzstd::CmzObject {
public:
	WINAPI CClipboard();
	virtual WINAPI ~CClipboard();

    inline DWORD WINAPI getClipboardSequenceNumber() { return GetClipboardSequenceNumber(); }
    VOID WINAPI getClipboardFormatName(mzstd::CmzString& name, UINT format);
    // Funcja zwraca uchwyt do okna, które ostatnio wywołało funkcję OpenClipboard i jeszcze nie
    // Zamknęło schowka przy pomocy funkcji CloseClipboard.
    inline HWND WINAPI getOpenClipboardWindow() { return GetOpenClipboardWindow(); }
    // Funkcja zwraca uchwyt do okna, które ostatnio umieściło w schowku dane.
    inline HWND WINAPI getClipboardOwner() { return GetClipboardOwner(); }
    inline BOOL WINAPI emptyClipboard() { return EmptyClipboard(); }
    // Zwraca true, jeśli podany format jest w schowku dostępny
    BOOL WINAPI isClipboardFormatAvailable(HWND hwnd, UINT format);
    // Zwraca łańcuch integerów reprezentujących aktualnie dostępne w schowku formaty
    // Inty rozdzielone są podanym separatorem i są zapisane szesnastkowo.
    mzstd::CmzString WINAPI getAllAvailableFormats(HWND hwnd, mzstd::CmzString separator = L" ");
    // Zwraca łańcuch makrodefinicji preprocesora zdefiniowanych w pliku winuser.h
    // odpowiadających aktualnie dostępnym w schowku formatom. Jeśli parametr withNumbers jest
    // równy TRUE, to obok nazwy makrodefinicji w nawiasie umieszczany jest numer szesnastkowy
    // odpowiadający danemu formatowi.
    mzstd::CmzString WINAPI getAllAvailableFormatsDefines(HWND hwnd, mzstd::CmzString separator = L" ", BOOL withNumbers = TRUE);

    // umieszcza w schowku tekst
    BOOL WINAPI copyTextToClipboard(HWND hwnd, const mzstd::CmzString& s);
    // pobiera ze schowka tekst (o ile taki jest dostępny). Jeśli tekst nie jest dostępny, to
    // zwracany jest pusty łańcuch.
    mzstd::CmzString WINAPI getTextFromClipboard(HWND hwnd);

protected:
    // lista dostępnych w schowku formatów
    mzstd::CmzArrayList* _availableFormats;

    // pobiera wszystkie dostępne w schowku formaty
    BOOL WINAPI _getAllAvailableFormats(HWND hwnd);
};


#endif
