#include "pch.h"
#include "mzBitmap.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"
#include "mzGraphicsUtils.h"
#include "mzJsonParser.h"
#include "mzMath.h"
#include <stdio.h>

using namespace mzstd;


VOID CmzBitmap::_commonInit(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor) {
    this->setRealClassName(L"CmzBitmap");
    this->_width = 0;
    this->_height = 0;
    this->_bgcolor = 0;
    this->hBitmap = NULL;
    this->_memDC = 0;
    this->_oldBitmapFromMemDC = NULL;
    this->_compatibleDCCreated = FALSE;
    this->_x = 0;
    this->_y = 0;
    this->_offsetX = 0;
    this->_offsetY = 0;
    this->_trackChanges = FALSE;
    if (width > 0 && height > 0) {
        // Here is update of attributes _width, _height, _bgcolor
        this->_createBlankBitmap(hdc, width, height, bgcolor);
    }
}

CmzBitmap::CmzBitmap(HINSTANCE hInst, CmzString fname) {
    this->_commonInit(NULL, 0, 0, RGB(0xff, 0xff, 0xff));
    this->fname = fname;

    if (FileExists(fname)) {
        this->hBitmap = (HBITMAP)LoadImage(hInst, fname.getBuffer(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        DWORD error = GetLastError();
        if (error != ERROR_SUCCESS) {
            ShowSystemError(error);
            return;
        }
        if (this->hBitmap) {
            BITMAP bm;
            LONG retval = GetObject(this->hBitmap, sizeof(bm), &bm);
            if (retval) {
                this->_width = bm.bmWidth;
                this->_height = bm.bmHeight;
            }

            this->validateBitmap();
        }
    }
}

CmzBitmap::CmzBitmap(DWORD width, DWORD height, COLORREF bgcolor) {
    this->_commonInit(NULL, width, height, bgcolor);
    this->fname = "";
}

CmzBitmap::CmzBitmap(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor) {
    this->_commonInit(hdc, width, height, bgcolor);
    this->fname = "";
}

CmzBitmap::CmzBitmap(HDC hdc) {
    this->_commonInit(hdc, 0, 0, 0);
    this->_memDC = hdc;
    this->fname = "";
}

CmzBitmap::CmzBitmap() {
    this->_commonInit(NULL, 200, 200, RGB(0xff, 0xff, 0xff));
    this->fname = "";
    this->validateBitmap();
}

CmzBitmap::~CmzBitmap() {
    this->_cleanup();
}

VOID CmzBitmap::_cleanup() {
    BOOL deleteMemDC = FALSE;
    
    if (this->hBitmap) {
        // mark delete DC as required only if Bitmap was created previously
        // if CmzBitmap was created for printing purpose then there is no internal hbitmap
        deleteMemDC = TRUE;
        DeleteObject(this->hBitmap);
        this->hBitmap = 0;
    }
    if (this->_memDC && deleteMemDC) {
        DeleteDC(this->_memDC);
    }
    this->_memDC = 0;
}

VOID CmzBitmap::_createBlankBitmap(HDC hdc, DWORD width, DWORD height, COLORREF bgcolor) {
    BOOL releaseDC = FALSE;
    if (!hdc) {
        hdc = GetDC(0);
        releaseDC = TRUE;
    }

    this->hBitmap = CreateCompatibleBitmap(hdc, width, height);
    if (this->hBitmap) {
        HDC memDC = CreateCompatibleDC(hdc);
        this->_width = width;
        this->_height = height;
        this->_bgcolor = bgcolor;

        HBRUSH br = CreateSolidBrush(bgcolor);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, this->hBitmap);
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = this->_width;
        rc.bottom = this->_height;
        SetBkMode(memDC, TRANSPARENT);
        FillRect(memDC, &rc, br);
        SelectObject(memDC, oldBitmap);
        DeleteObject(br);
        DeleteDC(memDC);
    }

    if (releaseDC) {
        ReleaseDC(0, hdc);
    }
}

VOID CmzBitmap::setSize(LONG width, LONG height) {
    this->_cleanup();
    this->fname = "";
    this->_commonInit(NULL, width, height, this->_bgcolor);
}

VOID CmzBitmap::setSize(HDC hdc, LONG width, LONG height) {
    this->_cleanup();
    this->fname = "";
    this->_commonInit(hdc, width, height, this->_bgcolor);
}

BOOL CmzBitmap::beginPaint(HDC hdc, BOOL createCompatibleDC) {
    if (!this->hBitmap) {
        return FALSE;
    }

    HDC ihdc = hdc;
    if (hdc == 0) {
        ihdc = GetDC(0);
        createCompatibleDC = TRUE;
    }

    if (createCompatibleDC) {
        this->_memDC = CreateCompatibleDC(ihdc);
        this->_compatibleDCCreated = TRUE;
    } else {
        this->_compatibleDCCreated = FALSE;
        this->_memDC = ihdc;
    }

    if (this->_memDC) {
        this->_oldBitmapFromMemDC = (HBITMAP)SelectObject(this->_memDC, this->hBitmap);
    }

    if (hdc == 0) {
        ReleaseDC(0, ihdc);
    }

    return this->_memDC != NULL;
}

BOOL CmzBitmap::endPaint() {
    if (!this->hBitmap) {
        return FALSE;
    }

    if (this->_memDC) {
        if (this->_oldBitmapFromMemDC) {
            SelectObject(this->_memDC, this->_oldBitmapFromMemDC);
        }
        if (this->_compatibleDCCreated) {
            DeleteDC(this->_memDC);
        }
        this->_memDC = 0;
    }

    return TRUE;
}

VOID CmzBitmap::bitblt(HDC hdc, LONG x, LONG y) {
    BitBlt(hdc, x, y, this->_width, this->_height, this->_memDC, 0, 0, SRCCOPY);
//    TransparentBlt(hdc, x, y, this->_width, this->_height, this->_memDC, 0, 0, this->_width, this->_height, 0xffffffff);
}

VOID CmzBitmap::bitblt(HDC hdcDst, LONG dstX, LONG dstY, LONG srcX, LONG srcY, LONG width, LONG height) {
    BitBlt(hdcDst, dstX, dstY, width, height, this->_memDC, srcX, srcY, SRCCOPY);
//    TransparentBlt(hdcDst, dstX, dstY, width, height, this->_memDC, srcX, srcY, width, height, 0xffffffff);
}

VOID CmzBitmap::drawTransparentBitmap(CmzBitmap *bmp, LONG x, LONG y, LONG frame, COLORREF transColor) {
    COLORREF clrTrans;
    LONG srcW = bmp->width();
    LONG srcH = bmp->height();
	HBITMAP hMaskBmp = CreateBitmap(srcW, srcH, 1, 1, NULL);
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HDC hdcMask = CreateCompatibleDC(NULL);

    HBITMAP hdcSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, bmp->hBitmap);
    HBITMAP hdcMaskOrigBmp = (HBITMAP)SelectObject(hdcMask, hMaskBmp);

    HBITMAP hTmpSrcBmp = CreateCompatibleBitmap(hdcSrc, srcW, srcH);
    HDC hdcTmpSrc = CreateCompatibleDC(NULL);
    HBITMAP hdcTmpSrcOrigBmp = (HBITMAP)SelectObject(hdcTmpSrc, hTmpSrcBmp);

    BitBlt(hdcTmpSrc, 0, 0, srcW, srcH, hdcSrc, 0, 0, SRCCOPY);
    SelectObject(hdcSrc, hdcSrcOrigBmp);
    DeleteDC(hdcSrc);

    if (transColor == NULL) {
        clrTrans = GetPixel(hdcTmpSrc, 0, 0);
    } else {
        clrTrans = transColor;
    }

    // Tworzymy maskę bitmapy źródłowej - na bitmapie maski każdy kolor bitmapy
    // źródłowej różny od koloru tła będzie czarny a kolor tła będzie biały.
    // Zapewniają to dwie poniższe funkcje.
    COLORREF origTmpSrcBkColor = SetBkColor(hdcTmpSrc, clrTrans);
    BitBlt(hdcMask, 0, 0, srcW, srcH, hdcTmpSrc, 0, 0, SRCCOPY);

    COLORREF origTmpSrcTextColor = SetTextColor(hdcTmpSrc, RGB(255,255,255));
    SetBkColor(hdcTmpSrc, RGB(0,0,0));
    BitBlt(hdcTmpSrc, 0, 0, srcW, srcH, hdcMask, 0, 0, SRCAND);
    SetTextColor(hdcTmpSrc, origTmpSrcTextColor);
    SetBkColor(hdcTmpSrc, origTmpSrcBkColor);

    BitBlt(this->_memDC, x, y, srcW, srcH, hdcMask, bmp->width() * frame, bmp->_offsetY, SRCAND);
    BitBlt(this->_memDC, x, y, srcW, srcH, hdcTmpSrc, bmp->width() * frame, bmp->_offsetY, SRCPAINT);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, srcW, srcH);
    }

    SelectObject(hdcMask, hdcMaskOrigBmp);
    SelectObject(hdcTmpSrc, hdcTmpSrcOrigBmp);
    DeleteDC(hdcMask);
    DeleteDC(hdcTmpSrc);
    DeleteObject(hMaskBmp);
    DeleteObject(hTmpSrcBmp);
}

