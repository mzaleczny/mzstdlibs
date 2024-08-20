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

#ifndef __MZSTDGUI_GUICONTROL_H__
#define __MZSTDGUI_GUICONTROL_H__

#include "mzstdguiCommon.h"
#include "mzString.h"
#include "mzSprite.h"
#include "mzLinkList.h"
#include "mzStdObject.h"
#include "mzCursor.h"

class CmzBitmap;
class CStyledWindow;
class CScrollbar;
class CTheme;
class CKeyboard;
class CScrollbar;


#define CONTROL_STATE_NORMAL                                     1L
#define CONTROL_STATE_HOVER                                      2L
#define CONTROL_STATE_PUSHED                                     4L
#define CONTROL_STATE_FOCUSED                                    8L
#define CONTROL_STATE_DISABLED                                   16L

#define     MOUSE_EVENT_ONMOUSEDOWN     1L
#define     MOUSE_EVENT_ONMOUSEMOVE     2L
#define     MOUSE_EVENT_ONMOUSEUP       3L


class DECLSPEC_MZSTDGUI CGuiControlItem : public mzstd::CmzObject {
public:
    WINAPI CGuiControlItem(mzstd::CmzString value, BOOL selected = FALSE, BOOL checked = FALSE, LONG imageIndex = -1, LONG data = 0);
    virtual WINAPI ~CGuiControlItem() {}
    
    mzstd::CmzString value;
    BOOL selected;
    BOOL checked;
    LONG imageIndex;
    LONG data;
    SIZE size;
};

class DECLSPEC_MZSTDGUI CGuiControl : public CSprite {
public:
	CStyledWindow *parentWnd;

    WINAPI CGuiControl(CSprite* parent, mzstd::CmzString name, CmzBitmap *canvas, LONG x, LONG y, BOOL editable = FALSE);
    WINAPI CGuiControl(CSprite* parent, CStyledWindow* parentWnd, mzstd::CmzString name, CmzBitmap *canvas, LONG x, LONG y, BOOL editable = FALSE);
    virtual WINAPI ~CGuiControl();
    virtual VOID WINAPI cleanupSprites();
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
    VOID WINAPI draw(LONG x, LONG y,
                        CmzBitmap* frame_top_left, CmzBitmap* frame_top, CmzBitmap* frame_top_right,
                        CmzBitmap* frame_bottom_left, CmzBitmap* frame_bottom, CmzBitmap* frame_bottom_right,
                        CmzBitmap* frame_left, CmzBitmap* frame_right, CmzBitmap* frame_bg);
    
	inline VOID WINAPI setParentWindow(CStyledWindow *wnd) { this->parentWnd = wnd; }
    virtual VOID WINAPI drawChildSprites(CmzBitmap* canvas);
    virtual VOID WINAPI invalidateChildSpritesCanvases();

    // Zwraca aktualne współrzędne kursora liczone względem lewego górnego rogu tego sprite'a.
    VOID WINAPI getCurrentMousePosition(LONG& x, LONG& y);
	// zwraca okno wewnątrz którego znajduje się ten sprite.
	CStyledWindow* WINAPI getParentWindow();
    virtual VOID WINAPI redraw();

