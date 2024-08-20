#ifndef __MZSTDGRAPHICS_GRAPHICS_UTILS_H__
#define __MZSTDGRAPHICS_GRAPHICS_UTILS_H__

#include "mzstdgraphicsCommon.h"
#include "mzString.h"

namespace mzstd {

DECLSPEC_MZSTDGRAPHICS VOID WINAPI SaveBitmapToFile(CmzString fname, HBITMAP hBitmap);
DECLSPEC_MZSTDGRAPHICS VOID WINAPI GetBitmapSize(HBITMAP hBitmap, SIZE &size);
DECLSPEC_MZSTDGRAPHICS COLORREF WINAPI HexToRGB(CmzString hex_color);

}

#endif