VOID CmzBitmap::drawTransparentBitmap(CmzBitmap *bmp, LONG x, LONG y, COLORREF transColor) {
    COLORREF clrTrans;
    LONG srcW = bmp->width();
    LONG srcH = bmp->height();
	HBITMAP hMaskBmp = CreateBitmap(srcW, srcH, 1, 1, NULL);
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HDC hdcMask = CreateCompatibleDC(NULL);

    HBITMAP hdcSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, bmp->hBitmap);
    HBITMAP hdcMaskOrigBmp = (HBITMAP)SelectObject(hdcMask, hMaskBmp);

    HBITMAP hTmpSrcBmp = CreateCompatibleBitmap(hdcSrc, srcW, srcH);
    HDC hdcTmpSrc = CreateCompatibleDC(NULL);
    HBITMAP hdcTmpSrcOrigBmp = (HBITMAP)SelectObject(hdcTmpSrc, hTmpSrcBmp);

    BitBlt(hdcTmpSrc, 0, 0, srcW, srcH, hdcSrc, 0, 0, SRCCOPY);
    SelectObject(hdcSrc, hdcSrcOrigBmp);
    DeleteDC(hdcSrc);

    if (transColor == NULL) {
        clrTrans = GetPixel(hdcTmpSrc, 0, 0);
    } else {
        clrTrans = transColor;
    }

    // Tworzymy maskę bitmapy źródłowej - na bitmapie maski każdy kolor bitmapy
    // źródłowej różny od koloru tła będzie czarny a kolor tła będzie biały.
    // Zapewniają to dwie poniższe funkcje.
    COLORREF origTmpSrcBkColor = SetBkColor(hdcTmpSrc, clrTrans);
    BitBlt(hdcMask, 0, 0, srcW, srcH, hdcTmpSrc, 0, 0, SRCCOPY);

    COLORREF origTmpSrcTextColor = SetTextColor(hdcTmpSrc, RGB(255,255,255));
    SetBkColor(hdcTmpSrc, RGB(0,0,0));
    BitBlt(hdcTmpSrc, 0, 0, srcW, srcH, hdcMask, 0, 0, SRCAND);
    SetTextColor(hdcTmpSrc, origTmpSrcTextColor);
    SetBkColor(hdcTmpSrc, origTmpSrcBkColor);

    BitBlt(this->_memDC, x, y, srcW, srcH, hdcMask, bmp->_offsetX, bmp->_offsetY, SRCAND);
    BitBlt(this->_memDC, x, y, srcW, srcH, hdcTmpSrc, bmp->_offsetX, bmp->_offsetY, SRCPAINT);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, srcW, srcH);
    }

    SelectObject(hdcMask, hdcMaskOrigBmp);
    SelectObject(hdcTmpSrc, hdcTmpSrcOrigBmp);
    DeleteDC(hdcMask);
    DeleteDC(hdcTmpSrc);
    DeleteObject(hMaskBmp);
    DeleteObject(hTmpSrcBmp);
}

VOID CmzBitmap::drawTransparentBitmap(CmzBitmap *bmp, COLORREF transColor) {
    COLORREF clrTrans;
    LONG srcW = bmp->width();
    LONG srcH = bmp->height();
	HBITMAP hMaskBmp = CreateBitmap(srcW, srcH, 1, 1, NULL);
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HDC hdcMask = CreateCompatibleDC(NULL);

    HBITMAP hdcSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, bmp->hBitmap);
    HBITMAP hdcMaskOrigBmp = (HBITMAP)SelectObject(hdcMask, hMaskBmp);

    HBITMAP hTmpSrcBmp = CreateCompatibleBitmap(hdcSrc, srcW, srcH);
    HDC hdcTmpSrc = CreateCompatibleDC(NULL);
    HBITMAP hdcTmpSrcOrigBmp = (HBITMAP)SelectObject(hdcTmpSrc, hTmpSrcBmp);

    BitBlt(hdcTmpSrc, 0, 0, srcW, srcH, hdcSrc, 0, 0, SRCCOPY);
    SelectObject(hdcSrc, hdcSrcOrigBmp);
    DeleteDC(hdcSrc);

    if (transColor == NULL) {
        clrTrans = GetPixel(hdcTmpSrc, 0, 0);
    } else {
        clrTrans = transColor;
    }

    // Tworzymy maskę bitmapy źródłowej - na bitmapie maski każdy kolor bitmapy
    // źródłowej różny od koloru tła będzie czarny a kolor tła będzie biały.
    // Zapewniają to dwie poniższe funkcje.
    COLORREF origTmpSrcBkColor = SetBkColor(hdcTmpSrc, clrTrans);
    BitBlt(hdcMask, 0, 0, srcW, srcH, hdcTmpSrc, 0, 0, SRCCOPY);

    COLORREF origTmpSrcTextColor = SetTextColor(hdcTmpSrc, RGB(255,255,255));
    SetBkColor(hdcTmpSrc, RGB(0,0,0));
    BitBlt(hdcTmpSrc, 0, 0, srcW, srcH, hdcMask, 0, 0, SRCAND);
    SetTextColor(hdcTmpSrc, origTmpSrcTextColor);
    SetBkColor(hdcTmpSrc, origTmpSrcBkColor);

    BitBlt(this->_memDC, bmp->_x, bmp->_y, srcW, srcH, hdcMask, bmp->_offsetX, bmp->_offsetY, SRCAND);
    BitBlt(this->_memDC, bmp->_x, bmp->_y, srcW, srcH, hdcTmpSrc, bmp->_offsetX, bmp->_offsetY, SRCPAINT);
    if (this->_trackChanges) {
        this->invalidateRect(bmp->_x, bmp->_y, srcW, srcH);
    }

    SelectObject(hdcMask, hdcMaskOrigBmp);
    SelectObject(hdcTmpSrc, hdcTmpSrcOrigBmp);
    DeleteDC(hdcMask);
    DeleteDC(hdcTmpSrc);
    DeleteObject(hMaskBmp);
    DeleteObject(hTmpSrcBmp);
}

VOID CmzBitmap::drawBitmap(CmzBitmap *bmp, LONG x, LONG y, LONG frame) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, x, y, bmp->width(), bmp->height(), bmpDC, bmp->width() * frame, bmp->_offsetY, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, bmp->width(), bmp->height());
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::drawBitmap(CmzBitmap *bmp, LONG x, LONG y) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, x, y, bmp->width(), bmp->height(), bmpDC, bmp->_offsetX, bmp->_offsetY, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, bmp->width(), bmp->width());
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::drawBitmap(CmzBitmap *bmp) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, bmp->_x, bmp->_y, bmp->width(), bmp->height(), bmpDC, bmp->_offsetX, bmp->_offsetY, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(bmp->_x, bmp->_y, bmp->width(), bmp->height());
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::drawBitmapPart(CmzBitmap *bmp, LONG x, LONG y, LONG frame, LONG srcX, LONG srcY, LONG srcWidth, LONG srcHeight) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, x, y, srcWidth, srcHeight, bmpDC, bmp->width() * frame + srcX, bmp->_offsetY + srcY, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, srcWidth, srcHeight);
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::drawBitmapPart(CmzBitmap *bmp, LONG x, LONG y, LONG srcX, LONG srcY, LONG srcWidth, LONG srcHeight) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, x, y, srcWidth, srcHeight, bmpDC, bmp->_offsetX + srcX, bmp->_offsetY + srcY, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, srcWidth, srcHeight);
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::drawBitmapPart(CmzBitmap *bmp, LONG srcX, LONG srcY, LONG srcWidth, LONG srcHeight) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, bmp->_x, bmp->_y, srcWidth, srcHeight, bmpDC, bmp->_offsetX + srcX, bmp->_offsetY + srcY, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(bmp->_x, bmp->_y, srcWidth, srcHeight);
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::drawFullBitmap(CmzBitmap *bmp, LONG x, LONG y) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    BitBlt(this->_memDC, x, y, bmp->_width, bmp->_height, bmpDC, 0, 0, SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, bmp->_width, bmp->_height);
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

