#include "pch.h"
#include "mzString.h"
#include <wchar.h>
#include <stdio.h>
#include "mzLocale.h"
#include "mzMath.h"
#include "mzStringList.h"
#include "mzArrayUtils.h"

#define DEFAULT_STRING_BUFFER_LENGTH	128

#define FORMAT_PARSING_STATE_NONE           0
#define FORMAT_PARSING_STATE_READFLAGS      1
#define FORMAT_PARSING_STATE_READWIDTH      2
#define FORMAT_PARSING_STATE_READPRECISION  3
#define FORMAT_PARSING_STATE_READTYPE       4

#define FORMAT_DEFAULT_PRECISION            4

#define WORD_BOUNDARY_CHARS     L" \t.,;?!"

namespace mzstd {

// --------------------          ą       ć       ę       ł       ń       ó       ś       ź       ż       Ą       Ć       Ę       Ł       Ń       Ó       Ś       Ź       Ż
BYTE allowed_codes_ansi[]	 = {  0xb9,   0xe6,   0xea,   0xb3,   0xf1,   0xf3,   0x9c,   0x9f,   0xbf,   0xa5,   0xc6,   0xca,   0xa3,   0xd1,   0xd3,   0x8c,   0x8f,   0xaf };
BYTE allowed_codes_iso2[]	 = {  0xb1,   0xe6,   0xea,   0xb3,   0xf1,   0xf3,   0xb6,   0xbc,   0xbf,   0xa1,   0xc6,   0xca,   0xa3,   0xd1,   0xd3,   0xa6,   0xac,   0xaf };
BYTE allowed_codes_oem_852[] = {  0xa5,   0x86,   0xa9,   0x88,   0xe4,   0xa2,   0x98,   0xab,   0xbe,   0xa4,   0x8f,   0xa8,   0x9d,   0xe3,   0xe0,   0x97,   0x8d,   0xbd };
WORD allowed_codes_utf8[]	 = {0x85c4, 0x87c4, 0x99c4, 0x82c5, 0x84c5, 0xb3c3, 0x9bc5, 0xbac5, 0xbcc5, 0x84c4, 0x86c4, 0x98c4, 0x81c5, 0x83c5, 0x93c3, 0x9ac5, 0xb9c5, 0xbbc5 };
WORD allowed_codes_unicode[] = {0x0105, 0x0107, 0x0119, 0x0142, 0x0144, 0x00f3, 0x015b, 0x017a, 0x017c, 0x0104, 0x0106, 0x0118, 0x0141, 0x0143, 0x00d3, 0x015a, 0x0179, 0x017b };


LONGLONG WINAPI DetectDataEncoding(BYTE *buf, size_t buflen) {
	if (buflen >= 3 && buf[0] == (BYTE)0xef && buf[1] == (BYTE)0xbb && buf[2] == (BYTE)0xbf) {
		return ENC_UTF8_WITH_BOM;
	}
	ULONGLONG i;
	BOOL is_utf8 = TRUE;
	BOOL is_unicode = TRUE;
	BOOL is_ansi = TRUE;
	BOOL is_iso2 = TRUE;
    BOOL is_oem_852 = TRUE;
	BYTE b;
	WORD w;


	// najpierw sprawdzamy czy UTF8
	for (i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII lub znak końca linii, to pasuje do każdego kodowania
		if (b == (BYTE)0x0a || b == (BYTE)0x0d || (b >= (BYTE)0x20 && b <= (BYTE)0x7f)) {
			continue;
		}

		// sprawdzaymy czy dopuszczalny znak utf8
		// pobieramy słowo
		w = 0;
		if (i + 1 < buflen) {
			w = buf[i+1];
			w <<= 8;
			w |= b;
		}

		if (!in_array(allowed_codes_utf8, sizeof(allowed_codes_utf8), w)) {
			is_utf8 = FALSE;
			break;
		}

		i++;
	}

	if (is_utf8) {
		return ENC_UTF8;
	}


	// sprawdzamy czy UNICODE
	for (i = 0; i < buflen; i+=2) {
		b = buf[i];
		// pobieramy słowo
		w = 0;
		if (i + 1 < buflen) {
			w = buf[i+1];
			w <<= 8;
			w |= b;
		}

		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (w >= (WORD)0x20 && w <= (WORD)0x7f) {
			continue;
		}


		if (!in_array(allowed_codes_unicode, sizeof(allowed_codes_unicode), w)) {
			is_unicode = FALSE;
			break;
		}
	}

	if (is_unicode) {
		return ENC_UNICODE;
	}

	// now we check if have one-byte encoding such as ANSI/ISO2/OEM852
	for (i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (BYTE)0x20 && b <= (BYTE)0x7f) {
			continue;
		}

		// sprawdzaymy czy dopuszczalny znak ansi
		if (is_ansi) {
			if (!in_array(allowed_codes_ansi, sizeof(allowed_codes_ansi), b)) {
				is_ansi = FALSE;
			}
		}

		// sprawdzaymy czy dopuszczalny znak ansi
		if (is_iso2) {
			if (!in_array(allowed_codes_iso2, sizeof(allowed_codes_iso2), b)) {
				is_iso2 = FALSE;
			}
		}

		// sprawdzaymy czy dopuszczalny znak oem_852
		if (is_oem_852) {
			if (!in_array(allowed_codes_oem_852, sizeof(allowed_codes_oem_852), b)) {
				is_oem_852 = FALSE;
			}
		}

		// jeśli wszystkie kodowania jednobajtowe już odpadły, to nie sprwdzamy dalej
		if (!is_ansi && !is_iso2 && !is_oem_852) {
			break;
		}
	}

    // We accept one-byte encoding in order: ANSI, ISO2, OEM_852
	if (is_ansi) {
		return ENC_ANSI;
	}

	if (is_iso2) {
		return ENC_ISO2;
	}

	if (is_oem_852) {
		return ENC_OEM_852;
	}

	return 0;
}


VOID WINAPI Iso2ToAnsi(BYTE *buf, size_t buflen) {
	BYTE b;
	for (ULONGLONG i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (BYTE)0x20 && b <= (BYTE)0x7f) {
			continue;
		}

		for (ULONGLONG k = 0; k < sizeof(allowed_codes_iso2); k++) {
			if (b == allowed_codes_iso2[k]) {
				buf[i] = allowed_codes_ansi[k];
				break;
			}
		}
	}
}


VOID WINAPI AnsiToIso2(BYTE *buf, size_t buflen) {
	BYTE b;
	for (ULONGLONG i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (BYTE)0x20 && b <= (BYTE)0x7f) {
			continue;
		}

		for (ULONGLONG k = 0; k < sizeof(allowed_codes_ansi); k++) {
			if (b == allowed_codes_ansi[k]) {
				buf[i] = allowed_codes_iso2[k];
				break;
			}
		}
	}
}

DECLSPEC_MZSTD VOID WINAPI Oem852ToAnsi(BYTE *buf, size_t buflen) {
	BYTE b;
	for (ULONGLONG i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (BYTE)0x20 && b <= (BYTE)0x7f) {
			continue;
		}

		for (ULONGLONG k = 0; k < sizeof(allowed_codes_oem_852); k++) {
			if (b == allowed_codes_oem_852[k]) {
				buf[i] = allowed_codes_ansi[k];
				break;
			}
		}
	}
}

DECLSPEC_MZSTD VOID WINAPI AnsiToOem852(BYTE *buf, size_t buflen) {
	BYTE b;
	for (ULONGLONG i = 0; i < buflen; i++) {
		b = buf[i];
		// jeśli mamy znak ASCII, to pasuje do każdego kodowania
		if (b >= (BYTE)0x20 && b <= (BYTE)0x7f) {
			continue;
		}

		for (ULONGLONG k = 0; k < sizeof(allowed_codes_ansi); k++) {
			if (b == allowed_codes_ansi[k]) {
				buf[i] = allowed_codes_oem_852[k];
				break;
			}
		}
	}
}

WORD WINAPI getCharType(WCHAR ch) {
    WORD type;
    BOOL result = GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, &ch, 1, &type);
    if (result) {
        return type;
    }

    return 0;
}

BOOL WINAPI isCharUpper(WCHAR ch) {
    return getCharType(ch) & C1_UPPER;
}

BOOL WINAPI isCharLower(WCHAR ch) {
    return getCharType(ch) & C1_LOWER;
}

BOOL WINAPI isCharDigit(WCHAR ch) {
    return getCharType(ch) & C1_DIGIT;
}

BOOL WINAPI isCharSpace(WCHAR ch) {
    return getCharType(ch) & C1_SPACE;
}

BOOL WINAPI isCharPunct(WCHAR ch) {
    return getCharType(ch) & C1_PUNCT;
}

BOOL WINAPI isCharCntrl(WCHAR ch) {
    return getCharType(ch) & C1_CNTRL;
}

BOOL WINAPI isCharBlank(WCHAR ch) {
    return getCharType(ch) & C1_BLANK;
}

BOOL WINAPI isCharXdigit(WCHAR ch) {
    return getCharType(ch) & C1_XDIGIT;
}

BOOL WINAPI isCharAlpha(WCHAR ch) {
    return getCharType(ch) & C1_ALPHA;
}

BOOL WINAPI isCharDefined(WCHAR ch) {
    return getCharType(ch) & C1_DEFINED;
}

BOOL WINAPI isCharAlphanumeric(WCHAR ch) {
    WORD type = getCharType(ch);
    return (type & C1_ALPHA) || (type & C1_DIGIT);
}

BOOL WINAPI isCharPrintable(WCHAR ch) {
    WORD type = getCharType(ch);
    return !(type & C1_CNTRL);
}

