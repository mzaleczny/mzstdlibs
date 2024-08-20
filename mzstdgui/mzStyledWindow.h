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

#ifndef __MZSTDGUI_STYLED_WINDOW_H__
#define __MZSTDGUI_STYLED_WINDOW_H__

#include "mzstdguiCommon.h"
#include "mzWindowClass.h"
#include "mzWindow.h"
#include "mzBitmap.h"
#include "mzAnimatedBitmap.h"
#include "mzGuiControl.h"
#include "mzLinkList.h"
#include "mzCaret.h"
#include "mzResource.h"


class CApplication;
class mzstd::CmzStdObject;
class CPanel;
class mzstd::CmzArrayList;
class CScrollbar;

class DECLSPEC_MZSTDGUI CStyledWindow : public CWindow {
public:
    WINAPI CStyledWindow(CApplication *app, CWindow* parentWindow, mzstd::CmzString name, CWindowClass *wc = NULL, HWND hWnd = NULL);
    WINAPI CStyledWindow(CApplication *app, CWindow* parentWindow, mzstd::CmzString name, mzstd::CmzString layoutFilename, CWindowClass *wc = NULL, HWND hWnd = NULL);
    WINAPI CStyledWindow(CApplication *app, CWindow* parentWindow, mzstd::CmzString name, mzstd::CmzString layout, INT layoutContentType, BOOL returnEditedValues);
    virtual WINAPI ~CStyledWindow();

    DWORD inline WINAPI regionWidth() { return this->_rcRegion.right - this->_rcRegion.left; }
    DWORD inline WINAPI regionHeight() { return this->_rcRegion.bottom - this->_rcRegion.top; }

    VOID WINAPI setWindowParametersBasedOnAppTheme();
    CSprite* WINAPI getSprite(mzstd::CmzString name);
    CSprite* WINAPI getChild(mzstd::CmzString name);
    CPanel* WINAPI getPanel(mzstd::CmzString name);
    CPanel* WINAPI getContainer();
    virtual VOID WINAPI loadGuiLayout(mzstd::CmzString content, INT layoutContentType = RESOURCE_TYPE_JSON, BOOL clearActualContent = TRUE);
    virtual VOID WINAPI loadGuiLayoutFromFile(mzstd::CmzString fname, INT layoutContentType = RESOURCE_TYPE_JSON, BOOL clearActualContent = TRUE);
    virtual VOID WINAPI clearGuiLayout();
    virtual VOID WINAPI createGuiLayout();
    // Funkcja zwraca bieżącą pozycję kursora względem lewego górnego narożnika okna. Zwraca
    // dokładnie te same wartości, które dostarczane są w zdarzenia WM_MOUSE...
    virtual VOID WINAPI getCurrentMousePosition(LONG& x, LONG& y);
    // Zwraca współrzędne sprite'a względem lewego górnego rogu canvasu okna
    virtual VOID WINAPI getSpritePosition(CSprite* sprite, LONG& x, LONG& y);
    inline CCaret* WINAPI getCaret() { return this->_caret; }
    inline CmzBitmap* WINAPI getCanvas() { return this->_canvas; }

    // odpowiada za wyrysowanie karetki
    VOID WINAPI drawCaret(HDC hdc = 0);