VOID CmzBitmap::stretchBitmap(CmzBitmap *bmp, LONG x, LONG y, LONG width, LONG height) {
    HDC bmpDC = CreateCompatibleDC(this->_memDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(bmpDC, bmp->hBitmap);
    StretchBlt(this->_memDC, x, y, width, height, bmpDC, bmp->_offsetX, bmp->_offsetY, bmp->width(), bmp->height(), SRCCOPY);
    if (this->_trackChanges) {
        this->invalidateRect(x, y, width, height);
    }
    SelectObject(bmpDC, oldBmp);
    DeleteDC(bmpDC);
}

COLORREF *CmzBitmap::getRgbTable() {
	LONG pixelCount, count = 0;

	// load image
	HDC hdc = ::GetDC(0);
	HDC memDC = ::CreateCompatibleDC(hdc);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, this->hBitmap);

	pixelCount = this->_width * this->_height;
	COLORREF* imgRgbTable = new COLORREF[pixelCount];

	for (LONG y = 0; y < this->_height; y++) {
		for (LONG x = 0; x < this->_width; x++) {
			imgRgbTable[count] = GetPixel(memDC, x, y);
			count++;
		}
	}

	SelectObject(memDC, hOldBmp);
	DeleteDC(memDC);
	ReleaseDC(0, hdc);

    return imgRgbTable;
}

LONG CmzBitmap::drawText(CmzFont *font, CmzString s, LONG x, LONG y, BOOL transparent, LONG extraFormat, LONG replacementFormat, float rotationAngle) {
    CmzFont* textFont = font;
    BOOL releaseFont = FALSE;

    LONG oldBkMode;
    if (transparent) {
        oldBkMode = this->setBkMode(TRANSPARENT);
    }
    LONG rotationAngleDegree = (LONG)(-(10 * rotationAngle * 180 / M_PI));
    if (rotationAngleDegree != 0) {
        releaseFont = TRUE;
        textFont = new CmzFont(this->_memDC, font->color, font->name, font->size, rotationAngleDegree);
    }

    HFONT oldFont = (HFONT)SelectObject(this->_memDC, textFont->hFont);
    SIZE size = font->measureString(s, this->_memDC);

    RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = x + size.cx;
    rc.bottom = y + size.cy;

    ULONG format = DT_SINGLELINE;
    format |= extraFormat;
    if ((format & DT_LEFT) != DT_LEFT && (format & DT_RIGHT) != DT_RIGHT && (format & DT_CENTER) != DT_CENTER) {
        format |= DT_LEFT;
    }
    if ((format & DT_TOP) != DT_TOP && (format & DT_BOTTOM) != DT_BOTTOM && (format & DT_VCENTER) != DT_VCENTER) {
        format |= DT_TOP;
    }

    if (replacementFormat > -1) {
        format = replacementFormat;
    }
    if (releaseFont) {
        format |= DT_NOCLIP;
    }
    SetTextColor(this->_memDC, textFont->color);
    LONG result = DrawTextEx(this->_memDC, s.getBuffer(), (INT)s.length(), &rc, format, NULL);
    SelectObject(this->_memDC, oldFont);
    if (this->_trackChanges) {
        this->invalidateRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }

    if (transparent) {
        this->setBkMode(oldBkMode);
    }
    if (releaseFont && textFont) {
        delete textFont;
    }
    
    return result;
}

LONG CmzBitmap::drawText(CmzFont *font, CmzString s, RECT rc, BOOL transparent, LONG extraFormat, LONG replacementFormat, float rotationAngle) {
    CmzFont* textFont = font;
    BOOL releaseFont = FALSE;

    LONG oldBkMode;
    if (transparent) {
        oldBkMode = this->setBkMode(TRANSPARENT);
    }

    LONG rotationAngleDegree = (LONG)(-(10 * rotationAngle * 180 / M_PI));
    if (rotationAngleDegree != 0) {
        releaseFont = TRUE;
        textFont = new CmzFont(this->_memDC, font->color, font->name, font->size, rotationAngleDegree);
    }

    HFONT oldFont = (HFONT)SelectObject(this->_memDC, textFont->hFont);
    ULONG format = DT_SINGLELINE;
    format |= extraFormat;
    if ((format & DT_LEFT) != DT_LEFT && (format & DT_RIGHT) != DT_RIGHT && (format & DT_CENTER) != DT_CENTER) {
        format |= DT_LEFT;
    }
    if ((format & DT_TOP) != DT_TOP && (format & DT_BOTTOM) != DT_BOTTOM && (format & DT_VCENTER) != DT_VCENTER) {
        format |= DT_TOP;
    }

    if (replacementFormat > -1) {
        format = replacementFormat;
    }
    if (releaseFont) {
        format |= DT_NOCLIP;
    }
    SetTextColor(this->_memDC, textFont->color);
    LONG result = DrawTextEx(this->_memDC, s.getBuffer(), (INT)s.length(), &rc, format, NULL);
    SelectObject(this->_memDC, oldFont);
    if (this->_trackChanges) {
        this->invalidateRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }

    if (transparent) {
        this->setBkMode(oldBkMode);
    }
    if (releaseFont && textFont) {
        delete textFont;
    }
    
    return result;
}

LONG CmzBitmap::drawText(CmzFont *font, COLORREF backgroundColor, CmzString s, RECT rc, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat) {
    RECT rcRect = rc;
    if (leftPadding != 0 || topPadding != 0 || rightPadding != 0 || bottomPadding != 0) {
        rc.left += leftPadding;
        rc.top += topPadding;
        rc.right -= rightPadding;
        rc.bottom -= bottomPadding;
    }
    if (borderStyle != PS_NULL || backgroundStyle != BS_NULL) {
        if (borderStyle == PS_NULL) {
            borderColor = backgroundColor;
        }
        this->drawRectangleEx(borderColor, backgroundColor, rcRect.left, rcRect.top, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, borderThickness, borderStyle, backgroundStyle, backgroundHatch, rotationAngle);
    }
    OffsetRect(&rc, dxTextOffset, dyTextOffset);
    LONG result = this->drawText(font, s, rc, TRUE, extraFormat, replacementFormat, rotationAngle);
    return result;
}

LONG CmzBitmap::drawText(CmzFont *font, COLORREF backgroundColor, CmzString s, LONG x, LONG y, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat) {
    SIZE size = font->measureString(s, this->_memDC);
    RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = rc.left + size.cx + leftPadding + rightPadding;
    rc.bottom = rc.top + size.cy + topPadding + bottomPadding;
    RECT rcRect = rc;

    if (leftPadding != 0 || topPadding != 0 || rightPadding != 0 || bottomPadding != 0) {
        rc.left += leftPadding;
        rc.top += topPadding;
        rc.right -= rightPadding;
        rc.bottom -= bottomPadding;
    }

    if (borderStyle != PS_NULL || backgroundStyle != BS_NULL) {
        if (borderStyle == PS_NULL) {
            borderColor = backgroundColor;
        }
        this->drawRectangleEx(borderColor, backgroundColor, rcRect.left, rcRect.top, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, borderThickness, borderStyle, backgroundStyle, backgroundHatch, rotationAngle);
    }
    OffsetRect(&rc, dxTextOffset, dyTextOffset);
    LONG result = this->drawText(font, s, rc, TRUE, extraFormat, replacementFormat, rotationAngle);
    return result;
}

LONG CmzBitmap::drawText(CmzFont *font, COLORREF backgroundColor, CmzString s, LONG charSpacing, RECT rc, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat) {
    LONG result = 0;
    if (fabs((float)charSpacing) < 0.01) {
        result = drawText(font, backgroundColor, s, rc, dxTextOffset, dyTextOffset, leftPadding, topPadding, rightPadding, bottomPadding, rotationAngle, borderThickness, borderStyle, borderColor, backgroundStyle, backgroundHatch, extraFormat, replacementFormat);
        return result;
    }

    if (borderStyle != PS_NULL || backgroundStyle != BS_NULL) {
        if (borderStyle == PS_NULL) {
            borderColor = backgroundColor;
        }
        this->drawRectangleEx(borderColor, backgroundColor, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, borderThickness, borderStyle, backgroundStyle, backgroundHatch, rotationAngle);
    }

    size_t len = s.length();
    SIZE size;
    WCHAR str[2];
    ZeroMemory(str, 2*sizeof(WCHAR));
    RECT letterRect;
    letterRect.left = rc.left + leftPadding;
    letterRect.top = rc.top + topPadding;

    for (ULONG i = 0; i < len; i++) {
        str[0] = s.getCharAt(i);
        size = font->measureString(str, this->_memDC);
        letterRect.right = letterRect.left + size.cx;
        letterRect.bottom = letterRect.top + size.cy;
        if (letterRect.left > rc.right || letterRect.right > rc.right) {
            break;
        }
        result = this->drawText(font, str, letterRect, TRUE, extraFormat, replacementFormat, rotationAngle);
        letterRect.left = letterRect.right + charSpacing;
    }
    
    return result;
}

