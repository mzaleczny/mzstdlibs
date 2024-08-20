#include "pch.h"
#include "mzAnimatedBitmap.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"

namespace mzstd {

CmzAnimatedBitmap::CmzAnimatedBitmap(HINSTANCE hInst, CmzString base_fname)
    : CmzBitmap()
{
    this->setRealClassName(L"CmzAnimatedBitmap");
    this->fname = base_fname;
    this->_step = 0;
    this->_looping = FALSE;
    this->_rewind = FALSE;

    // sprawdzamy ile jest plików klatek animacji
    LONG i;
    CmzString num;
    for (i = 1; i <= 100; i++) {
        num = i;
        if (i < 10) {
            num.prepend('0');
        }
        if (!FileExists(base_fname + '_' + num + ".bmp")) {
            break;
        }
    }

    this->_framesCount = i - 1;
    this->_currentFrame = 0;
    CmzBitmap *bmp = new CmzBitmap(hInst, base_fname + "_01.bmp");
    this->_frameWidth = bmp->width();
    this->_createBlankBitmap(NULL, bmp->width() * this->_framesCount, bmp->height(), RGB(255, 255, 255));
    this->beginPaint();
    this->drawBitmap(bmp);
    delete bmp;

    for (i = 2; i <= this->_framesCount; i++) {
        num = i;
        if (i < 10) {
            num.prepend('0');
        }
        bmp = new CmzBitmap(hInst, base_fname + '_' + num + ".bmp");
        this->drawBitmap(bmp, (i-1)*this->_frameWidth, 0);
        delete bmp;
    }
    this->endPaint();
}

VOID CmzAnimatedBitmap::forward() {
    if (this->_currentFrame < this->_framesCount - 1) {
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
    this->_offsetX = this->_currentFrame * this->_frameWidth;
}

VOID CmzAnimatedBitmap::backward() {
    if (this->_currentFrame > 0) {
        this->_currentFrame--;
    } else {
        if (this->_looping) {
            this->_currentFrame = this->_framesCount - 1;
        } else {
            this->_step = 0;
        }
    }
    this->_offsetX = this->_currentFrame * this->_frameWidth;
}

VOID CmzAnimatedBitmap::play(BOOL forward) {
    if (forward) {
        this->_step = 1;
    } else {
        this->_step = -1;
    }
}

VOID CmzAnimatedBitmap::update() {
    if (this->_step == 1) {
        this->forward();
    } else if (this->_step == -1) {
        this->backward();
    }
}

}
