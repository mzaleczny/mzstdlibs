#ifndef __MZSTDGRAPHICS_FONT_H__
#define __MZSTDGRAPHICS_FONT_H__

#include "mzstdgraphicsCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzStringList.h"

#define SYSTEM_WINDOW_CAPTION_FONT  1

class DECLSPEC_MZSTDGRAPHICS CmzFont : public mzstd::CmzObject {
public:
	HFONT hFont;
    mzstd::CmzString name;
    LONG size;
    COLORREF color;
    LONG rotationAngle;
    LONG weight;
    BOOL italic;
    BOOL underline;
    BOOL strikeout;
    
	WINAPI CmzFont(HDC hdc, COLORREF color, LONG type, LONG weight = FW_NORMAL);
	WINAPI CmzFont(HDC hdc, COLORREF color, mzstd::CmzString fontName, LONG size);
    // rotationAngle is in teenths of degrees
	WINAPI CmzFont(HDC hdc, COLORREF color, mzstd::CmzString fontName, LONG size, LONG rotationAngle);
    // rotationAngle is in teenths of degrees
	WINAPI CmzFont(HDC hdc, COLORREF color, mzstd::CmzString fontName, LONG size, LONG rotationAngle, LONG weight, BOOL italic, BOOL underline, BOOL strikeout);

	virtual WINAPI ~CmzFont();
	SIZE WINAPI measureString(mzstd::CmzString s, HDC hdc);
	SIZE WINAPI measureString(mzstd::CmzString s, LONG charSpacing, HDC hdc);
	SIZE WINAPI measureChar(WCHAR ch, HDC hdc);
	static mzstd::CmzStringList* WINAPI getAvailableSystemFontNames();


protected:
    WINAPI CmzFont() {};
    VOID WINAPI _commonInit(HDC hdc, COLORREF color, mzstd::CmzString fontName, LONG size, LONG rotationAngle, LONG weight, BOOL italic, BOOL underline, BOOL strikeout);
	VOID WINAPI _createFont(HDC hdc, COLORREF color, LONG type, LONG weight);

	static INT CALLBACK _fontEnumProcGetNames(const LOGFONT *lpelfe,
		const TEXTMETRIC *lpntme, DWORD FontType, LPARAM lParam);
};

#endif