LONG CmzBitmap::drawText(CmzFont *font, COLORREF backgroundColor, CmzString s, LONG charSpacing, LONG x, LONG y, LONG dxTextOffset, LONG dyTextOffset, LONG leftPadding, LONG topPadding, LONG rightPadding, LONG bottomPadding, float rotationAngle, LONG borderThickness, LONG borderStyle, COLORREF borderColor, ULONG backgroundStyle, ULONG_PTR backgroundHatch, LONG extraFormat, LONG replacementFormat) {
    LONG result = 0;
    if (fabs((float)charSpacing) < 0.01) {
        result = drawText(font, backgroundColor, s, x, y, dxTextOffset, dyTextOffset, leftPadding, topPadding, rightPadding, bottomPadding, rotationAngle, borderThickness, borderStyle, borderColor, backgroundStyle, backgroundHatch, extraFormat, replacementFormat);
        return result;
    }

    SIZE size = font->measureString(s, charSpacing, this->_memDC);
    RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = rc.left + size.cx + leftPadding + rightPadding;
    rc.bottom = rc.top + size.cy + topPadding + bottomPadding;
    result = this->drawText(font, backgroundColor, s, charSpacing, rc, dxTextOffset, dyTextOffset, leftPadding, topPadding, rightPadding, bottomPadding, rotationAngle, borderThickness, borderStyle, borderColor, backgroundStyle, backgroundHatch, extraFormat, replacementFormat);
    return result;
}

VOID CmzBitmap::setPen(COLORREF c, LONG width, LONG style) {
    HPEN pen = CreatePen(style, width, c);
    DeleteObject(SelectObject(this->_memDC, pen));
}

VOID CmzBitmap::setBrush(COLORREF c) {
    HBRUSH brush = CreateSolidBrush(c);
    DeleteObject(SelectObject(this->_memDC, brush));
}

LONG CmzBitmap::setBkMode(LONG bkMode) {
    return SetBkMode(this->_memDC, bkMode);
}

COLORREF CmzBitmap::setBkColor(COLORREF newBkColor) {
    COLORREF retval = NULL;

    if (this->_memDC) {
        retval = SetBkColor(this->_memDC, newBkColor);
    }

    return retval;
}

VOID WINAPI CmzBitmap::fillWithColor(COLORREF color) {
    CmzBitmap *bmpBg = new CmzBitmap(this->_width, this->_height, color);
    HDC hdcBg = CreateCompatibleDC(0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcBg, bmpBg->hBitmap);

    if (this->_memDC) {
        TransparentBlt(this->_memDC, 0, 0, this->_width, this->_height, hdcBg, 0, 0, bmpBg->width(), bmpBg->height(), 0xffffffff);
    } else {
        HDC hdcMemDC = CreateCompatibleDC(0);
        HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hdcMemDC, this->hBitmap);

        TransparentBlt(hdcMemDC, 0, 0, this->_width, this->_height, hdcBg, 0, 0, bmpBg->width(), bmpBg->height(), 0xffffffff);

        SelectObject(hdcMemDC, hOldMemBmp);
        DeleteDC(hdcMemDC);
    }
    if (this->_trackChanges) {
        this->invalidateRect(0, 0, this->_width, this->_height);
    }

    SelectObject(hdcBg, hOldBmp);
    DeleteDC(hdcBg);
    delete bmpBg;
}

VOID CmzBitmap::invalidateRect(LONG x, LONG y, LONG width, LONG height) {
    if (this->_trackChanges) {
        if (x < this->_updateRect.left) {
            this->_updateRect.left = x;
        }
        if (y < this->_updateRect.top) {
            this->_updateRect.top = y;
        }
        if (x + width > this->_updateRect.right) {
            this->_updateRect.right = x + width;
        }
        if (y + height > this->_updateRect.bottom) {
            this->_updateRect.bottom = y + height;
        }
    }
}

VOID CmzBitmap::drawRectangleEx(COLORREF color, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, LONG borderStyle, ULONG backgroundStyle, ULONG_PTR backgroundHatch, float rotationAngle) {
    LOGBRUSH lb;
    ZeroMemory(&lb, sizeof(lb));
    lb.lbStyle = backgroundStyle;
    if (backgroundStyle != BS_NULL) {
        lb.lbColor = backgroundColor;
        lb.lbHatch = backgroundHatch;
    }
    LONG rotationAngleDegree = (LONG)(10 * rotationAngle * 180 / M_PI);
    if (rotationAngleDegree == 0) {
        HBRUSH brush = CreateBrushIndirect(&lb);
        if (!brush) { return; }
        HPEN pen = CreatePen(borderStyle, borderThickness, color);
        if (!pen) {
            DeleteObject(brush);
            return;
        }
        HBRUSH oldBrush;
        HPEN oldPen;

        if (this->_memDC) {
            oldBrush = (HBRUSH)SelectObject(this->_memDC, brush);
            oldPen = (HPEN)SelectObject(this->_memDC, pen);
            Rectangle(this->_memDC, x, y, x + width, y + height);
            SelectObject(this->_memDC, oldBrush);
            SelectObject(this->_memDC, oldPen);
        } else {
            HDC hdcMemDC = CreateCompatibleDC(0);
            HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hdcMemDC, this->hBitmap);
            oldBrush = (HBRUSH)SelectObject(hdcMemDC, brush);
            oldPen = (HPEN)SelectObject(hdcMemDC, pen);
            Rectangle(hdcMemDC, x, y, x + width, y + height);
            SelectObject(hdcMemDC, oldBrush);
            SelectObject(hdcMemDC, oldPen);
            SelectObject(hdcMemDC, hOldMemBmp);
            DeleteDC(hdcMemDC);
        }

        DeleteObject(brush);
        DeleteObject(pen);
    } else {
        POINT points[4];
        this->_rotatePoint(x, y, rotationAngle, &points[0].x, &points[0].y, x, y);
        this->_rotatePoint(x + width, y, rotationAngle, &points[1].x, &points[1].y, x, y);
        this->_rotatePoint(x + width, y + height, rotationAngle, &points[2].x, &points[2].y, x, y);
        this->_rotatePoint(x, y + height, rotationAngle, &points[3].x, &points[3].y, x, y);

/*
        wprintf(L"RECT: (%d,%d), (%d,%d), (%d,%d), (%d,%d)\n",
                points[0].x, points[0].y,
                points[1].x, points[1].y,
                points[2].x, points[2].y,
                points[3].x, points[3].y);
        wprintf(L"%d, %d, %d, %d\n", rc.left, rc.top, rc.right, rc.bottom);
*/
        if (borderStyle != PS_NULL) {
            if (borderThickness < 2) {
                borderThickness = 2;
                borderStyle = PS_SOLID;
            }
            this->drawPolyline(color, points, 4, TRUE, borderThickness, borderStyle);
            LONG minX = points[0].x, maxX = points[0].x;
            LONG minY = points[0].y, maxY = points[0].y;
            for (LONG i = 1; i < 4; i++) {
                if (points[i].x < minX) { minX = points[i].x; }
                if (points[i].x > maxX) { maxX = points[i].x; }
                if (points[i].y < minY) { minY = points[i].y; }
                if (points[i].y > maxY) { maxY = points[i].y; }
            }
            //wprintf(L"minX: %d, maxX: %d, minY: %d, maxY: %d\n", minX, maxX, minY, maxY);
            this->floodFill(color, backgroundColor, (maxX + minX) / 2, (maxY + minY) / 2);
        } else {
            this->drawPolyline(color, points, 4, TRUE, borderThickness, borderStyle);
        }
    }
}

VOID CmzBitmap::drawRectangle(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, float rotationAngle) {
    this->drawRectangleEx(color, 0, x, y, width, height, borderThickness, PS_SOLID, BS_NULL, 0, rotationAngle);
}

VOID CmzBitmap::drawRectangle(COLORREF color, RECT rc, LONG borderThickness, LONG borderStyle, float rotationAngle) {
    this->drawRectangleEx(color, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, borderThickness, PS_SOLID, BS_NULL, 0, rotationAngle);
}

VOID CmzBitmap::fillRectangle(COLORREF borderColor, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, float rotationAngle) {
    this->drawRectangleEx(borderColor, backgroundColor, x, y, width, height, borderThickness, PS_SOLID, BS_SOLID, 0, rotationAngle);
}

VOID CmzBitmap::fillRectangle(COLORREF borderColor, COLORREF backgroundColor, RECT rc, LONG borderThickness, float rotationAngle) {
    this->drawRectangleEx(borderColor, backgroundColor, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, borderThickness, PS_SOLID, BS_SOLID, 0, rotationAngle);
}

