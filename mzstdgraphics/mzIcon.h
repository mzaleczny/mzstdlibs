#ifndef __MZSTDGRAPHICS_ICON_H__
#define __MZSTDGRAPHICS_ICON_H__

#include "mzstdgraphicscommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTDGRAPHICS CmzIcon : public CmzObject {
public:
    HICON hIcon;
    CmzString fname;

    WINAPI CmzIcon(HINSTANCE hInst, CmzString fname, LONG size = 32);
    virtual WINAPI ~CmzIcon();

    LONG inline WINAPI width() { return this->_width; }
    LONG inline WINAPI height() { return this->_height; }

protected:
    // rozmiary ikony
    LONG _width;
    LONG _height;

    // Blokujemy możliwość utworzenia bitmapy bez podania parametrów
    WINAPI CmzIcon() : hIcon(NULL), fname("") {}
};

}

#endif
