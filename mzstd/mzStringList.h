#ifndef __MZSTD_STRING_LIST_H__
#define __MZSTD_STRING_LIST_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzStringList : public CmzObject {
public:
    WINAPI CmzStringList(ULONGLONG capacity = 50);
    virtual WINAPI ~CmzStringList();

    VOID WINAPI append(CmzString data);
    VOID WINAPI append(CmzStringList *lst);
    inline VOID WINAPI push(CmzString data) { this->append(data); };
    VOID WINAPI addAsFirst(CmzString data);
    VOID WINAPI addAsFirst(WCHAR* data);
    VOID WINAPI insert(ULONGLONG pos, CmzString data);
    ULONGLONG WINAPI del(CmzString data);
    BOOL WINAPI delFirst(CmzString data);
    BOOL WINAPI delAt(ULONGLONG pos);
    LONGLONG WINAPI pos(CmzString data);
    BOOL WINAPI contains(CmzString data);
    inline CmzString& WINAPI getAt(ULONGLONG pos) { if (pos < this->_size) return *(CmzString*)this->_items[pos]; return this->_emptyStr; };
    VOID WINAPI setAt(ULONGLONG pos, CmzString data);
	inline CmzString& WINAPI getLast() { if (this->_size > 0) return *(CmzString*)this->_items[this->_size - 1]; return this->_emptyStr; };
    inline CmzString WINAPI pop() {
        CmzString retval;
        if (this->_size < 1) { return ""; }
        retval = this->getAt(this->_size - 1);
        this->delAt(this->_size - 1);
        return retval;
    }
    inline ULONGLONG WINAPI size() { return this->_size; };
    inline LPVOID* WINAPI getItemsBuf() { return this->_items; }
    VOID WINAPI clear();
	VOID WINAPI writeToFile(CmzString fname);
	VOID WINAPI readFromFile(CmzString fname);

    virtual CmzString WINAPI join(const WCHAR* glue = L" ");
    // Returns CmzString object (passed in first argument) containing substring separated by glue characters
    // and ended with double NULL. If arraysize argument is not NULL then it is set to total string size
    // including trailing double NULL chars.
    virtual VOID WINAPI joinIntoStringArray(CmzString* result, WCHAR glue = NULL);
    virtual CmzString WINAPI toString();
    WINAPI operator WCHAR*();

private:
    LPVOID* _items;
	CmzString _emptyStr;
    ULONGLONG _size;
    ULONGLONG _capacity;
	ULONGLONG _growBy;

    LPVOID* WINAPI createNullArray(ULONGLONG capacity);
};

}

#endif
