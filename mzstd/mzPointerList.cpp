#include "pch.h"
#include "mzPointerList.h"
#include "mzstd.h"
#include "mzMath.h"
#include <stdio.h>

namespace mzstd {

CmzPointerList::CmzPointerList(ULONGLONG capacity) {
    this->setRealClassName(L"CmzPointerList");
    this->_capacity = capacity;
    this->_size = 0;

    if (capacity > 0) {
        this->_items = this->_createNullArray(capacity);
        this->_size = 0;
    } else {
        this->_items = NULL;
    }
}

CmzPointerList::~CmzPointerList() {
    if (this->_items) {
        delete[] this->_items;
        this->_items = NULL;
    }
}

LPVOID* CmzPointerList::_createNullArray(ULONGLONG capacity) {
    ULONGLONG bufsize = capacity * sizeof(LPVOID);
    LPVOID* result = new LPVOID[bufsize];
    memset(result, 0, bufsize);
    return result;
}

VOID CmzPointerList::append(LPVOID data) {
    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LPVOID *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LPVOID) * this->_size);
            delete[] this->_items;
        }
        this->_items = newItems;
        this->_capacity = newCapacity;
    }
    this->_items[this->_size] = data;
    this->_size++;
}

VOID CmzPointerList::addAsFirst(LPVOID data) {
    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LPVOID *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy((newItems + sizeof(LPVOID)), this->_items, sizeof(LPVOID) * this->_size);
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

VOID CmzPointerList::insert(ULONGLONG pos, LPVOID data) {
    if (pos < 0 || pos > this->_size) {
        return;
    }

    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LPVOID *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LPVOID) * this->_size);
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

ULONGLONG CmzPointerList::del(LPVOID data) {
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

BOOL CmzPointerList::delFirst(LPVOID data) {
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

BOOL CmzPointerList::delAt(ULONGLONG pos) {
    if (pos >= this->_size) {
        return FALSE;
    }

    for (ULONGLONG i = pos + 1; i < this->_size; i++) {
        this->_items[i - 1] = this->_items[i];
    }
    this->_size--;

    return TRUE;
}

LONGLONG CmzPointerList::pos(LPVOID data) {
    for (ULONGLONG i = 0; i < this->_size; i++) {
        if (this->_items[i] == data) {
            return i;
        }
    }

    return -1;
}

BOOL CmzPointerList::contains(LPVOID data) {
    return this->pos(data) >= 0;
}

VOID CmzPointerList::clear() {
    this->_size = 0;
}

LPVOID CmzPointerList::getMinValue() {
    LPVOID min;
    size_t size = this->_size;

	if (size > 0) {
		min = this->_items[0];
		for (size_t i = 1; i < size; i++) {
			if (min > this->_items[i]) {
				min = this->_items[i];
			}
		}
		return min;
	}

    return NULL;
}

LPVOID CmzPointerList::getMaxValue() {
    LPVOID max = (LPVOID)0x7fffffff; // MAX_LONGLONG
    ULONGLONG size = this->_size;

    for (ULONGLONG i = 0; i < size; i++) {
        if (max < this->_items[i]) {
            max = this->_items[i];
        }
    }

    return max;
}

VOID CmzPointerList::sort(BOOL asc) {
    LONGLONG size = this->_size;
    LPVOID tmp;

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

CmzPointerList* CmzPointerList::clone() {
    CmzPointerList* newList = new CmzPointerList(this->_capacity);
    memcpy(newList->_items, this->_items, sizeof(LPVOID) * this->_capacity);
    newList->_size = this->_size;
    return newList;
}

CmzString CmzPointerList::toString() {
    CmzString s(COMMON_EMPTY_STRING);

    for (ULONGLONG i = 0; i < this->_size; i++) {
        if (i > 0) {
            s += L", ";
        }
        s += FormatMessage(L"%p", this->_items[i]);
    }

    return s;
}


VOID CmzPointerList::setAt(ULONGLONG pos, LPVOID data) {
    if (pos >= this->_size) {
        this->append(data);
    } else {
        this->_items[pos] = data;
    }
}

}
