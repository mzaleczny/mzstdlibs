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

#ifndef __MZSTDGUI_TEXTFIELD_H__
#define __MZSTDGUI_TEXTFIELD_H__

#include "mzstdguiCommon.h"
#include "mzGuiControl.h"
#include "mzString.h"
#include "mzFont.h"
#include "mzTheme.h"


class CCaret;
class CClipboard;

#define CTEXTFIELD_STATE_NORMAL                                     1L
#define CTEXTFIELD_STATE_HOVER                                      2L
#define CTEXTFIELD_STATE_ACTIVE                                     4L
#define CTEXTFIELD_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS      8L


class DECLSPEC_MZSTDGUI CTextField : public CGuiControl {
public:
	WINAPI CTextField(CSprite *parent, mzstd::CmzString name, LONG x = 0, LONG y = 0, LONG width = 100, BOOL tabStop = TRUE);
	virtual WINAPI ~CTextField();

    // Metoda odpowiada za przewijani i zaznaczanie tekstu w polu tekstowym podczas wciśnięcia
    // klawisza myszki i przeciągnięcia myszki na jeden z końców pola lub poza jego obszar.
    // ZWRACA TRUE, jeśli zaszła zmiana w wyświetlanej zawartości i należy odświeżyć sprite'a.
    virtual BOOL WINAPI update();
    // ustawia w kontrolce podany tekst oraz przed wyświetleniem wycina z niego wszystkie znaki
    // o kodzie < 32
    virtual VOID WINAPI setText(mzstd::CmzString text, BOOL redraw = TRUE);
    virtual VOID WINAPI hide();
    
    // czyści tekst kontrolki (atrybut this->_text) ze znaków o kodzie < 32.
    virtual VOID WINAPI cleanupText();
    // zwraca przechowywany w kontrolce tekst
    virtual mzstd::CmzString WINAPI getText();
    // zwraca aktualnie używany w kontrolce font
    CmzFont* WINAPI getFont();
    // zwraca minimalną szerokość kontrolki (width) w zależności od szerokości bitmap użytych
    // w aktualnym temacie graficznym (theme)
    LONG WINAPI getMinWidth();
    // Umieszcza focus na tej kontrolce a dokładnie:
    //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
    //      ParentWnd->setActiveControl(this) tego okna.
    //      Przełącza stan kontrolki w tryb CTEXTFIELD_STATE_ACTIVE.
    //      Ustawia wielkość i położenie karetki wewnątrz tej kontrolki a następnie pokazuje
    //      karetkę i ustawia czas ostatniego przełączenia widoczności karetki (aktualizowany
    //      przy każdym pojawieniu się i zniknięciu karetki)..
    virtual VOID WINAPI focus();
    // Usuwa focus z tej kontrolki a dokładnie:
    //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
    //      Usuwa zaznaczenie (nie usuwając tekstu na którym ono jest)
    //      Przełącza kontrolkę w stan CTEXTFIELD_STATE_HOVER lub CTEXTFIELD_STATE_NORMAL
    //      w zależności od aktualnego położenia myszy.
    virtual VOID WINAPI looseFocus();
    // Zwraca wysokość (height) karetki
    inline LONG WINAPI getCaretHeight() { return this->_lineHeight + 4; }
    // Aktualizuje położenie karetki i pokazuje ją (jeśli aktualnie była niewidoczna) w obrębie
    // tej kontrolki.
    VOID WINAPI updateCaretPos();
    // zwraca położenie karetki dla bieżących wartości atrybutów: this->_startChar oraz
    // this->_caretAtChar. Współrzędne są liczone względem lewego górnego rogu tej kontrolki.
    virtual POINT WINAPI calculateCaretPos();
    // zwraca współrzędne lewego górnego rogu prostokąta dla podanego znaku. Współrzędne są
    // liczone względem lewego górnego rogu tej kontrolki z uwzględnieniem wartości atrybutu:
    // this->_startChar. Jeśli karetka dla podanego znaku znajduje się poza wyświetlanym obszarem
    // po lewej stronie to zwracany jest point: (-1, -1) a jeśli jest po prawej stronie
    // wyświetlanego obszaru, to zwracany jest point: (-2, -2)
    virtual POINT WINAPI calculateCharPos(LONG currentChar);
    // Odpowiada za bezpośrednie wyrysowanie karetki na DC (nie bitmapie offscreenow-ej)
    // ParentOkna
    VOID WINAPI drawCaret();
    // Usuwa zaznaczenie (bez usunięcia tekstu, który ono wskazywało) i ewentualnie odrysowuje
    // kontrolkę
    VOID WINAPI clearSelection(BOOL redraw = TRUE);
    // Zwraca TRUE, jeśli w kontrolce jest ustawion zaznaczenie.
    inline BOOL WINAPI isSelection() {
        return this->_selStart != this->_selEnd;
    }
    // Usuwa ewentualne zaznaczenie (bez usunięcia tekstu) i wstawia podany tekst na podanej
    // pozycji.
    inline VOID WINAPI insertText(ULONG pos, const mzstd::CmzString& s, BOOL redraw = TRUE) {
        this->clearSelection(FALSE);
        this->_text.insertAt(pos, s);
        if (redraw) {
            this->redraw();
        }
    }
    // Wstawia tekst w tej kontrolce i ewentualnie ją odrysowuje. Jeśli w kontrolce było ustawione
    // zaznaczenie to zastępowany jest wskazany przez nie tekst a karetka ustawia się na końcu
    // nowo wklejonego tekstu. Jeśli nie było zaznaczenia, to tekst wstawiony jest w miejscu
    // w którym znajduje się karetka. Po wklejeniu karetka ustawiana jest na końcu wstawionego
    // tekstu.
    VOID WINAPI insertText(mzstd::CmzString s, BOOL redraw = TRUE);
    // zaznacza całą zawartość pola tekstowego.
    VOID WINAPI selectAll(BOOL redraw = TRUE);
    // Ustawia zaznaczenie na podstawie podanych parametrów: jego początku (start) i długości
    // (length) liczonej w znakach. Ewentualnie odrysowuje kontrolkę.
    VOID WINAPI setSelection(ULONG start, ULONG length, BOOL redraw = TRUE);
    // zastępuje tekst wskazywany przez zaznaczenie i ewentualnie odrysowuje kontrolkę.
    VOID WINAPI replaceSelectionWith(mzstd::CmzString replaceWith, BOOL redraw = TRUE);
    // usuwa tekst wskazywany przez zaznaczenie i ewentualnie odrysowuje kontrolkę.
    VOID WINAPI removeSelectedText(BOOL redraw = TRUE);
    // Zwraca długość zaznaczenia liczoną w znakach.
    virtual ULONG WINAPI getSelectionLength();
    // Zwraca łańcuch wskazywany przez zaznaczenie.
    virtual mzstd::CmzString WINAPI getSelectedText();
    // ustawia pozycję początkową: this->_startChar tak by znak wskazywany przez this->_caretAtChar
    // mieścił się na widoku w tym polu tekstowym i był możliwie bliska jego prawej krawędzi.
    // Zwraca TRUE, jesli nastą piła miana wartości this->_startChar.
    virtual BOOL WINAPI adjustStartCharForCaretAtChar();

