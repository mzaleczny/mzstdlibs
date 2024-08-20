#ifndef __STRING_H__
#define __STRING_H__

#include "mzstdCommon.h"

namespace mzstd {

#define FORMAT_ALIGN_LEFT       0
#define FORMAT_ALIGN_RIGHT      1


#define ENC_UTF8_WITH_BOM			1
#define ENC_UTF8					2
#define ENC_UNICODE					3
#define ENC_ANSI					10
#define ENC_ISO2					11
#define ENC_OEM_852                 12


class CmzStringList;
class CmzString;
class CmzObject;

// String encoding functions
DECLSPEC_MZSTD LONGLONG WINAPI DetectDataEncoding(BYTE *buf, size_t buflen);
DECLSPEC_MZSTD VOID WINAPI Iso2ToAnsi(BYTE *buf, size_t buflen);
DECLSPEC_MZSTD VOID WINAPI AnsiToIso2(BYTE *buf, size_t buflen);
DECLSPEC_MZSTD VOID WINAPI Oem852ToAnsi(BYTE *buf, size_t buflen);
DECLSPEC_MZSTD VOID WINAPI AnsiToOem852(BYTE *buf, size_t buflen);
//DECLSPEC_MZSTD VOID WINAPI UnicodeToAnsi(BYTE *buf, ULONGLONG buflen);


DECLSPEC_MZSTD WORD WINAPI getCharType(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharUpper(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharLower(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharDigit(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharSpace(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharPunct(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharCntrl(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharBlank(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharXdigit(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharAlpha(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharDefined(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharAlphanumeric(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharPrintable(WCHAR ch);
DECLSPEC_MZSTD BOOL WINAPI isCharSentenceBoundary(WCHAR ch);

DECLSPEC_MZSTD BOOL WINAPI isDecimal(const WCHAR* str);
DECLSPEC_MZSTD BOOL WINAPI isDouble(const WCHAR* str);
DECLSPEC_MZSTD CmzString WINAPI FormatString(const CmzString& s, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatTimeValue(SYSTEMTIME* value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatDateValue(SYSTEMTIME* value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatDateTimeValue(SYSTEMTIME* value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatStringValue(const WCHAR* value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatLONGValue(LONG value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatBoolValue(BOOL value, BOOL uppercase, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatCharValue(CHAR value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatWcharValue(WCHAR value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatULONGValue(ULONG value, LONG align, WCHAR fillChar, ULONG width);
DECLSPEC_MZSTD CmzString WINAPI FormatFloatValue(float value, LONG align, WCHAR fillChar, ULONG width, LONG precision);
DECLSPEC_MZSTD CmzString WINAPI FormatDoubleValue(double value, LONG align, WCHAR fillChar, ULONG width, LONG precision);
DECLSPEC_MZSTD CmzString WINAPI FormatHexValue(LONG value, LONG align, WCHAR fillChar, ULONG width, BOOL prepend0x, BOOL appendH, BOOL useSmallAlpha, LONG minLen);
DECLSPEC_MZSTD CmzString __cdecl FormatMessage(const WCHAR* format, ...);
DECLSPEC_MZSTD CmzString __cdecl FormatMessage(const WCHAR* format, va_list paramList);
DECLSPEC_MZSTD VOID __cdecl Stdout(const WCHAR* format, ...);
DECLSPEC_MZSTD VOID __cdecl Stdout(CmzString format, ...);
DECLSPEC_MZSTD VOID __cdecl Stderr(const WCHAR* format, ...);
DECLSPEC_MZSTD VOID __cdecl StdoutInEnc(LONG outputEncoding, const WCHAR* format, ...);
DECLSPEC_MZSTD VOID __cdecl StderrInEnc(LONG outputEncoding, const WCHAR* format, ...);

// Zwraca łańcuch z postacią heksadecymalną dla podanej liczby. Zwracany jest np ciąg 0x45.
// Parametr minHexNumber określa minimalną liczbę znaków po ciągu '0x'. Jeśli jest on wiekszy
// niż wynikowy łańcuch, to wtedy ten wynikowy łańcuch jest uzupełniany 0-ami po lewej stronie.
// Jeśli useSmallAlpah jest równe TRUE, to dla cyfr 10..15 użyte są znaki 'a'..'f', w przeciwnym
// razie użyte są znaki 'A'..'F'
DECLSPEC_MZSTD CmzString WINAPI intToHex(LONGLONG num, size_t minHexNumberLength = 2, BOOL useSmallAlpha = TRUE, BOOL prependx0 = TRUE);
DECLSPEC_MZSTD LONGLONG WINAPI hexToInt(CmzString hex);

class DECLSPEC_MZSTD CmzString {
public:
    // WARNING!
    // this two private attributes must be first in class structure, to allow proper CmzString* class
    // recognition.
    WCHAR* _originClassName;
    WCHAR* _realClassName;
    
public:    
	WINAPI CmzString();
	WINAPI CmzString(const CmzString &s);
	WINAPI CmzString(const WCHAR* string);
	WINAPI CmzString(WCHAR *string, size_t buf_size);
	WINAPI CmzString(const CHAR* string);
	WINAPI CmzString(WCHAR ch);
	WINAPI CmzString(WCHAR ch, LONG repeatCount);
	WINAPI CmzString(CHAR ch);
	WINAPI CmzString(UCHAR ch);
	WINAPI CmzString(INT number, BOOL toHex = FALSE, LONG minDigits = 0);
	WINAPI CmzString(UINT number, BOOL toHex = FALSE, LONG minDigits = 0);
	WINAPI CmzString(LONG number, BOOL toHex = FALSE, LONG minDigits = 0);
    WINAPI CmzString(ULONG number, BOOL toHex = FALSE, LONG minDigits = 0);
	WINAPI CmzString(ULONGLONG number, BOOL toHex = FALSE, LONG minDigits = 0);
	WINAPI CmzString(LONGLONG number, BOOL toHex = FALSE, LONG minDigits = 0);
    WINAPI CmzString(BOOL value, WCHAR uppercase); // uppercase can be: 'T'/'t' meaning uppercase/lowercase
	WINAPI CmzString(double number, LONG precision = 2);
	virtual WINAPI ~CmzString();

    virtual BOOL WINAPI equals(CmzObject* obj);
    virtual BOOL WINAPI equals(CmzString* str);
    virtual CmzString WINAPI toString();

	inline size_t WINAPI length() const { return wcslen(this->buf); }
    // Returns length of buffer till first double NULL character. if double NULL not found result is 0.
	ULONGLONG WINAPI lengthDoubleNull();
	inline BOOL WINAPI empty() { return this->length() == 0; }
	inline WCHAR* WINAPI getBuffer() const { return this->buf; }
	inline WCHAR* WINAPI toWCHAR() const { return this->buf; }
	VOID WINAPI setBuffer(WCHAR *buf, size_t buf_size);
	inline size_t WINAPI getBufferSize() const { return this->buf_size; }

    LONGLONG WINAPI charCount(CHAR ch);
    LONGLONG WINAPI charCount(WCHAR ch);
    ULONGLONG WINAPI wordCount();
    CmzString WINAPI getFirstToken(const WCHAR* tokenBoundaryChars);
    CmzString WINAPI popFirstToken(const WCHAR* tokenBoundaryChars);
    CmzString WINAPI getLastToken(const WCHAR* tokenBoundaryChars);
    CmzString WINAPI popLastToken(const WCHAR* tokenBoundaryChars);
    CmzString WINAPI getFirstWord();
    CmzString WINAPI popFirstWord();
    CmzString WINAPI getLastWord();
    CmzString WINAPI popLastWord();

    CmzString WINAPI substr(size_t start, size_t length = 0);

	// Funkcja sprawdza, czy string przechowuje wartość numeryczną. Jeśli dodatkowo
	// zostały podane niezerowe wskaźniki na zmienne isInt i isDouble, to zwracana
	// jest informacja, czy wartość jest liczbą całkowitą czy liczbą rzeczywistą.
	BOOL WINAPI isNumeric(BOOL *isInt = NULL, BOOL *isDouble = NULL);
	BOOL WINAPI isInt();
	BOOL WINAPI isDouble();
	LONG WINAPI toInt();
	LONG WINAPI toIntRound();
	LONG WINAPI toIntFloor();
	LONG WINAPI toIntCeil();
	double WINAPI toDouble();
    // Konwertuje przechowywany łańcuch UNICODE do ANSI i kopiuje wynik do podanego bufora.
    // Zwraca ilość skopiowanych znaków.
    ULONGLONG WINAPI toCharString(CHAR* dest, size_t destSize, LONGLONG outputEncoding = ENC_ANSI) const;
    ULONGLONG WINAPI toAnsi(CHAR* dest, size_t destSize) const {
        return this->toCharString(dest, destSize, ENC_ANSI);
    };
    ULONGLONG WINAPI toIso2(CHAR* dest, size_t destSize) const {
        return this->toCharString(dest, destSize, ENC_ISO2);
    };
    ULONGLONG WINAPI toOem852(CHAR* dest, size_t destSize) const {
        return this->toCharString(dest, destSize, ENC_OEM_852);
    };

    // Splits string at specified character and returns string list
    // if maxItems == 0, then splitting is unlimited
    CmzStringList* WINAPI explode(WCHAR ch, ULONGLONG maxItems = 0);
    // Splits string at specified character under constraintant that each item has minimum
    // minItemLength characters
    CmzStringList* WINAPI explodeAfterMinLength(WCHAR ch, ULONGLONG minItemLength);
    // Splits string into substrings of specified count words
    CmzStringList* WINAPI explodeToWords(ULONGLONG wordCount);
    
    // Splits string at each of the specified characters and returns string list
    CmzStringList* WINAPI explodeAtChar(const WCHAR* stringOfChars);
    
    VOID WINAPI rtrim(WCHAR ch);
    VOID WINAPI rtrimAllChars(const WCHAR* stringOfChars);
    inline VOID WINAPI rtrim() {
        this->rtrimAllChars(L" \t\n\r");
    }
    VOID WINAPI ltrim(WCHAR ch);
    VOID WINAPI ltrimAllChars(const WCHAR * stringOfChars);
    inline VOID WINAPI ltrim() {
        this->ltrimAllChars(L" \t\n\r");
    }
    VOID WINAPI trim(WCHAR ch);
    VOID WINAPI trimAllChars(const WCHAR* stringOfChars);
    inline VOID WINAPI trim() {
        this->rtrim();
        this->ltrim();
    }
    
    BOOL WINAPI startsWith(CmzString s);
    BOOL WINAPI startsWithChar(WCHAR ch);
    BOOL WINAPI startsWithOneOfChars(const WCHAR* stringOfChars);
    BOOL WINAPI endsWith(CmzString s);
    BOOL WINAPI endsWithChar(WCHAR ch);
    BOOL WINAPI endsWithOneOfChars(const WCHAR* stringOfChars);
    BOOL WINAPI contains(WCHAR ch);
    BOOL WINAPI contains(const WCHAR* substring);
    BOOL WINAPI containsOneOfChars(const WCHAR* stringOfChars);
    BOOL WINAPI containsWhiteSpace();
    LONGLONG WINAPI strpos(const WCHAR* substring, size_t startFrom = 0);
    LONGLONG WINAPI strposLast(const WCHAR* substring, size_t startFrom = 0);

	// Znajduje ostatnie wystąpienie podanego znaku i wstawia w jego miejsce NULL, czyli
	// obcina łańcuch na ostatnim wystąpieniu tego znaku.
    VOID WINAPI cutAtLast(WCHAR ch);
    VOID WINAPI cutAtLastChar(const WCHAR* stringOfChars);
    
    // Zwraca podłańcuch znajdujący się po ostatnim wystąpieniu znaku ch
    CmzString WINAPI strAfterLast(WCHAR ch);
    // Zwraca podłańcuch znajdujący się po ostatnim wystąpieniu któregokolwiek ze znaków podanych w ciągu
    // 'stringOfChars'
    CmzString WINAPI strAfterLastChar(const WCHAR* stringOfChars);

    // wstawia znak na podanej pozycji
    VOID WINAPI insertCharAt(WCHAR ch, size_t pos);
    // wstawia podłańcuch na podanej pozycji
    VOID WINAPI insertAt(size_t pos, const CmzString& s);
    // usuwa znak na podanej pozycji, przesuwając wszystkie znaki na prawo od niego o jedną pozycję w lewo
    VOID WINAPI deleteCharAt(size_t pos);
    VOID WINAPI deleteSubstr(size_t startpos, size_t length);

	// odcina z końca łańcucha podaną liczbę znaków
	VOID WINAPI truncateAtEnd(size_t numChars);
	// dokleja na początku łańcucha znak ch aż do osiągnięcia podanej długości łańcucha.
	// Jeśli długość łańcucha jest większa od podanej wymaganej długści, to nie robi
	// nic.
	VOID WINAPI padLeft(WCHAR ch, ULONG requiredSize);
	// Funkcja identyczna do powyższej z tym, że dokleja znaki na końcu łańcucha.
	VOID WINAPI padRight(WCHAR ch, ULONG requiredSize);

	// zamienia wszystkie wystąpienia znaku src znakiem dst i zwraca ilość
	// podmienionych znaków
	size_t WINAPI replace(WCHAR src, WCHAR dst);
    // zastępuje wszystkie znaki łańcucha o kodzie mniejszym niż podany kod podanym znakiem
    // i zwraca ilość zastąpionych znaków.
    size_t WINAPI replaceCharsWithCodeLessThan(WCHAR vkCode, WCHAR dst);
    // zastępuje podłańcuch zaczynający się od znaku o indeksie start i długości length podanym
    // łańcuchem.
	VOID WINAPI replace(size_t start, size_t length, const CmzString& replaceWith);
    // Zastępuje podany łańcuch 'toReplace' łańcuchem 'replaceWith'. Zwraca ilość wykonanych zastąpień
	size_t WINAPI replace(CmzString toReplace, CmzString replaceWith, size_t startFrom = 0, size_t maxReplacementCount = 0);

	// usuwa z lancucha wszystkie wystąpienia podanego znaku i zwraca ilość
	// usuniętych znaków.
	ULONGLONG WINAPI remove(WCHAR ch);
    // usuwa z łańcucha znaki o kodzie mniejszym niż podany kod i zwraca ilość usuniętych znaków.
    ULONGLONG WINAPI removeCharsWithCodeLessThan(WCHAR vkCode);
    
    // If minLen > 0 and length of the string is less than minLength then extra zeros are added at beginning.
    VOID WINAPI addPrefixZeros(ULONGLONG minLen = 0);

    WCHAR getCharAt(size_t i) const {
        return this->buf[i];
    }
    WCHAR lastChar() {
        size_t len = wcslen(this->buf);
        if (len) {
            return this->buf[len - 1];
        }
        return NULL;
    }

    // foreachSubcontentDo
    //    for each found subcontent (in this string) placed between beginTag and endTag calls function doFunc with
    //    pointer to found subcontent, its placement and specified pointer as params. If function doFunc return TRUE, then iterating is stopped.
    //    Return number of found and processed subcontents.
    size_t WINAPI foreachSubcontentDo(const WCHAR* beginTag, const WCHAR* endTag, BOOL (WINAPI *doFunc)(LONGLONG startPos, LONGLONG endPos, CmzString* subcontentItem, LPVOID customData), LPVOID customData);
    
	// dokleja na końcu tego łańcucha odpowiedni element i zwraca referencję do tego
	// łańcucha.
    CmzString& WINAPI append(const CmzString &s);
    CmzString& WINAPI append(const CHAR *s);
    CmzString& WINAPI append(WCHAR ch);
    CmzString& WINAPI append(CHAR ch);
    CmzString& WINAPI append(UCHAR ch);
    CmzString& WINAPI append(const WCHAR *s);
    CmzString& WINAPI append(INT num);
    CmzString& WINAPI append(UINT num);
    CmzString& WINAPI append(LONGLONG num);
    CmzString& WINAPI append(ULONGLONG num);
    CmzString& WINAPI append(double num);
    
	// Appends after end of buffer (after trailing NULL character) specified value. If necessary enlarge internal
    // buffer. At end of appended string appends additiona NULL character (that mean at end of such buffer there
    // are two NULL characters). Appended string is invisible in displaying in standard method. It is for use by
    // such functions as GetOpenFileName (to create filter).
    CmzString& WINAPI appendAfterEos(const CmzString &s, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(const CHAR *s, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(WCHAR ch, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(CHAR ch, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(UCHAR ch, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(const WCHAR *s, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(INT num, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(UINT num, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(LONGLONG num, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(ULONGLONG num, WCHAR glueChar = NULL);
    CmzString& WINAPI appendAfterEos(double num, WCHAR glueChar = NULL);
    
	// dokleja na początku tego łańcucha odpowiedni element i zwraca referencję do tego
	// łańcucha.
    CmzString& WINAPI prepend(CmzString &s);
    CmzString& WINAPI prepend(const CHAR *s);
    CmzString& WINAPI prepend(WCHAR ch);
    CmzString& WINAPI prepend(CHAR ch);
    CmzString& WINAPI prepend(UCHAR ch);
    CmzString& WINAPI prepend(const WCHAR *s);
    CmzString& WINAPI prepend(INT num);
    CmzString& WINAPI prepend(UINT num);
    CmzString& WINAPI prepend(LONG num);
    CmzString& WINAPI prepend(ULONG num);
    CmzString& WINAPI prepend(double num);

    ULONGLONG WINAPI linesCount();
    CmzString& WINAPI prependEachLine(CmzString &s);
    CmzString& WINAPI prependEachLine(const CHAR *s);
    CmzString& WINAPI prependEachLine(WCHAR ch);
    CmzString& WINAPI prependEachLine(CHAR ch);
    CmzString& WINAPI prependEachLine(UCHAR ch);
    CmzString& WINAPI prependEachLine(const WCHAR *s);
    CmzString& WINAPI prependEachLine(INT num);
    CmzString& WINAPI prependEachLine(UINT num);
    CmzString& WINAPI prependEachLine(LONGLONG num);
    CmzString& WINAPI prependEachLine(ULONGLONG num);
    CmzString& WINAPI prependEachLine(double num);

	// przeciążone operatory
    inline BOOL WINAPI operator==(const CmzString &s) { return wcscmp(this->buf, s.getBuffer()) == 0; };
    inline BOOL WINAPI operator==(const CHAR *s) { CmzString cs(s); return (*this) == cs; };
    inline BOOL WINAPI operator==(WCHAR ch) { CmzString cs(ch); return (*this) == cs; };
    inline BOOL WINAPI operator==(CHAR ch) { CmzString cs(ch); return (*this) == cs; };
    inline BOOL WINAPI operator==(UCHAR ch) { CmzString cs(ch); return (*this) == cs; };
    inline BOOL WINAPI operator==(const WCHAR *s) { CmzString cs(s); return (*this) == cs; };
    inline BOOL WINAPI operator==(INT num) { CmzString cs(num); return (*this) == cs; };
    inline BOOL WINAPI operator==(UINT num) { CmzString cs(num); return (*this) == cs; };
    inline BOOL WINAPI operator==(LONGLONG num) { CmzString cs(num); return (*this) == cs; };
    inline BOOL WINAPI operator==(ULONGLONG num) { CmzString cs(num); return (*this) == cs; };
    inline BOOL WINAPI operator==(double num) { CmzString cs(num); return (*this) == cs; };

    inline BOOL WINAPI operator!=(const CmzString &s) { return wcscmp(this->buf, s.getBuffer()) != 0; };
    inline BOOL WINAPI operator!=(const CHAR *s) { CmzString cs(s); return (*this) != cs; };
    inline BOOL WINAPI operator!=(const WCHAR* s) { CmzString cs(s); return (*this) != cs; };
    inline BOOL WINAPI operator!=(WCHAR ch) { CmzString cs(ch); return (*this) != cs; };
    inline BOOL WINAPI operator!=(CHAR ch) { CmzString cs(ch); return (*this) != cs; };
    inline BOOL WINAPI operator!=(UCHAR ch) { CmzString cs(ch); return (*this) != cs; };
    inline BOOL WINAPI operator!=(INT num) { CmzString cs(num); return (*this) != cs; };
    inline BOOL WINAPI operator!=(UINT num) { CmzString cs(num); return (*this) != cs; };
    inline BOOL WINAPI operator!=(LONGLONG num) { CmzString cs(num); return (*this) != cs; };
    inline BOOL WINAPI operator!=(ULONGLONG num) { CmzString cs(num); return (*this) != cs; };
    inline BOOL WINAPI operator!=(double num) { CmzString cs(num); return (*this) != cs; };

	CmzString& WINAPI operator=(const CmzString &s);
	CmzString& WINAPI operator=(const CHAR *s);
    CmzString& WINAPI operator=(const WCHAR* s);
	CmzString& WINAPI operator=(WCHAR ch);
	CmzString& WINAPI operator=(CHAR ch);
	CmzString& WINAPI operator=(UCHAR ch);
	CmzString& WINAPI operator=(INT num);
	CmzString& WINAPI operator=(UINT num);
	CmzString& WINAPI operator=(LONGLONG num);
	CmzString& WINAPI operator=(ULONGLONG num);
	CmzString& WINAPI operator=(double num);

	CmzString& WINAPI operator+=(const CmzString &s);
	CmzString& WINAPI operator+=(const CHAR *s);
    CmzString& WINAPI operator+=(const WCHAR* s);
	CmzString& WINAPI operator+=(CHAR ch);
	CmzString& WINAPI operator+=(UCHAR ch);
	CmzString& WINAPI operator+=(WCHAR ch);
	CmzString& WINAPI operator+=(INT num);
	CmzString& WINAPI operator+=(UINT num);
	CmzString& WINAPI operator+=(LONG num);
	CmzString& WINAPI operator+=(ULONG num);
	CmzString& WINAPI operator+=(LONGLONG num);
    CmzString& WINAPI operator+=(ULONGLONG num);
	CmzString& WINAPI operator+=(double num);

	CmzString WINAPI operator+(const CmzString &s);
	CmzString WINAPI operator+(const CHAR *s);
    CmzString WINAPI operator+(const WCHAR* s);
	CmzString WINAPI operator+(WCHAR ch);
	CmzString WINAPI operator+(CHAR ch);
	CmzString WINAPI operator+(UCHAR ch);
	CmzString WINAPI operator+(INT num);
	CmzString WINAPI operator+(UINT num);
	CmzString WINAPI operator+(LONG num);
	CmzString WINAPI operator+(ULONG num);
	CmzString WINAPI operator+(LONGLONG num);
	CmzString WINAPI operator+(double num);

    WCHAR WINAPI operator[](INT i) {
        return this->buf[i];
    }

    
    // Returned position is counted in WCHAR*
    LONGLONG WINAPI findDoubleNullPos();
    VOID WINAPI setStringArray(WCHAR *buf, ULONGLONG bufsize);
    
    static WCHAR WINAPI charToWchar(CHAR ch);
    // Returned position is counted in WCHAR*
    static LONGLONG WINAPI findDoubleNullPos(const WCHAR* buf, size_t bufsize);
    
    // Returns new string consisting of character ch repeated count times.
    static CmzString WINAPI makeString(WCHAR ch, ULONGLONG count);
    
    VOID WINAPI zeroBuffer();

    CmzString& WINAPI toUppercase();
    CmzString& WINAPI toLowercase();
    
protected:
	VOID WINAPI initBuffer();
	VOID WINAPI initBuffer(size_t size);
	VOID WINAPI extendBuffer(size_t added_size);
	VOID WINAPI cleanup();

private:
	size_t buf_size;
	WCHAR *buf;
	ULONGLONG _growBy;

	VOID WINAPI _commonInit();
};

}

#endif