VOID CmzBitmap::fillRectangle(COLORREF color, RECT rc, float rotationAngle) {
    this->drawRectangleEx(color, color, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 1, PS_SOLID, BS_SOLID, 0, rotationAngle);
}

VOID CmzBitmap::fillRectangle(COLORREF color, LONG x, LONG y, LONG width, LONG height, float rotationAngle) {
    this->drawRectangleEx(color, color, x, y, width, height, 1, PS_SOLID, BS_SOLID, 0, rotationAngle);
}

VOID CmzBitmap::drawRectangleBorder(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, LONG borderStyle, BOOL drawLeftFrame, BOOL drawTopFrame, BOOL drawRightFrame, BOOL drawBottomFrame, float rotationAngle) {
    if (drawLeftFrame) {
        this->drawVLine(color, x, y, height, borderThickness, borderStyle, rotationAngle);
    }
    if (drawTopFrame) {
        this->drawHLine(color, x, y, width, borderThickness, borderStyle, rotationAngle);
    }
    if (drawRightFrame) {
        this->drawVLine(color, x + width, y, height, borderThickness, borderStyle, rotationAngle);
    }
    if (drawBottomFrame) {
        this->drawHLine(color, x, y + height, width, borderThickness, borderStyle, rotationAngle);
    }
}

VOID CmzBitmap::drawRectangleLB(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, LONG borderStyle, float rotationAngle) {
    this->drawRectangleBorder(color, x, y, width, height, borderThickness, borderStyle, TRUE, FALSE, FALSE, TRUE, rotationAngle);
}

VOID CmzBitmap::drawRectangleBR(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, LONG borderStyle, float rotationAngle) {
    this->drawRectangleBorder(color, x, y, width, height, borderThickness, borderStyle, FALSE, FALSE, TRUE, TRUE, rotationAngle);
}

VOID CmzBitmap::drawRectangleLBR(COLORREF color, LONG x, LONG y, LONG width, LONG height, LONG borderThickness, LONG borderStyle, float rotationAngle) {
    this->drawRectangleBorder(color, x, y, width, height, borderThickness, borderStyle, TRUE, FALSE, TRUE, TRUE, rotationAngle);
}

VOID CmzBitmap::drawSquareEx(COLORREF color, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG borderThickness, LONG borderStyle, ULONG backgroundStyle, ULONG_PTR backgroundHatch, float rotationAngle) {
    this->drawRectangleEx(color, backgroundColor, x, y, width, width, borderThickness, borderStyle, backgroundStyle, backgroundHatch, rotationAngle);
}

VOID CmzBitmap::drawSquare(COLORREF color, LONG x, LONG y, LONG width, LONG borderThickness, LONG borderStyle, float rotationAngle) {
    this->drawRectangleEx(color, 0, x, y, width, width, borderThickness, borderStyle, BS_NULL, 0, rotationAngle);
}

VOID CmzBitmap::fillSquare(COLORREF color, COLORREF backgroundColor, LONG x, LONG y, LONG width, LONG borderThickness, LONG borderStyle, float rotationAngle) {
    this->drawRectangleEx(color, backgroundColor, x, y, width, width, borderThickness, borderStyle, BS_SOLID, 0, rotationAngle);
}

VOID CmzBitmap::fillSquare(COLORREF color, LONG x, LONG y, LONG width, float rotationAngle) {
    this->drawRectangleEx(color, color, x, y, width, width, 1, PS_SOLID, BS_SOLID, 0, rotationAngle);
}

VOID CmzBitmap::drawEllipseEx(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG rx, LONG ry, LONG borderThickness, LONG borderStyle, ULONG backgroundStyle, ULONG_PTR backgroundHatch) {
    LOGBRUSH lb;
    ZeroMemory(&lb, sizeof(lb));
    lb.lbStyle = backgroundStyle;
    if (backgroundStyle != BS_NULL) {
        lb.lbColor = backgroundColor;
        lb.lbHatch = backgroundHatch;
    }
    HBRUSH brush = CreateBrushIndirect(&lb);
    if (!brush) { return; }
    HPEN pen = CreatePen(borderStyle, borderThickness, color);
    if (!pen) {
        DeleteObject(brush);
        return;
    }
    HBRUSH oldBrush;
    HPEN oldPen;

    if (this->_memDC) {
        oldBrush = (HBRUSH)SelectObject(this->_memDC, brush);
        oldPen = (HPEN)SelectObject(this->_memDC, pen);
        Ellipse(this->_memDC, cx - rx, cy - ry, cx + rx, cy + ry);
        SelectObject(this->_memDC, oldBrush);
        SelectObject(this->_memDC, oldPen);
    } else {
        HDC hdcMemDC = CreateCompatibleDC(0);
        HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hdcMemDC, this->hBitmap);
        oldBrush = (HBRUSH)SelectObject(hdcMemDC, brush);
        oldPen = (HPEN)SelectObject(hdcMemDC, pen);
        Ellipse(hdcMemDC, cx - rx, cy - ry, cx + rx, cy + ry);
        SelectObject(hdcMemDC, oldBrush);
        SelectObject(hdcMemDC, oldPen);
        SelectObject(hdcMemDC, hOldMemBmp);
        DeleteDC(hdcMemDC);
    }

    DeleteObject(brush);
    DeleteObject(pen);
}

VOID CmzBitmap::drawEllipse(COLORREF color, LONG cx, LONG cy, LONG rx, LONG ry, LONG borderThickness, LONG borderStyle) {
    this->drawEllipseEx(color, 0, cx, cy, rx, ry, borderThickness, borderStyle, BS_NULL, 0);
}

VOID CmzBitmap::fillEllipse(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG rx, LONG ry, LONG borderThickness, LONG borderStyle) {
    this->drawEllipseEx(color, 0, cx, cy, rx, ry, borderThickness, borderStyle, BS_SOLID, 0);
}

VOID CmzBitmap::fillEllipse(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG rx, LONG ry) {
    this->drawEllipseEx(color, 0, cx, cy, rx, ry, 1, PS_SOLID, BS_SOLID, 0);
}

VOID CmzBitmap::drawCircleEx(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG r, LONG borderThickness, LONG borderStyle, ULONG backgroundStyle, ULONG_PTR backgroundHatch) {
    this->drawEllipseEx(color, backgroundColor, cx, cy, r, r, borderThickness, borderStyle, backgroundStyle, backgroundHatch);
}

VOID CmzBitmap::drawCircle(COLORREF color, LONG cx, LONG cy, LONG r, LONG borderThickness, LONG borderStyle) {
    this->drawEllipseEx(color, 0, cx, cy, r, r, borderThickness, borderStyle, BS_NULL, 0);
}

VOID CmzBitmap::fillCircle(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG r, LONG borderThickness, LONG borderStyle) {
    this->drawEllipseEx(color, backgroundColor, cx, cy, r, r, borderThickness, borderStyle, BS_SOLID, 0);
}

VOID CmzBitmap::fillCircle(COLORREF color, COLORREF backgroundColor, LONG cx, LONG cy, LONG r) {
    this->drawEllipseEx(color, backgroundColor, cx, cy, r, r, 1, PS_SOLID, BS_SOLID, 0);
}

VOID CmzBitmap::drawLine(COLORREF color, LONG x1, LONG y1, LONG x2, LONG y2, LONG thickness, LONG style, float rotationAngle) {
    LONG rotationAngleDegree = (LONG)(10 * rotationAngle * 180 / M_PI);
    if (rotationAngleDegree != 0) {
        this->_rotatePoint(x1, y1, rotationAngle, &x1, &y1);
        this->_rotatePoint(x2, y2, rotationAngle, &x2, &y2);
    }

    HPEN pen = CreatePen(style, thickness, color);
    HPEN oldPen;
    POINT p;

    if (pen) {
        if (this->_memDC) {
            oldPen = (HPEN)SelectObject(this->_memDC, pen);
            MoveToEx(this->_memDC, x1, y1, &p);
            LineTo(this->_memDC, x2, y2);
            SelectObject(this->_memDC, oldPen);
        } else {
            HDC hdcMemDC = CreateCompatibleDC(0);
            HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hdcMemDC, this->hBitmap);
            oldPen = (HPEN)SelectObject(hdcMemDC, pen);
            MoveToEx(hdcMemDC, x1, y1, &p);
            LineTo(hdcMemDC, x2, y2);
            SelectObject(hdcMemDC, oldPen);
            SelectObject(hdcMemDC, hOldMemBmp);
            DeleteDC(hdcMemDC);
        }
        DeleteObject(pen);
    }
}

VOID CmzBitmap::drawHLine(COLORREF color, LONG x, LONG y, LONG width, LONG thickness, LONG style, float rotationAngle) {
    this->drawLine(color, x, y, x + width, y, thickness, style, rotationAngle);
}

