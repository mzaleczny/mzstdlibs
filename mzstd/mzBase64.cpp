#include "pch.h"
#include "mzBase64.h"
#include "mzstdGlobals.h"
#include "mzFileUtils.h"

namespace mzstd {

VOID CmzBase64::_commonInit() {
    this->setRealClassName(L"CmzBase64");
    this->CHAR_63 = L'+';
    this->CHAR_64 = L'/';
    this->CHAR_PAD = L'=';
}

CmzBase64::CmzBase64() {
    this->_commonInit();
}

WINAPI CmzBase64::~CmzBase64() {
}

LONGLONG CmzBase64::encode(const UCHAR*inData, LONGLONG dataLength, CmzString& outCode) {
	outCode = L"";

	// output buffer which holds code during conversation
	LONGLONG len = this->getCodeLength(dataLength);
    WCHAR* out = new WCHAR[len * 2 + 1 * 2];

	// charachers used by Base64
	static const WCHAR alpha[] = 
	{
		L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z',
		L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z',
		L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', this->CHAR_63,this->CHAR_64
	};

	// mask - first six bits
	const LONGLONG mask = 0x3F;
    
	// used as temp 24-bits buffer
	union
	{
		UCHAR bytes[4];
		ULONGLONG block;
	} buffer;

	// coversation is done by taking three bytes at time of input data LONGLONG temp
	// then four six-bits values are extracted, converted to base64 characters
	// and at the end they are written to output buffer
    LONGLONG i, j, left, real_len = 0;
	for (i = 0, j = 0, left = dataLength; i < dataLength; i += 3, j += 4, left -= 3) {
		//------------------------
		// filling temp buffer

		// get first byte and puts it at MSB position in temp buffer
		buffer.bytes[2] = inData[i];

		// more data left?
		if (left > 1) {
			// get second byte and puts it at middle position in temp buffer
			buffer.bytes[1] = inData[i + 1];
			// more data left?
			if (left > 2) {
				// get third byte and puts it at LSB position in temp buffer
				buffer.bytes[0] = inData[i + 2];
            } else {
				// zero-padding of input data (last bytes)
				buffer.bytes[0] = 0;
            }
		} else {
			// zero-padding of input data (last two bytes)
			buffer.bytes[1] = 0;
			buffer.bytes[0] = 0;
		}

		//------------------------
		// constructing code from temp buffer
		// and putting it in output buffer

		// extract first and second six-bit value from temp buffer
		// and convert is to base64 character
		out[j] = alpha[(buffer.block >> 18) & mask];
		out[j + 1] = alpha[(buffer.block >> 12) & mask];
        real_len = j + 1 + 1;
		// more data left?
		if (left > 1) {
			// extract third six-bit value from temp buffer
			// and convert it to base64 character
			out[j + 2] = alpha[(buffer.block >> 6) & mask];
            real_len = j + 2 + 1;
			// more data left?
			if (left > 2) {
				// extract forth six-bit value from temp buffer
				// and convert it to base64 character
				out[j + 3] = alpha[buffer.block & mask];
                real_len = j + 3 + 1;
            } else {
				// pad output code
				out[j + 3] = CHAR_PAD;
                real_len = j + 3 + 1;
            }
		} else {
			// pad output code
			out[j + 2] = CHAR_PAD;
			out[j + 3] = CHAR_PAD;
            real_len = j + 3 + 1;
		}
	}

    out[real_len] = NULL;
	outCode = out;
	delete[] out;
	return real_len;
}

LONGLONG CmzBase64::decode(const CmzString& inCode, UCHAR* outData) {
    size_t codeLength = inCode.length();

	// used as temp 24-bits buffer
	union
	{
		UCHAR bytes[4];
		ULONGLONG block;
	} buffer;
	buffer.block = 0;

	// number of decoded bytes
	LONGLONG j = 0;

    size_t m;
    WCHAR x;
    LONGLONG val;
	for (size_t i = 0; i < codeLength; i++) {
		// position in temp buffer
		m = i % 4;

        x = inCode.getCharAt(i);
		val = 0;

		// converts base64 character to six-bit value
		if (x >= L'A' && x <= L'Z') {
			val = x - L'A';
        } else if (x >= L'a' && x <= L'z') {
			val = x - L'a' + L'Z' - L'A' + 1;
        } else if (x >= L'0' && x <= L'9') {
			val = x - L'0' + (L'Z' - L'A' + 1) * 2;
        } else if (x == this->CHAR_63) {
			val = 62;
        } else if (x == this->CHAR_64) {
			val = 63;
        }

		// padding chars are not decoded and written to output buffer
		if (x != this->CHAR_PAD) { 
			buffer.block |= val << (3 - m) * 6;
        } else {
			m--;
        }

		// temp buffer is full or end of code is reached
		// flushing temp buffer
		if (m == 3 || x == this->CHAR_PAD) {
			// writes byte from temp buffer (combined from two six-bit values) to output buffer
			outData[j++] = buffer.bytes[2];
			// more data left?
			if (x != this->CHAR_PAD || m > 1) {
				// writes byte from temp buffer (combined from two six-bit values) to output buffer
				outData[j++] = buffer.bytes[1];
				// more data left?
				if (x != this->CHAR_PAD || m > 2) {
					// writes byte from temp buffer (combined from two six-bit values) to output buffer
					outData[j++] = buffer.bytes[0];
                }
			}

			// restarts temp buffer
			buffer.block = 0;
		}

		// when padding CHAR is reached it is the end of code
		if (x == CHAR_PAD) {
			break;
        }
	}

	return j;
}

// Returns maximum size of decoded data based on size of Base64 code.
LONGLONG CmzBase64::getDataLength(LONGLONG codeLength) {
	return codeLength - codeLength / 4;
}

// Returns maximum length of Base64 code based on size of uncoded data.
LONGLONG CmzBase64::getCodeLength(LONGLONG dataLength) {
	LONGLONG len = dataLength + dataLength / 3 + (LONGLONG)(dataLength % 3 != 0);

	// output code size must be multiple of 4 bytes
	if (len % 4) {
		len += 4 - len % 4;
    }

	return len;
}

LONGLONG CmzBase64::encodeFileToVar(CmzString fname, CmzString& outCode) {
    DWORD fsize = GetFileSize(fname);
    BYTE* buffer = new BYTE[fsize];
    ReadBinaryFile(fname, buffer);
    LONGLONG len = this->encode(buffer, fsize, outCode);
    delete[] buffer;
    return len;
}

LONGLONG CmzBase64::encodeFile(CmzString fname, CmzString outputFName) {
    if (outputFName.length() < 1) {
        outputFName = fname;
    }
    CmzString outCode;
    LONGLONG len = this->encodeFileToVar(fname, outCode);
    WriteFile(outputFName, outCode);
    return len;
}

LONGLONG CmzBase64::decodeFileToVar(CmzString fname, UCHAR* outData) {
    CmzString inCode;
    inCode = ReadFile(fname);
    LONGLONG len = this->decode(inCode, outData);
    return len;
}

LONGLONG CmzBase64::decodeFile(CmzString fname, CmzString outputFName) {
    if (outputFName.length() < 1) {
        outputFName = fname;
    }
    CmzString inCode;
    inCode = ReadFile(fname);
    size_t bufSize = this->getDataLength((LONGLONG)inCode.length()) + 1;
    BYTE* buffer = new BYTE[bufSize];
    LONGLONG len = this->decode(inCode, buffer);
    WriteFile(outputFName, buffer, static_cast<DWORD>(len));
    delete[] buffer;
    return len;
}

}
