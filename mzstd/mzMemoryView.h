#ifndef __MZSTD_MEMORY_VIEW_H__
#define __MZSTD_MEMORY_VIEW_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzMemoryView : public CmzObject {
public:
    WINAPI CmzMemoryView(LPVOID pointer, DWORD size, LONGLONG offset);
    virtual WINAPI ~CmzMemoryView();

    VOID WINAPI setMemory(LPVOID pointer, DWORD size);
    CmzString WINAPI getStringOfBytes(BOOL withOffsets = TRUE, BOOL withASCII = TRUE);
    CmzString WINAPI getStringOfWords(BOOL withOffsets = TRUE, BOOL withASCII = TRUE);
    CmzString WINAPI getStringOfDwords(BOOL withOffsets = TRUE, BOOL withASCII = TRUE);
    
    VOID WINAPI print(CmzString indent = L"", BOOL dumpAsBytes = FALSE, BOOL dumpAsWords = FALSE, BOOL dumpAsDwords = TRUE);
    CmzString WINAPI getDump(CmzString indent = L"", BOOL dumpAsBytes = FALSE, BOOL dumpAsWords = FALSE, BOOL dumpAsDwords = TRUE);
    virtual CmzString WINAPI toString();

private:
    LPVOID _pointer;
    DWORD _size;
    LONGLONG _offset;
};

}

#endif
