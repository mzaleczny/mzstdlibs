#ifndef __MZSTDGRAPHICS_CANVAS_FILE_H__
#define __MZSTDGRAPHICS_CANVAS_FILE_H__

#include "mzstdgraphicscommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzCanvas.h"
#include "mzPointerList.h"

namespace mzstd {

class DECLSPEC_MZSTDGRAPHICS CmzCanvasFile : public CmzObject {
public:
    WINAPI CmzCanvasFile();
    virtual WINAPI ~CmzCanvasFile();

    BOOL WINAPI drawJsonFile(CmzString fname, CmzCanvas* canvas);

protected:
    // array improving font management
    CmzPointerList* _cachedFonts;

    CmzFont* WINAPI getCachedFont(WCHAR* fontName, LONG fontSize, LONG fontColor);
    VOID WINAPI _commonInit();
    virtual VOID WINAPI _cleanup();
    VOID WINAPI _cleanCachedFonts();

private:
    // Variable valid only during call to drawJsonFile method.
    // It is set to canvas pointer and NULL-ed just before drawJsonFile method return.
    CmzCanvas* _canvasOperatedOn;

    VOID WINAPI getPageProperties(CmzStdObjectProperty* itemPage, float* width, float* height,
        float* marginLeft, float* marginTop, float* marginRight, float* marginBottom,
        CmzString* munits);
    VOID WINAPI getCommonShapeProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
        float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
        float* x, float* y, float* width, float* height,
        float* borderSize, CmzString* borderStyle, CmzString* borderColor, float* rotationAngle);
    VOID WINAPI getBorderProperties(CmzString prefix, CmzStdObjectProperty* itemShape,
        float* borderSize, CmzString* borderStyle, CmzString* borderColor);
    VOID WINAPI getBackgroundProperties(CmzString prefix, CmzStdObjectProperty* itemShape,
        CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle);
    CmzFont* WINAPI getFontProperties(CmzString prefix, CmzStdObjectProperty* itemShape, CmzString* fontName, float* fontSize,
        CmzString* fontColor, CmzString* fontStyle);
    VOID WINAPI getRectProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
        float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
        float* x, float* y, float* width, float* height,
        float* borderSize, CmzString* borderStyle, CmzString* borderColor,
        CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
        float* rotationAngle);
    CmzFont* WINAPI getTextProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
        float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
        float* x, float* y, float* width, float* height,
        CmzString* text, float* dxTextOffset, float* dyTextOffset,
        CmzString* fontName, float* fontSize, CmzString* fontStyle, CmzString* fontColor,
        float* borderSize, CmzString* borderStyle, CmzString* borderColor,
        CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
        float* charSpacing, float* rotationAngle, CmzString* align, CmzString* valign,
        float* leftPadding, float* topPadding, float* rightPadding, float* bottomPadding);
    CmzFont* WINAPI getDateFieldProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
        float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
        float* x, float* y, float* width, float* height,
        CmzString* year, CmzString* month, CmzString *day, float* groupSpacing, float* fieldHeight,
        CmzString* fontName, float* fontSize, CmzString* fontColor, CmzString* fontStyle,
        float* borderSize, CmzString* borderColor, CmzString* borderStyle,
        CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
        float* rotationAngle);
    VOID WINAPI getInputFieldProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
            float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
            float* x, float* y, float* width, float* height,
            CmzString* label, CmzString* text, BOOL* eachTextLetterInSeparateBox,
            CmzString* labelFontName, float* labelFontSize, CmzString* labelFontColor, CmzString* labelFontStyle,
            CmzString* textFontName, float* textFontSize, CmzString* textFontColor, CmzString* textFontStyle,
            float* labelOffsetY, float* labelPaddingLeft, float* labelPaddingTop, float* labelPaddingRight, float* labelPaddingBottom,
            float* labelLineBorderSize, CmzString* labelLineBorderStyle, CmzString* labelLineBorderColor,
            CmzString* labelLineBackgroundColor, CmzString* labelLineBackgroundStyle, CmzString* labelLineBackgroundHatch,
            float* borderSize, CmzString* borderStyle, CmzString* borderColor,
            CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
            CmzString* textFieldBackgroundColor,
            float* textFieldBoxCount, float* textFieldSeparatorHeight, float* textFieldSeparatorThickness,
            float* rotationAngle,
            CmzFont** textFont, CmzFont** labelFont);
    CmzFont* WINAPI getTextInRectsProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
            float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
            float* x, float* y, float* width, float* height,
            CmzString* text,
            CmzString* textFontName, float* textFontSize, CmzString* textFontColor, CmzString* textFontStyle,
            float* borderSize, CmzString* borderStyle, CmzString* borderColor,
            CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
            float* unitBorderSize, CmzString* unitBorderStyle, CmzString* unitBorderColor,
            CmzString* unitBackgroundColor, CmzString* unitBackgroundStyle, CmzString* unitBackgroundHatchStyle,
            float* unitWidth, float* unitHeight, float* unitPadding, float* spacing, BOOL* drawEmptyFields,
            float* rotationAngle);
    VOID WINAPI getLineProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
        float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
        float* x, float* y, float* x2, float* y2, float* thickness, CmzString* style,
        CmzString* color, float* rotationAngle);

    LONG WINAPI stringStyleToPenStyle(CmzString style);
    ULONG WINAPI stringStyleToBrushStyle(CmzString style);
    ULONG_PTR WINAPI stringStyleToBrushHatchStyle(CmzString style);
};

}

#endif
