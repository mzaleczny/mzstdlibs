#ifndef __MZSTDGRAPHICS_BITMAP_H__
#define __MZSTDGRAPHICS_BITMAP_H__

#include "mzstdgraphicsCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzFont.h"

class CStdObjectProperty;

class DECLSPEC_MZSTDGRAPHICS CmzBitmap : public mzstd::CmzObject {
public:
    HBITMAP hBitmap;
    mzstd::CmzString fname;

    WINAPI CmzBitmap();
    WINAPI CmzBitmap(HINSTANCE hInst, mzstd::CmzString fname);
    WINAPI CmzBitmap(DWORD width, DWORD height, COLORREF bgcolor);
    WINAPI CmzBitmap(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor);
    WINAPI CmzBitmap(HDC hdc);
    virtual WINAPI ~CmzBitmap();

    virtual inline LONG WINAPI width() { return this->_width; }
    virtual inline LONG WINAPI height() { return this->_height; }
    virtual VOID WINAPI bitblt(HDC hdc, LONG x = 0, LONG y = 0);
    virtual VOID WINAPI bitblt(HDC hdcDst, LONG dstX, LONG dstY, LONG srcX, LONG srcY, LONG width, LONG height);
    virtual VOID WINAPI drawBitmap(CmzBitmap* bmp, LONG x, LONG y, LONG frame);
    virtual VOID WINAPI drawBitmap(CmzBitmap* bmp, LONG x, LONG y);
    virtual VOID WINAPI drawBitmap(CmzBitmap* bmp);
    virtual VOID WINAPI drawBitmapPart(CmzBitmap* bmp, LONG x, LONG y, LONG frame, LONG srcX, LONG srcY, LONG srcWidth, LONG srcHeight);
    virtual VOID WINAPI drawBitmapPart(CmzBitmap* bmp, LONG x, LONG y, LONG srcX, LONG srcY, LONG srcWidth, LONG srcHeight);
    virtual VOID WINAPI drawBitmapPart(CmzBitmap* bmp, LONG srcX, LONG srcY, LONG srcWidth, LONG srcHeight);
    virtual VOID WINAPI drawTransparentBitmap(CmzBitmap* bmp, LONG x, LONG y, LONG frame, COLORREF transColor);
    virtual VOID WINAPI drawTransparentBitmap(CmzBitmap* bmp, LONG x, LONG y, COLORREF transColor);
    virtual VOID WINAPI drawTransparentBitmap(CmzBitmap* bmp, COLORREF transColor);
    // Ryrsuje pełną bitmapę a nie pojedyńczą jej klatkę. Ma
    // zastosowanie w przypadku obiektów klasy CAnimatedBitmap, które
    // mogą się składać z wielu klatek animacji.
    VOID WINAPI drawFullBitmap(CmzBitmap* bmp, LONG x, LONG y);
    virtual VOID WINAPI stretchBitmap(CmzBitmap* bmp, LONG x, LONG y, LONG width, LONG height);
    // alpha = 0.0 - bmp is drawed transparent
    // alpha = 1.0 - bmp is drawed opaque
    BOOL WINAPI drawAlphaBlend(CmzBitmap* bmp, LONG x, LONG y, float alpha);
    VOID WINAPI setOpacity(float opacity);