    // Funkcja ustawiająca focus na kontrolce - implementacja we właściwej kontrolce
    virtual VOID WINAPI focus() {}
    // Funkcja odbierająca kontrolce focus - implementacja we właściwej kontrolce
    virtual VOID WINAPI looseFocus() {}
    VOID WINAPI setState(LONG state, BOOL redraw = TRUE);
    // dodaje do bieżącego stanu podany stan (wartości stanów muszą być potęgami dwójki, gdyż
    // operacja ta jest realizowana jako orowanie podanego i aktualnego stanu)
    VOID WINAPI addState(LONG state, BOOL redraw = TRUE);
    // usuwa z bieżącego stanu podany stan (wartości stanów muszą być potęgami dwójki, gdyż
    // operacja ta jest realizowana jako andowanie zaprzeczenia podanego stanu i aktualnego stanu)
    VOID WINAPI removeState(LONG state, BOOL redraw = TRUE);
    inline BOOL WINAPI hasState(LONG states) { return (this->_state & states) == states; }
    inline BOOL WINAPI hasExactState(LONG state) { return this->_state == state; }
    // Ustawia podaną wartość tekstową w spricie o podanej nazwie znajdującym się w oknie o podanej
    // nazwie. Jeśli jako nazwę okna podano pusty łańcuch, to docelowy sprite jest wyszukiwany
    // w oknie w którym leży ten sprite.
    // Metoda zwraca TRUE jeśli docelowy sprite został znaleziony i ustawiono mu tekst.
    BOOL WINAPI setTextForSprite(mzstd::CmzString value, mzstd::CmzString spriteName, mzstd::CmzString wndName = L"", BOOL redraw = TRUE);
    // jeśli ta metoda zwróci TRUE, to należy przerysować okienko
    virtual BOOL WINAPI update();
    CTheme* WINAPI getTheme();
    mzstd::CmzCursor* WINAPI getCursor();

	virtual inline VOID WINAPI addChild(CSprite *child) { return; }
    virtual inline VOID WINAPI prependChild(CSprite *child) { return; }
    virtual BOOL WINAPI removeChild(CSprite *child);
    virtual BOOL WINAPI removeChild(mzstd::CmzString childName);
    virtual CSprite* WINAPI getChild(mzstd::CmzString name);
    inline mzstd::CmzLinkList* WINAPI getChildren() { return &this->_sprites; }
    CKeyboard* WINAPI getKbd();