    // Funkcje obsługi zdarzeń
    virtual VOID WINAPI onDraw(HDC hdc);
    virtual LRESULT WINAPI onNChitTest(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onSize(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onSizing(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onGetMinMaxInfo(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onMouseMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onNCLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onNCLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onEraseBkgnd(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) { return 1; }
    virtual LRESULT WINAPI onExitSizeMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onWmUserEvent(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WINAPI onNCLButtonDblClk(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam);
    virtual VOID WINAPI onTimer(ULONG uID, ULONG curTickCount);
    virtual VOID WINAPI onActivation();
    virtual VOID WINAPI onDeactivation();

    virtual VOID WINAPI setSize(LONG width, LONG height);
    virtual VOID WINAPI setPosition(LONG x, LONG y);

    // Funkcje obsługi klawiatury wywoływane przed ewentualnym wywołaniem funkcji obsługi
    // klawiatury aktywnej kontrolki (o ile jest taka). Jeśli którakolwiek z tych funkcji zwróci
    // TRUE, to zdarzenie nie jest przekazywane do aktywnej kontrolki.
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
    virtual BOOL WINAPI onEscDown() { return FALSE; }
    virtual BOOL WINAPI onEscPressed() { return FALSE; }
    virtual BOOL WINAPI onEscUp() { return FALSE; }
    virtual BOOL WINAPI onEnterDown() { return FALSE; }
    virtual BOOL WINAPI onEnterPressed() { return FALSE; }
    virtual BOOL WINAPI onEnterUp() { return FALSE; }
    virtual BOOL WINAPI onTabDown() { return FALSE; }
    virtual BOOL WINAPI onTabPressed() { return FALSE; }
    virtual BOOL WINAPI onTabUp() { return FALSE; }

    virtual VOID WINAPI calculateAndApplyRegion();
    VOID WINAPI addChild(CSprite *sprite);
    VOID WINAPI removeChild(CSprite *sprite);
    VOID WINAPI removeChild(mzstd::CmzString name);
    inline VOID WINAPI captureMouse(CGuiControl* sprite) { this->_spriteThatCapturedMouse = sprite; }
    inline BOOL WINAPI isMouseCaptured() { return this->_spriteThatCapturedMouse != NULL; }
    inline CSprite* WINAPI getSpriteThatCapturedMouse() { return this->_spriteThatCapturedMouse; }
    VOID WINAPI setActiveControl(CGuiControl* sprite);
    inline VOID WINAPI setOnlyActiveControlPointer(CGuiControl* sprite) { this->_activeControl = sprite; }
    inline CSprite* WINAPI getActiveControl() { return this->_activeControl; }

    virtual VOID WINAPI allowResizing(BOOL value);
    
    // pretwarza zdarzenia klawiatury
    VOID WINAPI dispatchKeyboardEvents();

    inline VOID WINAPI registerSprite(CSprite* sprite) {
        this->_allSprites.append((LONGLONG)sprite);
    }
    inline VOID WINAPI unregisterSprite(CSprite* sprite) {
        this->_allSprites.del((LONGLONG)sprite);
    }

    // Zwraca sprite'a o podanej nazwie
    CSprite* WINAPI getSpriteByName(mzstd::CmzString name);
    mzstd::CmzString WINAPI getSpriteValue(mzstd::CmzString spriteName);
    VOID WINAPI setSpriteValue(mzstd::CmzString spriteName, mzstd::CmzString value);
    
    // Przenosi focus do następnej kontrolki. Jeśli żadna kontrolka nie była w tym momencie
    // aktywna, to focus ustawiany jest na pierwszej kontrolce, która na to pozwala.
    VOID WINAPI moveFocusForward();
    // Przenosi focus do poprzedniej kontrolki. Jeśli żadna kontrolka nie była w tym momencie
    // aktywna, to focus ustawiany jest na pierwszej kontrolce, która na to pozwala.
    VOID WINAPI moveFocusBackward();
    // Wypełnia podaną listę wszystkimi sprite'ami, które mogą przyjąć focus
    VOID WINAPI getAllFocusableSprites(mzstd::CmzArrayList* lst);
    // Wypełnia podaną listę wszystkimi GUI CONTROL'ami, które mogą przyjąć focus
    VOID WINAPI getAllFocusableGuiControls(mzstd::CmzArrayList* lst);

    virtual VOID WINAPI centerOnScreen();

protected:
    HRGN _hrgn;
    // lista wyświetlanych animowanych bitmap znajdujących się na formularzu. Zawartość tej listy
    // powinna zostać zwolniona.
    mzstd::CmzLinkList _sprites;
    // Lista wszystkich utworzonych sprite'ów które leżą w tym okienku (tzn. których metoda
    // getParentWindow zwraca wskaźnik do tego okienka). Są tu sprite'y dodane do pojemników takich
    // jak na przykład CPanel i zarządzane oraz zwalniane przez te pojemniki (sprite'y dodane do
    // pojemników nie są dodawane do listy this->_sprites). Sprite'y należące do tej listy nie
    // powinny być zwalniane.
    // Lista ta jest np. pomocna przy obsłudze klawiszy TAB i SHIFT+TAB pozwalających na
    // przechodzenie pomiedzy kontrolkami mogącymi przyjąć focus.
    // sprte'y do tej listy są dodawane w konstruktorze klasy CSprite a usuwane w jej
    // destruktorze.
    mzstd::CmzLinkList _allSprites;

    // Prostokąt na bitmapie całego okienka (włącznie z obramowaniem)
    // w którym rysowana jest zawartość naszego okienka (wyznaczona
    // prez region)
    RECT _rcRegion;

    // szerokości obramowania
    LONG _borderTop;
    LONG _borderLeft;
    LONG _borderRight;
    LONG _borderBottom;
    LONG _captionHeight;

    // Wskaźnik na sprite'a, na którym zarezerwowano zdarzenia WM_MOUSE (w wyniku kliknięcia na nim).
    // Jeśli jest różny od NULL, to inne Sprite'y powinny ignorować zdarzenia myszki.
    CGuiControl* _spriteThatCapturedMouse;

    // Aktywna kontrolka - ta która aktualnie ma focus
    CGuiControl* _activeControl;

    // karetka - migający wskaźnik w polu tekstowym
    CCaret* _caret;


    // Parametry wpływające na szybkość obsługi zdarzeń klawiatury
    // opóźnienie po pierwszym wciśnięciu klawisza
    LONG _delayAfterKeyDown;
    // opóźnienie pomiędzy kolejnymi zdarzeniami onKeyPress (wywoływanymi w przypadku, gdy klawisz
    // jest wciśnięty)
    LONG _delayAfterKeyPress;
    // przechowuje aktualne opóźnienie w obsłudze kolejnego zdarzenia klawiatury onKeyDown lub
    // onKeyPress. Ma jedną z wartości: this->_delayAfterKeyDown lub this->_delayAfterKeyPress
    LONG _currentDelay;
    // czas obsługi ostatniego zdarzenia klawiatury onKeyDown lub onKeyPress. W zdarzeniu
    // onKeyUp czas ten jest zerowany.
    ULONG _lastKeyEventTime;
    
    // if _layoutFilename file exists, then it is used to create layout in createGuiLayout function.
    mzstd::CmzString _layoutFilename;
    
    // If all values edited in window shoul be returned on window destroy. If yes, in window destructor there is created
    // CmzMap object containing edited values (CExtPairs mzstd::CmzString=>mzstd::CmzString: "editedVariableName"=>"edtitedVariableValue")
    // and to parent window is sended message with pointer to the object.
    BOOL    _returnEditedValues;
    
    // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
    virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& updateCaretPos, BOOL& redraw);

    VOID WINAPI _commonInit(CApplication *app, mzstd::CmzString name, mzstd::CmzString layoutFilename, CWindowClass *wc = NULL, HWND hWnd = NULL);
protected:    
    virtual VOID WINAPI _updateCanvas();
    VOID WINAPI _adjustChildrenSize();
    VOID WINAPI _internalProcessOnGetEditedValues(mzstd::CmzStdObject* eventInfo);
};

typedef CStyledWindow* PStyledWindow;

#endif