    BOOL WINAPI applyFilter3x3(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22);
    BOOL WINAPI drawBitmapEx(CmzBitmap* bmp, LONG x, LONG y, VOID(WINAPI* pixelManipulationRoutine)(BYTE bmpRed, BYTE bmpGreen, BYTE bmpBlue, BYTE* thisRed, BYTE* thisGreen, BYTE* thisBlue));
    // extraFormat zawiera dodatkowe flagi (oprócz flag dmyślnych, czyli
    // DT_SINGLELINE | DT_LEFT | DT_BOTTOM wykorzystanych w tej metodzie), które zostaną użyte do
    // wypisania tekstu przy pomocy standardowej funkcji GDI. replacementFormat zawiera flagi które
    // zostaną użyte do wypisania tekstu przy pomocy standardowej funkcji GDI zastępując flagi
    // domyślne. Jeśli którykolwiek z tych parametrów jest równy -1, to nie jest on brany pod
    // uwagę.
    LONG WINAPI drawText(CmzFont* font, mzstd::CmzString s, RECT rc, BOOL transparent = TRUE, LONG extraFormat = 0, LONG replacementFormat = -1, float rotationAngle = 0.0);
    LONG WINAPI drawText(CmzFont* font, mzstd::CmzString s, LONG x, LONG y, BOOL transparent = TRUE, LONG extraFormat = 0, LONG replacementFormat = -1, float rotationAngle = 0.0);
    LONG WINAPI drawText(CmzFont* font, COLORREF backgroundColor, mzstd::CmzString s, RECT rc, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat);
    LONG WINAPI drawText(CmzFont* font, COLORREF backgroundColor, mzstd::CmzString s, LONG x, LONG y, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat);
    LONG WINAPI drawText(CmzFont* font, COLORREF backgroundColor, mzstd::CmzString s, LONG charSpacing, RECT rc, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat);
    LONG WINAPI drawText(CmzFont* font, COLORREF backgroundColor, mzstd::CmzString s, LONG charSpacing, LONG x, LONG y, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat);