	// jeśli ta metoda zwróci false, to w zdarzeniu onMouseMove okna zostanie
	// sprawdzone czy podany punkt leży wewnątrz sprite'a i jeśli tak, to zostanie
	// uruchomiona animacja sprite'a. W przeciwnym razie animacja nie jest
	// odgrywana. Jeśli ta metoda zwróci true, to znaczy, że została już wykonana
	// jakaś akcja wewnątrz sprite'a uwzględniająca aktualną pozycję kursora myszy.
	virtual BOOL WINAPI onMouseMove(LONG x, LONG y) { return FALSE; }
    // Jeśli metoda zwróci TRUE, to znaczy że sprite obsłużył to zdarzenie i nie zostanie
    // ono przekazane dalej.
	virtual BOOL WINAPI onMouseDown(LONG x, LONG y) { return FALSE; }
    // Jeśli metoda zwróci TRUE, to znaczy że sprite obsłużył to zdarzenie i nie zostanie
    // ono przekazane dalej.
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y) { return FALSE; }
    
    // funkcja obsługi kliknięcia
    virtual VOID WINAPI onClick();

    // Funkcja wywoływana w momencie wciśnięcia klawisza. Jeśli klawisz jest przytrzymany jako
    // wciśnięty, to za każdym następnym razem wywoływana jest tylko funkcja onKeyPressed.
    // Jeśli wciśnięto tylko klawisz systemowy jak np. ALT, SHIFT, CONTROL to argument
    // systemKey jest równy TRUE.
    virtual BOOL WINAPI onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) { return FALSE; }
    // Funkcja wywoływana za każdym razem gdy klawisz jest ciągle wciśnięty (oprócz pierwszego
    // razu, kiedy to wywołana jest funkcja onKeyDown).
    // Jeśli wciśnięto tylko klawisz systemowy jak np. ALT, SHIFT, CONTROL to argument
    // systemKey jest równy TRUE.
    virtual BOOL WINAPI onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) { return FALSE; }
    // Funkcja wywoływana w momencie zwolnienia wciśniętego klawisza
    // Jeśli wciśnięto tylko klawisz systemowy jak np. ALT, SHIFT, CONTROL to argument
    // systemKey jest równy TRUE.
    virtual BOOL WINAPI onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) { return FALSE; }
        
    virtual BOOL WINAPI onEscDown() { return FALSE; }
    virtual BOOL WINAPI onEscPressed();
    virtual BOOL WINAPI onEscUp() { return FALSE; }
    virtual BOOL WINAPI onEnterDown() { return FALSE; }
    virtual BOOL WINAPI onEnterPressed();
    virtual BOOL WINAPI onEnterUp() { return FALSE; }
    virtual BOOL WINAPI onTabDown() { return FALSE; }
    virtual BOOL WINAPI onTabPressed() { return FALSE; }
    virtual BOOL WINAPI onTabUp() { return FALSE; }
    // method onChildNotification is called by the child contrl if any event occur during the time the child
    // has focus. It is for example used for notify parent that editing was ended either by ESC or Enter
    // keypressing.
    virtual LONG WINAPI onChildNotification(CGuiControl* child, LONG data);

    // Functions that delegates mouse events to child controls
    virtual BOOL WINAPI doChildOnMouseEvent(LONG mouseEventType, LONG x, LONG y);
    virtual BOOL WINAPI doChildOnMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI doChildOnMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI doChildOnMouseUp(LONG x, LONG y);

    virtual VOID WINAPI invalidate();
    // Funkcja wywoływana przez scrollbar który jest podczepiony do tego obiektu w momencie zmiany
    // jego pozycji (tj położenia suwaka).
    virtual VOID WINAPI onThumbChange(LONG oldPosition, LONG curPosition, CScrollbar* scrollbar) { return; }

    VOID WINAPI setEditable(BOOL value);
    
    virtual VOID WINAPI addScrollbars(BOOL vertical, BOOL horizontal, LONG vmin, LONG vmax, LONG hmin, LONG hmax);
    virtual VOID WINAPI addVerticalScrollbar(LONG min, LONG max, LONG size = 0);
    virtual BOOL WINAPI removeVerticalScrollbar();
    virtual VOID WINAPI addHorizontalScrollbar(LONG min, LONG max, LONG size = 0);
    virtual BOOL WINAPI removeHorizontalScrollbar();
    virtual VOID WINAPI addEditor();
    virtual BOOL WINAPI removeEditor();
    virtual mzstd::CmzStdObject WINAPI getValue() { mzstd::CmzStdObject o; return o; }
    virtual VOID WINAPI setValue(mzstd::CmzStdObject value, BOOL redraw = TRUE) {}
    virtual VOID WINAPI setValue(mzstd::CmzString value, BOOL redraw = TRUE) {}
    virtual VOID WINAPI edit();
    virtual VOID WINAPI onEditorShow() {};
    virtual VOID WINAPI endEdit(BOOL acceptChanges);
    // Do not delete value argument inside below method
    virtual VOID WINAPI onApplyEditorChanges(mzstd::CmzStdObject* value) {};
    
    virtual VOID WINAPI resetEditor();
    virtual VOID WINAPI setEditorValue(mzstd::CmzStdObject* value);
    virtual VOID WINAPI enable();
    virtual VOID WINAPI disable();

protected:
    // czy wciśnięto lewy przycisk myszki
    BOOL _lMouseButtonPressed;
    // Wskaźnik na aktualnie ustawiony w aplikacji theme
    CTheme *_theme;
    // Wskaźnik na kursor aplikacji - nie trzeba go zwalniać bo robi to CApplication
    mzstd::CmzCursor* _cursor;
    // if control is editable
    BOOL _editable;
    // if we are editor control
    BOOL _isEditor;
    
    // List of child sprites
    mzstd::CmzLinkList _sprites;
    // Attribute _vscrollbar is redundant. It is for quicker access to vertical scrollbar which is in in _sprites
    // list. It is destroyed during destroying the list.
    CScrollbar* _vscrollbar;
    // Attribute _hscrollbar is redundant. It is for quicker access to horizontal scrollbar which is in _sprites
    // list. It is destroyed during destroying the list.
    CScrollbar* _hscrollbar;
    // Attribute _editor is redundant. It is for quicker access to an editor which is in _sprites list.
    // It is destroyed during destroying the list.
    CGuiControl* _editor;

	VOID WINAPI _commonInit(BOOL editable);
};

typedef CGuiControl* PGuiControl;

#endif
