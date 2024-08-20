#include "pch.h"
#include "mzFont.h"
#include "mzSystemParameters.h"
using namespace mzstd;

VOID CmzFont::_commonInit(HDC hdc, COLORREF color, CmzString fontName, LONG size, LONG rotationAngle, LONG weight, BOOL italic, BOOL underline, BOOL strikeout) {
    this->setRealClassName(L"CmzFont");
    this->color = color;
    this->name = fontName;
    this->size = size;
    this->rotationAngle = rotationAngle;
    this->weight = weight;
    this->italic = italic;
    this->underline = underline;
    this->strikeout = strikeout;
    
    BOOL releaseDC = FALSE;
    if (hdc == NULL) {
        hdc = GetDC(0);
        releaseDC = TRUE;
    }
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWidth = 0;
	lf.lfWeight = weight;
    lf.lfEscapement = rotationAngle;
    //lf.lfOrientation = rotationAngle;
    lf.lfItalic = italic;
    lf.lfUnderline = underline;
    lf.lfStrikeOut = strikeout;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	wcsncpy(&lf.lfFaceName[0], fontName.getBuffer(), 32);

	this->hFont = CreateFontIndirect(&lf);
    if (releaseDC) {
        ReleaseDC(0, hdc);
    }
}

// FW_NORMAL
CmzFont::CmzFont(HDC hdc, COLORREF color, LONG type, LONG weight) {
    this->setRealClassName(L"CmzFont");
	this->_createFont(hdc, color, type, weight);
}

CmzFont::CmzFont(HDC hdc, COLORREF color, CmzString fontName, LONG size) {
    this->_commonInit(hdc, color, fontName, size, 0, FW_NORMAL, FALSE, FALSE, FALSE);
}

CmzFont::CmzFont(HDC hdc, COLORREF color, CmzString fontName, LONG size, LONG rotationAngle) {
    this->_commonInit(hdc, color, fontName, size, rotationAngle, FW_NORMAL, FALSE, FALSE, FALSE);
}

CmzFont::CmzFont(HDC hdc, COLORREF color, CmzString fontName, LONG size, LONG rotationAngle, LONG weight, BOOL italic, BOOL underline, BOOL strikeout) {
    this->_commonInit(hdc, color, fontName, size, rotationAngle, weight, italic, underline, strikeout);
}

CmzFont::~CmzFont() 
{
	if (this->hFont) {
		DeleteObject(this->hFont); 
	}
}

VOID CmzFont::_createFont(HDC hdc, COLORREF color, LONG type, LONG weight)
{
	NONCLIENTMETRICS ncm = GetNonClientMetrics();
    LOGFONT lf;
    BOOL releaseDC = FALSE;
    if (hdc == NULL) {
        hdc = GetDC(0);
        releaseDC = TRUE;
    }
    LONG logpixelsy = GetDeviceCaps(hdc, LOGPIXELSY);
    
	if (this->hFont) {
		DeleteObject(this->hFont);
        this->hFont = NULL;
    }
	
    switch (type) {
        case SYSTEM_WINDOW_CAPTION_FONT:
            lf = ncm.lfCaptionFont;
            weight = FW_SEMIBOLD;
            break;
        default:
            this->_commonInit(hdc, color, L"Arial", 10, 0, FW_NORMAL, FALSE, FALSE, FALSE);
            if (releaseDC) {
                ReleaseDC(0, hdc);
            }
            return;
    }
    
    lf.lfWeight = weight;
    this->color = color;
    this->name = lf.lfFaceName;
    this->size = lf.lfHeight * 72 / logpixelsy;
    this->rotationAngle = lf.lfEscapement;
    this->weight = lf.lfWeight;
    this->italic = lf.lfItalic;
    this->underline = lf.lfUnderline;
    this->strikeout = lf.lfStrikeOut;
    this->hFont = CreateFontIndirect(&lf);
    
    if (releaseDC) {
        ReleaseDC(0, hdc);
    }
} 

SIZE CmzFont::measureString(CmzString s, HDC hdc) {
	HDC hdcInternal;
    if (hdc == 0) {
        hdcInternal = GetDC(0);
    } else {
        hdcInternal = hdc;
    }

    HFONT oldFont = (HFONT)SelectObject(hdcInternal, this->hFont);
    SIZE size;
    GetTextExtentPoint(hdcInternal, s.getBuffer(), (INT)s.length(), &size);
	SelectObject(hdcInternal, oldFont);

    if (hdc == 0) {
        ReleaseDC(0, hdcInternal);
    }

	return size;
}

SIZE CmzFont::measureString(CmzString s, LONG charSpacing, HDC hdc) {
//SIZE s1; s1.cx = 40; s1.cy = 12; return s1;
	HDC hdcInternal;
    if (hdc == 0) {
        hdcInternal = GetDC(0);
    } else {
        hdcInternal = hdc;
    }

    SIZE size, letterSize;
    size_t len = s.length();
    WCHAR str[2];
    ZeroMemory(str, 2*sizeof(WCHAR));

    size.cx = 0;
    size.cy = 0;
    
    HFONT oldFont = (HFONT)SelectObject(hdcInternal, this->hFont);
    for (ULONG i = 0; i < len; i++) {
        str[0] = s.getCharAt(i);
        GetTextExtentPoint(hdcInternal, str, 1, &letterSize);
        size.cx += letterSize.cx;
        if (letterSize.cy > size.cy) {
            size.cy = letterSize.cy;
        }
        if (i < len - 1) {
            size.cx += charSpacing;
        }
    }

	SelectObject(hdcInternal, oldFont);
    if (hdc == 0) {
        ReleaseDC(0, hdcInternal);
    }

	return size;
}

SIZE CmzFont::measureChar(WCHAR ch, HDC hdc) {
SIZE s1; s1.cx = 40; s1.cy = 12; return s1;
	HDC hdcInternal;
    if (hdc == 0) {
        hdcInternal = GetDC(0);
    } else {
        hdcInternal = hdc;
    }
    WCHAR str[2];

    str[0] = ch;
    str[1] = NULL;

    HFONT oldFont = (HFONT)SelectObject(hdcInternal, this->hFont);
    SIZE size;
    GetTextExtentPoint(hdcInternal, str, 1, &size);
	SelectObject(hdcInternal, oldFont);

    if (hdc == 0) {
        ReleaseDC(0, hdcInternal);
    }

	return size;
}

CmzStringList *CmzFont::getAvailableSystemFontNames() {
	HDC hdc = GetDC(0);
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));

	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfFaceName[0] = NULL;
	lf.lfPitchAndFamily = 0;

	CmzStringList *result = new CmzStringList();
	EnumFontFamiliesEx(hdc, &lf, CmzFont::_fontEnumProcGetNames, (LPARAM)result, 0);
	ReleaseDC(0, hdc);

	return result;
}

INT CmzFont::_fontEnumProcGetNames(const LOGFONT *lpelfe, const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam) {
	CmzString *fontName = new CmzString(lpelfe->lfFaceName);
	CmzStringList *lst = (CmzStringList*)lParam;
	if (lst) {
		// jeśli font o takiej nazwie już w tablicy istnieje, to go nie dodajemy
		BOOL exists = FALSE;
		CmzString s;
		for (ULONG i = 0; i < lst->size(); i++) {
			s = lst->getAt(i);
			if (s == *fontName) {
				exists = TRUE;
				break;
			}
		}
		if (!exists) {
			lst->append(*fontName);
		}
	}

	delete fontName;
	return 1;
}