BOOL WINAPI isCharSentenceBoundary(WCHAR ch) {
    if (wcschr(WORD_BOUNDARY_CHARS, ch) != NULL) {
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI isDecimal(const WCHAR* str) {
    if (!str) return FALSE;

    size_t len = wcslen(str);
    if (len < 1) return FALSE;

    for (size_t i = 0; i < len; i++) {
        if (i == 0 && str[i] == L'-') {
            continue;
        }

        if (!iswdigit(str[i])) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL WINAPI isDouble(const WCHAR* str) {
    if (!str) return FALSE;

    size_t len = wcslen(str);
    BOOL hasDot = FALSE;
    WCHAR ch;

    if (len < 1) return FALSE;

    for (size_t i = 0; i < len; i++) {
        ch = str[i];

        if (i == 0 && ch == L'-') {
            continue;
        }

        if (!hasDot && (ch == L'.' || ch == L',')) {
            hasDot = TRUE;
            continue;
        }

        if (!iswdigit(ch)) {
            return FALSE;
        }
    }

    return TRUE;
}

CmzString WINAPI FormatString(const CmzString& s, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result(s);
    size_t len = result.length();

    if (width > 0 && width > len) {
        ULONG paddingLen = static_cast<ULONG>(width - len);
        CmzString padding = CmzString::makeString(fillChar, paddingLen);
        if (align == FORMAT_ALIGN_RIGHT) {
            result.prepend(padding);
        } else {
            result.append(padding);
        }
    }

    return result;
}

CmzString WINAPI FormatLONGValue(LONG value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result(value);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatULONGValue(ULONG value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result(value);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatBoolValue(BOOL value, BOOL uppercase, LONG align, WCHAR fillChar, ULONG width) {
    WCHAR uc = L'T';
    if (!uppercase) {
        uc = L't';
    }
    CmzString result(value, uc);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatCharValue(CHAR value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result(value);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatWcharValue(WCHAR value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result(value);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatFloatValue(float value, LONG align, WCHAR fillChar, ULONG width, LONG precision) {
    if (precision == 0) {
        precision = FORMAT_DEFAULT_PRECISION;
    }

    CmzString result(value, precision);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatDoubleValue(double value, LONG align, WCHAR fillChar, ULONG width, LONG precision) {
    if (precision == 0) {
        precision = FORMAT_DEFAULT_PRECISION;
    }

    CmzString result(value, precision);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatHexValue(LONG value, LONG align, WCHAR fillChar, ULONG width, BOOL prepend0x, BOOL appendH, BOOL useSmallAlpha, LONG minLen) {
    CmzString result = intToHex(value, minLen, useSmallAlpha, prepend0x);
    if (appendH) {
        result += L'h';
    }
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatStringValue(const WCHAR* value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result(value);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatTimeValue(SYSTEMTIME* value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result = CmzString((ULONG)value->wHour, FALSE, 2) + L":" + CmzString((ULONG)value->wMinute, FALSE, 2) + L":" +
                     CmzString((ULONG)value->wSecond, FALSE, 2) + L"." + CmzString((ULONG)value->wMilliseconds, FALSE, 5);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatDateValue(SYSTEMTIME* value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result = CmzString((ULONG)value->wYear, FALSE, 4) + L"-" + CmzString((ULONG)value->wMonth, FALSE, 2) + L"-" +
                     CmzString((ULONG)value->wDay, FALSE, 2);
    return FormatString(result, align, fillChar, width);
}

CmzString WINAPI FormatDateTimeValue(SYSTEMTIME* value, LONG align, WCHAR fillChar, ULONG width) {
    CmzString result = CmzString((ULONG)value->wYear, FALSE, 4) + L"-" + CmzString((ULONG)value->wMonth, FALSE, 2) + L"-" +
                     CmzString((ULONG)value->wDay, FALSE, 2) + L" " +
                     CmzString((ULONG)value->wHour, FALSE, 2) + L":" + CmzString((ULONG)value->wMinute, FALSE, 2) + L":" +
                     CmzString((ULONG)value->wSecond, FALSE, 2) + L"." + CmzString((ULONG)value->wMilliseconds, FALSE, 5);
    return FormatString(result, align, fillChar, width);
}

CmzString __cdecl FormatMessage(const WCHAR* format, va_list paramList) {
    // format parameter is a string that can contain fields of following formats:
    // %[flags][width][.precision]type
    //     [flags] can be one CHAR of following:
    //          - - align value to the left of space defined by [width] parameter (default align to the right)
    //          _(WCHAR) - defines filling CHAR (default is space: ' ') for padding value if [width] value
    //                     is greater than text length of the value
    //     [width] is number defining CHAR width of the string in which value is placed. If [width] is greater
    //             than text length of the value then the rest of the place is filled by filling CHAR.
    //     [.precision] - in case of floating point values defines number of digits after period to print (default = 4)
    //                    in other cases it doesn't matter
    //     type:
    //         d   - LONG value
    //         dbx - LONG value with hex (BYTE size)
    //         dwx - LONG value with hex (WORD size)
    //         ddx - LONG value with hex (DWORD size)
    //         ud  - UNSIGNED LONG value
    //         f   - floating point value
    //         fd  - double value
    //         b   - boolean value (true/false)
    //         B   - boolean value (TRUE/FALSE)
    //         tm  - insert current time (no value is get from parameters stack) in format: 'hh:mm:ss.ms'
    //         dt  - insert current date (no value is get from parameters stack) in format: 'yyyy-mm-dd'
    //         dtm - insert current date time (no value is get from parameters stack) in format: 'yyyy-mm-dd hh:mm:ss.ms'
    //         x   - UNSIGNED LONG value presented as hex, use small hex digits
    //         #x  - UNSIGNED LONG value presented as hex preceded by L"0x", use small hex digits
    //         xh  - UNSIGNED LONG value presented as hex followed by L'h', use small hex digits
    //         #9x - UNSIGNED LONG value presented as hex preceded by L"0x" and left-padded by L'0', use small hex digits
    //               to the specified count
    //         x9h - UNSIGNED LONG value presented as hex followed by L'h' and left-padded by L'0'
    //               to the specified count, use small hex digits
    //         X   - UNSIGNED LONG value presented as hex, use large hex digits
    //         #X  - UNSIGNED LONG value presented as hex preceded by L"0x", use large hex digits
    //         Xh  - UNSIGNED LONG value presented as hex followed by L'h', use large hex digits
    //         #9X - UNSIGNED LONG value presented as hex preceded by L"0x" and left-padded by L'0', use large hex digits
    //               to the specified count
    //         X9h - UNSIGNED LONG value presented as hex followed by L'h' and left-padded by L'0'
    //               to the specified count, use large hex digits
    //         p   - alias for #8x
    //         P   - alias for #8X
    //         s   - wide string value (parameter must be WCHAR*)
    //         sc  - string value (parameter must be CHAR*)
    //         cs  - string value (parameter must be CmzString*)
    //         o   - object value (parameter must be CObject*) - prints result of o->toString()
    //         c   - char value (parameter must be CHAR)
    //         wc  - wide char value (parameter must be WCHAR)
    CmzString result;
    CmzString token;
    size_t len = wcslen(format);
    ULONG i;
    WCHAR ch;
    BOOL token_started = FALSE;
    LONG fieldAlign = FORMAT_ALIGN_RIGHT;
    LONG fieldWidth = 0;
    LONG fieldPrecision = 0;
    WCHAR fieldFillChar = L' ';
    LONG fieldReadingState = FORMAT_PARSING_STATE_NONE;
    LONG LONGValue;
    ULONG uLONGValue;
    BOOL boolValue;
    WCHAR* wstrValue;
    WCHAR wcharValue;
    CHAR* strValue;
    CHAR charValue;
    //float floatValue;
    double doubleValue;
    LONG doExtraIncrement = 0;
    BOOL valueFormatted = FALSE;

    i = 0;
    while (i < len) {
        ch = format[i];
        i++;
        if (ch != L'%' && !token_started) {
            result += ch;
        } else {
            if (ch == L'%') {
                if (!token_started) {
                    token_started = TRUE;
                    fieldReadingState = FORMAT_PARSING_STATE_READFLAGS;
                } else {
                    result += ch;
                }
            } else {
                switch (fieldReadingState) {
                    case FORMAT_PARSING_STATE_READFLAGS: // read flags
                        if (ch == L'-') {
                            fieldAlign = FORMAT_ALIGN_LEFT;
                            token = COMMON_EMPTY_STRING;
                            continue;
                        }
                        if (ch == L'_') {
                            if (i < len) {
                                fieldFillChar = format[i];
                                fieldReadingState = FORMAT_PARSING_STATE_READWIDTH;
                                token = COMMON_EMPTY_STRING;
                                i++;
                            }
                            continue;
                        }
                        fieldReadingState = FORMAT_PARSING_STATE_READWIDTH;
                        i--;
                        break;
                    case FORMAT_PARSING_STATE_READWIDTH: // read width
                        if (isCharDigit(ch)) {
                            token += ch;
                            continue;
                        }
                        if (token.length() > 0) {
                            fieldWidth = token.toInt();
                        }
                        if (ch == L'.') {
                            fieldReadingState = FORMAT_PARSING_STATE_READPRECISION;
                            token = COMMON_EMPTY_STRING;
                            continue;
                        }
                        fieldReadingState = FORMAT_PARSING_STATE_READTYPE;
                        token = COMMON_EMPTY_STRING;
                        i--;
                        break;
                    case FORMAT_PARSING_STATE_READPRECISION: // read precision
                        if (isCharDigit(ch)) {
                            token += ch;
                            continue;
                        }
                        if (token.length() > 0) {
                            fieldPrecision = token.toInt();
                        }
                        fieldReadingState = FORMAT_PARSING_STATE_READTYPE;
                        token = COMMON_EMPTY_STRING;
                        i--;
                        break;
                    case FORMAT_PARSING_STATE_READTYPE: // read type
                        doExtraIncrement = 0;
                        valueFormatted = FALSE;
                        // Format time value
                        if (!valueFormatted && ch == L't' && i < len && format[i] == L'm') {
                            SYSTEMTIME tm;
                            GetLocalTime(&tm);
                            result += FormatTimeValue(&tm, fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format date-time value
                        if (!valueFormatted && ch == L'd' && i < len && format[i] == L't' && i + 1 < len && format[i+1] == L'm') {
                            SYSTEMTIME tm;
                            GetLocalTime(&tm);
                            result += FormatDateTimeValue(&tm, fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format date value
                        if (!valueFormatted && ch == L'd' && i < len && format[i] == L't') {
                            SYSTEMTIME tm;
                            GetLocalTime(&tm);
                            result += FormatDateValue(&tm, fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format decimal BYTE value with hex
                        if (!valueFormatted && ch == L'd' && i < len && format[i] == L'b' && i+1 < len && format[i+1] == L'x') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatLONGValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth) + L" [";
                            result += FormatHexValue(uLONGValue, FORMAT_ALIGN_LEFT, L'0', 0, TRUE, FALSE, TRUE, 2) + L"]";
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format decimal WORD value with hex
                        if (!valueFormatted && ch == L'd' && i < len && format[i] == L'w' && i+1 < len && format[i+1] == L'x') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatLONGValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth) + L" [";
                            result += FormatHexValue(uLONGValue, FORMAT_ALIGN_LEFT, L'0', 0, TRUE, FALSE, TRUE, 4) + L"]";
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format decimal DWORD value with hex
                        if (!valueFormatted && ch == L'd' && i < len && format[i] == L'd' && i+1 < len && format[i+1] == L'x') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatLONGValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth) + L" [";
                            result += FormatHexValue(uLONGValue, FORMAT_ALIGN_LEFT, L'0', 0, TRUE, FALSE, TRUE, 8) + L"]";
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format decimal value
                        if (!valueFormatted && ch == L'd') {
                            LONGValue = va_arg(paramList, LONG);
                            result += FormatLONGValue(LONGValue, fieldAlign, fieldFillChar, fieldWidth);
                            valueFormatted = TRUE;
                        }
                        // Format boolean value
                        if (!valueFormatted && (ch == L'b' || ch == L'B')) {
                            boolValue = va_arg(paramList, BOOL);
                            BOOL uppercase = TRUE;
                            if (ch == L'b') {
                                uppercase = FALSE;
                            }
                            result += FormatBoolValue(boolValue, uppercase, fieldAlign, fieldFillChar, fieldWidth);
                            valueFormatted = TRUE;
                        }
                        // Format unsigned decimal value
                        if (!valueFormatted && ch == L'u' && i < len && format[i] == L'd') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatULONGValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format double value
                        if (!valueFormatted && ch == L'f' && i < len && format[i] == L'd') {
                            doubleValue = va_arg(paramList, double);
                            result += FormatDoubleValue(doubleValue, fieldAlign, fieldFillChar, fieldWidth, fieldPrecision);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format float value
                        if (!valueFormatted && ch == L'f') {
                            // float - 4bytes, double - 8bytes
                            // 2 lines of code below is OK, but it shouldn't
                            doubleValue = va_arg(paramList, double);
                            result += FormatDoubleValue(doubleValue, fieldAlign, fieldFillChar, fieldWidth, fieldPrecision);
                            // --- Below code doesn't work! Don't know why ---
                            /*
                            floatValue = va_arg(paramList, float); // Here is wrong getting parameter
                            wprintf(L"---%f\n", floatValue); // floatValue is ERROR
                            result += FormatFloatValue(floatValue, fieldAlign, fieldFillChar, fieldWidth, fieldPrecision);
                            */
                            valueFormatted = TRUE;
                        }
                        // Format pointer value
                        if (!valueFormatted && (ch == L'p' || ch == L'P')) {
                            uLONGValue = va_arg(paramList, ULONG);
                            if (ch == L'p') {
                                result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, TRUE, FALSE, TRUE, 8);
                            } else {
                                result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, TRUE, FALSE, FALSE, 8);
                            }
                            valueFormatted = TRUE;
                        }
                        // Format string value
                        if (!valueFormatted && ch == L's' && i < len && format[i] == L'c') {
                            strValue = va_arg(paramList, CHAR*);
                            CmzString s(strValue);
                            result += FormatStringValue(s.getBuffer(), fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format wide string value
                        if (!valueFormatted && ch == L's') {
                            wstrValue = va_arg(paramList, WCHAR*);
                            CmzString s(wstrValue);
                            result += FormatStringValue(s.getBuffer(), fieldAlign, fieldFillChar, fieldWidth);
                            valueFormatted = TRUE;
                        }
                        // Format CmzString value
                        if (!valueFormatted && ch == L'c' && i < len && format[i] == L's') {
                            CmzString* s = va_arg(paramList, CmzString*);
                            result += FormatStringValue(s->getBuffer(), fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format CmzString value
                        if (!valueFormatted && ch == L'c') {
                            charValue = va_arg(paramList, CHAR);
                            result += FormatCharValue(charValue, fieldAlign, fieldFillChar, fieldWidth);
                            valueFormatted = TRUE;
                        }
                        // Format CmzString value
                        if (!valueFormatted && ch == L'w' && i < len && format[i] == L'c') {
                            wcharValue = va_arg(paramList, WCHAR);
                            result += FormatWcharValue(wcharValue, fieldAlign, fieldFillChar, fieldWidth);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format object value
                        if (!valueFormatted && ch == L'o') {
                            CmzObject* obj = va_arg(paramList, CmzObject*);
                            result += FormatStringValue(obj->toString().getBuffer(), fieldAlign, fieldFillChar, fieldWidth);
                            valueFormatted = TRUE;
                        }
                        // Format #9x value
                        if (!valueFormatted && ch == L'#' && i < len && isCharDigit(format[i]) && i+1 < len && format[i+1] == L'x') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, TRUE, FALSE, TRUE, format[i] - L'0');
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format #9X value
                        if (!valueFormatted && ch == L'#' && i < len && isCharDigit(format[i]) && i+1 < len && format[i+1] == L'X') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, TRUE, FALSE, FALSE, format[i] - L'0');
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format #x9h value
                        if (!valueFormatted && ch == L'x' && i < len && isCharDigit(format[i]) && i+1 < len && format[i+1] == L'h') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, FALSE, TRUE, TRUE, format[i] - L'0');
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format X9h value
                        if (!valueFormatted && ch == L'X' && i < len && isCharDigit(format[i]) && i+1 < len && format[i+1] == L'h') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, FALSE, TRUE, FALSE, format[i] - L'0');
                            doExtraIncrement = 2;
                            valueFormatted = TRUE;
                        }
                        // Format xh value
                        if (!valueFormatted && ch == L'x' && i < len && format[i] == L'h') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, FALSE, TRUE, TRUE, 1);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format Xh value
                        if (!valueFormatted && ch == L'X' && i < len && format[i] == L'h') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, FALSE, TRUE, FALSE, 1);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format x (lowercase hex) value
                        if (!valueFormatted && ch == L'x') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, FALSE, FALSE, TRUE, 1);
                            valueFormatted = TRUE;
                        }
                        // Format X (uppercase hex) value
                        if (!valueFormatted && ch == L'X') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, FALSE, FALSE, FALSE, 1);
                            valueFormatted = TRUE;
                        }
                        // Format #x value
                        if (!valueFormatted && ch == L'#' && i < len && format[i] == L'x') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, TRUE, FALSE, TRUE, 1);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        // Format #X value
                        if (!valueFormatted && ch == L'#' && i < len && format[i] == L'X') {
                            uLONGValue = va_arg(paramList, ULONG);
                            result += FormatHexValue(uLONGValue, fieldAlign, fieldFillChar, fieldWidth, TRUE, FALSE, FALSE, 1);
                            doExtraIncrement = 1;
                            valueFormatted = TRUE;
                        }
                        token_started = FALSE;
                        token = COMMON_EMPTY_STRING;
                        fieldAlign = FORMAT_ALIGN_RIGHT;
                        fieldWidth = 0;
                        fieldPrecision = 0;
                        fieldFillChar = L' ';
                        fieldReadingState = FORMAT_PARSING_STATE_NONE;
                        i += doExtraIncrement;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return result;
}

CmzString __cdecl FormatMessage(const WCHAR* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    CmzString result = FormatMessage(format, paramList);
    va_end(paramList);

    return result;
}

VOID __cdecl Stdout(CmzString format, ...) {
    va_list paramList;
    va_start(paramList, format);
    CmzString s = FormatMessage(format.toWCHAR(), paramList);
    fwprintf(stdout, L"%s", s.getBuffer());
    fflush(stdout);
    va_end(paramList);
}

VOID __cdecl Stdout(const WCHAR* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    CmzString s = FormatMessage(format, paramList);
    fwprintf(stdout, L"%s", s.getBuffer());
    fflush(stdout);
    va_end(paramList);
}

VOID __cdecl Stderr(const WCHAR* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    CmzString s = FormatMessage(format, paramList);
    fwprintf(stderr, L"%s", s.getBuffer());
    fflush(stderr);
    va_end(paramList);
}

VOID __cdecl StdoutInEnc(LONG outputEncoding, const WCHAR* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    CmzString s = FormatMessage(format, paramList);
    va_end(paramList);

    size_t str_len = s.length();
    size_t buf_size = str_len + 1;
    CHAR* outputstr = new CHAR[buf_size];
    if (outputstr) {
        if (outputEncoding == ENC_ANSI) {
            s.toAnsi(outputstr, buf_size);
        } else if (outputEncoding == ENC_ISO2) {
            s.toIso2(outputstr, buf_size);
        } else if (outputEncoding == ENC_OEM_852) {
            s.toOem852(outputstr, buf_size);
        } else {
            s.toAnsi(outputstr, buf_size);
        }
        fprintf(stdout, "%s", outputstr);
        fflush(stdout);
        delete[] outputstr;
    }
}

VOID __cdecl StderrInEnc(LONG outputEncoding, const WCHAR* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    CmzString s = FormatMessage(format, paramList);
    size_t str_len = s.length();
    CHAR* outputstr = new CHAR[str_len+1];
    if (outputstr) {
        if (outputEncoding == ENC_ANSI) {
            s.toAnsi(outputstr, str_len);
        } else if (outputEncoding == ENC_ISO2) {
            s.toIso2(outputstr, str_len);
        } else if (outputEncoding == ENC_OEM_852) {
            s.toOem852(outputstr, str_len);
        } else {
            s.toAnsi(outputstr, str_len);
        }
        fprintf(stderr, "%s", outputstr);
        fflush(stderr);
        delete[] outputstr;
    }
    va_end(paramList);
}





VOID CmzString::_commonInit() {
	this->_growBy = DEFAULT_STRING_BUFFER_LENGTH;
    this->buf_size = 0;
    this->buf = NULL;

    size_t bufsize = wcslen(::cmzStringClassName) + 1;
    this->_originClassName = new WCHAR[bufsize];
    if (this->_originClassName) {
        wcsncpy(this->_originClassName, ::cmzStringClassName, bufsize);
    }
    this->_realClassName = new WCHAR[64];
    if (this->_realClassName) {
        wcsncpy(this->_realClassName, ::cmzStringClassName, 64);
    }
}

CmzString::CmzString() {
	this->_commonInit();
	this->initBuffer();
}

CmzString::CmzString(const CmzString &s) {
	this->_commonInit();
	this->initBuffer(s.getBufferSize());
    // copy all memory
	CopyMemory((VOID*)this->buf, (VOID*)s.getBuffer(), s.getBufferSize() * 2);
}

CmzString::CmzString(const WCHAR *string) {
	this->_commonInit();
	if (string == NULL || string[0] == NULL) {
		this->initBuffer();
	} else {
		this->initBuffer(wcslen(string) + 1);
		wcscat(this->buf, string);
	}
}

CmzString::CmzString(WCHAR *string, size_t buf_size) {
	this->_commonInit();
	this->buf = NULL;
	this->setBuffer(string, buf_size);
}

CmzString::CmzString(const CHAR* string) {
	this->_commonInit();
	if (string == NULL || string[0] == NULL) {
		this->initBuffer();
	} else {
        size_t buf_size = strlen(string) + 1;
		this->initBuffer(buf_size);
        LONGLONG encoding = DetectDataEncoding((BYTE*)string, buf_size);
        if (encoding == ENC_UTF8) {
            MultiByteToWideChar(CP_UTF8, 0, string, -1, this->buf, (INT)this->buf_size);
        } else {
            MultiByteToWideChar(CP_ACP, 0, string, -1, this->buf, (INT)this->buf_size);
        }
	}
}

CmzString::CmzString(WCHAR ch) {
	this->_commonInit();
	this->initBuffer();
	this->buf[0] = ch;
	this->buf[1] = NULL;
}

CmzString::CmzString(WCHAR ch, LONG repeatCount) {
	this->_commonInit();
	this->initBuffer(repeatCount + 1);
	for (LONG i = 0; i < repeatCount; i++) {
		this->buf[i] = ch;
	}
	this->buf[repeatCount] = NULL;
}

CmzString::CmzString(CHAR ch) {
	this->_commonInit();
	this->initBuffer();
	CHAR tmp[2];
	tmp[0] = ch;
	tmp[1] = NULL;
	MultiByteToWideChar(CP_ACP, 0, tmp, -1, this->buf, (INT)this->buf_size);
}

CmzString::CmzString(UCHAR ch) {
	this->_commonInit();
	this->initBuffer();
    /*
	WCHAR w;
    if (ch < 127) {
        w = ch;
    } else {
        UCHAR* ptr = (UCHAR*)&w;
        ptr[0] = 0xc0 | ((ch & 0xc0) >> 24);
        ptr[1] = ((ch & 0x3f) | 0x80);
    }
    this->buf[0] = w;
    this->buf[1] = NULL;
    */
    UCHAR tmp[2];
	tmp[0] = ch;
	tmp[1] = NULL;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)tmp, -1, this->buf, (INT)this->buf_size);
}

CmzString::CmzString(INT number, BOOL toHex, LONG minDigits) {
	this->_commonInit();
	this->initBuffer();
	_itow(number, this->buf, toHex ? 16 : 10);
    this->addPrefixZeros(minDigits);
}

CmzString::CmzString(UINT number, BOOL toHex, LONG minDigits) {
	this->_commonInit();
	this->initBuffer();
	_ui64tow(number, this->buf, toHex ? 16 : 10);
    this->addPrefixZeros(minDigits);
}

CmzString::CmzString(LONG number, BOOL toHex, LONG minDigits) {
	this->_commonInit();
	this->initBuffer();
    if (sizeof(LONG) == sizeof(INT)) {
        _itow(number, this->buf, toHex ? 16 : 10);
    } else {
        _i64tow(number, this->buf, toHex ? 16 : 10);
    }
    this->addPrefixZeros(minDigits);
}

CmzString::CmzString(ULONG number, BOOL toHex, LONG minDigits) {
    this->_commonInit();
    this->initBuffer();
    if (sizeof(ULONG) == sizeof(UINT)) {
        _ui64tow(number, this->buf, toHex ? 16 : 10);
    }
    else {
        _ui64tow(number, this->buf, toHex ? 16 : 10);
    }
    this->addPrefixZeros(minDigits);
}

CmzString::CmzString(ULONGLONG number, BOOL toHex, LONG minDigits) {
	this->_commonInit();
	this->initBuffer();
    _ui64tow(number, this->buf, toHex ? 16 : 10);
    this->addPrefixZeros(minDigits);
}

CmzString::CmzString(LONGLONG number, BOOL toHex, LONG minDigits) {
	this->_commonInit();
	this->initBuffer();
    _i64tow(number, this->buf, toHex ? 16 : 10);
    this->addPrefixZeros(minDigits);
}

CmzString::CmzString(BOOL value, WCHAR uppercase) {
	this->_commonInit();
    this->initBuffer(6);
    if (value) {
        if (uppercase == 'T') {
            wcscat(this->buf, L"TRUE");
        } else {
            wcscat(this->buf, L"true");
        }
    } else {
        if (uppercase == 'T') {
            wcscat(this->buf, L"FALSE");
        } else {
            wcscat(this->buf, L"false");
        }
    }
}

CmzString::CmzString(double number, LONG precision) {
	this->_commonInit();
	this->initBuffer();

	const WCHAR* formats[21] = {
		L"%.0f", L"%.1f", L"%.2f", L"%.3f", L"%.4f", L"%.5f", L"%.6f", L"%.7f",
		L"%.8f", L"%.9f", L"%.10f", L"%.11f", L"%.12f", L"%.13f", L"%.14f",
		L"%.15f", L"%.16f", L"%.17f", L"%.18f", L"%.19f", L"%.20f"
	};
	if (precision < 0 || precision > 20) {
		precision = 2;
	}

	swprintf(this->buf, this->buf_size, formats[precision], number);
    CmzLocale* locale = new CmzLocale();
    WCHAR sdecimal = L',';
    if (locale) {
        sdecimal = locale->locale_sdecimal[0];
        delete locale;
    }

	this->replace(L'.', sdecimal);
}

CmzString::~CmzString() {
	this->cleanup();
}

VOID CmzString::initBuffer() {
	this->initBuffer(DEFAULT_STRING_BUFFER_LENGTH);
}

VOID CmzString::initBuffer(size_t size) {
    this->buf = new WCHAR[size];
    if (!this->buf) {
        this->buf_size = 0;
        return;
    }
	this->buf_size = size;
	this->buf[0] = NULL;
}

VOID CmzString::zeroBuffer() {
    if (this->buf) {
        ZeroMemory(this->buf, this->buf_size*2);
    }
}

VOID CmzString::extendBuffer(size_t added_size) {
	size_t new_size = this->buf_size + added_size;
	WCHAR *b = new WCHAR[new_size];
    if (!b) {
        return;
    }
	if (this->buf) {
        CopyMemory(b, this->buf, this->buf_size * 2);
        delete[] this->buf;
	}
	this->buf_size = new_size;
	this->buf = b;
}

VOID CmzString::cleanup() {
	if (this->buf) {
        delete[] this->buf;
		this->buf = NULL;
		this->buf_size = 0;
	}

    if (this->_originClassName) {
        delete[] this->_originClassName;
        this->_originClassName = NULL;
    }

    if (this->_realClassName) {
        delete[] this->_realClassName;
        this->_realClassName = NULL;
    }
}

VOID CmzString::setBuffer(WCHAR *buf, size_t buf_size) {
	this->cleanup();
	if (buf != NULL) {
		this->buf = buf;
		this->buf_size = buf_size;
	}
}

VOID CmzString::addPrefixZeros(ULONGLONG minLen) {
    size_t len = wcslen(this->buf);
    if ((size_t)minLen > len) {
        ULONGLONG headsize = minLen - (ULONGLONG)len;
        WCHAR* prep = new WCHAR[headsize+1];
        if (!prep) {
            return;
        }
        for (ULONGLONG i = 0; i < headsize; i++) {
            prep[i] = L'0';
        }
        prep[headsize] = NULL;
        this->prepend(prep);
        delete[] prep;
    }
}

BOOL CmzString::equals(CmzString* str) {
    if (str) {
        return wcscmp(this->buf, str->buf) == 0;
    }
    return FALSE;
}


BOOL CmzString::equals(CmzObject* obj) {
    return FALSE;
}

CmzString CmzString::toString() {
    return *this;
}

CmzString &CmzString::append(const CmzString &s) {
	if (this->buf_size <= this->length() + s.length()) {
		this->extendBuffer(s.length() + DEFAULT_STRING_BUFFER_LENGTH);
	}
	wcscat(this->buf, s.getBuffer());
	return *this;
}

CmzString &CmzString::append(const CHAR *s) {
	CmzString cs(s);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(WCHAR ch) {
	CmzString cs(ch);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(CHAR ch) {
	CmzString cs(ch);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(UCHAR ch) {
	CmzString cs(ch);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(const WCHAR *s) {
	*this += s;
	return *this;
}

CmzString &CmzString::append(INT num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(UINT num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(LONGLONG num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(ULONGLONG num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::append(double num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

// STATIC method!
LONGLONG CmzString::findDoubleNullPos(const WCHAR* buf, size_t bufsize) {
    ULONGLONG pos = 0;
    BOOL found = FALSE;
    while (pos + 1 < bufsize) {
        if (buf[pos] == NULL && buf[pos+1] == NULL) {
            found = TRUE;
            break;
        }
        pos++;
    }

    if (found) {
        return pos;
    }

    return -1;
}

LONGLONG CmzString::findDoubleNullPos() {
    return CmzString::findDoubleNullPos(this->buf, this->buf_size);
}

ULONGLONG CmzString::lengthDoubleNull() {
    LONGLONG pos = this->findDoubleNullPos();
    if (pos >= 0) {
        return pos;
    }
    return 0;
}

VOID CmzString::setStringArray(WCHAR *buf, ULONGLONG bufsize) {
    LONGLONG pos = CmzString::findDoubleNullPos(this->buf, this->buf_size);
    if (pos >= 0) {
        if ((LONGLONG)this->buf_size < pos+2) {
            this->extendBuffer(pos+2 - this->buf_size);
        }
        this->buf[0] = NULL;
        CopyMemory((VOID*)this->buf, (VOID*)buf, (pos+2) * 2);
    }
}

CmzString& CmzString::appendAfterEos(const CmzString &s, WCHAR glueChar) {
    return this->appendAfterEos(s.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(const CHAR *s, WCHAR glueChar) {
	CmzString cs(s);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(WCHAR ch, WCHAR glueChar) {
	CmzString cs(ch);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(CHAR ch, WCHAR glueChar) {
	CmzString cs(ch);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(UCHAR ch, WCHAR glueChar) {
	CmzString cs(ch);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(const WCHAR *s, WCHAR glueChar) {
    if (s) {
        size_t strlen = this->length();
        size_t s_len = wcslen(s);
        if (s_len > 0) {
            size_t requiredBufferSize = strlen + 1 + s_len + 1 + 1;
            if (this->buf_size < requiredBufferSize) {
                this->extendBuffer(requiredBufferSize + DEFAULT_STRING_BUFFER_LENGTH);
            }

            LONGLONG appendAtPos = -1;
            if (strlen > 0) {
                appendAtPos = this->findDoubleNullPos();
            }
            if (appendAtPos == -1) {
                appendAtPos = strlen > 0 ? (LONGLONG)(strlen + 1) : 0;
            } else {
                appendAtPos++;
            }

            if (appendAtPos > 0) {
                this->buf[appendAtPos+1] = NULL;
                wcscat(&this->buf[appendAtPos], s);
                this->buf[appendAtPos-1] = glueChar;
                this->buf[appendAtPos + s_len] = glueChar;
                this->buf[appendAtPos + s_len + 1] = glueChar;
            } else {
                wcscat(this->buf, s);
                this->buf[s_len] = glueChar;
                this->buf[s_len + 1] = glueChar;
            }
        }
    }
    
	return *this;
}

CmzString& CmzString::appendAfterEos(INT num, WCHAR glueChar) {
	CmzString cs(num);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(UINT num, WCHAR glueChar) {
	CmzString cs(num);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(LONGLONG num, WCHAR glueChar) {
	CmzString cs(num);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(ULONGLONG num, WCHAR glueChar) {
	CmzString cs(num);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString& CmzString::appendAfterEos(double num, WCHAR glueChar) {
	CmzString cs(num);
	return this->appendAfterEos(cs.buf, glueChar);
}

CmzString &CmzString::prepend(CmzString &s) {
    return this->prepend(s.buf);
}

CmzString &CmzString::prepend(const CHAR *s) {
	CmzString cs(s);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(WCHAR ch) {
	CmzString cs(ch);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(CHAR ch) {
	CmzString cs(ch);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(UCHAR ch) {
	CmzString cs(ch);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(const WCHAR *s) {
    if (s) {
        size_t s_len = wcslen(s);
        if (s_len > 0) {
            size_t size = this->buf_size;
            size_t this_len = this->length();

            if (size < this_len + s_len + 1) {
                size = this_len + s_len + 1;
            }

            WCHAR *tmp = new WCHAR[size];
            if (tmp) {
                tmp[0] = NULL;
                wcscat(tmp, s);
                wcscat(tmp, this->buf);
                if (size > this->buf_size) {
                    this->setBuffer(tmp, size);
                } else {
                    wcscpy(this->buf, tmp);
                    delete[] tmp;
                }
            }
        }
    }

    return *this;
}

CmzString &CmzString::prepend(INT num) {
	CmzString cs(num);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(UINT num) {
	CmzString cs(num);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(LONG num) {
	CmzString cs(num);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(ULONG num) {
	CmzString cs(num);
    return this->prepend(cs.buf);
}

CmzString &CmzString::prepend(double num) {
	CmzString cs(num);
    return this->prepend(cs.buf);
}

VOID CmzString::padLeft(WCHAR ch, ULONG requiredSize) {
	size_t len = this->length();

	if (len >= requiredSize) {
		return;
	}

	LONG extraLength = static_cast<LONG>(requiredSize - len);
	CmzString prependStr(ch, extraLength);
	this->prepend(prependStr);
}

VOID CmzString::padRight(WCHAR ch, ULONG requiredSize) {
	size_t len = this->length();
	size_t pos = len;

	if (len >= requiredSize) {
		return;
	}

	if (this->buf_size <= requiredSize) {
		this->extendBuffer(this->buf_size + DEFAULT_STRING_BUFFER_LENGTH);
	}
	while (len < requiredSize) {
		this->buf[pos] = ch;
		pos++;
		len++;
	}
	this->buf[pos] = NULL;
}

CmzString &CmzString::operator=(const CmzString &s) {
	if (this->buf_size <= s.getBufferSize()) {
		this->extendBuffer((DEFAULT_STRING_BUFFER_LENGTH > s.getBufferSize() - this->buf_size) ? DEFAULT_STRING_BUFFER_LENGTH : s.getBufferSize());
	}
	this->buf[0] = NULL;
    // copy all memory
	CopyMemory((VOID*)this->buf, (VOID*)s.getBuffer(), s.getBufferSize() * 2);

	return *this;
}

CmzString &CmzString::operator=(const CHAR *s) {
	CmzString cs(s);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(WCHAR ch) {
	CmzString cs(ch);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(CHAR ch) {
	CmzString cs(ch);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(UCHAR ch) {
	CmzString cs(ch);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(const WCHAR *s) {
    if (!s) {
        this->buf[0] = NULL;
    } else {
        size_t strlen = wcslen(s);
        if (strlen == 0) {
            this->buf[0] = NULL;
        } else {
            if (strlen+1 > this->buf_size) {
                this->extendBuffer(this->buf_size + strlen);
            }
            wcscpy(this->buf, s);
        }
    }

	return *this;
}

CmzString &CmzString::operator=(INT num) {
	CmzString cs(num);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(UINT num) {
	CmzString cs(num);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(LONGLONG num) {
	CmzString cs(num);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(ULONGLONG num) {
	CmzString cs(num);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator=(double num) {
	CmzString cs(num);
	*this = cs;
	return *this;
}

CmzString &CmzString::operator+=(const CmzString &s) {
	if (this->buf_size <= this->length() + s.length()) {
		this->extendBuffer(s.length() + this->_growBy);
		this->_growBy *= 2;
	}
	wcscat(this->buf, s.getBuffer());
	return *this;
}

CmzString &CmzString::operator+=(const CHAR *s) {
	CmzString cs(s);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(WCHAR ch) {
	CmzString cs(ch);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(CHAR ch) {
	CmzString cs(ch);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(UCHAR ch) {
	CmzString cs(ch);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(const WCHAR *s) {
    if (s) {
        size_t s_len = wcslen(s);
        if (this->buf_size < this->length() + s_len + 1) {
            this->extendBuffer(s_len + this->_growBy);
            this->_growBy *= 2;
        }
        wcscat(this->buf, s);
    }
	return *this;
}

CmzString &CmzString::operator+=(INT num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(UINT num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(LONG num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString &CmzString::operator+=(ULONG num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString& CmzString::operator+=(LONGLONG num) {
    CmzString cs(num);
    *this += cs;
    return *this;
}

CmzString& CmzString::operator+=(ULONGLONG num) {
    CmzString cs(num);
    *this += cs;
    return *this;
}

CmzString &CmzString::operator+=(double num) {
	CmzString cs(num);
	*this += cs;
	return *this;
}

CmzString CmzString::operator+(const CmzString &s) {
	size_t size;
    if (this->buf_size >= this->length() + s.length() + 1) {
		size = this->buf_size;
	} else {
		size = this->buf_size + s.length();
	}

	WCHAR *buf = new WCHAR[size];
	buf[0] = NULL;
    wcscat(buf, this->buf);
    wcscat(buf, s.getBuffer());

	CmzString newstr(buf, size);
	return newstr;
}

CmzString CmzString::operator+(const CHAR *s) {
	CmzString cs(s);
	return ((*this) + cs);
}

CmzString CmzString::operator+(WCHAR ch) {
	CmzString cs(ch);
	return ((*this) + cs);
}

CmzString CmzString::operator+(CHAR ch) {
	CmzString cs(ch);
	return ((*this) + cs);
}

CmzString CmzString::operator+(UCHAR ch) {
	CmzString cs(ch);
	return ((*this) + cs);
}

CmzString CmzString::operator+(const WCHAR *s) {
    if (s) {
        size_t strlen = wcslen(s);
        if (strlen > 0) {
            size_t size = this->length() + strlen + 1;
            WCHAR *buf = new WCHAR[size];
            buf[0] = NULL;
            wcscat(buf, this->buf);
            wcscat(buf, s);
            CmzString result(buf, size);
            return result;
        }
    }

	return this->getBuffer();
}

CmzString CmzString::operator+(INT num) {
	CmzString cs(num);
	return ((*this) + cs);
}

CmzString CmzString::operator+(UINT num) {
	CmzString cs(num);
	return ((*this) + cs);
}

CmzString CmzString::operator+(LONG num) {
	CmzString cs(num);
	return ((*this) + cs);
}

CmzString CmzString::operator+(ULONG num) {
	CmzString cs(num);
	return ((*this) + cs);
}

CmzString CmzString::operator+(LONGLONG num) {
	CmzString cs(num);
	return ((*this) + cs);
}

CmzString CmzString::operator+(double num) {
	CmzString cs(num);
	return ((*this) + cs);
}

LONGLONG CmzString::charCount(CHAR ch) {
    LONGLONG count = 0;

    CHAR source[2];
    WCHAR dest[2];

    source[0] = ch;
    source[1] = NULL;
	MultiByteToWideChar(CP_ACP, 0, source, -1, dest, 2);

    WCHAR cmp = dest[0];
    size_t len = this->length();
    for (size_t i = 0; i < len; i++) {
        if (this->buf[i] == cmp) {
            count++;
        }
    }

    return count;
}

LONGLONG CmzString::charCount(WCHAR ch) {
    LONGLONG count = 0;
    size_t len = this->length();

    for (size_t i = 0; i < len; i++) {
        if (this->buf[i] == ch) {
            count++;
        }
    }

    return count;
}

ULONGLONG CmzString::wordCount() {
    ULONGLONG count = 0;
    size_t len = this->length();
    size_t i;
    WCHAR ch = 0;

    for (i = 0; i < len; i++) {
        ch = this->buf[i];
        if (isCharSentenceBoundary(ch)) {
            count++;
        }
    }

    if (i > 0) {
        if (!isCharSentenceBoundary(ch)) {
            count++;
        }
    }

    return count;
}

CmzString CmzString::getFirstToken(const WCHAR* tokenBoundaryChars) {
    size_t len = this->length();
    ULONGLONG i;
    WCHAR ch;
    CmzString result;

    for (i = 0; i < len; i++) {
        ch = this->buf[i];
        if (wcschr(tokenBoundaryChars, ch) == NULL) {
            result += ch;
        } else {
            break;
        }
    }

    return result;
}

CmzString CmzString::popFirstToken(const WCHAR* tokenBoundaryChars) {
    size_t len = this->length();
    ULONGLONG i;
    WCHAR ch;
    CmzString result;

    for (i = 0; i < len; i++) {
        ch = this->buf[i];
        if (wcschr(tokenBoundaryChars, ch) == NULL) {
            result += ch;
        } else {
            break;
        }
    }

    this->deleteSubstr(0, i);
    this->ltrimAllChars(tokenBoundaryChars);

    return result;
}

CmzString CmzString::getLastToken(const WCHAR* tokenBoundaryChars) {
    return this->strAfterLastChar(tokenBoundaryChars);
}

CmzString CmzString::popLastToken(const WCHAR* tokenBoundaryChars) {
    CmzString result(this->strAfterLastChar(tokenBoundaryChars));
    this->cutAtLastChar(tokenBoundaryChars);
    this->rtrimAllChars(tokenBoundaryChars);
    return result;
}

CmzString CmzString::getFirstWord() {
    return this->getFirstToken(WORD_BOUNDARY_CHARS);
}

CmzString CmzString::popFirstWord() {
    return this->popFirstToken(WORD_BOUNDARY_CHARS);
}

CmzString CmzString::getLastWord() {
    return this->getLastToken(WORD_BOUNDARY_CHARS);
}

CmzString CmzString::popLastWord() {
    return this->popLastToken(WORD_BOUNDARY_CHARS);
}

CmzStringList* CmzString::explode(WCHAR ch, ULONGLONG maxItems) {
    ULONGLONG num = 0;
    if (maxItems == 0) {
        memset(&maxItems, 0xff, sizeof(maxItems));
    }

    CmzStringList* retval = new CmzStringList(10);
    CmzString tmp = COMMON_EMPTY_STRING;

    WCHAR cmp = ch;
    size_t len = this->length();
    ULONGLONG i;
    for (i = 0; i < len; i++) {
        if (this->buf[i] == cmp) {
            retval->push(tmp);
            tmp = COMMON_EMPTY_STRING;
            num++;
            if (num == maxItems - 1) {
                if (i+1 < len) {
                    CmzString s(&this->buf[i+1]);
                    retval->push(s);
                }
                return retval;
            }
        } else {
            tmp += this->buf[i];
        }
    }

    retval->setAt(num, tmp);
    return retval;
}

CmzStringList* CmzString::explodeAfterMinLength(WCHAR ch, ULONGLONG minItemLength) {
    LONGLONG num = 0;

    CmzStringList* retval = new CmzStringList(20);
    CmzString tmp = COMMON_EMPTY_STRING;

    WCHAR cmp;
    size_t len = this->length();
    size_t curpos = 0;
    while (curpos < len) {
        cmp = this->buf[curpos];
        if (cmp == ch && (tmp.length() >= minItemLength)) {
            retval->push(tmp);
            tmp = COMMON_EMPTY_STRING;
            num++;
        } else {
            tmp += cmp;
        }

        curpos++;
    }

    retval->setAt(num, tmp);
    return retval;
}

CmzStringList* CmzString::explodeToWords(ULONGLONG wordCount) {
    LONGLONG num = 0;

    CmzStringList* retval = new CmzStringList(20);
    CmzString tmp = COMMON_EMPTY_STRING;

    WCHAR cmp;
    size_t len = this->length();
    size_t curpos = 0;
    while (curpos < len) {
        cmp = this->buf[curpos];
        if (isCharSentenceBoundary(cmp)) {
            if (curpos > 0 && isCharSentenceBoundary(this->buf[curpos-1])) {
                curpos++;
                continue;
            }
            if (tmp.wordCount() < wordCount) {
                tmp += cmp;
            } else {
                retval->push(tmp);
                tmp = COMMON_EMPTY_STRING;
                num++;
            }
        } else {
            tmp += cmp;
        }

        curpos++;
    }

    retval->setAt(num, tmp);
    return retval;
}

CmzStringList* CmzString::explodeAtChar(const WCHAR* stringOfChars) {
    LONGLONG num = 0;
    LONGLONG length = 20; // default size of string list

    CmzStringList* retval = new CmzStringList(length);
    CmzString tmp = COMMON_EMPTY_STRING;

    size_t len = this->length();
    for (size_t i = 0; i < len; i++) {
        if (wcschr(stringOfChars, this->buf[i]) != NULL) {
            retval->push(tmp);
            tmp = COMMON_EMPTY_STRING;
            num++;
        } else {
            tmp += this->buf[i];
        }
    }

    retval->setAt(num, tmp);
    return retval;
}

CmzString CmzString::substr(size_t start, size_t length) {
    size_t start_pos = start;

    if (start_pos >= this->length()) {
        return "";
    }

    if (length == 0) {
        length = this->length() - start_pos;
    }

    size_t new_buf_size = length + 1;
    WCHAR *new_buf = new WCHAR[new_buf_size];
    memcpy(new_buf, &this->buf[start_pos], length * 2);
    new_buf[length] = NULL;
    CmzString s(new_buf, new_buf_size);
    return s;
}

VOID CmzString::rtrim(WCHAR ch) {
	size_t len = this->length();
	if (len > 0) {
		size_t i = len - 1;
		while (i >= 0 && this->buf[i] == ch) {
			this->buf[i] = NULL;
			i--;
		}
	}
}

VOID CmzString::rtrimAllChars(const WCHAR* stringOfChars) {
	size_t len = this->length();
	if (len > 0) {
		size_t i = len - 1;
		while (i >= 0 && wcschr(stringOfChars, this->buf[i]) != NULL) {
			this->buf[i] = NULL;
			i--;
		}
	}
}

VOID CmzString::ltrim(WCHAR ch) {
    size_t start_pos = 0;
    size_t len = this->length();
    while (start_pos < len && this->buf[start_pos] == ch) {
        start_pos++;
    }

    if (start_pos == this->length()) {
        this->buf[0] = NULL;
        return;
    }

    size_t pos = 0;
    while (start_pos < len) {
        this->buf[pos] = this->buf[start_pos];
        pos++;
        start_pos++;
    }
    this->buf[pos] = NULL;
}

VOID CmzString::ltrimAllChars(const WCHAR* stringOfChars) {
    size_t start_pos = 0;
    size_t len = this->length();
    while (start_pos < len && wcschr(stringOfChars, this->buf[start_pos]) != NULL) {
        start_pos++;
    }

    if (start_pos == this->length()) {
        this->buf[0] = NULL;
        return;
    }

    ULONGLONG pos = 0;
    while (start_pos < len) {
        this->buf[pos] = this->buf[start_pos];
        pos++;
        start_pos++;
    }
    this->buf[pos] = NULL;
}

VOID CmzString::trim(WCHAR ch) {
    this->ltrim(ch);
    this->rtrim(ch);
}

BOOL CmzString::startsWith(CmzString s) {
    return wcsstr(this->buf, s.buf) == this->buf;
}

BOOL CmzString::startsWithChar(WCHAR ch) {
    return this->buf[0] == ch;
}

BOOL CmzString::startsWithOneOfChars(const WCHAR* stringOfChars) {
    return wcschr(stringOfChars, this->buf[0]) != NULL;
}

BOOL CmzString::endsWith(CmzString s) {
    size_t slen = s.length();
    size_t len = this->length();
    WCHAR *buf = this->buf + len - slen;
    return wcsstr(buf, s.buf) == buf;
}

BOOL CmzString::endsWithChar(WCHAR ch) {
    size_t len = this->length();
    if (len > 0) {
        return this->buf[len-1] == ch;
    }
    return FALSE;
}

BOOL CmzString::endsWithOneOfChars(const WCHAR* stringOfChars) {
    WCHAR ch = this->lastChar();
    return wcschr(stringOfChars, ch) != NULL;
}

BOOL CmzString::contains(const WCHAR* substring) {
    return wcsstr(this->buf, substring) != NULL;
}

BOOL CmzString::contains(WCHAR ch) {
    size_t len = this->length();
    for (size_t i = 0; i < len; i++) {
        if (this->buf[i] == ch) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CmzString::containsOneOfChars(const WCHAR* stringOfChars) {
    size_t len = this->length();
    for (size_t i = 0; i < len; i++) {
        if (wcschr(stringOfChars, this->buf[i]) != NULL) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CmzString::containsWhiteSpace() {
    size_t len = this->length();
    for (size_t i = 0; i < len; i++) {
        if (isCharSpace(this->buf[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

VOID CmzString::trimAllChars(const WCHAR* stringOfChars) {
    this->ltrimAllChars(stringOfChars);
    this->rtrimAllChars(stringOfChars);
}

VOID CmzString::cutAtLast(WCHAR ch) {
    size_t pos = this->length() - 1;
    while (pos >= 0 && this->buf[pos] != ch) {
        pos--;
    }

    if (pos >= 0) {
        this->buf[pos] = NULL;
    }
}

VOID CmzString::cutAtLastChar(const WCHAR* stringOfChars) {
    if (!stringOfChars || stringOfChars[0] == NULL) {
        return;
    }

	size_t len = this->length();
	if (len > 0) {
		size_t pos = len - 1;
		WCHAR ch;
		size_t len = wcslen(stringOfChars);
		BOOL breakLoop = FALSE;

		while (pos >= 0) {
			for (size_t i = 0; i < len; i++) {
				ch = stringOfChars[i];
				if (this->buf[pos] == ch) {
					breakLoop = TRUE;
					break;
				}
			}

			if (breakLoop) {
				break;
			}

			pos--;
		}

		if (pos >= 0) {
			this->buf[pos] = NULL;
		}
	}
}

CmzString CmzString::strAfterLast(WCHAR ch) {
	size_t len = this->length();
	if (len > 0) {
		size_t pos = len - 1;
		while (pos >= 0 && this->buf[pos] != ch) {
			pos--;
		}

		if (pos == this->length() - 1) {
			return "";
		}

		if (pos >= 0) {
			return this->substr(pos + 1);
		}
	}

    return "";
}

CmzString CmzString::strAfterLastChar(const WCHAR* stringOfChars) {
    if (!stringOfChars || stringOfChars[0] == NULL) {
        return COMMON_EMPTY_STRING;
    }

    size_t thislen = this->length();
	if (thislen > 0) {
		size_t pos = thislen - 1;
		size_t len = wcslen(stringOfChars);
		WCHAR ch;
		BOOL breakLoop = FALSE;

		while (pos >= 0) {
			for (size_t i = 0; i < len; i++) {
				ch = stringOfChars[i];
				if (this->buf[pos] == ch) {
					breakLoop = TRUE;
					break;
				}
			}
			if (breakLoop) {
				break;
			}
			pos--;
		}

		if (pos == thislen - 1) {
			return COMMON_EMPTY_STRING;
		}

		if (pos >= 0) {
			return this->substr(pos + 1);
		}
	}

    return COMMON_EMPTY_STRING;
}

VOID CmzString::truncateAtEnd(size_t numChars) {
	if (numChars > 0) {
		size_t len = this->length();
		if (numChars > len) {
			numChars = len;
		}
		size_t pos = len;
		if (pos > numChars) {
			pos -= numChars;
		} else {
			pos = 0;
		}
		this->buf[pos] = NULL;
	}
}

ULONGLONG CmzString::linesCount() {
    ULONGLONG count = 0;
    if (this->buf && this->buf[0] != NULL) {
        count = 1;
    }

    if (!this->buf) {
        return count;
    }

    size_t len = this->length();
    for (size_t i = 0; i < len; i++) {
        if (this->buf[i] == L'\n') {
            count++;
        }
    }

    return count;
}

CmzString& CmzString::prependEachLine(CmzString &s) {
    return this->prependEachLine(s.buf);
}

CmzString& CmzString::prependEachLine(const CHAR *s) {
	CmzString cs(s);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(WCHAR ch) {
	CmzString cs(ch);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(CHAR ch) {
	CmzString cs(ch);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(UCHAR ch) {
	CmzString cs(ch);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(const WCHAR *s) {
    if (s) {
        size_t s_len = wcslen(s);
        size_t this_len = this->length();
        if (s_len > 0) {
            ULONGLONG linesCount = this->linesCount();
            if (linesCount == 0) {
                linesCount = 1;
            }

            size_t extraChars = linesCount * s_len;
            size_t new_buf_size = this_len + extraChars + 1;
            WCHAR *new_buf = new WCHAR[new_buf_size];
            if (new_buf) {
                size_t src_i;
                size_t dst_i;

                new_buf[0] = NULL;
                memcpy(new_buf, s, s_len * 2);
                new_buf[s_len] = NULL;
                dst_i = s_len;
                for (src_i = 0; src_i < this_len; src_i++) {
                    new_buf[dst_i] = this->buf[src_i];
                    dst_i++;
                    if (this->buf[src_i] == L'\n' && src_i+1 < this_len && this->buf[src_i] != NULL) {
                        memcpy(&new_buf[dst_i], s, s_len * 2);
                        dst_i += s_len;
                    }
                }
                new_buf[dst_i] = NULL;

                if (this->buf) {
                    delete[] this->buf;
                }
                this->buf = new_buf;
                this->buf_size = new_buf_size;
            }
        }
    }
    
    return *this;
}

CmzString& CmzString::prependEachLine(INT num) {
	CmzString cs(num);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(UINT num) {
	CmzString cs(num);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(LONGLONG num) {
	CmzString cs(num);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(ULONGLONG num) {
	CmzString cs(num);
    return this->prependEachLine(cs.buf);
}

CmzString& CmzString::prependEachLine(double num) {
	CmzString cs(num);
    return this->prependEachLine(cs.buf);
}


BOOL CmzString::isInt() {
	if (!this->buf) {
		return FALSE;
	}

	BOOL retval = TRUE;
	size_t i = 0;
	size_t len = this->length();
	if (i < len && (this->buf[i] == L'-' || this->buf[i] == L'+')) {
		i++;
	}
    // jeśli łańcuch składa się z samego minusa lub plusa, to nie jest to liczba
    if (i == len) {
        return FALSE;
    }

	for (; i < len; i++) {
		if (!iswdigit(this->buf[i])) {
			retval = FALSE;
			break;
		}
	}

	return retval;
}

BOOL CmzString::isDouble() {
	BOOL retval = TRUE;
	BOOL decsep_found = FALSE;
	size_t i = 0;
	size_t len = this->length();

	if (i < len && (this->buf[i] == L'-' || this->buf[i] == L'+')) {
		i++;
	}
    // jeśli łańcuch składa się z samego minusa lub plusa, to nie jest to liczba
    if (i == len) {
        return FALSE;
    }
	if (i < len && (this->buf[i] == L',' || this->buf[i] == L'.')) {
		i++;
		decsep_found = TRUE;
	}

	for (; i < len; i++) {
		if (iswdigit(this->buf[i])) {
			continue;
		}
		if ((this->buf[i] == L',' || this->buf[i] == L'.') && !decsep_found) {
			decsep_found = TRUE;
			continue;
		}
		retval = FALSE;
		break;
	}

	return retval;
}


BOOL CmzString::isNumeric(BOOL *isInt, BOOL *isDouble) {
	BOOL _isInt = TRUE;
	BOOL _isDouble = TRUE;
	//BOOL retval = TRUE;
	BOOL decsep_found = FALSE;
	size_t i = 0;
	size_t len = this->length();
    //CLocale* locale = new CLocale();

	if (i < len && (this->buf[i] == L'-' || this->buf[i] == L'+')) {
		i++;
	}
    // jeśli łańcuch składa się z samego minusa lub plusa, to nie jest to liczba
    if (i == len) {
        *isInt = FALSE;
        *isDouble = FALSE;
        return FALSE;
    }
	if (i < len && (this->buf[i] == L',' || this->buf[i] == L'.')) {
		i++;
		decsep_found = TRUE;
		_isInt = FALSE;
	}

	for (; i < len; i++) {
		if (iswdigit(this->buf[i])) {
			continue;
		}
		if ((this->buf[i] == L',' || this->buf[i] == L'.') && !decsep_found) {
			decsep_found = TRUE;
			_isInt = FALSE;
			continue;
		}
		_isDouble = FALSE;
		_isInt = FALSE;
		break;
	}

	if (isInt) {
		*isInt = _isInt;
	}
	if (isDouble) {
		*isDouble = _isDouble;
	}

	return _isInt || _isDouble;
}

LONG CmzString::toIntRound() {
	BOOL isInt;
	BOOL isDouble;
	BOOL isNumeric = this->isNumeric(&isInt, &isDouble);

	if (!isNumeric) {
		return 0L;
	}

	if (isDouble && !isInt) {
		double retval = this->toDouble();
		return round(retval);
	}

	return (_wtol(this->buf));
}


LONG CmzString::toIntFloor() {
	BOOL isInt;
	BOOL isDouble;
	BOOL isNumeric = this->isNumeric(&isInt, &isDouble);

	if (!isNumeric) {
		return 0L;
	}

	if (isDouble && !isInt) {
		double retval = this->toDouble();
		return (LONG)floor(retval);
	}

	return (_wtol(this->buf));
}

LONG CmzString::toIntCeil() {
	BOOL isInt;
	BOOL isDouble;
	BOOL isNumeric = this->isNumeric(&isInt, &isDouble);

	if (!isNumeric) {
		return 0L;
	}

	if (isDouble && !isInt) {
		double retval = this->toDouble();
		return (LONG)ceil(retval);
	}

	return (_wtol(this->buf));
}

LONG CmzString::toInt() {
	BOOL isInt;
	BOOL isDouble;
	BOOL isNumeric = this->isNumeric(&isInt, &isDouble);

	if (!isNumeric) {
		return 0L;
	}

	if (isDouble && !isInt) {
		double retval = this->toDouble();
		return (LONG)retval;
	}

	return (_wtol(this->buf));
}

double CmzString::toDouble() {
	if (!this->isDouble()) {
		return 0.0;
	}

	CmzString num = *this;
    CmzLocale* locale = new CmzLocale();
    WCHAR sdecimal = L',';
    if (locale) {
        sdecimal = locale->locale_sdecimal[0];
        delete locale;
    }

	num.replace(sdecimal, L'.');
	return _wtof(num.buf);
}

size_t CmzString::replace(WCHAR src, WCHAR dst) {
	size_t count = 0;
	size_t len = this->length();
	for (size_t i = 0; i < len; i++) {
		if (this->buf[i] == src) {
			this->buf[i] = dst;
			count++;
		}
	}

	return count;
}

size_t CmzString::replaceCharsWithCodeLessThan(WCHAR vkCode, WCHAR dst) {
	size_t count = 0;
	size_t len = this->length();
	for (ULONGLONG i = 0; i < len; i++) {
		if (this->buf[i] < vkCode) {
			this->buf[i] = dst;
			count++;
		}
	}

	return count;
}

VOID CmzString::replace(size_t start, size_t length, const CmzString& replaceWith) {
	if (length < 1) {
		return;
	}

    size_t str_len = this->length();
    if (str_len < 1 || start >= str_len) {
        this->append(replaceWith);
        return;
    }

    // end_index - index ostatniego znaku, który usuwamy
    size_t end_index = start + length - 1;
    size_t lengthOfStringToRemove;
    size_t new_buf_size;

    // jeśli zastępujemy fragment od pozycji start do końca łańcucha, to
    if (end_index >= str_len) {
        end_index = str_len - 1;
		if (end_index - start + 1 >= 0) {
			lengthOfStringToRemove = end_index - start + 1;
		} else {
			lengthOfStringToRemove = 0;
		}
    }
    // poniżej zastępujemy podłańcuch wewnątrz łańcucha
    else {
        lengthOfStringToRemove = length;
    }

    new_buf_size = str_len + replaceWith.length() - lengthOfStringToRemove + 1;
    WCHAR* new_buf = new WCHAR[new_buf_size];
    // Jeśli usuwamy nie od pierwszego znaku, to kopiujemy do bufora wyjściowego tekst przed
    // usuwanym łańcuchem
    if (start > 0) {
        CopyMemory(new_buf, this->buf, start * sizeof(WCHAR));
    }
    // wklejamy łańcuch replaceWith
    size_t replaceWithLen = replaceWith.length();
    if (replaceWithLen > 0) {
        CopyMemory(&new_buf[start], replaceWith.getBuffer(), replaceWithLen * sizeof(WCHAR));
    }
    // jeśli końcowy indeks jest przed końcem źródłowego łańcucha, to dokopiowujemy pozostały
    // tekst bufora źródłowego
    size_t endPartLen = 0;
    if (end_index < str_len - 1) {
        endPartLen = str_len - end_index - 1;
        CopyMemory(&new_buf[start + replaceWithLen], &this->buf[end_index + 1], endPartLen * sizeof(WCHAR));
    }

    // NULL-ujemy na końcu nowego łańcucha
    new_buf[start + replaceWithLen + endPartLen] = NULL;
    this->setBuffer(new_buf, new_buf_size);
}


size_t CmzString::replace(CmzString toReplace, CmzString replaceWith, size_t startFrom, size_t maxReplacementCount) {
    if (maxReplacementCount == 0) {
        maxReplacementCount = (size_t)-1;
    }

    if (startFrom >= this->length()) {
        return 0;
    }

    size_t toReplaceLength = toReplace.length();
    size_t replaceWithLength = replaceWith.length();
    WCHAR* pos = wcsstr(&this->buf[startFrom], toReplace.buf);
    size_t count = 0;
    while (pos) {
        startFrom = (size_t)(pos - this->buf);
        this->replace(startFrom, toReplaceLength, replaceWith);
        count++;
        startFrom += replaceWithLength;
        if (startFrom >= this->length()) {
            break;
        }
        pos = wcsstr(&this->buf[startFrom], toReplace.buf);
    }

    return count;
}

ULONGLONG CmzString::remove(WCHAR ch) {
	ULONGLONG count = 0;
	LONGLONG len = (LONGLONG)this->length();
	for (LONGLONG i = 0; i < len; i++) {
		if (this->buf[i] == ch) {
			for (LONGLONG j = i + 1; j < len; j++) {
				this->buf[j - 1] = this->buf[j];
			}
			len--;
			count++;
            i--;
		}
	}
	this->buf[len] = NULL;

	return count;
}

ULONGLONG CmzString::removeCharsWithCodeLessThan(WCHAR vkCode) {
	ULONGLONG count = 0;
	LONGLONG len = (LONGLONG)this->length();
	for (LONGLONG i = 0; i < len; i++) {
		if (this->buf[i] < vkCode) {
			for (LONGLONG j = i + 1; j < len; j++) {
				this->buf[j - 1] = this->buf[j];
			}
			len--;
			count++;
            i--;
		}
	}
	this->buf[len] = NULL;

	return count;
}

VOID CmzString::insertCharAt(WCHAR ch, size_t pos) {
    size_t str_len = this->length();
    // poniżej jest plus 2 bo liczymy dodatkowy znak oraz znak NULL kończący łańcuch
	if (this->buf_size <= str_len + 2) {
		this->extendBuffer(2 + DEFAULT_STRING_BUFFER_LENGTH);
	}

    if (pos >= str_len) {
        this->buf[str_len] = ch;
        this->buf[str_len + 1] = NULL;
    } else {
        // przenosimy podłańcuch od podanej pozycji o jeden znak w prawo
        size_t j;
        for (j = str_len; j > pos && j > 0; j--) {
            this->buf[j] = this->buf[j - 1];
        }
        this->buf[j] = ch;
        this->buf[str_len + 1] = NULL;
    }
}

VOID CmzString::insertAt(size_t pos, const CmzString& s) {
    size_t this_len = this->length();
    size_t s_len = s.length();

    size_t new_buf_size = this_len + s_len + 1;
    WCHAR* new_buf = new WCHAR[new_buf_size];

    if (this_len == 0) {
        CopyMemory(new_buf, s.getBuffer(), s_len * sizeof(WCHAR));
    } else {
        if (pos >= this_len) {
            CopyMemory(new_buf, this->buf, this_len * sizeof(WCHAR));
            CopyMemory(&new_buf[this_len], s.getBuffer(), s_len * sizeof(WCHAR));
        } else {
            if (pos > 0) {
                CopyMemory(new_buf, this->buf, pos * sizeof(WCHAR));
            }
            CopyMemory(&new_buf[pos], s.getBuffer(), s_len * sizeof(WCHAR));
            CopyMemory(&new_buf[pos + s_len], &this->buf[pos], (this_len - pos) * sizeof(WCHAR));
        }
    }

    new_buf[new_buf_size-1] = NULL;
    this->setBuffer(new_buf, new_buf_size);
}

VOID CmzString::deleteCharAt(size_t pos) {
    size_t str_len = this->length();
    if (pos < str_len) {
        if (str_len == 1) {
            this->buf[0] = NULL;
            return;
        }
        size_t j;
        for (j = pos; j < str_len - 1; j++) {
            this->buf[j] = this->buf[j + 1];
        }
        this->buf[str_len - 1] = NULL;
    }
}

VOID CmzString::deleteSubstr(size_t startpos, size_t length) {
    if (length == 0) {
        return;
    }

    size_t copyFrom = startpos + length;
    size_t len = this->length();

    if (startpos < len) {
        this->buf[startpos] = NULL;
        if (copyFrom < len) {
            wcscat(&this->buf[startpos], &this->buf[copyFrom]);
        }
    }
}

ULONGLONG CmzString::toCharString(CHAR* dest, size_t destSize, LONGLONG outputEncoding) const {
    if (!dest) {
        return 0;
    }

    size_t str_len = this->length();
    if (str_len < 1) {
        dest[0] = NULL;
        return 0;
    }

    LONGLONG retval = WideCharToMultiByte(CP_ACP, 0, this->buf, (INT)str_len, (LPSTR)dest, (INT)destSize, NULL, NULL);
    if (retval >= 0) {
        dest[retval] = NULL;
    }

    if (outputEncoding == ENC_ANSI) {
        return retval;
    }

    if (str_len >= destSize) {
        str_len = destSize - 1;
    }

    if (outputEncoding == ENC_ISO2) {
        AnsiToIso2((BYTE*)dest, str_len);
        return retval;
    }

    if (outputEncoding == ENC_OEM_852) {
        AnsiToOem852((BYTE*)dest, str_len);
        return retval;
    }

    return retval;
}

LONGLONG CmzString::strpos(const WCHAR* substring, size_t startFrom) {
    if (startFrom < this->length()) {
        WCHAR* pos = wcsstr(&this->buf[startFrom], substring);
        if (pos != NULL) {
            return (LONGLONG)(pos - this->buf);
        }
    }

    return -1;
}

LONGLONG CmzString::strposLast(const WCHAR* substring, size_t startFrom) {
    if (startFrom < this->length()) {
        WCHAR* lastPos = NULL;
        WCHAR* pos = wcsstr(&this->buf[startFrom], substring);

        while (pos != NULL) {
            lastPos = pos;
            startFrom = pos - this->buf + 1;
            pos = wcsstr(&this->buf[startFrom], substring);
        }

        if (lastPos == NULL) {
            return -1;
        }

        return (LONGLONG)(lastPos - this->buf);
    }

    return -1;
}

size_t CmzString::foreachSubcontentDo(const WCHAR* beginTag, const WCHAR* endTag, BOOL (WINAPI *doFunc)(LONGLONG startPos, LONGLONG endPos, CmzString* subcontentItem, LPVOID customData), LPVOID customData) {
    LONGLONG pos, pos2;
    CmzString subcontent;
    size_t beginTagLen;
    size_t endTagLen;
    size_t count = 0;

    beginTagLen = wcslen(beginTag);
    endTagLen = wcslen(endTag);

    if (!doFunc) {
        return 0;
    }

    if (beginTagLen < 1 || endTagLen < 1) {
        return 0;
    }

    pos = this->strpos(beginTag);
    pos2 = this->strpos(endTag, pos + beginTagLen);

    while (pos >= 0 && pos2 > pos) {
        pos += (LONGLONG)beginTagLen;
        subcontent = this->substr(pos, pos2 - pos);
        count++;
        if (doFunc(pos, pos2 - 1, &subcontent, customData)) {
            break;
        }
        pos = this->strpos(beginTag, pos2 + 1);
        pos2 = this->strpos(endTag, pos + beginTagLen);
    }

    return count;
}

CmzString WINAPI intToHex(LONGLONG num, size_t minHexNumberLength, BOOL useSmallAlpha, BOOL prependx0) {
    CmzString retval = COMMON_EMPTY_STRING;
    const WCHAR* smallAlpha = L"abcdef";
    const WCHAR* largeAlpha = L"ABCDEF";
    const WCHAR* usedAlpha;
    if (useSmallAlpha) {
        usedAlpha = smallAlpha;
    } else {
        usedAlpha = largeAlpha;
    }

    // jeśliminHexNumberLength jest liczbą nieparzystą, to zwiększamy go o 1, żeby ilość
    // wyświetlanych cyfr była parzysta
    if (minHexNumberLength % 2) {
        minHexNumberLength += 1;
    }

    DWORD len = sizeof(LONGLONG) * 8; // rozmiar LONGLONG-a w bitach
    DWORD numCompare = len / 4;   // porównujemy łańcuchy 4-bitowe, numCompare zawiera ilość tych
                                // porównań
    LONGLONG mask = 0x0f; // maska wskazuje najpierw najmłodsze 4 bity
    LONGLONG out;
    for (DWORD i = 0; i < numCompare; i++) {
        out = num & mask;
        out >>= i * 4;
        if (out < 10) {
            retval.prepend((DWORD)out);
        } else {
            out -= 10;
            retval.prepend(usedAlpha[out]);
        }
        // przesuwamy maskę o 4 bity w lewo
        mask <<= 4;
    }

    // usuwamy początkowe zera
    retval.ltrim(L'0');

    // jeśli minimalna długość łańcucha jest większa od aktualnej, to paddujemy łańcuch 0-ami
    // z lewej strony
    size_t padCount = 0;
	size_t retval_len = retval.length();
	if (minHexNumberLength >= retval_len) {
		padCount = minHexNumberLength - retval_len;
	}
    while (padCount > 0) {
        retval.prepend(L"0");
        padCount--;
    }

    if (prependx0) {
        retval.prepend(L"0x");
    }

    return retval;
}

LONGLONG WINAPI hexToInt(CmzString hex) {
    size_t len = hex.length();
    if (len < 1) {
        return 0;
    }

    WCHAR *buf = hex.getBuffer();
    if (buf[len-1] == L'h') {
        buf[len-1] = NULL;
        len -= 1;
    }

    if (len == 0) {
        return 0;
    }

    if (buf[0] == L'#') {
        hex.ltrim(L'#');
        len -= 1;
    }

    if (len == 0) {
        return 0;
    }

    if (buf[0] == L'0' && buf[1] == L'x') {
        hex = hex.substr(2);
        len -= 2;
    }

    if (len == 0) {
        return 0;
    }

    LONGLONG retval = 0;
    buf = hex.getBuffer();
    LONGLONG count = 0;
    LONGLONG maxcount = sizeof(LONGLONG)<<1;
    WCHAR c;
    LONGLONG b;
    for (LONGLONG i = (LONGLONG)len - 1; i >= 0; i--) {
        c = buf[i];
        if (c >= 0x30 && c <= 0x39) {
            b = (LONGLONG)(c - 0x30);
        } else {
            if (c == L'a' || c == L'A') {
                b = 10;
            } else if (c == L'b' || c == L'B') {
                b = 11;
            } else if (c == L'c' || c == L'C') {
                b = 12;
            } else if (c == L'd' || c == L'D') {
                b = 13;
            } else if (c == L'e' || c == L'E') {
                b = 14;
            } else if (c == L'f' || c == L'F') {
                b = 15;
            } else {
                return 0;
            }
        }
        retval = retval | (b << (count * 4));
        count++;
        if (count == maxcount) {
            break;
        }
    }

    return retval;
}



// *****************************************
// Static CmzString member functions
// *****************************************
WCHAR CmzString::charToWchar(CHAR ch) {
    CHAR source[2];
    WCHAR dest[2];

    source[0] = ch;
    source[1] = NULL;
	MultiByteToWideChar(CP_ACP, 0, source, -1, dest, 2);

    return dest[0];
}

CmzString CmzString::makeString(WCHAR ch, ULONGLONG count) {
    ULONGLONG buf_size = count + 1;
    WCHAR* buf = new WCHAR[buf_size];
    if (!buf) {
        return COMMON_EMPTY_STRING;
    }

    for (ULONGLONG i = 0; i < count; i++) {
        buf[i] = ch;
    }
    buf[count] = NULL;

    return CmzString(buf, buf_size);
}

CmzString& CmzString::toUppercase() {
    _wcsupr(this->buf);
    return *this;
}

CmzString& CmzString::toLowercase() {
    _wcslwr(this->buf);
    return *this;
}

}
