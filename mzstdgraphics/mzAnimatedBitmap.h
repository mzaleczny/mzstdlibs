#ifndef __MZSTDGRAPHICS_ANIMATED_BITMAP_H__
#define __MZSTDGRAPHICS_ANIMATED_BITMAP_H__

#include "mzstdgraphicsCommon.h"
#include "mzBitmap.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTDGRAPHICS CmzAnimatedBitmap : public CmzBitmap {
public:
    WINAPI CmzAnimatedBitmap(HINSTANCE hInst, CmzString base_fname);
    virtual WINAPI ~CmzAnimatedBitmap() {}
    inline LONG WINAPI getCurrentFrame() { return this->_currentFrame; }
    inline VOID WINAPI setCurrentFrame(LONG currentFrame) { this->_currentFrame = currentFrame; }
    VOID WINAPI forward();
    VOID WINAPI backward();
    virtual inline LONG WINAPI width() { return this->_frameWidth; }
    inline LONG WINAPI fullWidth() { return this->_width; }
    VOID WINAPI play(BOOL forward = TRUE);
    inline BOOL WINAPI isPlaying() { return this->_step != 0; }
    inline VOID WINAPI stop() { this->_step = 0; }
    inline VOID WINAPI loop() { this->_looping = TRUE; }
    inline BOOL WINAPI isLooping() { return this->_looping; }
    inline VOID WINAPI setRewind(BOOL v) { this->_rewind = v; }
    inline BOOL WINAPI isRewind() { return this->_rewind; }
    VOID WINAPI update();
    inline LONG WINAPI getFramesCount() { return this->_framesCount; }

private:
    LONG _currentFrame;
    LONG _framesCount;
    LONG _frameWidth;
    // Flaga sygnalizująca co ile jest zmieniana klatka animacji.
    // Jeśli ma wartość niezerową, to znaczy że odgrywana jest
    // animacja.
    LONG _step;
    // Flaga sygnalizująca czy po odtworzeniu całej animacji ma się
    // ona zacząć od początku.
    BOOL _looping;
    // Flaga sygnalizująca czy po zakończeniu i zatrzymaniu animacji ma nastąpić
    // powrót do pierwszej klatki.
    BOOL _rewind;
};

}

#endif
