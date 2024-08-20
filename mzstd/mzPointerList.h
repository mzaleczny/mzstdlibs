#ifndef __MZSTD_POINTER_LIST_H__
#define __MZSTD_POINTER_LIST_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzPointerList : public CmzObject {
public:
    WINAPI CmzPointerList(ULONGLONG capacity = 100);
    virtual WINAPI ~CmzPointerList();

    VOID WINAPI append(LPVOID data);
    inline VOID WINAPI push(LPVOID data) { this->append(data); };
    VOID WINAPI addAsFirst(LPVOID data);
    VOID WINAPI insert(ULONGLONG pos, LPVOID data);
    ULONGLONG WINAPI del(LPVOID data);
    BOOL WINAPI delFirst(LPVOID data);
    BOOL WINAPI delAt(ULONGLONG pos);
    LONGLONG WINAPI pos(LPVOID data);
    BOOL WINAPI contains(LPVOID data);
    LPVOID WINAPI getMinValue();
    LPVOID WINAPI getMaxValue();
    // Bubble-sort (default in ascending order)
    VOID WINAPI sort(BOOL asc = TRUE);
    inline LPVOID WINAPI getAt(ULONGLONG pos) { if (pos < this->_size) return this->_items[pos]; return 0; };
    VOID WINAPI setAt(ULONGLONG pos, LPVOID data);
    inline LPVOID WINAPI getFirst() { if (this->_size > 0) return this->_items[0]; return 0; };
    inline LPVOID WINAPI getLast() { if (this->_size > 0) return this->_items[this->_size - 1]; return 0; };
    inline LPVOID WINAPI pop() {
        LPVOID retval;
        if (this->_size < 1) { return 0; }
        retval = this->getAt(this->_size - 1);
        this->delAt(this->_size - 1);
        return retval;
    }
    inline ULONGLONG WINAPI size() { return this->_size; };
    inline LONGLONG WINAPI capacity() { return this->_capacity; };
    virtual VOID WINAPI clear();
    inline LPVOID* WINAPI getItemsBuf() { return this->_items; }

    CmzPointerList* WINAPI clone();
    virtual CmzString WINAPI toString();

protected:
    LPVOID* _items;
    ULONGLONG _size;
    LONGLONG _capacity;

    LPVOID* WINAPI _createNullArray(ULONGLONG capacity);
};

}

#endif
