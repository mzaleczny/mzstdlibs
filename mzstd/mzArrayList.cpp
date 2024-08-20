#include "pch.h"
#include "mzArrayList.h"
#include "mzMath.h"
#include <stdio.h>

mzstd::CmzArrayList::CmzArrayList(ULONGLONG capacity) {
    this->setRealClassName(L"mzstd::CmzArrayList");
    this->_capacity = capacity;
    this->_size = 0;

    if (capacity > 0) {
        this->_items = this->_createNullArray(capacity);
        this->_size = 0;
    } else {
        this->_items = NULL;
    }
}

mzstd::CmzArrayList::~CmzArrayList() {
    if (this->_items) {
        delete[] this->_items;
        this->_items = NULL;
    }
}

LONGLONG* mzstd::CmzArrayList::_createNullArray(ULONGLONG capacity) {
    ULONGLONG bufsize = capacity * sizeof(LONGLONG);
    LONGLONG* result = new LONGLONG[bufsize];
    memset(result, 0, bufsize);
    return result;
}

VOID mzstd::CmzArrayList::append(LONGLONG data) {
    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LONGLONG *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LONGLONG) * this->_size);
            delete[] this->_items;
        }
        this->_items = newItems;
        this->_capacity = newCapacity;
    }
    this->_items[this->_size] = data;
    this->_size++;
}

VOID mzstd::CmzArrayList::addAsFirst(LONGLONG data) {
    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LONGLONG *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy((newItems + sizeof(LONGLONG)), this->_items, sizeof(LONGLONG) * this->_size);
            delete[] this->_items;
        }
        newItems[0] = data;
        this->_items = newItems;
        this->_capacity = newCapacity;
        this->_size++;
    } else {
        for (LONGLONG i = this->_size; i > 0; i--) {
            this->_items[i] = this->_items[i-1];
        }
        this->_items[0] = data;
        this->_size++;
    }
}

VOID mzstd::CmzArrayList::insert(ULONGLONG pos, LONGLONG data) {
    if (pos < 0 || pos > this->_size) {
        return;
    }

    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LONGLONG *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LONGLONG) * this->_size);
            delete[] this->_items;
        }
        this->_items = newItems;
        this->_capacity = newCapacity;
    }

    for (LONGLONG i = (LONGLONG)this->_size; i > (LONGLONG)pos; i--) {
        this->_items[i] = this->_items[i-1];
    }
    this->_items[pos] = data;
    this->_size++;
}