VOID CmzBitmap::drawVLine(COLORREF color, LONG x, LONG y, LONG height, LONG thickness, LONG style, float rotationAngle) {
    this->drawLine(color, x, y, x, y + height, thickness, style, rotationAngle);
}

VOID CmzBitmap::drawPolyline(COLORREF color, POINT* pointsArray, ULONG pointsCount, BOOL closeShape, LONG thickness, LONG style, float rotationAngle) {
    LONG rotationAngleDegree = (LONG)(10 * rotationAngle * 180 / M_PI);
    if (rotationAngleDegree != 0) {
        POINT* p;
        for (ULONG i = 0; i < pointsCount; i++) {
            p = &pointsArray[i];
            this->_rotatePoint(p->x, p->y, rotationAngle, &p->x, &p->y);
        }
    }

    HPEN pen = CreatePen(style, thickness, color);
    HPEN oldPen;
    POINT* firstPoint = &pointsArray[0];
    if (pen) {
        POINT p;
        if (this->_memDC) {
            oldPen = (HPEN)SelectObject(this->_memDC, pen);
            MoveToEx(this->_memDC, firstPoint->x, firstPoint->y, &p);
            Polyline(this->_memDC, pointsArray, pointsCount);
            if (closeShape && (firstPoint->x != pointsArray[pointsCount-1].x || firstPoint->y != pointsArray[pointsCount-1].y)) {
                MoveToEx(this->_memDC, firstPoint->x, firstPoint->y, &p);
                LineTo(this->_memDC, pointsArray[pointsCount-1].x, pointsArray[pointsCount-1].y);
            }
            SelectObject(this->_memDC, oldPen);
        } else {
            HDC hdcMemDC = CreateCompatibleDC(0);
            HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hdcMemDC, this->hBitmap);
            oldPen = (HPEN)SelectObject(hdcMemDC, pen);
            MoveToEx(hdcMemDC, firstPoint->x, firstPoint->y, &p);
            Polyline(hdcMemDC, pointsArray, pointsCount);
            if (closeShape && (firstPoint->x != pointsArray[pointsCount-1].x || firstPoint->y != pointsArray[pointsCount-1].y)) {
                MoveToEx(hdcMemDC, firstPoint->x, firstPoint->y, &p);
                LineTo(hdcMemDC, pointsArray[pointsCount-1].x, pointsArray[pointsCount-1].y);
            }
            SelectObject(hdcMemDC, oldPen);
            SelectObject(hdcMemDC, hOldMemBmp);
            DeleteDC(hdcMemDC);
        }
        DeleteObject(pen);
    }
}

VOID CmzBitmap::floodFill(COLORREF borderColor, COLORREF fillColor, LONG xStart, LONG yStart, ULONG backgroundStyle, ULONG_PTR backgroundHatch) {
    if (backgroundStyle == BS_NULL) {
        return;
    }

    LOGBRUSH lb;
    ZeroMemory(&lb, sizeof(lb));
    lb.lbStyle = backgroundStyle;
    lb.lbColor = fillColor;
    lb.lbHatch = backgroundHatch;

    HBRUSH brush = CreateBrushIndirect(&lb);
    if (!brush) { return; }
    HBRUSH oldBrush;

    if (this->_memDC) {
        oldBrush = (HBRUSH)SelectObject(this->_memDC, brush);
        FloodFill(this->_memDC, xStart, yStart, borderColor);
        SelectObject(this->_memDC, oldBrush);
    } else {
        HDC hdcMemDC = CreateCompatibleDC(0);
        HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hdcMemDC, this->hBitmap);
        oldBrush = (HBRUSH)SelectObject(hdcMemDC, brush);
        FloodFill(hdcMemDC, xStart, yStart, borderColor);
        SelectObject(hdcMemDC, oldBrush);
        SelectObject(hdcMemDC, hOldMemBmp);
        DeleteDC(hdcMemDC);
    }

    DeleteObject(brush);
}

VOID CmzBitmap::saveToFile(CmzString fname) {
    if (this->hBitmap) {
        SaveBitmapToFile(fname, this->hBitmap);
    }
}

BOOL CmzBitmap::drawAlphaBlend(CmzBitmap *bmp, LONG x, LONG y, float alpha) {
    if (alpha < 0.0) {
        alpha = 0.0;
    }
    if (alpha > 1.0) {
        alpha = 1.0;
    }

    LONG srcW = bmp->width();
    LONG srcH = bmp->height();

    HDC hdcSrc = CreateCompatibleDC(NULL);
    HBITMAP hdcSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, bmp->hBitmap);

    /*
    SOMETHING NOT WORKING AS EXPECTED TO.

    BLENDFUNCTION bf;
    ZeroMemory(&bf, sizeof(BLENDFUNCTION));
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = (BYTE)(alpha * 255);
    bf.AlphaFormat = AC_SRC_ALPHA;

    BOOL result = AlphaBlend(this->_memDC, x, y, srcW, srcH, hdcSrc, 0, 0, srcW, srcH, bf);
    if (!result) {
        ShowLastSystemError();
    }

    return result;
    */

    LONG thisbmpContentBytesPerLine = (((24 * this->_width + 31) & (~31)) / 8);
    LONG thisbmpContentBytes = thisbmpContentBytesPerLine * this->_height;
    LONG bmpContentBytesPerLine = (((24 * bmp->_width + 31) & (~31)) / 8);
    LONG bmpContentBytes = bmpContentBytesPerLine * bmp->_height;
    HANDLE heap = GetProcessHeap();
    BYTE* thisbmpDIBits = (BYTE*)HeapAlloc(heap, 0, thisbmpContentBytes);
    BYTE* bmpDIBits = (BYTE*)HeapAlloc(heap, 0, bmpContentBytes);

    BITMAPINFOHEADER bmpInfoHeader, thisbmpInfoHeader;
    LONG bitmapInfoHeaderSize = sizeof(BITMAPINFOHEADER);

    ZeroMemory(&bmpInfoHeader, bitmapInfoHeaderSize);
    bmpInfoHeader.biSize          = bitmapInfoHeaderSize;
    bmpInfoHeader.biWidth         = bmp->_width;
    bmpInfoHeader.biHeight        = bmp->_height;
    bmpInfoHeader.biPlanes        = 1;
    bmpInfoHeader.biBitCount      = 24;
    bmpInfoHeader.biCompression   = BI_RGB;

    ZeroMemory(&thisbmpInfoHeader, bitmapInfoHeaderSize);
    thisbmpInfoHeader.biSize          = bitmapInfoHeaderSize;
    thisbmpInfoHeader.biWidth         = this->_width;
    thisbmpInfoHeader.biHeight        = this->_height;
    thisbmpInfoHeader.biPlanes        = 1;
    thisbmpInfoHeader.biBitCount      = 24;
    thisbmpInfoHeader.biCompression   = BI_RGB;

    BITMAPINFO bmpInfo, thisbmpInfo;
    CopyMemory(&bmpInfo.bmiHeader, &bmpInfoHeader, bitmapInfoHeaderSize);
    CopyMemory(&thisbmpInfo.bmiHeader, &thisbmpInfoHeader, bitmapInfoHeaderSize);

    GetDIBits(hdcSrc, bmp->hBitmap, 0, bmp->_height, bmpDIBits, &bmpInfo, DIB_RGB_COLORS);
    GetDIBits(this->_memDC, this->hBitmap, 0, this->_height, thisbmpDIBits, &thisbmpInfo, DIB_RGB_COLORS);

    //CopyMemory(thisbmpDIBits, bmpDIBits, bmpContentBytes);
    LONG dst_offset;
    LONG src_offset;
    LONG dy;
    LONG count;
    for (LONG i = 0; i < bmp->_height; i++) {
        if (y+i < this->_height) {
            // if we start drawing at this coord then its like we draw at y=0, because of bitmap pixel data format
            dy = this->_height - bmp->_height;
            // we need adjust y coord to draw at expected y-position
            dst_offset = (i+dy-y)*thisbmpContentBytesPerLine + x * 3;
            src_offset = i * bmpContentBytesPerLine;
            count = bmpContentBytesPerLine;
            // we protect drawing from pixels wrapping to next lines
            if (x*3 + bmp->_width*3 > this->_width*3) {
                count = this->_width*3 - x*3;
            }
            // now at &thisbmpDIBits[dst_offset] we have destination pixels to manipulate on
            // and at &bmpDIBits[src_offset] we have source pixels to manipulate on
            // each pixel consist of three BYTE values
            // now we do alpha blending
            for (LONG j = 0; j < count; j+=3) {
                thisbmpDIBits[dst_offset+j] = (BYTE)((1-alpha)*thisbmpDIBits[dst_offset+j] + (alpha)*bmpDIBits[src_offset+j]); // BLUE
                thisbmpDIBits[dst_offset+j+1] = (BYTE)((1-alpha)*thisbmpDIBits[dst_offset+j+1] + (alpha)*bmpDIBits[src_offset+j+1]); // GREEN
                thisbmpDIBits[dst_offset+j+2] = (BYTE)((1-alpha)*thisbmpDIBits[dst_offset+j+2] + (alpha)*bmpDIBits[src_offset+j+2]); // RED
            }
            // if we replace for loop above with line below, we have simple bitblt functionality
            //CopyMemory(&thisbmpDIBits[dst_offset], &bmpDIBits[src_offset], count);
            /*
            // Here we have code to ensure which BYTES are responsible for RED, GREEN, BLUE values
            // set one of them to 0xff and the rest to 0x00, to check
            for (LONG j = 0; j < count; j++) {
                thisbmpDIBits[dst_offset+j] = 0xff; // BLUE
                thisbmpDIBits[dst_offset+j+1] = 0x00; // GREEN
                thisbmpDIBits[dst_offset+j+2] = 0x00; // RED
            }
            */
        }
    }

    SetDIBits(this->_memDC, this->hBitmap, 0, this->_height, thisbmpDIBits, &thisbmpInfo, DIB_RGB_COLORS);

    SelectObject(hdcSrc, hdcSrcOrigBmp);
    DeleteDC(hdcSrc);
    HeapFree(heap, 0, thisbmpDIBits);
    HeapFree(heap, 0, bmpDIBits);

    return TRUE;
}

