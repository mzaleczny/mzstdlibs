#ifndef __MZSTDGRAPHICS_CANVAS_H__
#define __MZSTDGRAPHICS_CANVAS_H__

#include "mzstdgraphicscommon.h"
#include "mzBitmap.h"
#include "mzArrayList.h"
#include "mzFont.h"

namespace mzstd {

class CmzStdObjectProperty;

class DECLSPEC_MZSTDGRAPHICS CmzCanvas : public CmzBitmap {
public:
    WINAPI CmzCanvas();
    WINAPI CmzCanvas(DWORD width, DWORD height, COLORREF bgcolor);
    WINAPI CmzCanvas(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor, BOOL isPrintCanvas = FALSE);
    WINAPI CmzCanvas(HDC hdc, BOOL isPrintCanvas = FALSE);
    virtual WINAPI ~CmzCanvas();

    VOID WINAPI beginPrint();
    VOID WINAPI endPrint();
    
    inline BOOL WINAPI isPrintCanvas() { return this->_isPrintCanvas; }
    virtual VOID WINAPI drawBitmap(CmzBitmap *bmp, LONG x, LONG y);
    
    // Method necessary for drawing tiny font-size texts on printer canvas, because some printers does not draw
    // text of size < 8. Workaround is creating auxiliry bitmap, draw text of proper recalculated size onto it
    // and then blit it onto the printer canvas. Currently works only for vertical and horizontal drawed texts.
    LONG WINAPI drawTinyText(CmzFont *font, CmzString s, LONG x, LONG y, BOOL transparent = TRUE, LONG extraFormat = 0, LONG replacementFormat = -1, float rotationAngle = 0.0);

    VOID WINAPI drawDateFieldDDMMYYYY(CmzFont* font, CmzString year, CmzString month, CmzString day,
        LONG x, LONG y, LONG width, LONG height, LONG groupSpacing, LONG fieldHeight,
        COLORREF borderColor, LONG borderThickness, LONG borderStyle,
        COLORREF backgroundColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch,
        float rotationAngle);
    VOID WINAPI drawLabeledTextField(CmzFont* font, CmzFont* labelFont, LONG x, LONG y, LONG width, LONG height,
        CmzString label, CmzString text, BOOL eachTextLetterInSeparateBox,
        LONG labelOffsetY, LONG labelPaddingLeft, LONG labelPaddingTop, LONG labelPaddingRight, LONG labelPaddingBottom,
        COLORREF labelLineBorderColor, LONG labelLineBorderThickness, LONG labelLineBorderStyle,
        COLORREF labelLineBackgroundColor, ULONG labelLineBackgroundStyle, ULONG_PTR labelLineBackgroundHatchStyle,
        COLORREF borderColor, LONG borderSize, LONG borderStyle,
        COLORREF backgroundColor, ULONG backgroundStyle, ULONG_PTR backgroundHatchStyle,
        COLORREF textFieldBackgroundColor,
        LONG textFieldBoxCount, LONG textFieldSeparatorHeight, LONG textFieldSeparatorThickness,
        float rotationAngle);
    VOID WINAPI drawTextInRectangles(CmzFont* font, LONG x, LONG y, LONG width, LONG height,
        CmzString text,
        COLORREF borderColor, LONG borderSize, LONG borderStyle,
        COLORREF backgroundColor, ULONG backgroundStyle, ULONG_PTR backgroundHatchStyle,
        COLORREF unitBorderColor, LONG unitBorderSize, LONG unitBorderStyle,
        COLORREF unitBackgroundColor, ULONG unitBackgroundStyle, ULONG_PTR unitBackgroundHatchStyle,
        LONG unitWidth, LONG unitHeight, LONG unitPadding, LONG spacing, BOOL drawEmptyFields,
        float rotationAngle);

    // gradientDirection can be 0, 1
    //VOID WINAPI drawGradientedRectangle(LONG x, LONG y, LONG width, LONG height, float borderThickness = 1.0, Color borderColor, Color startGradientColor = Color(0xff,0x99,0xcc,0xff), Color endGradientColor = Color(0xff,0x00,0x00,0x66), LONG gradientDirection = 0, float rotationAngle = 0.0);
    VOID WINAPI drawGradientedRectangle(LONG x, LONG y, LONG width, LONG height, LONG borderThickness = 1, COLORREF borderColor = 0, COLORREF startGradientColor= RGB(0x99,0xcc,0xff), COLORREF endGradientColor = RGB(0x00,0x00,0x66), LONG gradientDirection = 0, float rotationAngle = 0.0);
    
protected:
    // if this is a printer canvas
    BOOL _isPrintCanvas;
    
    VOID WINAPI _commonInit(BOOL isPrintCanvas);
    virtual VOID WINAPI _cleanup();
private:
};

}

#endif