    // zwraca numer ostatniego widocznego (wyświetlonego w całości a nie tylko w części)
    // w polu tesktowym znaku w danej szerokości. Jeśli parametr max_inner_width jest równy -1,
    // to zwracany jest numer ostatniego znaku w całym polu tekstowym. Jeśli jest > 0, to
    // zwracany jest ostatni znak mieszczący się w podanej szerokości.
    virtual LONG WINAPI getLastVisibleCharPos(LONG max_inner_width = -1);
    // Wykorzystuje funkcję this->getLastVisibleCharPos i zwraca ostatni widoczny w całości
    // w podanej szerokości znak.
    virtual WCHAR WINAPI getLastVisibleChar(LONG max_inner_width = -1);

    // Funkcje obsługi zdarzeń
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
    virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

    virtual BOOL WINAPI onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual BOOL WINAPI onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual BOOL WINAPI onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);

protected:
    mzstd::CmzString _text;
    CmzFont* _font;
    LONG _lineHeight;
    // numer znaku licząc od 0, od którego jest wyświetlany tekst w polu tekstowym
    LONG _startChar;
    // numer znaku licząc od 0, przed którym znajduje się karetka
    LONG _caretAtChar;
    // Wskaźnik na karetkę przechowywaną w ParentWindow - nie trzeba go zwalniać bo robi to
    // ParentWindow.
    CCaret* _caret;
    // Wskaźnik na schowek przechowywany w CApplication - nie trzeba go zwalniać bo robi to
    // CApplication.
    CClipboard* _clipboard;

    // Poniższe dwa atrybuty określają znaki przed którymi zaczyna się i kończy zaznaczenie.
    LONG _selStart;
    LONG _selEnd;
    // Numer znaku przed którym znajdował się kursor w momencie pojawienia się zaznaczenia
    LONG _selBegin;

    // Zwraca prostokąt położenia selekcji. Współrzędne tego prostokąta są liczone względem
    // początku tej kontrolki.
    virtual RECT WINAPI _calculateSelectionRectForText(mzstd::CmzString* s);

    // poniższa funkcja odpowiada za ustawienie karetki najbliżej miejsca kliknięcia, ale tak by
    // znajdowała się między literami.
    // localX i localY, to współrzędne kliknięcia względem początku tego pola tekstowego.
    virtual VOID WINAPI _positionCaretNearClickedPoint(LONG localX, LONG localY);

    virtual VOID WINAPI _commonInit(mzstd::CmzString text, LONG width, BOOL tabStop);
    virtual VOID WINAPI _updateSelection(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl, ULONG vkKey, LONG lastCaretAtChar, BOOL& updateCaretPos, BOOL& redraw);
    virtual VOID WINAPI _updateCursorPosition(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl, ULONG vkKey, BOOL& updateCaretPos, BOOL& redraw);
    // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
    virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& updateCaretPos, BOOL& redraw);
    virtual LONG WINAPI _calculateInnerWidth();
    virtual inline LONG WINAPI _getMaxXPosAllowedForContent() {
        return this->_theme->textfield_frame_left->width() + this->_calculateInnerWidth();
    }
private:
    virtual VOID WINAPI _updateCanvas();
};
typedef CTextField* PTextField;

#endif
