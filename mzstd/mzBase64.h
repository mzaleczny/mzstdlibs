#ifndef __MZSTD_BASE64_H__
#define __MZSTD_BASE64_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {
/*
Tabela znaków w kodowaniu base64:

Value Encoding  Value Encoding  Value Encoding  Value Encoding
   0 A            17 R            34 i            51 z
   1 B            18 S            35 j            52 0
   2 C            19 T            36 k            53 1
   3 D            20 U            37 l            54 2
   4 E            21 V            38 m            55 3
   5 F            22 W            39 n            56 4
   6 G            23 X            40 o            57 5
   7 H            24 Y            41 p            58 6
   8 I            25 Z            42 q            59 7
   9 J            26 a            43 r            60 8
  10 K            27 b            44 s            61 9
  11 L            28 c            45 t            62 +
  12 M            29 d            46 u            63 /
  13 N            30 e            47 v
  14 O            31 f            48 w         (pad) =
  15 P            32 g            49 x
  16 Q            33 h            50 y
*/

// Encoding and decoding Base64 code
class DECLSPEC_MZSTD CmzBase64 : public CmzObject {
public:
	// 63rd CHAR used for Base64 code
	WCHAR CHAR_63;
	// 64th CHAR used for Base64 code
	WCHAR CHAR_64;
	// Char used for padding
	WCHAR CHAR_PAD;

    WINAPI CmzBase64();
    virtual WINAPI ~CmzBase64();

	// Encodes binary data to Base64 code. Returns size of encoded data.
	LONGLONG WINAPI encode(const UCHAR* inData, LONGLONG dataLength, CmzString& outCode);
	// Decodes Base64 code to binary data. Returns size of decoded data.
	LONGLONG WINAPI decode(const CmzString& inCode, UCHAR* outData);
	// Returns maximum size of decoded data based on size of Base64 code.
	LONGLONG WINAPI getDataLength(LONGLONG codeLength);
	// Returns maximum length of Base64 code based on size of uncoded data.
	LONGLONG WINAPI getCodeLength(LONGLONG dataLength);
    // koduje plik do zmiennej
    LONGLONG WINAPI encodeFileToVar(CmzString fname, CmzString& outCode);
    // koduje plik w miejscu
    LONGLONG WINAPI encodeFile(CmzString fname, CmzString outputFName = L"");
    // dekoduje plik do zmiennej
    LONGLONG WINAPI decodeFileToVar(CmzString fname, UCHAR* outData);
    // dekoduje plik w miejscu
    LONGLONG WINAPI decodeFile(CmzString fname, CmzString outputFName = L"");
    
protected:
    // inicjalizacja
    VOID WINAPI _commonInit();
};

}

#endif
