#include "pch.h"
#include "mzSprite.h"
#include "mzAnimatedBitmap.h"

using namespace mzstd;

CSprite::CSprite(CSprite* const parent, mzstd::CmzString name, CmzBitmap *canvas, LONG x, LONG y) {
    this->parent = parent;
	this->_commonInit(name, canvas, x, y);
}

VOID CSprite::_commonInit(mzstd::CmzString &name, CmzBitmap *canvas, LONG x, LONG y) {
    this->setRealClassName(L"CSprite");
    this->_state = 0;
    this->name = name;
    this->canvas = NULL;
    this->setCanvas(canvas);
    this->_destroyCanvas = FALSE;
    this->x = x;
    this->y = y;
    this->_currentFrame = 0;
    this->_step = 0;
    this->_looping = FALSE;
    this->_rewind = FALSE;
    this->_visible = TRUE;
    this->_enabled = TRUE;
    this->_tabStop = FALSE;
    this->_disableTabkey = FALSE;
    this->_doEnterAsTabkey = FALSE;
    this->_autosize = FALSE;
    this->_transparentDrawing = FALSE;
    // zielony
    this->_transparentColor = 0x404040;// 0xffffffff;// RGB(0, 255, 0);
    this->_backgroundColor = this->_transparentColor;
}

CSprite::~CSprite() {
    if (this->_destroyCanvas && this->canvas) {
        delete this->canvas;
    }
}

VOID CSprite::setCanvas(CmzBitmap *canvas) {
    if (!canvas) {
        return;
    }
    
    if (this->canvas && this->_destroyCanvas) {
        delete this->canvas;
    }
    
    this->canvas = canvas;
    if (canvas) {
        this->width = canvas->width();
        this->height = canvas->height();
    } else {
        this->width = 0;
        this->height = 0;
    }
    this->_needUpdate = TRUE;
}

BOOL CSprite::pointIn(LONG x, LONG y) {
    if (
        x >= this->x && x < this->x + this->width &&
        y >= this->y && y < this->y + this->height
       ) {
           return TRUE;
    }
    return FALSE;
}

VOID CSprite::onDraw(CmzBitmap *dest) {
	if (!this->canvas) return;
    if (!this->_visible) return;

    if (this->_needUpdate) {
        this->_updateCanvas();
    }
    if (this->_transparentDrawing) {
        dest->drawTransparentBitmap(this->canvas, this->x, this->y, this->_currentFrame, this->_transparentColor);
    } else {
        dest->drawBitmap(this->canvas, this->x, this->y, this->_currentFrame);
    }
}

VOID CSprite::play(BOOL forward) {
    CmzAnimatedBitmap *b = dynamic_cast<CmzAnimatedBitmap*>(this->canvas);
    if (b) {
        if (forward) {
            this->_step = 1;
        } else {
            this->_step = -1;
        }
    }
}

BOOL CSprite::update() {
    CmzAnimatedBitmap *b = dynamic_cast<CmzAnimatedBitmap*>(this->canvas);
    if (b) {
        if (this->isPlaying()) {
            if (this->_step == 1) {
                this->forward();
            } else if (this->_step == -1) {
                this->backward();
            }
            return TRUE;
        }
    }

    return FALSE;
}

VOID CSprite::forward() {
    CmzAnimatedBitmap *b = dynamic_cast<CmzAnimatedBitmap*>(this->canvas);
    if (b) {
        if (this->_currentFrame < b->getFramesCount() - 1) {
            this->_currentFrame++;
        } else {
            if (this->_looping) {
                this->_currentFrame = 0;
            } else {
                this->_step = 0;
                if (this->_rewind) {
                    this->_currentFrame = 0;
                }
            }
        }
    }
}

VOID CSprite::backward() {
    CmzAnimatedBitmap *b = dynamic_cast<CmzAnimatedBitmap*>(this->canvas);
    if (b) {
        if (this->_currentFrame > 0) {
            this->_currentFrame--;
        } else {
            if (this->_looping) {
                this->_currentFrame = b->getFramesCount() - 1;
            } else {
                this->_step = 0;
            }
        }
    }
}

VOID CSprite::setSize(LONG width, LONG height) {
    if (this->_destroyCanvas && this->canvas) {
        delete this->canvas;
        this->canvas = NULL;
    }
    this->setCanvas(new CmzBitmap(width, height, this->_backgroundColor));
    this->_destroyCanvas = TRUE;
}

VOID CSprite::setProportionalSizeRelativeToParent() {
}

VOID CSprite::redraw() {
    // jeśli sprite, nie ma przypisanego rodzica oraz okna lub jest niewidoczny, to go nie
    // odrysowujemy bo jeszcze nie jest wyświetlany
    if (!this->_visible || !this->parent) {
        return;
    }
    this->_needUpdate = TRUE;
    if(this->parent) {
        this->parent->redraw();
    }
}

VOID CSprite::invalidate() {
    this->_needUpdate = TRUE;
}

mzstd::CmzString CSprite::getText() {
    return this->toString();
}

VOID CSprite::setText(mzstd::CmzString value, BOOL redraw) {
}

VOID CSprite::hide() {
    if (this->_visible) {
        this->_visible = FALSE;
        if (this->parent) {
            this->parent->redraw();
        }
    }
}