BOOL CmzBitmap::drawBitmapEx(CmzBitmap *bmp, LONG x, LONG y, VOID (WINAPI *pixelManipulationRoutine)(BYTE bmpRed, BYTE bmpGreen, BYTE bmpBlue, BYTE* thisRed, BYTE* thisGreen, BYTE* thisBlue)) {
    LONG srcW = bmp->width();
    LONG srcH = bmp->height();

    HDC hdcSrc = CreateCompatibleDC(NULL);
    HBITMAP hdcSrcOrigBmp = (HBITMAP)SelectObject(hdcSrc, bmp->hBitmap);

    LONG thisbmpContentBytesPerLine = (((24 * this->_width + 31) & (~31)) / 8);
    LONG thisbmpContentBytes = thisbmpContentBytesPerLine * this->_height;
    LONG bmpContentBytesPerLine = (((24 * bmp->_width + 31) & (~31)) / 8);
    LONG bmpContentBytes = bmpContentBytesPerLine * bmp->_height;
    HANDLE heap = GetProcessHeap();
    BYTE* thisbmpDIBits = (BYTE*)HeapAlloc(heap, 0, thisbmpContentBytes);
    BYTE* bmpDIBits = (BYTE*)HeapAlloc(heap, 0, bmpContentBytes);

    BITMAPINFOHEADER bmpInfoHeader, thisbmpInfoHeader;
    LONG bitmapInfoHeaderSize = sizeof(BITMAPINFOHEADER);

    ZeroMemory(&bmpInfoHeader, bitmapInfoHeaderSize);
    bmpInfoHeader.biSize          = bitmapInfoHeaderSize;
    bmpInfoHeader.biWidth         = bmp->_width;
    bmpInfoHeader.biHeight        = bmp->_height;
    bmpInfoHeader.biPlanes        = 1;
    bmpInfoHeader.biBitCount      = 24;
    bmpInfoHeader.biCompression   = BI_RGB;

    ZeroMemory(&thisbmpInfoHeader, bitmapInfoHeaderSize);
    thisbmpInfoHeader.biSize          = bitmapInfoHeaderSize;
    thisbmpInfoHeader.biWidth         = this->_width;
    thisbmpInfoHeader.biHeight        = this->_height;
    thisbmpInfoHeader.biPlanes        = 1;
    thisbmpInfoHeader.biBitCount      = 24;
    thisbmpInfoHeader.biCompression   = BI_RGB;

    BITMAPINFO bmpInfo, thisbmpInfo;
    CopyMemory(&bmpInfo.bmiHeader, &bmpInfoHeader, bitmapInfoHeaderSize);
    CopyMemory(&thisbmpInfo.bmiHeader, &thisbmpInfoHeader, bitmapInfoHeaderSize);

    GetDIBits(hdcSrc, bmp->hBitmap, 0, bmp->_height, bmpDIBits, &bmpInfo, DIB_RGB_COLORS);
    GetDIBits(this->_memDC, this->hBitmap, 0, this->_height, thisbmpDIBits, &thisbmpInfo, DIB_RGB_COLORS);

    //CopyMemory(thisbmpDIBits, bmpDIBits, bmpContentBytes);
    LONG dst_offset;
    LONG src_offset;
    LONG dy;
    LONG count;
    for (LONG i = 0; i < bmp->_height; i++) {
        if (y+i < this->_height) {
            // if we start drawing at this coord then its like we draw at y=0, because of bitmap pixel data format
            dy = this->_height - bmp->_height;
            // we need adjust y coord to draw at expected y-position
            dst_offset = (i+dy-y)*thisbmpContentBytesPerLine + x * 3;
            src_offset = i * bmpContentBytesPerLine;
            count = bmpContentBytesPerLine;
            // we protect drawing from pixels wrapping to next lines
            if (x*3 + bmp->_width*3 > this->_width*3) {
                count = this->_width*3 - x*3;
            }
            // now at &thisbmpDIBits[dst_offset] we have destination pixels to manipulate on
            // and at &bmpDIBits[src_offset] we have source pixels to manipulate on
            // each pixel consist of three BYTE values
            // now we do alpha blending
            for (LONG j = 0; j < count; j+=3) {
                pixelManipulationRoutine(
                    bmpDIBits[src_offset+j+2], // RED
                    bmpDIBits[src_offset+j+1], // GREEN
                    bmpDIBits[src_offset+j], // BLUE

                    &thisbmpDIBits[dst_offset+j+2], // RED
                    &thisbmpDIBits[dst_offset+j+1], // GREEN
                    &thisbmpDIBits[dst_offset+j]); // BLUE
            }
            // if we replace for loop above with line below, we have simple bitblt functionality
            //CopyMemory(&thisbmpDIBits[dst_offset], &bmpDIBits[src_offset], count);
            /*
            // Here we have code to ensure which BYTES are responsible for RED, GREEN, BLUE values
            // set one of them to 0xff and the rest to 0x00, to check
            for (LONG j = 0; j < count; j++) {
                thisbmpDIBits[dst_offset+j] = 0xff; // BLUE
                thisbmpDIBits[dst_offset+j+1] = 0x00; // GREEN
                thisbmpDIBits[dst_offset+j+2] = 0x00; // RED
            }
            */
        }
    }

    SetDIBits(this->_memDC, this->hBitmap, 0, this->_height, thisbmpDIBits, &thisbmpInfo, DIB_RGB_COLORS);

    SelectObject(hdcSrc, hdcSrcOrigBmp);
    DeleteDC(hdcSrc);
    HeapFree(heap, 0, thisbmpDIBits);
    HeapFree(heap, 0, bmpDIBits);

    return TRUE;
}

VOID CmzBitmap::_rotatePoint(LONG x, LONG y, float rotationAngle, LONG* xRotated, LONG* yRotated, LONG rotationOriginX, LONG rotationOriginY) {
    if (x == rotationOriginX && y == rotationOriginY) {
        *xRotated = x;
        *yRotated = y;
        return;
    }

    //*xRotated = x * cos(rotationAngle) + y * sin(rotationAngle);
    //*yRotated = x * (-sin(rotationAngle)) + y * cos(rotationAngle);
    float dx = static_cast<float>(fabs((float)x - rotationOriginX));
    float dy = static_cast<float>(fabs((float)y - rotationOriginY));
    float r = static_cast<float>(sqrt(dx*dx + dy*dy));
    float sinAngleForPoint = dy / r;
    float cosAngleForPoint = dx / r;
    float angleForPoint = static_cast<float>(asin(sinAngleForPoint));
    float angleForRotatedPoint = angleForPoint + rotationAngle;

    float sinAngleForRotatedPoint = static_cast<float>(sin(angleForRotatedPoint));
    float cosAngleForRotatedPoint = static_cast<float>(cos(angleForRotatedPoint));
    *xRotated = mzstd::round(r * cosAngleForRotatedPoint) + rotationOriginX;
    *yRotated = mzstd::round(r * sinAngleForRotatedPoint) + rotationOriginY;
    dx = static_cast<float>(fabs((float)*xRotated - rotationOriginX));
    dy = static_cast<float>(fabs((float)*yRotated - rotationOriginY));
    float rRotated = static_cast<float>(sqrt(dx*dx + dy*dy));

    /*
    wprintf(L"_rotatePoint: (%d, %d) by %f around (%d, %d)\n",
                    x, y, rotationAngle, rotationOriginX, rotationOriginY, r);
    wprintf(
            L"              r:                        %f\n"
            L"              angleForPoint:            %f, (sin: %f)\n"
            L"              angleForRotatedPoint:     %f, (sin: %f)\n"
            L"              sinAngleForPoint:         %f,\n"
            L"              cosAngleForPoint:         %f,\n"
            L"              sinAngleForRotatedPoint:  %f,\n"
            L"              cosAngleForRotatedPoint:  %f,\n"
            L"              sin^2+cos^2:              %f,\n"
            L"              sinRot^2+cosRot^2:        %f,\n"
            L"              rRotated:                 %f,\n"
            L"  result point: (%d, %d)\n",
            r,
            angleForPoint, sin(angleForPoint), angleForRotatedPoint, sin(angleForRotatedPoint),
            sinAngleForPoint, cosAngleForPoint,
            sinAngleForRotatedPoint, cosAngleForRotatedPoint,
            sinAngleForPoint*sinAngleForPoint + cosAngleForPoint*cosAngleForPoint,
            sinAngleForRotatedPoint*sinAngleForRotatedPoint+cosAngleForRotatedPoint*cosAngleForRotatedPoint,
            rRotated,
            *xRotated, *yRotated);
    */
}

