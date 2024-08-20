#ifndef __ARRAY_LIST_H__
#define __ARRAY_LIST_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzArrayList : public CmzObject {
public:
    WINAPI CmzArrayList(ULONGLONG capacity = 100);
    virtual WINAPI ~CmzArrayList();

    VOID WINAPI append(LONGLONG data);
    inline VOID WINAPI push(LONGLONG data) { this->append(data); };
    VOID WINAPI addAsFirst(LONGLONG data);
    VOID WINAPI insert(ULONGLONG pos, LONGLONG data);
    ULONGLONG WINAPI del(LONGLONG data);
    BOOL WINAPI delFirst(LONGLONG data);
    BOOL WINAPI delAt(ULONGLONG pos);
    LONGLONG WINAPI pos(LONGLONG data);
    BOOL WINAPI contains(LONGLONG data);
    LONGLONG WINAPI getMinValue();
    LONGLONG WINAPI getMaxValue();
    // Bubble-sort (default in ascending order)
    VOID WINAPI sort(BOOL asc = TRUE);
    VOID WINAPI insertRandomValues(ULONGLONG count, ULONGLONG minValue, ULONGLONG maxValue);
    // Makes all values are set randomed
    VOID WINAPI randomizeAll();
    VOID WINAPI randomizeAll(ULONGLONG minValue, ULONGLONG maxValue);
    // as methods: randomizeAll() and randomizeAll(ULONGLONG minValue, ULONGLONG maxValue) but first
    // makes this->_size = this->_capacity
    VOID WINAPI randomizeAllCapacity();
    VOID WINAPI randomizeAllCapacity(ULONGLONG minValue, ULONGLONG maxValue);
    // Finds min value that is greater than the this->getMinValue() and is not in the array
    LONGLONG WINAPI getFirstAvailableValue();
    inline LONGLONG WINAPI getAt(ULONGLONG pos) { if (pos < this->_size) return this->_items[pos]; return 0; };
    VOID WINAPI setAt(ULONGLONG pos, LONGLONG data);
    inline LONGLONG WINAPI getFirst() { if (this->_size > 0) return this->_items[0]; return 0; };
    inline LONGLONG WINAPI getLast() { if (this->_size > 0) return this->_items[this->_size - 1]; return 0; };
    inline LONGLONG WINAPI pop() {
        LONGLONG retval;
        if (this->_size < 1) { return 0; }
        retval = this->getAt(this->_size - 1);
        this->delAt(this->_size - 1);
        return retval;
    }
    inline ULONGLONG WINAPI size() { return this->_size; };
    inline ULONGLONG WINAPI capacity() { return this->_capacity; };
    virtual VOID WINAPI clear();
    inline LONGLONG* WINAPI getItemsBuf() { return this->_items; }

    CmzArrayList* WINAPI clone();
    virtual CmzString WINAPI toString();

protected:
    LONGLONG* _items;
    ULONGLONG _size;
    ULONGLONG _capacity;

    LONGLONG* WINAPI _createNullArray(ULONGLONG capacity);
};

}

#endif
