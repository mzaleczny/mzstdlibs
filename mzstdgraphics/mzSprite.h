#ifndef __MZSTDGRAPHICS_SPRITE_H__
#define __MZSTDGRAPHICS_SPRITE_H__

#include "mzstdgraphicsCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzBitmap.h"

class DECLSPEC_MZSTDGRAPHICS CSprite : public mzstd::CmzObject {
public:
    LONG x, y;
    LONG width, height;
    CmzBitmap* canvas;
    mzstd::CmzString name;
    CSprite *parent;

    WINAPI CSprite(CSprite* parent, mzstd::CmzString name, CmzBitmap *canvas, LONG x, LONG y);
    virtual WINAPI ~CSprite();
    virtual VOID WINAPI onDraw(CmzBitmap *dest);
    virtual RECT WINAPI getRect() {
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = this->width;
        rc.bottom = this->height;
        return rc;
    }

    virtual mzstd::CmzString WINAPI getText();
    virtual VOID WINAPI setText(mzstd::CmzString value, BOOL redraw = TRUE);
    
    virtual VOID WINAPI redraw();
    BOOL WINAPI pointIn(LONG x, LONG y);
    VOID WINAPI play(BOOL forward = TRUE);
    // jeśli ta metoda zwróci TRUE, to należy przerysować okienko
    virtual BOOL WINAPI update();

    VOID WINAPI setPosition(LONG x, LONG y) {
        this->x = x;
        this->y = y;
    }

	virtual inline VOID WINAPI addChild(CSprite *child) { return; }
    virtual inline VOID WINAPI prependChild(CSprite *child) { return; }
    virtual inline BOOL WINAPI removeChild(CSprite *child) { return FALSE; }
    virtual CSprite* WINAPI getChild(mzstd::CmzString name) { return NULL; }

    BOOL inline WINAPI isPlaying() { return this->_step != 0; }
    VOID inline WINAPI stop() { this->_step = 0; }
    VOID inline WINAPI loop() { this->_looping = TRUE; }
    BOOL inline WINAPI isLooping() { return this->_looping; }
    VOID inline WINAPI setRewind(BOOL v) { this->_rewind = v; }
    BOOL inline WINAPI isRewind() { return this->_rewind; }
    VOID WINAPI forward();
    VOID WINAPI backward();
    virtual VOID WINAPI setSize(LONG width, LONG height);
	virtual VOID WINAPI setSizeRelativeToParent(LONG parentWidth, LONG parentHeight) {}
    VOID WINAPI setProportionalSizeRelativeToParent();
    VOID WINAPI setCanvas(CmzBitmap *canvas);
    inline BOOL WINAPI getAutosize() { return this->_autosize; };
    inline VOID WINAPI setAutoSize(BOOL autosize) { this->_autosize = autosize; };

    virtual VOID WINAPI invalidate();
    
    VOID inline WINAPI show() { if (!this->_visible) { this->_visible = TRUE; this->redraw(); } }
    virtual VOID WINAPI hide();
    BOOL WINAPI isVisible() { return this->_visible; }

    // Funkcja zwraca TRUE, jesli można do tego sprite'a przejść przy pomocy klawisza TabStop
    // (tzn. przekazać mu focus)
    BOOL WINAPI canTabStop() { return this->_visible && this->_enabled && this->_tabStop; }
    VOID WINAPI setTabStop(BOOL tabStop) { this->_tabStop = tabStop; }
    
    BOOL WINAPI isEnabled() { return this->_enabled; }
    virtual VOID WINAPI enable() { this->_enabled = TRUE; }
    virtual VOID WINAPI disable() { this->_enabled = FALSE; }
    
    VOID WINAPI enableTabkey() { this->_disableTabkey = FALSE; }
    VOID WINAPI disableTabkey() { this->_disableTabkey = TRUE; }
    BOOL WINAPI isTabkeyEnabled() { return !this->_disableTabkey; }

    VOID WINAPI doEnterAsTabkey(BOOL value) { this->_doEnterAsTabkey = value; }
    BOOL WINAPI isEnterAsTabkey() { return this->_doEnterAsTabkey; }
    
    VOID WINAPI setTransparentDrawing(BOOL value) { this->_transparentDrawing = value; }

protected:
    // stan sprite'a - wykorzystywany tylko przez niektóre klasy dziedziczące
    LONG _state;
    LONG _currentFrame;
    LONG _step;
    BOOL _looping;
    BOOL _rewind;
    // czy sprite jest widoczny
    BOOL _visible;
    // czy sprite jest dostępny
    BOOL _enabled;
    // czy do sprite'a można przejść przy pomocy klawisza Tab lub Shift+Tab
    BOOL _tabStop;
    // if TRUE then tab key is disabled if this sprite has focus
    BOOL _disableTabkey;
    // if TRUE then pressing ENTER key do the same as pressing TAB key
    BOOL _doEnterAsTabkey;

	// poniższy atrybut określa, czy przy zmianie rozmiaru okienka dany sprite ma
	// mieć rozmiar zmieniony relatywnie do swojego parenta
    BOOL _autosize;
    // atrybut określa czy w metodzie onDraw ma być wywołana metoda
    // updateCanvas odpowiedzialna za przerysowanie canvasu
    BOOL _needUpdate;
    // czy przy rysowaniu sprite'a uwzględniać kolory transparentne
    BOOL _transparentDrawing;
    // kolor, który wskazuje transparentne części bitmapy
    COLORREF _transparentColor;
    // bitmap filling color when creating canvas, default to this->_transparentColor
    COLORREF _backgroundColor;
    // Czy mamy zwolnić Canvas. Jeśli została wykonana metoda setSize, to tak.
    BOOL _destroyCanvas;

    virtual VOID WINAPI _updateCanvas() { this->_needUpdate = FALSE; };
	VOID WINAPI _commonInit(mzstd::CmzString &name, CmzBitmap *canvas, LONG x, LONG y);
};

typedef CSprite* PSprite;

#endif