VOID CmzBitmap::getDIBits(BYTE** bmBits, DWORD* size) {
    *bmBits = NULL;
    *size = 0;
    
    if (this->_width < 1 || this->_height < 1) {
        return;
    }

    BITMAPINFO info;
    ZeroMemory(&info, sizeof(info));
    BOOL paintingBeginned = FALSE;
    if (!this->_memDC) {
        this->beginPaint();
        paintingBeginned = TRUE;
    }
    if (!this->_memDC) {
        return;
    }

    LONG result = GetDIBits(this->_memDC, this->hBitmap, 0, this->_height, NULL, &info, DIB_RGB_COLORS);
    if (!result) {
        // For large bitmaps returns ERROR_SUCCESS!
        //ShowLastSystemError();
        return;
    }
    
    LONG bitmapContentBytes = (((24 * this->_width + 31) & (~31)) / 8) * this->_height;
    HANDLE heap = GetProcessHeap();
    *bmBits = (BYTE*)HeapAlloc(heap, 0, bitmapContentBytes);
    if (*bmBits) {
        ZeroMemory(&info, sizeof(info));
        result = GetDIBits(this->_memDC, this->hBitmap, 0, this->_height, *bmBits, &info, DIB_RGB_COLORS);
        if (result > 0) {
            *size = bitmapContentBytes;
        } else {
            //ShowLastSystemError();
            HeapFree(heap, 0, *bmBits);
        }
    }

    if (paintingBeginned) {
        this->endPaint();
    }
}

VOID CmzBitmap::setOpacity(float opacity) {
    CmzBitmap* bmp = new CmzBitmap(this->_width, this->_height, RGB(0xff, 0xff, 0xff));
    if (bmp) {
        this->drawAlphaBlend(bmp, 0, 0, opacity);
        delete bmp;
    }
}

BOOL CmzBitmap::applyFilter3x3(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22) {
    LONG w = this->_width;
    LONG h = this->_height;

	if (w < 3 || h < 3) {
		return FALSE;
	}
    LONG thisbmpContentBytesPerLine = (((24 * this->_width + 31) & (~31)) / 8);
    LONG thisbmpContentBytes = thisbmpContentBytesPerLine * this->_height;
    HANDLE heap = GetProcessHeap();
    BYTE* thisbmpDIBits = (BYTE*)HeapAlloc(heap, 0, thisbmpContentBytes);
    BYTE* bmpDIBits = (BYTE*)HeapAlloc(heap, 0, thisbmpContentBytes);

    BITMAPINFOHEADER thisbmpInfoHeader;
    LONG bitmapInfoHeaderSize = sizeof(BITMAPINFOHEADER);

    ZeroMemory(&thisbmpInfoHeader, bitmapInfoHeaderSize);
    thisbmpInfoHeader.biSize          = bitmapInfoHeaderSize;
    thisbmpInfoHeader.biWidth         = this->_width;
    thisbmpInfoHeader.biHeight        = this->_height;
    thisbmpInfoHeader.biPlanes        = 1;
    thisbmpInfoHeader.biBitCount      = 24;
    thisbmpInfoHeader.biCompression   = BI_RGB;

    BITMAPINFO thisbmpInfo;
    CopyMemory(&thisbmpInfo.bmiHeader, &thisbmpInfoHeader, bitmapInfoHeaderSize);

    GetDIBits(this->_memDC, this->hBitmap, 0, this->_height, thisbmpDIBits, &thisbmpInfo, DIB_RGB_COLORS);

    LONG offset, prevOffset, nextOffset;
    LONG count;
	BYTE R, G, B;
	BYTE RN, GN, BN;
	BYTE R00, R01, R02, R10, R11, R12, R20, R21, R22;
	BYTE G00, G01, G02, G10, G11, G12, G20, G21, G22;
	BYTE B00, B01, B02, B10, B11, B12, B20, B21, B22;
	float s;
	float sum = f00 + f01 + f02 + f10 + f11 + f12 + f20 + f21 + f22;
    for (LONG i = 1; i < h - 1; i++) {
		offset = i*thisbmpContentBytesPerLine;
		prevOffset = (i-1)*thisbmpContentBytesPerLine;
		nextOffset = (i+1)*thisbmpContentBytesPerLine;
		count = thisbmpContentBytesPerLine;
		// now at &thisbmpDIBits[offset] we have destination pixels to manipulate on
		// each pixel consist of three BYTE values
		for (LONG j = 3; j < count-3; j+=3) {
			B = thisbmpDIBits[offset+j];
			B00 = thisbmpDIBits[prevOffset+j-3];
			B01 = thisbmpDIBits[prevOffset+j];
			B02 = thisbmpDIBits[prevOffset+j+3];
			B10 = thisbmpDIBits[offset+j-3];
			B11 = thisbmpDIBits[offset+j];
			B12 = thisbmpDIBits[offset+j+3];
			B20 = thisbmpDIBits[nextOffset+j-3];
			B21 = thisbmpDIBits[nextOffset+j];
			B22 = thisbmpDIBits[nextOffset+j+3];

			G = thisbmpDIBits[offset+j+1];
			G00 = thisbmpDIBits[prevOffset+j+1-3];
			G01 = thisbmpDIBits[prevOffset+j+1];
			G02 = thisbmpDIBits[prevOffset+j+1+3];
			G10 = thisbmpDIBits[offset+j+1-3];
			G11 = thisbmpDIBits[offset+j+1];
			G12 = thisbmpDIBits[offset+j+1+3];
			G20 = thisbmpDIBits[nextOffset+j+1-3];
			G21 = thisbmpDIBits[nextOffset+j+1];
			G22 = thisbmpDIBits[nextOffset+j+1+3];

			R = thisbmpDIBits[offset+j+2];
			R00 = thisbmpDIBits[prevOffset+j+2-3];
			R01 = thisbmpDIBits[prevOffset+j+2];
			R02 = thisbmpDIBits[prevOffset+j+2+3];
			R10 = thisbmpDIBits[offset+j+2-3];
			R11 = thisbmpDIBits[offset+j+2];
			R12 = thisbmpDIBits[offset+j+2+3];
			R20 = thisbmpDIBits[nextOffset+j+2-3];
			R21 = thisbmpDIBits[nextOffset+j+2];
			R22 = thisbmpDIBits[nextOffset+j+2+3];

			s = f00 * B00 + f01 * B01 + f02 * B02 +
				f10 * B10 + f11 * B11 + f12 * B12 +
				f20 * B20 + f21 * B21 + f22 * B22;
			BN = (BYTE)(s/sum);
			thisbmpDIBits[offset+j] = BN;

			
			s = f00 * G00 + f01 * G01 + f02 * G02 +
				f10 * G10 + f11 * G11 + f12 * G12 +
				f20 * G20 + f21 * G21 + f22 * G22;
			GN = (BYTE)(s/sum);
			thisbmpDIBits[offset+j+1] = GN;

			
			s = f00 * R00 + f01 * R01 + f02 * R02 +
				f10 * R10 + f11 * R11 + f12 * R12 +
				f20 * R20 + f21 * R21 + f22 * R22;
			RN = (BYTE)(s/sum);
			thisbmpDIBits[offset+j+2] = RN;
		}
    }

    SetDIBits(this->_memDC, this->hBitmap, 0, this->_height, thisbmpDIBits, &thisbmpInfo, DIB_RGB_COLORS);

    HeapFree(heap, 0, thisbmpDIBits);
    HeapFree(heap, 0, bmpDIBits);

    return TRUE;
}