ULONGLONG mzstd::CmzArrayList::del(LONGLONG data) {
    ULONGLONG removed = 0;

    for (ULONGLONG i = 0; i < this->_size; i++) {
        if (this->_items[i] == data) {
            for (ULONGLONG j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            this->_size--;
            removed++;
        }
    }

    return removed;
}

BOOL mzstd::CmzArrayList::delFirst(LONGLONG data) {
    BOOL removed = FALSE;

    for (ULONGLONG i = 0; i < this->_size; i++) {
        if (this->_items[i] == data) {
            for (ULONGLONG j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            this->_size--;
            removed = TRUE;
            break;
        }
    }

    return removed;
}

BOOL mzstd::CmzArrayList::delAt(ULONGLONG pos) {
    if (pos >= this->_size) {
        return FALSE;
    }

    for (ULONGLONG i = pos + 1; i < this->_size; i++) {
        this->_items[i - 1] = this->_items[i];
    }
    this->_size--;

    return TRUE;
}

LONGLONG mzstd::CmzArrayList::pos(LONGLONG data) {
    for (ULONGLONG i = 0; i < this->_size; i++) {
        if (this->_items[i] == data) {
            return i;
        }
    }

    return -1;
}

BOOL mzstd::CmzArrayList::contains(LONGLONG data) {
    return this->pos(data) >= 0;
}

VOID mzstd::CmzArrayList::clear() {
    this->_size = 0;
}

LONGLONG mzstd::CmzArrayList::getMinValue() {
    LONGLONG min = (LONGLONG)0xffffffff; // MIN_LONGLONG
    LONGLONG size = this->_size;

    for (LONGLONG i = 0; i < size; i++) {
        if (min > this->_items[i]) {
            min = this->_items[i];
        }
    }

    return min;
}

LONGLONG mzstd::CmzArrayList::getMaxValue() {
    LONGLONG max = (LONGLONG)0x7fffffff; // MAX_LONGLONG
    LONGLONG size = this->_size;

    for (LONGLONG i = 0; i < size; i++) {
        if (max < this->_items[i]) {
            max = this->_items[i];
        }
    }

    return max;
}

LONGLONG mzstd::CmzArrayList::getFirstAvailableValue() {
    if (this->_size < 1) {
        return 1;
    }
    if (this->_size == 1) {
        return this->_items[0] + 1;
    }

    mzstd::CmzArrayList* lst = this->clone();
    lst->sort();

    for (ULONGLONG i = 1; i < lst->_size; i++) {
        if (lst->_items[i] - lst->_items[i-1] > 1) {
            return lst->_items[i-1] + 1;
        }
    }

    LONGLONG retval = lst->_items[lst->_size-1] + 1;
    delete lst;
    return retval;
}

VOID mzstd::CmzArrayList::sort(BOOL asc) {
    LONGLONG size = this->_size;
    LONGLONG tmp;

    if (asc) {
        for (LONGLONG i = 0; i < size - 1; i++) {
            for (LONGLONG j = i + 1; j < size; j++) {
                if (this->_items[i] > this->_items[j]) {
                    tmp = this->_items[j];
                    this->_items[j] = this->_items[i];
                    this->_items[i] = tmp;
                }
            }
        }
    } else {
        for (LONGLONG i = 0; i < size - 1; i++) {
            for (LONGLONG j = i + 1; j < size; j++) {
                if (this->_items[i] < this->_items[j]) {
                    tmp = this->_items[j];
                    this->_items[j] = this->_items[i];
                    this->_items[i] = tmp;
                }
            }
        }
    }
}

VOID mzstd::CmzArrayList::insertRandomValues(ULONGLONG count, ULONGLONG minValue, ULONGLONG maxValue) {
    if (this->_size + count > this->_capacity) {
        ULONGLONG newCapacity = this->_size + count;
        LONGLONG *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LONGLONG) * this->_size);
            delete[] this->_items;
        }
        this->_items = newItems;
        this->_capacity = newCapacity;
    }

    for (ULONG i = 0; i < count; i++) {
        this->_items[i + this->_size] = random(minValue, maxValue);
    }
}

VOID mzstd::CmzArrayList::randomizeAllCapacity() {
    this->_size = this->_capacity;
    this->randomizeAll();
}

VOID mzstd::CmzArrayList::randomizeAllCapacity(ULONGLONG minValue, ULONGLONG maxValue) {
    this->_size = this->_capacity;
    this->randomizeAll(minValue, maxValue);
}

VOID mzstd::CmzArrayList::randomizeAll() {
    for (ULONGLONG i = 0; i < this->_size; i++) {
        this->_items[i] = random();
    }
}

VOID mzstd::CmzArrayList::randomizeAll(ULONGLONG minValue, ULONGLONG maxValue) {
    for (ULONGLONG i = 0; i < this->_size; i++) {
        this->_items[i] = random(minValue, maxValue);
    }
}

mzstd::CmzArrayList* mzstd::CmzArrayList::clone() {
    mzstd::CmzArrayList* newList = new mzstd::CmzArrayList(this->_capacity);
    memcpy(newList->_items, this->_items, sizeof(LONGLONG) * this->_capacity);
    newList->_size = this->_size;
    return newList;
}

mzstd::CmzString mzstd::CmzArrayList::toString() {
    CmzString s(COMMON_EMPTY_STRING);

    for (ULONGLONG i = 0; i < this->_size; i++) {
        if (i > 0) {
            s += L", ";
        }
        s += this->_items[i];
    }

    return s;
}


VOID mzstd::CmzArrayList::setAt(ULONGLONG pos, LONGLONG data) {
    if (pos >= this->_size) {
        this->append(data);
    } else {
        this->_items[pos] = data;
    }
}
