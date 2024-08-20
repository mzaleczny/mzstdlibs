#include "pch.h"
#include "mzCanvasFile.h"
#include "mzstd.h"
#include "mzstdgraphics.h"
#include "stdio.h"
#include "mzMath.h"
#include "mzLogger.h"
#include "mzGraphicsUtils.h"
#include "mzJsonParser.h"
#include "mzMessageBox.h"


namespace mzstd {

VOID CmzCanvasFile::_commonInit() {
    this->setRealClassName(L"CmzCanvasFile");
}

CmzCanvasFile::CmzCanvasFile() {
    this->_commonInit();
    this->_cachedFonts = new CmzPointerList();
}

CmzCanvasFile::~CmzCanvasFile() {
    this->_cleanup();
}

VOID CmzCanvasFile::_cleanup() {
    this->_cleanCachedFonts();
}

VOID CmzCanvasFile::_cleanCachedFonts() {
    ULONGLONG size = this->_cachedFonts->size();
    CmzFont* font;
    for (ULONGLONG i = 0; i < size; i++) {
        font = (CmzFont*)this->_cachedFonts->getAt(i);
        if (font) {
            delete font;
        }
    }
    this->_cachedFonts->clear();
}

CmzFont* CmzCanvasFile::getCachedFont(WCHAR* fontName, LONG fontSize, LONG fontColor) {
    ULONGLONG size = this->_cachedFonts->size();
    CmzFont* font;
    for (ULONGLONG i = 0; i < size; i++) {
        font = (CmzFont*)this->_cachedFonts->getAt(i);
        if (font->name == fontName && font->size == fontSize && font->color == fontColor) {
            return font;
        }
    }

    return NULL;
}

BOOL CmzCanvasFile::drawJsonFile(CmzString fname, CmzCanvas* canvas) {
    BOOL result = FALSE;

    if (FileExists(fname)) {
        CmzJsonParser parser;

        CmzStdObject* data = parser.parseFile(fname);
        if (!data) {
            if (!parser.isError()) {
                AlertBox(CmzString(L"Plik [") + fname + L"] nie zawiera obiektów json");
            } else {
                ErrorBox(parser.getErrorMessage());
            }
            return FALSE;
        }

        this->_canvasOperatedOn = canvas;
		CmzStdObjectProperty* root = data->get(L"root");
        CmzPointerList* items;
        CmzStdObjectProperty* item;
		if (root->type == STDOBJ_PROPERTY_ARRAY) {
			items = root->aValue;
            if (items) {
                float pageWidth = 0;
                float pageHeight = 0;
                float pageMarginLeft = 0, pageMarginRight = 0, pageMarginTop = 0, pageMarginBottom = 0;

                CmzString munits = L"px";
                item = (CmzStdObjectProperty*)items->getAt(0);
                this->getPageProperties(item, &pageWidth, &pageHeight, &pageMarginLeft, &pageMarginTop, &pageMarginRight, &pageMarginBottom, &munits);
                if (item && pageWidth > 0 && pageHeight > 0) {
                    if (canvas->getDC()) {
                        canvas->endPaint();
                    }
                    canvas->setSize((LONG)pageWidth, (LONG)pageHeight);
                    canvas->beginPaint();
                    item = item->oValue->get(L"items");
                    if (item && item->type == STDOBJ_PROPERTY_ARRAY) {
                        items = item->aValue;
                        if (items) {
                            ULONGLONG itemsSize = items->size();
                            CmzStdObjectProperty* subitem;
                            CmzStdObjectProperty* subitemProp;

                            float x, y, x2, y2;
                            float width, height;
                            float thickness, dxTextOffset, dyTextOffset, fieldHeight;
                            BOOL drawLeftFrame, drawTopFrame, drawRightFrame, drawBottomFrame;
                            CmzString color, style;
                            float rotationAngle, groupSpacing;
                            LONG penStyle;
                            ULONG brushStyle;
                            ULONG_PTR brushHatchStyle;
                            CmzString borderColor, borderStyle;
                            float borderSize;
                            CmzString backgroundColor, backgroundStyle, backgroundHatchStyle;
                            COLORREF rgbColor, rgbBackgroundColor, rgbBorderColor;
                            CmzString fontName, fontColor, fontStyle;
                            float fontSize, charSpacing;
                            CmzString text, align, valign;
                            float leftPadding, topPadding, rightPadding, bottomPadding;
                            float unitWidth, unitHeight, unitPadding, spacing;

                            CmzString label, labelFontName, labelFontColor, labelFontStyle;
                            float labelFontSize, labelOffsetY, labelPaddingLeft, labelPaddingTop, labelPaddingRight, labelPaddingBottom;
                            float labelLineBorderSize;
                            CmzString labelLineBorderStyle, labelLineBorderColor;
                            CmzString labelLineBackgroundColor, labelLineBackgroundStyle, labelLineBackgroundHatchStyle;
                            LONG labelPenStyle;
                            ULONG labelBrushStyle;
                            ULONG_PTR labelBrushHatchStyle;

                            CmzString textFieldBackgroundColor;
                            float textFieldBoxWidth, textFieldSeparatorHeight, textFieldSeparatorThickness;

                            CmzFont* font;
                            CmzFont* labelFont;
                            CmzString year, month, day;
                            BOOL eachTextLetterInSeparateBox, drawEmptyFields;

                            for (ULONGLONG i = 0; i < itemsSize; i++) {
                                subitem = (CmzStdObjectProperty*)items->getAt(i);
                                if (subitem && subitem->type == STDOBJ_PROPERTY_OBJECT) {
                                    subitemProp = subitem->oValue->get(L"name");
                                    if (subitemProp && subitemProp->type == STDOBJ_PROPERTY_VALUE) {
                                        if (subitemProp->value == L"Shape") {
                                            subitemProp = subitem->oValue->get(L"type");
                                            if (subitemProp && subitemProp->type == STDOBJ_PROPERTY_VALUE) {
                                                LOG(subitemProp->value.toWCHAR());
                                                if (subitemProp->value.startsWith(L"Rect")) {
                                                    this->getRectProperties(subitem,
                                                        pageWidth, pageHeight,
                                                        pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom,
                                                        &x, &y, &width, &height,
                                                        &borderSize, &borderStyle, &borderColor,
                                                        &backgroundColor, &backgroundStyle, &backgroundHatchStyle,
                                                        &rotationAngle);

                                                    rgbBorderColor = HexToRGB(borderColor);
                                                    rgbBackgroundColor = HexToRGB(backgroundColor);
                                                    penStyle = this->stringStyleToPenStyle(borderStyle);
                                                    brushStyle = this->stringStyleToBrushStyle(backgroundStyle);
                                                    brushHatchStyle = this->stringStyleToBrushHatchStyle(backgroundHatchStyle);
                                                    if (backgroundColor == COMMON_EMPTY_STRING) {
                                                        brushStyle = BS_NULL;
                                                    }
                                                    if (subitemProp->value == L"Rect") {
                                                       canvas->drawRectangleEx(rgbBorderColor, rgbBackgroundColor, (LONG)x, (LONG)y, (LONG)width, (LONG)height, (LONG)borderSize, penStyle, brushStyle, brushHatchStyle, rotationAngle);
                                                    } else {
                                                        drawLeftFrame = FALSE;
                                                        drawTopFrame = FALSE;
                                                        drawRightFrame = FALSE;
                                                        drawBottomFrame = FALSE;
                                                        if (subitemProp->value == L"RectLBR") {
                                                            drawLeftFrame = TRUE;
                                                            drawBottomFrame = TRUE;
                                                            drawRightFrame = TRUE;
                                                        } else if (subitemProp->value == L"RectLB") {
                                                            drawLeftFrame = TRUE;
                                                            drawBottomFrame = TRUE;
                                                        } else if (subitemProp->value == L"RectBR") {
                                                            drawBottomFrame = TRUE;
                                                            drawRightFrame = TRUE;
                                                        }

                                                        if (drawLeftFrame || drawTopFrame || drawRightFrame || drawBottomFrame) {
                                                            canvas->drawRectangleBorder(rgbBorderColor, (LONG)x, (LONG)y, (LONG)width, (LONG)height, (LONG)borderSize, penStyle, drawLeftFrame, drawTopFrame, drawRightFrame, drawBottomFrame, rotationAngle);
                                                        }
                                                    }
                                                    continue;
                                                } else if (subitemProp->value == L"Text") {
                                                    font = this->getTextProperties(subitem, pageWidth, pageHeight,
                                                        pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom,
                                                        &x, &y, &width, &height,
                                                        &text, &dxTextOffset, &dyTextOffset,
                                                        &fontName, &fontSize, &fontStyle, &fontColor,
                                                        &borderSize, &borderStyle, &borderColor,
                                                        &backgroundColor, &backgroundStyle, &backgroundHatchStyle,
                                                        &charSpacing, &rotationAngle, &align, &valign,
                                                        &leftPadding, &topPadding, &rightPadding, &bottomPadding);
                                                    if (font) {
                                                        RECT rc;
                                                        rc.left = (LONG)x;
                                                        rc.top = (LONG)y;
                                                        rc.right = (LONG)(x+width + leftPadding + rightPadding);
                                                        rc.bottom = (LONG)(y+height + topPadding + bottomPadding);
                                                        penStyle = this->stringStyleToPenStyle(borderStyle);
                                                        brushStyle = this->stringStyleToBrushStyle(backgroundStyle);
                                                        brushHatchStyle = this->stringStyleToBrushHatchStyle(backgroundHatchStyle);
                                                        LONG extraFormat = 0;
                                                        if (align == L"left") { extraFormat |= DT_LEFT; }
                                                        if (align == L"right") { extraFormat |= DT_RIGHT; }
                                                        if (align == L"center") { extraFormat |= DT_CENTER; }
                                                        if (valign == L"top") { extraFormat |= DT_TOP; }
                                                        if (valign == L"bottom") { extraFormat |= DT_BOTTOM; }
                                                        if (valign == L"center") { extraFormat |= DT_VCENTER; }
                                                        canvas->drawText(font, HexToRGB(backgroundColor), text, (LONG)charSpacing, rc, (LONG)dxTextOffset, (LONG)dyTextOffset, (LONG)leftPadding, (LONG)topPadding, (LONG)rightPadding, (LONG)bottomPadding, rotationAngle, (LONG)borderSize, penStyle, HexToRGB(borderColor), brushStyle, brushHatchStyle, extraFormat, -1);
                                                    }
                                                    continue;
                                                } else if (subitemProp->value == L"Line" || subitemProp->value == L"HLine" || subitemProp->value == L"VLine") {
                                                    this->getLineProperties(subitem,
                                                        pageWidth, pageHeight,
                                                        pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom,
                                                        &x, &y, &x2, &y2, &thickness, &style, &color, &rotationAngle);
                                                    rgbColor = HexToRGB(color);
                                                    penStyle = this->stringStyleToPenStyle(style);
                                                    canvas->drawLine(rgbColor, (LONG)x, (LONG)y, (LONG)x2, (LONG)y2, (LONG)thickness, penStyle, rotationAngle);
                                                    continue;
                                                } else if (subitemProp->value == L"DateField") {
                                                    font = this->getDateFieldProperties(subitem,
                                                        pageWidth, pageHeight,
                                                        pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom,
                                                        &x, &y, &width, &height,
                                                        &year, &month, &day, &groupSpacing, &fieldHeight,
                                                        &fontName, &fontSize, &fontColor, &fontStyle,
                                                        &borderSize, &borderColor, &borderStyle,
                                                        &backgroundColor, &backgroundStyle, &backgroundHatchStyle,
                                                        &rotationAngle);
                                                    penStyle = this->stringStyleToPenStyle(borderStyle);
                                                    brushStyle = this->stringStyleToBrushStyle(backgroundStyle);
                                                    brushHatchStyle = this->stringStyleToBrushHatchStyle(backgroundHatchStyle);
                                                    canvas->drawDateFieldDDMMYYYY(font, year, month, day,
                                                        (LONG)x, (LONG)y, (LONG)width, (LONG)height, (LONG)groupSpacing, (LONG)fieldHeight,
                                                        HexToRGB(borderColor), (LONG)borderSize, penStyle,
                                                        HexToRGB(backgroundColor), brushStyle, brushHatchStyle,
                                                        rotationAngle);
                                                    continue;
                                                } else if (subitemProp->value == L"InputField") {
                                                    this->getInputFieldProperties(subitem,
                                                        pageWidth, pageHeight,
                                                        pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom,
                                                        &x, &y, &width, &height,
                                                        &label, &text, &eachTextLetterInSeparateBox,
                                                        &labelFontName, &labelFontSize, &labelFontColor, &labelFontStyle,
                                                        &fontName, &fontSize, &fontColor, &fontStyle,
                                                        &labelOffsetY, &labelPaddingLeft, &labelPaddingTop, &labelPaddingRight, &labelPaddingBottom,
                                                        &labelLineBorderSize, &labelLineBorderStyle, &labelLineBorderColor,
                                                        &labelLineBackgroundColor, &labelLineBackgroundStyle, &labelLineBackgroundHatchStyle,
                                                        &borderSize, &borderStyle, &borderColor,
                                                        &backgroundColor, &backgroundStyle, &backgroundHatchStyle,
                                                        &textFieldBackgroundColor,
                                                        &textFieldBoxWidth, &textFieldSeparatorHeight, &textFieldSeparatorThickness,
                                                        &rotationAngle,
                                                        &font, &labelFont);
                                                        
                                                    penStyle = this->stringStyleToPenStyle(borderStyle);
                                                    brushStyle = this->stringStyleToBrushStyle(backgroundStyle);
                                                    brushHatchStyle = this->stringStyleToBrushHatchStyle(backgroundHatchStyle);
                                                    labelPenStyle = this->stringStyleToPenStyle(labelLineBorderStyle);
                                                    labelBrushStyle = this->stringStyleToBrushStyle(labelLineBackgroundStyle);
                                                    labelBrushHatchStyle = this->stringStyleToBrushHatchStyle(labelLineBackgroundHatchStyle);
                                                    canvas->drawLabeledTextField(font, labelFont, (LONG)x, (LONG)y, (LONG)width, (LONG)height,
                                                        label, text, eachTextLetterInSeparateBox,
                                                        (LONG)labelOffsetY, (LONG)labelPaddingLeft, (LONG)labelPaddingTop, (LONG)labelPaddingRight, (LONG)labelPaddingBottom,
                                                        HexToRGB(labelLineBorderColor), (LONG)labelLineBorderSize, labelPenStyle,
                                                        HexToRGB(labelLineBackgroundColor), labelBrushStyle, labelBrushHatchStyle,
                                                        HexToRGB(borderColor), (LONG)borderSize, penStyle,
                                                        HexToRGB(backgroundColor), brushStyle, brushHatchStyle,
                                                        HexToRGB(textFieldBackgroundColor),
                                                        (LONG)textFieldBoxWidth, (LONG)textFieldSeparatorHeight, (LONG)textFieldSeparatorThickness,
                                                        rotationAngle);
                                                    continue;
                                                } else if (subitemProp->value == L"TextInRects") {
                                                    font = this->getTextInRectsProperties(subitem,
                                                        pageWidth, pageHeight,
                                                        pageMarginLeft, pageMarginTop, pageMarginRight, pageMarginBottom,
                                                        &x, &y, &width, &height,
                                                        &text,
                                                        &fontName, &fontSize, &fontColor, &fontStyle,
                                                        &borderSize, &borderStyle, &borderColor,
                                                        &backgroundColor, &backgroundStyle, &backgroundHatchStyle,
                                                        &labelLineBorderSize, &labelLineBorderStyle, &labelLineBorderColor,
                                                        &labelLineBackgroundColor, &labelLineBackgroundStyle, &labelLineBackgroundHatchStyle,
                                                        &unitWidth, &unitHeight, &unitPadding, &spacing, &drawEmptyFields,
                                                        &rotationAngle);
                                                        
                                                    penStyle = this->stringStyleToPenStyle(borderStyle);
                                                    brushStyle = this->stringStyleToBrushStyle(backgroundStyle);
                                                    brushHatchStyle = this->stringStyleToBrushHatchStyle(backgroundHatchStyle);
                                                    labelPenStyle = this->stringStyleToPenStyle(labelLineBorderStyle);
                                                    labelBrushStyle = this->stringStyleToBrushStyle(labelLineBackgroundStyle);
                                                    labelBrushHatchStyle = this->stringStyleToBrushHatchStyle(labelLineBackgroundHatchStyle);
                                                    canvas->drawTextInRectangles(font, (LONG)x, (LONG)y, (LONG)width, (LONG)height,
                                                        text, HexToRGB(borderColor), (LONG)borderSize, penStyle,
                                                        HexToRGB(backgroundColor), brushStyle, brushHatchStyle,
                                                        HexToRGB(labelLineBorderColor), (LONG)labelLineBorderSize, labelPenStyle,
                                                        HexToRGB(labelLineBackgroundColor), labelBrushStyle, labelBrushHatchStyle,
                                                        (LONG)unitWidth, (LONG)unitHeight, (LONG)unitPadding, (LONG)spacing, drawEmptyFields,
                                                        rotationAngle);
                                                    continue;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (canvas->getDC() != NULL) {
                        canvas->endPaint();
                    }
                }
            }
		}
    }

    this->_canvasOperatedOn = NULL;
    return result;
}

LONG CmzCanvasFile::stringStyleToPenStyle(CmzString style) {
    if (style == L"solid") {
        return PS_SOLID;
    }
    if (style == L"transparent") {
        return PS_NULL;
    }
    if (style == L"dash") {
        return PS_DASH;
    }
    if (style == L"dot") {
        return PS_DOT;
    }
    if (style == L"dashdot") {
        return PS_DASHDOT;
    }
    if (style == L"dashdotdot") {
        return PS_DASHDOTDOT;
    }
    if (style == L"insideframe") {
        return PS_INSIDEFRAME;
    }
    return PS_SOLID;
}

ULONG CmzCanvasFile::stringStyleToBrushStyle(CmzString style) {
    if (style == L"solid") {
        return BS_SOLID;
    }
    if (style == L"transparent") {
        return BS_NULL;
    }
    if (style == L"dibpattern") {
        return BS_DIBPATTERN;
    }
    if (style == L"dibpattern8x8") {
        return BS_DIBPATTERN8X8;
    }
    if (style == L"dibpatternpt") {
        return BS_DIBPATTERNPT;
    }
    if (style == L"hatched") {
        return BS_HATCHED;
    }
    if (style == L"hollow") {
        return BS_HOLLOW;
    }
    if (style == L"pattern") {
        return BS_PATTERN;
    }
    if (style == L"pattern8x8") {
        return BS_PATTERN8X8;
    }
    return BS_SOLID;
}

ULONG_PTR CmzCanvasFile::stringStyleToBrushHatchStyle(CmzString style) {
    if (style == L"bdiagonal") {
        return HS_BDIAGONAL;
    }
    if (style == L"cross") {
        return HS_CROSS;
    }
    if (style == L"diagcross") {
        return HS_DIAGCROSS;
    }
    if (style == L"fdiagonal") {
        return HS_FDIAGONAL;
    }
    if (style == L"horizontal") {
        return HS_HORIZONTAL;
    }
    if (style == L"vertical") {
        return HS_VERTICAL;
    }
    return HS_BDIAGONAL;
}

VOID CmzCanvasFile::getPageProperties(CmzStdObjectProperty* itemPage, float* width, float* height, float* marginLeft, float* marginTop, float* marginRight, float* marginBottom, CmzString* munits) {
    if (width) { *width = 0; }
    if (height) { *height = 0; }
    if (munits) { *munits = L"px"; }
    if (marginLeft) { *marginLeft = 0; }
    if (marginRight) { *marginRight = 0; }
    if (marginTop) { *marginTop = 0; }
    if (marginBottom) { *marginBottom = 0; }

    if (!itemPage) {
        return;
    }

    CmzStdObject* page;
    CmzStdObjectProperty* item;
    if (itemPage && itemPage->type == STDOBJ_PROPERTY_OBJECT) {
        page = itemPage->oValue;
        item = page->get(L"name");
        if (item && item->value == L"Page") {
            if (width) {
                item = page->get(L"width");
                if (item) { *width = (float)page->getAsDouble(L"width"); }
            }

            if (height) {
                item = page->get(L"height");
                if (item) { *height = (float)page->getAsDouble(L"height"); }
            }

            if (munits) {
                item = page->get(L"munits");
                if (item) { *munits = page->getAsString(L"munits"); }
            }

            if (marginLeft || marginRight || marginTop || marginBottom) {
                item = page->get(L"margin");
                if (item) {
                    if (marginLeft) { *marginLeft = (float)page->getAsDouble(L"margin"); }
                    if (marginRight) { *marginRight = *marginLeft; }
                    if (marginTop) { *marginTop = *marginLeft; }
                    if (marginBottom) { *marginBottom = *marginLeft; }
                }
                item = page->get(L"margin-left");
                if (item) {
                    if (marginLeft) { *marginLeft = (float)page->getAsDouble(L"margin-left"); }
                }
                item = page->get(L"margin-right");
                if (item) {
                    if (marginRight) { *marginRight = (float)page->getAsDouble(L"margin-right"); }
                }
                item = page->get(L"margin-top");
                if (item) {
                    if (marginTop) { *marginTop = (float)page->getAsDouble(L"margin-top"); }
                }
                item = page->get(L"margin-bottom");
                if (item) {
                    if (marginBottom) { *marginBottom = (float)page->getAsDouble(L"margin-bottom"); }
                }
            }
        }
    }
}

VOID CmzCanvasFile::getBorderProperties(CmzString prefix, CmzStdObjectProperty* itemShape,
                float* borderSize, CmzString* borderStyle, CmzString* borderColor) {
    if (!itemShape) {
        return;
    }

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    if (prefix != COMMON_EMPTY_STRING) {
        prefix += L'-';
    }

    shapeProp = shape->get(prefix + L"border-size");
    if (shapeProp) {
        *borderSize = (float)shape->getAsDouble(prefix + L"border-size");
        if (*borderStyle == COMMON_EMPTY_STRING) {
            if (*borderSize > 0.1) {
                *borderStyle = L"solid";
            } else {
                *borderStyle = L"transparent";
            }
        }
    }

    shapeProp = shape->get(prefix + L"border-style");
    if (shapeProp) {
        *borderStyle = shape->getAsString(prefix + L"border-style");
    }

    shapeProp = shape->get(prefix + L"border-color");
    if (shapeProp) {
        *borderColor = shape->getAsString(prefix + L"border-color");
    }
}

VOID CmzCanvasFile::getBackgroundProperties(CmzString prefix, CmzStdObjectProperty* itemShape,
                CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle) {
    if (!itemShape) {
        return;
    }

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    if (prefix != COMMON_EMPTY_STRING) {
        prefix += L'-';
    }

    shapeProp = shape->get(prefix + L"background-color");
    if (shapeProp) {
        *backgroundColor = shape->getAsString(prefix + L"background-color");
        *backgroundStyle = L"solid";
    }

    shapeProp = shape->get(prefix + L"background-style");
    if (shapeProp) {
        *backgroundStyle = shape->getAsString(prefix + L"background-style");
    }

    shapeProp = shape->get(prefix + L"background-hatch-style");
    if (shapeProp) {
        *backgroundHatchStyle = shape->getAsString(prefix + L"background-hatch-style");
    }
}

VOID CmzCanvasFile::getCommonShapeProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
                    float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
                    float* x, float* y, float* width, float* height,
                    float* borderSize, CmzString* borderStyle, CmzString* borderColor, float* rotationAngle) {
    if (!itemShape) {
        return;
    }

    this->getBorderProperties(COMMON_EMPTY_STRING, itemShape, borderSize, borderStyle, borderColor);

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;
    CmzStdObjectProperty* property;
    CmzString value;
    float parentClientWidth = parentWidth - parentMarginLeft - parentMarginRight;
    float parentClientHeight = parentHeight - parentMarginTop - parentMarginBottom;

    shapeProp = shape->get(L"x");
    if (shapeProp) {
        *x = (float)shape->getAsDouble(L"x");
        *x += parentMarginLeft;
    }

    shapeProp = shape->get(L"y");
    if (shapeProp) {
        *y = (float)shape->getAsDouble(L"y");
        *y += parentMarginTop;
    }

    shapeProp = shape->get(L"width");
    if (shapeProp) {
        property = shape->get(L"width");
        if (property) {
            if (property->type == STDOBJ_PROPERTY_VALUE_INT || property->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
                *width = (float)shape->getAsDouble(L"width");
            } else if (property->type == STDOBJ_PROPERTY_VALUE) {
                value = shape->getAsString(L"width");
                value.remove(L'%');
                *width = (float)value.toDouble() * parentClientWidth / 100;
            }
        }
    }

    shapeProp = shape->get(L"height");
    if (shapeProp) {
        property = shape->get(L"height");
        if (property) {
            if (property->type == STDOBJ_PROPERTY_VALUE_INT || property->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
                *height = (float)shape->getAsDouble(L"height");
            } else if (property->type == STDOBJ_PROPERTY_VALUE) {
                value = shape->getAsString(L"height");
                value.remove(L'%');
                *height = (float)value.toDouble() * parentClientHeight / 100;
            }
        }
    }

    shapeProp = shape->get(L"rotation-angle");
    if (shapeProp) {
        *rotationAngle = (float)shape->getAsDouble(L"rotation-angle");
    }
}

CmzFont* CmzCanvasFile::getFontProperties(CmzString prefix, CmzStdObjectProperty* itemShape, CmzString* fontName, float* fontSize,
                                    CmzString* fontColor, CmzString* fontStyle) {
    CmzStdObjectProperty* shapeProp;
    *fontName = L"Arial";
    *fontSize = 10;
    *fontStyle = COMMON_EMPTY_STRING;
    *fontColor = "#000000";
    if (!itemShape) {
        return NULL;
    }

    CmzStdObject* shape = itemShape->oValue;
    CmzString value;

    if (prefix != COMMON_EMPTY_STRING) {
        prefix += L'-';
    }

    shapeProp = shape->get(prefix + L"font-name");
    if (shapeProp) {
        *fontName = shape->getAsString(prefix + L"font-name");
    }

    shapeProp = shape->get(prefix + L"font-size");
    if (shapeProp) {
        *fontSize = (float)shape->getAsDouble(prefix + L"font-size");
    }

    shapeProp = shape->get(prefix + L"font-color");
    if (shapeProp) {
        *fontColor = shape->getAsString(prefix + L"font-color");
    }

    shapeProp = shape->get(prefix + L"font-style");
    if (shapeProp) {
        *fontStyle = shape->getAsString(prefix + L"font-style");
    }

    COLORREF rgbFontColor = HexToRGB(*fontColor);
    CmzFont* font = this->getCachedFont(fontName->getBuffer(), (LONG)*fontSize, rgbFontColor);
    if (!font && (*fontName != COMMON_EMPTY_STRING) && (*fontSize > 0)) {
        font = new CmzFont(this->_canvasOperatedOn->getDC(), rgbFontColor, *fontName, (LONG)*fontSize);
        this->_cachedFonts->push(font);
    }

    return font;
}

VOID CmzCanvasFile::getRectProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
                    float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
                    float* x, float* y, float* width, float* height,
                    float* borderSize, CmzString* borderStyle, CmzString* borderColor,
                    CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
                    float* rotationAngle) {
    *x = 0;
    *y = 0;
    *width = 0;
    *height = 0;
    *borderColor = L"#000000";
    *borderSize = 1;
    *borderStyle = COMMON_EMPTY_STRING;
    *rotationAngle = 0;
    *backgroundColor = COMMON_EMPTY_STRING;
    *backgroundStyle = COMMON_EMPTY_STRING;
    *backgroundHatchStyle = COMMON_EMPTY_STRING;
    if (!itemShape) {
        return;
    }

    this->getCommonShapeProperties(itemShape, parentWidth, parentHeight,
                    parentMarginLeft, parentMarginTop, parentMarginRight, parentMarginBottom,
                    x, y, width, height, borderSize, borderStyle, borderColor, rotationAngle);
    this->getBackgroundProperties(COMMON_EMPTY_STRING, itemShape, backgroundColor, backgroundStyle, backgroundHatchStyle);
}

CmzFont* CmzCanvasFile::getTextProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
                float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
                float* x, float* y, float* width, float* height,
                CmzString* text, float* dxTextOffset, float* dyTextOffset,
                CmzString* fontName, float* fontSize, CmzString* fontStyle, CmzString* fontColor,
                float* borderSize, CmzString* borderStyle, CmzString* borderColor,
                CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
                float* charSpacing, float* rotationAngle, CmzString* align, CmzString* valign,
                float* leftPadding, float* topPadding, float* rightPadding, float* bottomPadding) {
    *x = 0;
    *y = 0;
    *width = 0;
    *height = 0;
    *borderColor = COMMON_EMPTY_STRING;
    *fontColor = L"#000000";
    *borderSize = 0;
    *borderStyle = COMMON_EMPTY_STRING;
    *rotationAngle = 0;
    *align = L"left";
    *valign = L"top";
    *leftPadding = 0;
    *topPadding = 0;
    *rightPadding = 0;
    *bottomPadding = 0;
    *text = COMMON_EMPTY_STRING;
    *backgroundColor = COMMON_EMPTY_STRING;
    *backgroundStyle = L"transparent";
    *backgroundHatchStyle = COMMON_EMPTY_STRING;
    *charSpacing = 0;
    *dxTextOffset = 0;
    *dyTextOffset = 0;
    if (!itemShape) {
        return NULL;
    }

    this->getCommonShapeProperties(itemShape, parentWidth, parentHeight,
                    parentMarginLeft, parentMarginTop, parentMarginRight, parentMarginBottom,
                    x, y, width, height, borderSize, borderStyle, borderColor, rotationAngle);
    CmzFont* font = this->getFontProperties(COMMON_EMPTY_STRING, itemShape, fontName, fontSize, fontColor, fontStyle);
    this->getBackgroundProperties(COMMON_EMPTY_STRING, itemShape, backgroundColor, backgroundStyle, backgroundHatchStyle);

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    shapeProp = shape->get(L"text");
    if (shapeProp) {
        *text = shape->getAsString(L"text");
    }

    shapeProp = shape->get(L"char-spacing");
    if (shapeProp) {
        *charSpacing = (float)shape->getAsDouble(L"char-spacing");
    }

    shapeProp = shape->get(L"align");
    if (shapeProp) {
        *align = shape->getAsString(L"align");
    }

    shapeProp = shape->get(L"valign");
    if (shapeProp) {
        *valign = shape->getAsString(L"valign");
    }

    shapeProp = shape->get(L"padding");
    if (shapeProp) {
        *leftPadding = (float)shape->getAsDouble(L"padding");
        *topPadding = *leftPadding;
        *rightPadding = *leftPadding;
        *bottomPadding = *leftPadding;
    }

    shapeProp = shape->get(L"left-padding");
    if (shapeProp) {
        *leftPadding = (float)shape->getAsDouble(L"left-padding");
    }

    shapeProp = shape->get(L"top-padding");
    if (shapeProp) {
        *topPadding = (float)shape->getAsDouble(L"top-padding");
    }

    shapeProp = shape->get(L"right-padding");
    if (shapeProp) {
        *rightPadding = (float)shape->getAsDouble(L"right-padding");
    }

    shapeProp = shape->get(L"bottom-padding");
    if (shapeProp) {
        *bottomPadding = (float)shape->getAsDouble(L"bottom-padding");
    }

    shapeProp = shape->get(L"dx-text-offset");
    if (shapeProp) {
        *dxTextOffset = (float)shape->getAsDouble(L"dx-text-offset");
    }

    shapeProp = shape->get(L"dy-text-offset");
    if (shapeProp) {
        *dyTextOffset = (float)shape->getAsDouble(L"dy-text-offset");
    }

    if (*width == 0 && *height == 0 && font) {
        SIZE textSize;
        HDC hdc = 0;
        if (this->_canvasOperatedOn) {
            hdc = this->_canvasOperatedOn->getDC();
        }

        if (fabs(*charSpacing) > 0.01) {
            textSize = font->measureString(*text, (LONG)*charSpacing, hdc);
        } else {
            textSize = font->measureString(*text, hdc);
        }
        *width = (float)textSize.cx;
        *height = (float)textSize.cy;
    }

    return font;
}

VOID CmzCanvasFile::getLineProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
                float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
                float* x, float* y, float* x2, float* y2, float* thickness, CmzString* style, CmzString* color,
                float* rotationAngle) {
    float width, height, unusedFloat;
    CmzString unusedString;
    this->getCommonShapeProperties(itemShape, parentWidth, parentHeight,
                    parentMarginLeft, parentMarginTop, parentMarginRight, parentMarginBottom,
                    x, y, &width, &height,
                    &unusedFloat, &unusedString, &unusedString, rotationAngle);

    *x2 = 0;
    *y2 = 0;
    *color = L"#000000";
    *thickness = 1;
    *style = L"solid";
    if (!itemShape) {
        return;
    }

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    shapeProp = shape->get(L"thickness");
    if (shapeProp) {
        *thickness = (float)shape->getAsDouble(L"thickness");
    }

    shapeProp = shape->get(L"style");
    if (shapeProp) {
        *style = shape->getAsString(L"style");
    }

    shapeProp = shape->get(L"color");
    if (shapeProp) {
        *color = shape->getAsString(L"color");
    }

    shapeProp = shape->get(L"x2");
    if (shapeProp) {
        *x2 = (float)shape->getAsDouble(L"x2");
        *x2 += parentMarginLeft;
    }

    shapeProp = shape->get(L"y2");
    if (shapeProp) {
        *y2 = (float)shape->getAsDouble(L"y2");
        *y2 += parentMarginTop;
    }

    if (width > 0.1) {
        *x2 = *x + width;
        *y2 = *y;
    }

    if (height > 0.1) {
        *y2 = *y + height;
        *x2 = *x;
    }
}

CmzFont* CmzCanvasFile::getDateFieldProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
                float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
                float* x, float* y, float* width, float* height,
                CmzString* year, CmzString* month, CmzString *day, float* groupSpacing, float* fieldHeight,
                CmzString* fontName, float* fontSize, CmzString* fontColor, CmzString* fontStyle,
                float* borderSize, CmzString* borderColor, CmzString* borderStyle,
                CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
                float* rotationAngle) {
    *x = 0;
    *y = 0;
    *width = 150;
    *height = 30;
    *fontColor = L"#000000";
    *borderColor = L"#000000";
    *borderSize = 1;
    *borderStyle = L"solid";
    *rotationAngle = 0;
    *backgroundColor = COMMON_EMPTY_STRING;
    *backgroundStyle = L"transparent";
    *backgroundHatchStyle = COMMON_EMPTY_STRING;
    *year = COMMON_EMPTY_STRING;
    *month = COMMON_EMPTY_STRING;
    *day = COMMON_EMPTY_STRING;
    *groupSpacing = 4;
    *fieldHeight = 4;
    if (!itemShape) {
        return NULL;
    }

    this->getCommonShapeProperties(itemShape, parentWidth, parentHeight,
                    parentMarginLeft, parentMarginTop, parentMarginRight, parentMarginBottom,
                    x, y, width, height, borderSize, borderStyle, borderColor, rotationAngle);
    CmzFont* font = this->getFontProperties(COMMON_EMPTY_STRING, itemShape, fontName, fontSize, fontColor, fontStyle);
    this->getBackgroundProperties(COMMON_EMPTY_STRING, itemShape, backgroundColor, backgroundStyle, backgroundHatchStyle);

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    shapeProp = shape->get(L"year");
    if (shapeProp) {
        *year = shape->getAsInt(L"year");
    }

    shapeProp = shape->get(L"month");
    if (shapeProp) {
        *month = shape->getAsInt(L"month");
    }

    shapeProp = shape->get(L"day");
    if (shapeProp) {
        *day = shape->getAsInt(L"day");
    }

    shapeProp = shape->get(L"group-spacing");
    if (shapeProp) {
        *groupSpacing = (float)shape->getAsDouble(L"group-spacing");
    }

    shapeProp = shape->get(L"field_height");
    if (shapeProp) {
        *fieldHeight = (float)shape->getAsDouble(L"field-height");
    }

    return font;
}

VOID CmzCanvasFile::getInputFieldProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
    float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
    float* x, float* y, float* width, float* height,
    CmzString* label, CmzString* text, BOOL* eachTextLetterInSeparateBox,
    CmzString* labelFontName, float* labelFontSize, CmzString* labelFontColor, CmzString* labelFontStyle,
    CmzString* textFontName, float* textFontSize, CmzString* textFontColor, CmzString* textFontStyle,
    float* labelOffsetY, float* labelPaddingLeft, float* labelPaddingTop, float* labelPaddingRight, float* labelPaddingBottom,
    float* labelLineBorderSize, CmzString* labelLineBorderStyle, CmzString* labelLineBorderColor,
    CmzString* labelLineBackgroundColor, CmzString* labelLineBackgroundStyle, CmzString* labelLineBackgroundHatchStyle,
    float* borderSize, CmzString* borderStyle, CmzString* borderColor,
    CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
    CmzString* textFieldBackgroundColor,
    float* textFieldBoxCount, float* textFieldSeparatorHeight, float* textFieldSeparatorThickness,
    float* rotationAngle,
    CmzFont** textFont, CmzFont** labelFont) {

    *x = 0;
    *y = 0;
    *width = 150;
    *height = 30;
    *label = COMMON_EMPTY_STRING;
    *text = COMMON_EMPTY_STRING;
    *eachTextLetterInSeparateBox = FALSE;
    *labelFontName = L"Araial";
    *labelFontSize = 10;
    *labelFontColor = L"#000000";
    *labelFontStyle = COMMON_EMPTY_STRING;
    *textFontName = L"Araial";
    *textFontSize = 10;
    *textFontColor = L"#000000";
    *textFontStyle = COMMON_EMPTY_STRING;

    *labelOffsetY = 0;
    *labelPaddingLeft = 0;
    *labelPaddingTop = 0;
    *labelPaddingRight = 0;
    *labelPaddingBottom = 0;
    *labelLineBorderSize = 1;
    *labelLineBorderStyle = L"solid";
    *labelLineBorderStyle = L"transparent";
    *labelLineBorderColor = L"#000000";
    *labelLineBackgroundColor = L"#ffffff";
    *labelLineBackgroundStyle = L"transparent";
    *labelLineBackgroundHatchStyle = COMMON_EMPTY_STRING;

    *borderSize = 1;
    *borderStyle = L"solid";
    *borderColor = L"#000000";
    *backgroundColor = L"#ffffff";
    *backgroundStyle = L"solid";
    *backgroundHatchStyle = COMMON_EMPTY_STRING;

    *textFieldBackgroundColor = L"ffffff";
    *textFieldBoxCount = 10;
    *textFieldSeparatorHeight = 4;
    *textFieldSeparatorThickness = 1;
    *rotationAngle = 0;

    *labelFont = NULL;
    *textFont = NULL;

    if (!itemShape) {
        return;
    }

    this->getCommonShapeProperties(itemShape, parentWidth, parentHeight,
                    parentMarginLeft, parentMarginTop, parentMarginRight, parentMarginBottom,
                    x, y, width, height, borderSize, borderStyle, borderColor, rotationAngle);
    *textFont = this->getFontProperties(COMMON_EMPTY_STRING, itemShape, textFontName, textFontSize, textFontColor, textFontStyle);
    *labelFont = this->getFontProperties(L"label", itemShape, labelFontName, labelFontSize, labelFontColor, labelFontStyle);
    this->getBorderProperties(L"label", itemShape, labelLineBorderSize, labelLineBorderStyle, labelLineBorderColor);
    this->getBackgroundProperties(L"label", itemShape, labelLineBackgroundColor, labelLineBackgroundStyle, labelLineBackgroundHatchStyle);
    this->getBackgroundProperties(COMMON_EMPTY_STRING, itemShape, backgroundColor, backgroundStyle, backgroundHatchStyle);

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    shapeProp = shape->get(L"text");
    if (shapeProp) {
        *text = shape->getAsString(L"text");
    }

    shapeProp = shape->get(L"label");
    if (shapeProp) {
        *label = shape->getAsString(L"label");
    }

    shapeProp = shape->get(L"text-box-item-count");
    if (shapeProp) {
        *textFieldBoxCount = (float)shape->getAsDouble(L"text-box-item-count");
    }

    shapeProp = shape->get(L"text-box-separator-height");
    if (shapeProp) {
        *textFieldSeparatorHeight = (float)shape->getAsInt(L"text-box-separator-height");
    }

    shapeProp = shape->get(L"text-box-separator-thickness");
    if (shapeProp) {
        *textFieldSeparatorThickness = (float)shape->getAsInt(L"text-box-separator-thickness");
    }

    shapeProp = shape->get(L"text-box-background-color");
    if (shapeProp) {
        *textFieldBackgroundColor = shape->getAsString(L"text-box-background-color");
    }

    shapeProp = shape->get(L"text-box-separate-letters");
    if (shapeProp) {
        *eachTextLetterInSeparateBox = static_cast<BOOL>(shape->getAsInt(L"text-box-separate-letters"));
    }
    
    shapeProp = shape->get(L"label-padding");
    if (shapeProp) {
        *labelPaddingLeft = (float)shape->getAsDouble(L"label-padding");
        *labelPaddingTop = *labelPaddingLeft;
        *labelPaddingRight = *labelPaddingLeft;
        *labelPaddingBottom = *labelPaddingLeft;
    }

    shapeProp = shape->get(L"label-padding-left");
    if (shapeProp) {
        *labelPaddingLeft = (float)shape->getAsDouble(L"label-padding-left");
    }

    shapeProp = shape->get(L"label-padding-top");
    if (shapeProp) {
        *labelPaddingTop = (float)shape->getAsDouble(L"label-padding-top");
    }

    shapeProp = shape->get(L"label-padding-right");
    if (shapeProp) {
        *labelPaddingRight = (float)shape->getAsDouble(L"label-padding-right");
    }

    shapeProp = shape->get(L"label-padding-bottom");
    if (shapeProp) {
        *labelPaddingBottom = (float)shape->getAsDouble(L"label-padding-bottom");
    }

    shapeProp = shape->get(L"label-offset-y");
    if (shapeProp) {
        *labelOffsetY = (float)shape->getAsDouble(L"label-offset-y");
    }

    if ((*label != COMMON_EMPTY_STRING) && (*labelFont == NULL)) {
        if ((*textFont)->size - 2 > 0) {
            *labelFont = this->getCachedFont(textFontName->getBuffer(), (*textFont)->size-2, (*textFont)->color);
            if (*labelFont == NULL) {
                *labelFont = new CmzFont(this->_canvasOperatedOn->getDC(), (*textFont)->color, *textFontName, (*textFont)->size-2);
                this->_cachedFonts->push(labelFont);
            }
        }
    }
}

CmzFont* CmzCanvasFile::getTextInRectsProperties(CmzStdObjectProperty* itemShape, float parentWidth, float parentHeight,
    float parentMarginLeft, float parentMarginTop, float parentMarginRight, float parentMarginBottom,
    float* x, float* y, float* width, float* height,
    CmzString* text,
    CmzString* textFontName, float* textFontSize, CmzString* textFontColor, CmzString* textFontStyle,
    float* borderSize, CmzString* borderStyle, CmzString* borderColor,
    CmzString* backgroundColor, CmzString* backgroundStyle, CmzString* backgroundHatchStyle,
    float* unitBorderSize, CmzString* unitBorderStyle, CmzString* unitBorderColor,
    CmzString* unitBackgroundColor, CmzString* unitBackgroundStyle, CmzString* unitBackgroundHatchStyle,
    float* unitWidth, float* unitHeight, float* unitPadding, float* spacing, BOOL* drawEmptyFields,
    float* rotationAngle) {
    
    *x = 0;
    *y = 0;
    *width = 150;
    *height = 30;
    *text = COMMON_EMPTY_STRING;
    *textFontName = L"Araial";
    *textFontSize = 10;
    *textFontColor = L"#000000";
    *textFontStyle = COMMON_EMPTY_STRING;
    
    *borderColor = L"#000000";
    *borderSize = 1;
    *borderStyle = L"solid";
    *backgroundColor = L"#ffffff";
    *backgroundStyle = L"solid";
    *backgroundHatchStyle = COMMON_EMPTY_STRING;
    
    *unitBorderColor = L"#000000";
    *unitBorderSize = 1;
    *unitBorderStyle = L"solid";
    *unitBackgroundColor = L"#ffffff";
    *unitBackgroundStyle = L"solid";
    *unitBackgroundHatchStyle = COMMON_EMPTY_STRING;
    
    *unitWidth = 20;
    *unitHeight = 20;
    *unitPadding = 2;
    *spacing = 4;
    *drawEmptyFields = TRUE;
    *rotationAngle = 0;
    
    if (!itemShape) {
        return NULL;
    }

    this->getCommonShapeProperties(itemShape, parentWidth, parentHeight,
                    parentMarginLeft, parentMarginTop, parentMarginRight, parentMarginBottom,
                    x, y, width, height, borderSize, borderStyle, borderColor, rotationAngle);
    CmzFont* font = this->getFontProperties(COMMON_EMPTY_STRING, itemShape, textFontName, textFontSize, textFontColor, textFontStyle);
    this->getBackgroundProperties(COMMON_EMPTY_STRING, itemShape, backgroundColor, backgroundStyle, backgroundHatchStyle);
    this->getBorderProperties(L"unit", itemShape, unitBorderSize, unitBorderStyle, unitBorderColor);
    this->getBackgroundProperties(L"unit", itemShape, unitBackgroundColor, unitBackgroundStyle, unitBackgroundHatchStyle);

    CmzStdObject* shape = itemShape->oValue;
    CmzStdObjectProperty* shapeProp;

    shapeProp = shape->get(L"text");
    if (shapeProp) {
        *text = shape->getAsString(L"text");
    }

    shapeProp = shape->get(L"unit-width");
    if (shapeProp) {
        *unitWidth = (float)shape->getAsDouble(L"unit-width");
    }

    shapeProp = shape->get(L"unit-height");
    if (shapeProp) {
        *unitHeight = (float)shape->getAsDouble(L"unit-height");
    }

    shapeProp = shape->get(L"unit-padding");
    if (shapeProp) {
        *unitPadding = (float)shape->getAsDouble(L"unit-padding");
    }

    shapeProp = shape->get(L"spacing");
    if (shapeProp) {
        *spacing = (float)shape->getAsDouble(L"spacing");
    }

    shapeProp = shape->get(L"draw-empty-fields");
    if (shapeProp) {
        *drawEmptyFields = static_cast<BOOL>(shape->getAsInt(L"draw-empty-fields"));
    }

    return font;
}

}
