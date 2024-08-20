#include "pch.h"
#include "mzCanvas.h"
#include "mzstd.h"
#include "mzstdgraphics.h"
#include "stdio.h"
#include "mzMath.h"

namespace mzstd {

VOID CmzCanvas::_commonInit(BOOL isPrintCanvas) {
    this->setRealClassName(L"CmzCanvas");
    this->_isPrintCanvas = isPrintCanvas;
    if (isPrintCanvas) {
        this->_width = GetDeviceCaps(this->_memDC, HORZRES);;
        this->_height = GetDeviceCaps(this->_memDC, VERTRES);
    }
}

CmzCanvas::CmzCanvas(DWORD width, DWORD height, COLORREF bgcolor)
    : CmzBitmap(width, height, bgcolor)
{
    this->_commonInit(FALSE);
}

CmzCanvas::CmzCanvas(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor, BOOL isPrintCanvas)
    : CmzBitmap(hdc)
{
    this->_commonInit(isPrintCanvas);
}

CmzCanvas::CmzCanvas(HDC hdc, BOOL isPrintCanvas) : CmzBitmap(hdc) {
    this->_commonInit(isPrintCanvas);
}

CmzCanvas::CmzCanvas() : CmzBitmap() {
    this->_commonInit(FALSE);
}

CmzCanvas::~CmzCanvas() {
    this->_cleanup();
}

VOID CmzCanvas::_cleanup() {
}

VOID CmzCanvas::drawDateFieldDDMMYYYY(CmzFont* font, CmzString year, CmzString month, CmzString day,
                                        LONG x, LONG y, LONG width, LONG height, LONG groupSpacing, LONG fieldHeight,
                                        COLORREF borderColor, LONG borderThickness, LONG borderStyle,
                                        COLORREF backgroundColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch,
                                        float rotationAngle) {
    LONG itemWidth = (width - 2*groupSpacing)/8;
    LONG by = y + height-1 - fieldHeight;

    this->fillRectangle(backgroundColor, x, y, width, height, rotationAngle);

    POINT points[14];
    points[0].x = x;  points[0].y = by;
    points[1].x = x;  points[1].y = by+fieldHeight;
    points[2].x = x + itemWidth;  points[2].y = by+fieldHeight;
    points[3].x = x + itemWidth;  points[3].y = by;
    points[4].x = x + itemWidth;  points[4].y = by+fieldHeight;
    points[5].x = x + 2*itemWidth;  points[5].y = by+fieldHeight;
    points[6].x = x + 2*itemWidth;  points[6].y = by;
    points[7].x = x + 2*itemWidth;  points[7].y = by+fieldHeight;
    this->drawPolyline(borderColor, points, 8, FALSE, borderThickness, borderStyle, rotationAngle);

    // group spacing
    points[0].x = x + 2*itemWidth + groupSpacing;  points[0].y = by;
    // second two element group field for month
    points[1].x = x + 2*itemWidth + groupSpacing;  points[1].y = by+fieldHeight;
    points[2].x = x + 2*itemWidth + groupSpacing + itemWidth;  points[2].y = by+fieldHeight;
    points[3].x = x + 2*itemWidth + groupSpacing + itemWidth;  points[3].y = by;
    points[4].x = x + 2*itemWidth + groupSpacing + itemWidth;  points[4].y = by+fieldHeight;
    points[5].x = x + 2*itemWidth + groupSpacing + 2*itemWidth;  points[5].y = by+fieldHeight;
    points[6].x = x + 2*itemWidth + groupSpacing + 2*itemWidth;  points[6].y = by;
    points[7].x = x + 2*itemWidth + groupSpacing + 2*itemWidth;  points[7].y = by+fieldHeight;
    this->drawPolyline(borderColor, points, 8, FALSE, borderThickness, borderStyle, rotationAngle);

    // group spacing
    points[0].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing;  points[0].y = by;
    // third two element group field for year
    points[1].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing;  points[1].y = by+fieldHeight;
    points[2].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + itemWidth;  points[2].y = by+fieldHeight;
    points[3].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + itemWidth;  points[3].y = by;
    points[4].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + itemWidth;  points[4].y = by+fieldHeight;
    points[5].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 2*itemWidth;  points[5].y = by+fieldHeight;
    points[6].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 2*itemWidth;  points[6].y = by;
    points[7].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 2*itemWidth;  points[7].y = by+fieldHeight;
    points[8].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 3*itemWidth;  points[8].y = by+fieldHeight;
    points[9].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 3*itemWidth;  points[9].y = by;
    points[10].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 3*itemWidth;  points[10].y = by+fieldHeight;
    if (x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 4*itemWidth >= width) {
        points[11].x = x + width - 1;  points[11].y = by+fieldHeight;
        points[12].x = x + width - 1;  points[12].y = by;
        points[13].x = x + width - 1;  points[13].y = by+fieldHeight;
    } else {
        points[11].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 4*itemWidth;  points[11].y = by+fieldHeight;
        points[12].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 4*itemWidth;  points[12].y = by;
        points[13].x = x + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + 4*itemWidth;  points[13].y = by+fieldHeight;
    }
    this->drawPolyline(borderColor, points, 14, FALSE, borderThickness, borderStyle, rotationAngle);

    size_t i, len = day.length();
    WCHAR str[2];
    ZeroMemory(str, 2*sizeof(WCHAR));
    SIZE size;
    if (len < 2) {
        day.prepend(L'0');
        len++;
    }
    
    if (font) {
        for (i = 0; i < len; i++) {
            str[0] = day.getCharAt(i);
            size = font->measureString(str, this->_memDC);
            this->drawText(font, str, (INT)(x + i*itemWidth + (itemWidth-size.cx) / 2), y + height - size.cy, TRUE, 0, -1);
        }
    }
    
    len = month.length();
    if (len < 2) {
        month.prepend(L'0');
        len++;
    }
    for (i = 0; i < len; i++) {
        str[0] = month.getCharAt(i);
        size = font->measureString(str, this->_memDC);
        this->drawText(font, str, (INT)(x + i*itemWidth + 2*itemWidth + groupSpacing + (itemWidth-size.cx) / 2), y + height - size.cy, TRUE, 0, -1);
    }


    len = year.length();
    while (len < 4) {
        year.prepend(L'0');
        len = year.length();
    }
    for (i = 0; i < len; i++) {
        str[0] = year.getCharAt(i);
        size = font->measureString(str, this->_memDC);
        this->drawText(font, str, (INT)(x + i*itemWidth + 2*itemWidth + groupSpacing + 2*itemWidth + groupSpacing + (itemWidth-size.cx) / 2), y + height - size.cy, TRUE, 0, -1);
    }
}

VOID CmzCanvas::drawLabeledTextField(CmzFont* font, CmzFont* labelFont, LONG x, LONG y, LONG width, LONG height,
    CmzString label, CmzString text, BOOL eachTextLetterInSeparateBox,
    LONG labelOffsetY, LONG labelPaddingLeft, LONG labelPaddingTop, LONG labelPaddingRight, LONG labelPaddingBottom,
    COLORREF labelLineBorderColor, LONG labelLineBorderThickness, LONG labelLineBorderStyle,
    COLORREF labelLineBackgroundColor, ULONG labelLineBackgroundStyle, ULONG_PTR labelLineBackgroundHatchStyle,
    COLORREF borderColor, LONG borderSize, LONG borderStyle,
    COLORREF backgroundColor, ULONG backgroundStyle, ULONG_PTR backgroundHatchStyle,
    COLORREF textFieldBackgroundColor,
    LONG textFieldBoxCount, LONG textFieldSeparatorHeight, LONG textFieldSeparatorThickness,
    float rotationAngle) {

    LONG labelTextWidth = 0;
    LONG labelTextHeight = 0;
    LONG labelTextHPadding = 0;
    LONG textWidth = 0;
    LONG textHeight = 0;
    SIZE size = {0, 0};
    LONG cx, cy;

    if (labelFont) {
        size = labelFont->measureString(label, this->_memDC);
        labelTextWidth = size.cx;
        labelTextHeight = size.cy;
        size = labelFont->measureString(L"A", this->_memDC);
        labelTextHPadding = size.cx / 2;
    }

    if (font) {
        size = font->measureString(text, this->_memDC);
        textWidth = size.cx;
        textHeight = size.cy;
    }

    // adding component background
    this->drawRectangleEx(borderColor, backgroundColor, x, y, width, height, borderSize, borderStyle, backgroundStyle, backgroundHatchStyle, rotationAngle);

    // label line bg
    RECT rc;
    if (labelFont && label != COMMON_EMPTY_STRING) {
        rc.left = x + 1;
        rc.top = y + 1 + labelOffsetY;
        rc.right = rc.left + width - 1;
        rc.bottom = rc.top + labelPaddingTop + labelPaddingBottom + labelTextHeight;
        this->drawText(labelFont, labelLineBackgroundColor, label, rc, 0, 0, labelPaddingLeft, labelPaddingTop, labelPaddingRight, labelPaddingBottom,
                       0, labelLineBorderThickness, labelLineBorderStyle, labelLineBorderColor,
                       labelLineBackgroundStyle, labelLineBackgroundHatchStyle, 0, -1);
    }

    BOOL drawText = (text != COMMON_EMPTY_STRING) && font;
    LONG textDY = 2;

    // text field bg
    LONG labelHeight = labelPaddingTop + labelTextHeight + labelPaddingBottom;
    LONG textFieldDY = labelHeight + labelOffsetY;
    cy = rc.bottom - 1;
    this->fillRectangle(textFieldBackgroundColor, x+1, cy, width - 2, height - textFieldDY - 1, rotationAngle);
    // now we add field separators at the bottom
    LONG textFieldBoxWidth = (width-2) / textFieldBoxCount;
    if (textFieldBoxWidth > 0.01) {
        LONG i;
        cy = y + height - textFieldSeparatorHeight;
        WCHAR str[2];
        str[1] = NULL;
        for (i = 1; i < textFieldBoxCount; i++) {
            cx = x+1 + i * textFieldBoxWidth;
            this->drawVLine(borderColor, cx, cy, textFieldSeparatorHeight, textFieldSeparatorThickness, PS_SOLID, rotationAngle);
            if (drawText && eachTextLetterInSeparateBox) {
                str[0] = text.getCharAt(i-1);
                size = font->measureString(str, this->_memDC);
                cx -= textFieldBoxWidth;
                this->drawText(font, str, cx + (textFieldBoxWidth - size.cx) / 2, y + height - textHeight - textDY, TRUE, 0, -1);
            }
        }
        
        if (drawText && eachTextLetterInSeparateBox && text.length() >= (size_t)i) {
            str[0] = text.getCharAt(i-1);
            size = font->measureString(str, this->_memDC);
            cx = x+1 + (i-1) * textFieldBoxWidth;
            if (cx + (textFieldBoxWidth - size.cx)/2 + size.cx < x + width) {
                this->drawText(font, str, cx + (textFieldBoxWidth - size.cx) / 2, y + height - textHeight - textDY, TRUE, 0, -1);
            }
        }
    }

    if (drawText && !eachTextLetterInSeparateBox) {
        rc.left = x+1;
        rc.top = y + height - textHeight - textDY;
        rc.right = rc.left + width - 1;
        rc.bottom = rc.top + textHeight;
        this->drawText(font, textFieldBackgroundColor, text, rc, 0, 0, 2, 0, 0, 0, 0, 0, PS_NULL, 0, BS_SOLID, 0, 0, -1);
    }
}

VOID CmzCanvas::drawTextInRectangles(CmzFont* font, LONG x, LONG y, LONG width, LONG height,
    CmzString text,
    COLORREF borderColor, LONG borderSize, LONG borderStyle,
    COLORREF backgroundColor, ULONG backgroundStyle, ULONG_PTR backgroundHatchStyle,
    COLORREF unitBorderColor, LONG unitBorderSize, LONG unitBorderStyle,
    COLORREF unitBackgroundColor, ULONG unitBackgroundStyle, ULONG_PTR unitBackgroundHatchStyle,
    LONG unitWidth, LONG unitHeight, LONG unitPadding, LONG spacing, BOOL drawEmptyFields,
    float rotationAngle) {
    
    LONG textWidth = 0;
    LONG textHeight = 0;
    SIZE size = {0, 0};
    LONG cx, cy;

    if (font) {
        size = font->measureString(text, this->_memDC);
        textWidth = size.cx;
        textHeight = size.cy;
    }

    this->drawRectangleEx(0, backgroundColor, x, y, width, height, borderSize, borderStyle, backgroundStyle, backgroundHatchStyle, rotationAngle);
    
    if (unitWidth < 1 || unitHeight < 1) {
        size = font->measureString(L"A", this->_memDC);
        if (unitWidth < 1) {
            unitWidth = size.cx + 2*unitPadding;
        }
        if (unitHeight < 1) {
            unitHeight = size.cy + 2*unitPadding;
        }
    }
    
    if (!font || (text == COMMON_EMPTY_STRING)) {
        return;
    }
    
    if (spacing < 1) {
        spacing = 2;
    }

    // text field bg
    RECT rc;
    LONG i;
    WCHAR str[2];
    str[1] = NULL;
    if (unitHeight < 1) {
        unitHeight = textHeight + 2*unitPadding;;
    }
    cy = y + (height-unitHeight)/2;
    size_t textLen = text.length();
    LONG lineWidth;
    if (unitWidth > 0) {
        lineWidth = (LONG)textLen * unitWidth + ((LONG)textLen-1) * spacing;
    } else {
        lineWidth = textWidth + ((LONG)textLen-1) * spacing;
    }
    
    if (lineWidth < width) {
        x += (width-lineWidth)/2;
    }
    
    for (i = 0; i < (LONG)textLen; i++) {
        cx = x + i * (unitWidth + spacing);
        str[0] = text.getCharAt(i);
        if (!drawEmptyFields && str[0] == L' ') {
            continue;
        }
        size = font->measureString(str, this->_memDC);
        if (unitWidth > 0) {
            if (cx + unitWidth <= x + width) {
                rc.left = cx;
                rc.top = cy;
                rc.right = rc.left + unitWidth;
                rc.bottom = rc.top + unitHeight;
                this->drawText(font, unitBackgroundColor, str, rc, 0, 0, unitPadding, unitPadding, unitPadding, unitPadding, 0, unitBorderSize, unitBorderStyle, unitBorderColor, unitBackgroundStyle, unitBackgroundHatchStyle, DT_CENTER | DT_VCENTER, -1);
            }
        } else {
            if (cx + size.cx <= x + width) {
                this->drawText(font, unitBackgroundColor, str, cx, y, 0, 0, unitPadding, unitPadding, unitPadding, unitPadding, 0, unitBorderSize, unitBorderStyle, unitBorderColor, unitBackgroundStyle, unitBackgroundHatchStyle, 0, -1);
            }
        }
    }
}

LONG CmzCanvas::drawTinyText(CmzFont *font, CmzString s, LONG x, LONG y, BOOL transparent, LONG extraFormat, LONG replacementFormat, float rotationAngle) {
    LONG result = 0;
    if (!font) {
        return result;
    }
    if (!this->isPrintCanvas() || font->size >= 8) {
        result = __super::drawText(font, s, x, y, transparent, extraFormat, replacementFormat, rotationAngle);
    } else {
        if (fabs(fabs(rotationAngle)-M_PI/2) < 0.01 || fabs(rotationAngle) < 0.01) {
            if (this->_memDC) {
                LONG bx, by;
                LONG bw, bh;
                SIZE size = font->measureString(s.toWCHAR(), this->_memDC);
                if (rotationAngle > 0.01) {
                    bx = 0;
                    by = 0;
                    bw = size.cy;
                    bh = size.cx;
                } else if (rotationAngle < -0.01) {
                    bx = 0;
                    by = size.cx;
                    bw = size.cy;
                    bh = size.cx;
                    y -= bh;
                } else {
                    bx = 0;
                    by = 0;
                    bw = size.cx;
                    bh = size.cy;
                }
                COLORREF bgColor = RGB(0xff, 0xff, 0xff);
                CmzBitmap* tmpBmp = new CmzBitmap(bw, bh, bgColor);
                if (tmpBmp) {
                    tmpBmp->beginPaint();
                    LONG clx = GetDeviceCaps(this->getDC(), LOGPIXELSX);
                    LONG  lx = GetDeviceCaps(tmpBmp->getDC(), LOGPIXELSX);
                    // Calculate proper size of font on the bitmap
                    LONG fsize = (LONG)(font->size*(float)clx/(float)lx);
                    CmzFont* tinyBmpFont = new CmzFont(tmpBmp->getDC(), font->color, font->name, fsize, 0, font->weight, font->italic, font->underline, font->strikeout);
                    if (tinyBmpFont) {
                        result = tmpBmp->drawText(tinyBmpFont, s, bx, by, transparent, extraFormat, replacementFormat, rotationAngle);
                        tmpBmp->endPaint();
                        tmpBmp->saveToFile(L"d:\\common_data\\test.bmp");
                        this->drawBitmap(tmpBmp, x, y);
                        delete tinyBmpFont;
                    }
                    delete tmpBmp;
                }
            }
        }
    }
    
    return result;
}

VOID CmzCanvas::drawBitmap(CmzBitmap *bmp, LONG x, LONG y) {
    HDC bmpDC = NULL;
    if (this->isPrintCanvas()) {
        bmpDC = CreateCompatibleDC(0);
    } else {
        bmpDC = CreateCompatibleDC(this->_memDC);
    }
    if (bmpDC) {
        HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
        BitBlt(this->_memDC, x, y, bmp->width(), bmp->height(), bmpDC, 0, 0, SRCCOPY);
        SelectObject(bmpDC, oldBmp);
        DeleteDC(bmpDC);
    }
}

//VOID CmzCanvas::drawGradientedRectangle(LONG x, LONG y, LONG width, LONG height, float borderThickness, Color borderColor, Color startGradientColor, Color endGradientColor, float rotationAngle) {
VOID CmzCanvas::drawGradientedRectangle(LONG x, LONG y, LONG width, LONG height, LONG borderThickness, COLORREF borderColor, COLORREF startGradientColor, COLORREF endGradientColor, LONG gradientDirection, float rotationAngle) {
/*
    Graphics graphics(this->_memDC);
    // graphics.SetPageUnit(UnitInch); - to set drawing unit
    // SolidBrush blackBrush(Color(255, 0, 0, 0));
    PointF pt1(0.8f, 1.6f);
    PointF pt2(0.8f, 1.6f);
    if (gradientDirection == 1) {
        pt1 = PointF(x, y);
        pt2 = PointF(x+width, y);
    } else {
        pt1 = PointF(x, y);
        pt2 = PointF(x, y+height);
    }
    LinearGradientBrush gradientBrush(pt1, pt2, startGradientColor, endGradientColor);
    graphics.FillRectangle(&gradientBrush, 0, 0, 4, 3);
    if (borderThickness) {
        Pen pen(borderColor, borderThickness);
        graphics.DrawRectangle(&pen, Rect(x, y, width, height));
    }
*/
    this->drawRectangle(borderColor, x, y, width, height, borderThickness, rotationAngle);
    COLORREF color = startGradientColor;
    float stepR = (float)(GetRValue(endGradientColor) - GetRValue(startGradientColor));
    float stepG = (float)(GetGValue(endGradientColor) - GetGValue(startGradientColor));
    float stepB = (float)(GetBValue(endGradientColor) - GetBValue(startGradientColor));
    float rValue = (float)(GetRValue(color));
    float gValue = (float)(GetGValue(color));
    float bValue = (float)(GetBValue(color));
    if (gradientDirection == 1) {
        stepR /= width;
        stepG /= width;
        stepB /= width;
        for (LONG i = 0; i < width-2; i++) {
            this->drawLine(color, x+i+1, y+1, x+i+1, y+height-1, 1, PS_SOLID, rotationAngle);
            rValue += stepR;
            gValue += stepG;
            bValue += stepB;
            color = RGB(rValue, gValue, bValue);
        }
    } else {
        stepR /= height;
        stepG /= height;
        stepB /= height;
        for (LONG i = 0; i < height-2; i++) {
            this->drawLine(color, x+1, y+i+1, x+width-1, y+i+1, 1, PS_SOLID, rotationAngle);
            rValue += stepR;
            gValue += stepG;
            bValue += stepB;
            color = RGB(rValue, gValue, bValue);
        }
    }
}

}
