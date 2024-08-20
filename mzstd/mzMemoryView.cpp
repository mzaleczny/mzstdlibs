#include "pch.h"
#include "mzMemoryView.h"

namespace mzstd {

CmzMemoryView::CmzMemoryView(LPVOID pointer, DWORD size, LONGLONG offset) {
    this->setRealClassName(L"CmzMemoryView");
    this->setMemory(pointer, size);
    this->_offset = offset;
}

CmzMemoryView::~CmzMemoryView() {
}

VOID CmzMemoryView::setMemory(LPVOID pointer, DWORD size) {
    this->_pointer = pointer;
    this->_size = size;
}

CmzString CmzMemoryView::getStringOfBytes(BOOL withOffsets, BOOL withASCII) {
    CmzString result;
    CmzString line;
    CmzString ascii;
    CmzString indentation = L"  ";
    CmzString spacing = L"    ";
    CmzString asciiIndentation = indentation.substr(0, indentation.length()/2);
    
    size_t count = this->_size;
    BYTE* ptr = (BYTE*)this->_pointer;
    UCHAR ch;
    LONGLONG currentOffset = this->_offset;
    line = COMMON_EMPTY_STRING;
    for (size_t i = 0; i < count; i++) {
        line += FormatHexValue(ptr[i], FORMAT_ALIGN_LEFT, L'0', 2, FALSE, FALSE, TRUE, 2);
        if (withASCII) {
            ch = (UCHAR)ptr[i];
            if (ch < 32 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
        }
        if ((i+1)%16 > 0) {
            if ((i+1)%8 == 0) {
                line += indentation + indentation;
                if (withASCII) {
                    ascii += asciiIndentation;
                }
            } else {
                line += indentation;
            }
        } else {
            if (withASCII) {
                line += spacing + ascii;
                ascii = COMMON_EMPTY_STRING;
            }
            if (withOffsets) {
                line = FormatHexValue(static_cast<LONG>(currentOffset), FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8) + L":  " + line;
                currentOffset += 16;
            }
            result += line + L"\n";
            line = COMMON_EMPTY_STRING;
        }
    }
    
    if (line != COMMON_EMPTY_STRING) {
        if (withASCII) {
            // we must pad line with spaces
            LONGLONG padCount = 16 - this->_size % 16;
            for (LONGLONG i = 0; i < padCount-1; i++) {
                line += CmzString(L"  ") + indentation;
            }
            if (16-padCount < 8) {
                line += indentation + CmzString(L"  ");
            } else if (padCount == 8) {
                line += indentation;
            } else {
                line += CmzString(L"  ");
            }
            line += spacing + ascii;
            ascii = COMMON_EMPTY_STRING;
        }
        if (withOffsets) {
            line = FormatHexValue(static_cast<LONG>(currentOffset), FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8) + L":  " + line;
            currentOffset += 16;
        }
        result += line;
    }
    
    result.rtrim();
    return result;
}

CmzString CmzMemoryView::getStringOfWords(BOOL withOffsets, BOOL withASCII) {
    CmzString result;
    CmzString ascii;
    CmzString indentation = L"  ";
    CmzString spacing = L"    ";
    CmzString asciiIndentation = indentation.substr(0, indentation.length()/2);
    
    size_t count = this->_size / sizeof(WORD);
    WORD* ptr = (WORD*)this->_pointer;
    BYTE* ptrb;
    UCHAR ch;
    for (size_t i = 0; i < count; i++) {
        result += FormatHexValue(ptr[i], FORMAT_ALIGN_LEFT, L'0', 4, FALSE, FALSE, TRUE, 4);
        if (withASCII) {
            ptrb = (BYTE*)&ptr[i];
            ch = (UCHAR)ptrb[i];
            if (ch == 0 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
            
            ch = (UCHAR)ptrb[i+1];
            if (ch == 0 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
        }
        if ((i+1)%8 > 0) {
            if ((i+1)%4 == 0) {
                result += indentation + indentation;
                if (withASCII) {
                    ascii += asciiIndentation;
                }
            } else {
                result += indentation;
            }
        } else {
            if (withASCII) {
                result += spacing + ascii;
                ascii = COMMON_EMPTY_STRING;
            }
            result += L"\n";
        }
    }
    
    /* TODO as in this->getStringOfBytes
    if (line != COMMON_EMPTY_STRING) {
        if (withASCII) {
            // we must pad line with spaces
            LONGLONG padCount = 8 - this->_size % 8;
            for (LONGLONG i = 0; i < padCount-1; i++) {
                line += CmzString(L"  ") + indentation;
            }
            if (8-padCount < 4) {
                line += indentation + CmzString(L"  ");
            } else if (padCount == 4) {
                line += indentation;
            } else {
                line += CmzString(L"  ");
            }
            line += spacing + ascii;
            ascii = COMMON_EMPTY_STRING;
        }
        if (withOffsets) {
            line = FormatHexValue(currentOffset, FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8) + L":  " + line;
            currentOffset += 16;
        }
        result += line;
    }
    */
    result.rtrim();
    return result;
}

CmzString CmzMemoryView::getStringOfDwords(BOOL withOffsets, BOOL withASCII) {
    CmzString result;
    CmzString ascii;
    CmzString indentation = L"  ";
    CmzString spacing = L"    ";
    CmzString asciiIndentation = indentation.substr(0, indentation.length()/2);
    
    size_t count = this->_size / sizeof(DWORD);
    DWORD* ptr = (DWORD*)this->_pointer;
    BYTE* ptrb;
    UCHAR ch;
    for (size_t i = 0; i < count; i++) {
        result += FormatHexValue(ptr[i], FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8);
        if (withASCII) {
            ptrb = (BYTE*)&ptr[i];
            ch = (UCHAR)ptrb[i];
            if (ch == 0 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
            
            ch = (UCHAR)ptrb[i+1];
            if (ch == 0 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
            
            ch = (UCHAR)ptrb[i+2];
            if (ch == 0 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
            
            ch = (UCHAR)ptrb[i+3];
            if (ch == 0 || ch > 127) {
                ch = '.';
            }
            ascii += ch;
        }
        if ((i+1)%4 > 0) {
            if ((i+1)%2 == 0) {
                result += indentation + indentation;
                if (withASCII) {
                    ascii += asciiIndentation;
                }
            } else {
                result += indentation;
            }
        } else {
            if (withASCII) {
                result += spacing + ascii;
                ascii = COMMON_EMPTY_STRING;
            }
            result += L"\n";
        }
    }
    
    /* TODO as in this->getStringOfBytes
    if (line != COMMON_EMPTY_STRING) {
        if (withASCII) {
            // we must pad line with spaces
            LONGLONG padCount = 4 - this->_size % 4;
            for (LONGLONG i = 0; i < padCount-1; i++) {
                line += CmzString(L"  ") + indentation;
            }
            if (4-padCount < 2) {
                line += indentation + CmzString(L"  ");
            } else if (padCount == 2) {
                line += indentation;
            } else {
                line += CmzString(L"  ");
            }
            line += spacing + ascii;
            ascii = COMMON_EMPTY_STRING;
        }
        if (withOffsets) {
            line = FormatHexValue(currentOffset, FORMAT_ALIGN_LEFT, L'0', 8, FALSE, FALSE, TRUE, 8) + L":  " + line;
            currentOffset += 16;
        }
        result += line;
    }
    */
    result.rtrim();
    return result;
}

VOID CmzMemoryView::print(CmzString indent, BOOL dumpAsBytes, BOOL dumpAsWords, BOOL dumpAsDwords) {
    CmzString output = this->getDump(indent, dumpAsBytes, dumpAsWords, dumpAsDwords);
    if (output != COMMON_EMPTY_STRING) {
        Stdout(output.getBuffer());
    }
}

CmzString CmzMemoryView::getDump(CmzString indent, BOOL dumpAsBytes, BOOL dumpAsWords, BOOL dumpAsDwords) {
    if (dumpAsBytes || dumpAsWords || dumpAsDwords) {
        CmzString output;
        CmzString dump;
        if (dumpAsBytes) {
            output += indent + L"BYTE DUMP:\n";
            dump = this->getStringOfBytes();
            dump.prependEachLine((indent + L"    ").getBuffer());
            output += dump + "\n";
        }
        if (dumpAsWords) {
            output += indent + L"WORD DUMP:\n";
            dump = this->getStringOfWords();
            dump.prependEachLine((indent + L"    ").getBuffer());
            output += dump + "\n";
        }
        if (dumpAsDwords) {
            output += indent + L"DWORD DUMP:\n";
            dump = this->getStringOfDwords();
            dump.prependEachLine((indent + L"    ").getBuffer());
            output += dump + "\n";
        }    
        return output;
    }
    
    return COMMON_EMPTY_STRING;
}

CmzString CmzMemoryView::toString() {
    return this->getDump(L"", TRUE, FALSE, FALSE);
}

}