    VOID WINAPI drawRectangleEx(COLORREF color, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, ULONG backgroundStyle = BS_NULL, ULONG_PTR backgroundHatch = 0, float rotationAngle = 0.0);
    VOID WINAPI drawRectangle(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, float rotationAngle = 0.0);
    VOID WINAPI drawRectangle(COLORREF color, RECT rc, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI fillRectangle(COLORREF borderColor, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, float rotationAngle = 0.0);
    VOID WINAPI fillRectangle(COLORREF borderColor, COLORREF backgroundColor, RECT rc, LONG borderThickness = 1, float rotationAngle = 0.0);
    VOID WINAPI fillRectangle(COLORREF color, RECT rc, float rotationAngle = 0.0);
    VOID WINAPI fillRectangle(COLORREF color, LONG x, LONG y, LONG width, LONG height, float rotationAngle = 0.0);

    VOID WINAPI drawRectangleBorder(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, BOOL drawLeftFrame = TRUE, BOOL drawTopFrame = TRUE, BOOL drawRightFrame = TRUE, BOOL drawBottomFrame = TRUE, float rotationAngle = 0.0);
    VOID WINAPI drawRectangleLB(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI drawRectangleBR(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI drawRectangleLBR(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, float rotationAngle = 0.0);

    VOID WINAPI drawLine(COLORREF color, LONG x1, LONG y1, LONG x2, LONG y2, LONG thickness = 1, LONG style = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI drawHLine(COLORREF color, LONG x, LONG y, LONG width, LONG thickness = 1, LONG style = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI drawVLine(COLORREF color, LONG x, LONG y, LONG height, LONG thickness = 1, LONG style = PS_SOLID, float rotationAngle = 0.0);

    VOID WINAPI drawPolyline(COLORREF color, POINT* pointsArray, ULONG pointsCount, BOOL closeShape = TRUE, LONG thickness = 1, LONG style = PS_SOLID, float rotationAngle = 0.0);

    VOID WINAPI drawSquareEx(COLORREF color, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, ULONG backgroundStyle = BS_NULL, ULONG_PTR backgroundHatch = 0, float rotationAngle = 0.0);
    VOID WINAPI drawSquare(COLORREF color, LONG x, LONG y, LONG width, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI fillSquare(COLORREF color, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, float rotationAngle = 0.0);
    VOID WINAPI fillSquare(COLORREF color, LONG x, LONG y, LONG width, float rotationAngle = 0.0);

    VOID WINAPI drawEllipseEx(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG rx, LONG ry, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, ULONG backgroundStyle = BS_NULL, ULONG_PTR backgroundHatch = 0);
    VOID WINAPI drawEllipse(COLORREF color, LONG cx, LONG cy, LONG rx, LONG ry, LONG borderThickness = 1, LONG borderStyle = PS_SOLID);
    VOID WINAPI fillEllipse(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG rx, LONG ry, LONG borderThickness = 1, LONG borderStyle = PS_SOLID);
    VOID WINAPI fillEllipse(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG rx, LONG ry);

    VOID WINAPI drawCircleEx(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG r, LONG borderThickness = 1, LONG borderStyle = PS_SOLID, ULONG backgroundStyle = BS_NULL, ULONG_PTR backgroundHatch = 0);
    VOID WINAPI drawCircle(COLORREF color, LONG cx, LONG cy, LONG r, LONG borderThickness = 1, LONG borderStyle = PS_SOLID);
    VOID WINAPI fillCircle(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG r, LONG borderThickness = 1, LONG borderStyle = PS_SOLID);
    VOID WINAPI fillCircle(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG r);

    VOID WINAPI floodFill(COLORREF borderColor, COLORREF fillColor, LONG xStart, LONG yStart, ULONG backgroundStyle = BS_SOLID, ULONG_PTR backgroundHatch = 0);

    VOID WINAPI setSize(LONG width, LONG height);
    VOID WINAPI setSize(HDC hdc, LONG width, LONG height);

    VOID WINAPI setPen(COLORREF c, LONG width = 1, LONG style = PS_SOLID);
    VOID WINAPI setBrush(COLORREF c);
    LONG WINAPI setBkMode(LONG bkMode);
    COLORREF WINAPI setBkColor(COLORREF newBkColor);
    VOID WINAPI fillWithColor(COLORREF color);
    inline LONG WINAPI getX() { return this->_x; }
    inline LONG WINAPI getY() { return this->_y; }
    inline VOID WINAPI setXY(LONG x, LONG y) { this->_x = x; this->_y = y; }
    inline HDC WINAPI getDC() { return this->_memDC; }
    inline VOID WINAPI turnOnTrackingChanges() { this->_trackChanges = TRUE; }
    inline VOID WINAPI turnOffTrackingChanges() { this->_trackChanges = TRUE; }
    inline VOID WINAPI validateBitmap() { ZeroMemory(&this->_updateRect, sizeof(this->_updateRect)); }
    VOID WINAPI invalidateRect(LONG x, LONG y, LONG width, LONG height);
    inline RECT WINAPI getUpdateRect() { return this->_updateRect; }

    COLORREF* WINAPI getRgbTable();
    // Returns bits of bitmap. Returned memory should be deleted.
    VOID WINAPI getDIBits(BYTE** bmBits, DWORD* size);

    BOOL WINAPI beginPaint(HDC hdc = 0, BOOL createCompatibleDC = TRUE);
    BOOL WINAPI endPaint();

    // Saves bitmap to file.
    VOID WINAPI saveToFile(mzstd::CmzString fname);
    inline VOID WINAPI setBgcolor(COLORREF color) { this->_bgcolor = color; }
    inline COLORREF WINAPI getBgcolor() { return this->_bgcolor; }

protected:
    // czy rejestrować region bitmapy, który został zmieniony przez jedną z funkcji składowych
    BOOL _trackChanges;
    // Najmniejszy prostokąt oblewający wprowadzone na bitmapie zmiany. Jeśli nie było żadnych
    // zmian, to wszystkie jego składowe są równe 0.
    RECT _updateRect;

    // rozmiary bitmapy
    LONG _width;
    LONG _height;
    COLORREF _bgcolor;

    // Współrzędne położenia tej bitmapy na bitmapie na której jest
    // ona rysowana. Są one wykorzystywane w medodzie drawBitmap
    // wywołanej bez dodatkowych parametrów.
    LONG _x;
    LONG _y;
    // offsety używane w funkcjach rysujących bitmapy
    LONG _offsetX;
    LONG _offsetY;

    HDC _memDC;
    HBITMAP _oldBitmapFromMemDC;
    BOOL _compatibleDCCreated;

    VOID WINAPI _createBlankBitmap(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor);
    VOID WINAPI _commonInit(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor);
    virtual VOID WINAPI _cleanup();
    VOID WINAPI _rotatePoint(LONG x, LONG y, float rotationAngle, LONG* xRotated, LONG* yRotated, LONG rotationOriginX = 0, LONG rotationOriginY = 0);

private:
};

#endif
