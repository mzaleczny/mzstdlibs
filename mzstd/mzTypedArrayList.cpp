#include "pch.h"
#include "mzTypedArrayList.h"

namespace mzstd {

CmzTypedArrayList::CmzTypedArrayList(ULONGLONG capacity) {
    this->setRealClassName(L"CmzTypedArrayList");
    this->_capacity = capacity;
    this->_size = 0;

    if (capacity > 0) {
        this->_items = this->_createNullArray(capacity);
    } else {
        this->_items = NULL;
    }
}

CmzTypedArrayList::~CmzTypedArrayList() {
    this->clear();
    
    if (this->_items) {
        delete[] this->_items;
    }
}

LPVOID* CmzTypedArrayList::_createNullArray(ULONGLONG capacity) {
    ULONGLONG bufsize = capacity * sizeof(LONGLONG);
    LPVOID* result = new LPVOID[bufsize];
    memset(result, 0, bufsize); 
    return result;
}

VOID CmzTypedArrayList::append(LPVOID data, CmzString type) {
    CmzTypedArrayListItem* item = new CmzTypedArrayListItem(type, data);
    
    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LPVOID *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LONGLONG) * this->_size);
            delete[] this->_items;
        }
        this->_items = newItems;
        this->_capacity = newCapacity;
    }
    this->_items[this->_size] = item;
    this->_size++;
}

VOID CmzTypedArrayList::addAsFirst(LPVOID data, CmzString type) {
    CmzTypedArrayListItem* item = new CmzTypedArrayListItem(type, data);

    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LPVOID *newItems = this->_createNullArray(newCapacity);
        if (this->_items) {
            memcpy((newItems + sizeof(LONGLONG)), this->_items, sizeof(LONGLONG) * this->_size);
            delete[] this->_items;
        }
        newItems[0] = item;
        this->_items = newItems;
        this->_capacity = newCapacity;
        this->_size++;
    } else {
        for (LONGLONG i = this->_size; i > 0; i--) {
            this->_items[i] = this->_items[i-1];
        }
        this->_items[0] = item;
        this->_size++;
    }
}

VOID CmzTypedArrayList::insert(ULONGLONG pos, LPVOID data, CmzString type) {
    if (pos < 0 || pos > this->_size) {
        return;
    }

    CmzTypedArrayListItem* item = new CmzTypedArrayListItem(type, data);

    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + 100;
        LPVOID *newItems = this->_createNullArray(newCapacity);
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
    this->_items[pos] = item;
    this->_size++;
}

ULONGLONG CmzTypedArrayList::del(LPVOID data, CmzString type) {
    ULONGLONG removed = 0;
    CmzTypedArrayListItem* item;

    for (ULONGLONG i = 0; i < this->_size; i++) {
        item = (CmzTypedArrayListItem*)this->_items[i];
        if (item->pointer == data && item->type == type) {
            for (ULONGLONG j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            delete item;
            this->_size--;
            removed++;
        }
    }

    return removed;
}

BOOL CmzTypedArrayList::delFirst(LPVOID data, CmzString type) {
    BOOL removed = FALSE;
    CmzTypedArrayListItem* item;

    for (ULONGLONG i = 0; i < this->_size; i++) {
        item = (CmzTypedArrayListItem*)this->_items[i];
        if (item->pointer == data && item->type == type) {
            for (ULONGLONG j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            delete item;
            this->_size--;
            removed = TRUE;
            break;
        }
    }

    return removed;
}

BOOL CmzTypedArrayList::delAt(ULONGLONG pos) {
    if (pos >= this->_size) {
        return FALSE;
    }

    CmzTypedArrayListItem* item = (CmzTypedArrayListItem*)this->_items[pos];
    delete item;
    for (ULONGLONG i = pos + 1; i < this->_size; i++) {
        this->_items[i - 1] = this->_items[i];
    }
    this->_size--;

    return TRUE;
}

LONGLONG CmzTypedArrayList::pos(LPVOID data, CmzString type) {
    CmzTypedArrayListItem* item;

    for (ULONGLONG i = 0; i < this->_size; i++) {
        item = (CmzTypedArrayListItem*)this->_items[i];
        if (item->pointer == data && item->type == type) {
            return i;
        }
    }

    return -1;
}

BOOL CmzTypedArrayList::contains(LPVOID data, CmzString type) {
    return this->pos(data, type) >= 0;
}

VOID CmzTypedArrayList::clear() {
    CmzTypedArrayListItem* item;
    LONGLONG len = this->_size;
    for (LONGLONG i = 0; i < len; i++) {
        item = (CmzTypedArrayListItem*)this->_items[i];
        if (item) {
            delete item;
        }
    }
    
    this->_size = 0;
}

CmzTypedArrayListItem* CmzTypedArrayList::getAt(ULONGLONG pos) {
    if (pos < this->_size) return (CmzTypedArrayListItem*)this->_items[pos];
    return NULL;
}

VOID CmzTypedArrayList::getAt(ULONGLONG pos, LPVOID& data, CmzString& type) {
    CmzTypedArrayListItem* item;
    if (pos < this->_size) {
        item = (CmzTypedArrayListItem*)this->_items[pos];
        data = item->pointer;
        type = item->type;
    } else {
        data = 0;
        type = COMMON_EMPTY_STRING;
    }
}

LPVOID CmzTypedArrayList::getDataAt(ULONGLONG pos) {
    CmzTypedArrayListItem* item;
    if (pos < this->_size) {
        item = (CmzTypedArrayListItem*)this->_items[pos];
        return item->pointer;
    }
    
    return 0;
}

CmzString CmzTypedArrayList::getTypeAt(ULONGLONG pos) {
    CmzTypedArrayListItem* item;
    if (pos < this->_size) {
        item = (CmzTypedArrayListItem*)this->_items[pos];
        return item->type;
    }
    
    return COMMON_EMPTY_STRING;
}

CmzTypedArrayListItem* CmzTypedArrayList::getFirst() {
    if (this->_size > 0) return (CmzTypedArrayListItem*)this->_items[0]; return NULL;
}

VOID CmzTypedArrayList::getFirst(LPVOID& data, CmzString& type) {
    CmzTypedArrayListItem* item;
    if (this->_size > 0) {
        item = (CmzTypedArrayListItem*)this->_items[0];
        data = item->pointer;
        type = item->type;
    } else {
        data = 0;
        type = COMMON_EMPTY_STRING;
    }
}

LPVOID CmzTypedArrayList::getDataAtFirst() {
    CmzTypedArrayListItem* item;
    if (this->_size > 0) {
        item = (CmzTypedArrayListItem*)this->_items[0];
        return item->pointer;
    }
    
    return 0;
}

CmzString CmzTypedArrayList::getTypeAtFirst() {
    CmzTypedArrayListItem* item;
    if (this->_size > 0) {
        item = (CmzTypedArrayListItem*)this->_items[0];
        return item->type;
    }
    
    return COMMON_EMPTY_STRING;
}

CmzTypedArrayListItem* CmzTypedArrayList::getLast() {
    if (this->_size > 0) return (CmzTypedArrayListItem*)this->_items[this->_size-1]; return NULL;
}

VOID CmzTypedArrayList::getLast(LPVOID& data, CmzString& type) {
    CmzTypedArrayListItem* item;
    if (this->_size > 0) {
        item = (CmzTypedArrayListItem*)this->_items[this->_size-1];
        data = item->pointer;
        type = item->type;
    } else {
        data = 0;
        type = COMMON_EMPTY_STRING;
    }
}

LPVOID CmzTypedArrayList::getDataAtLast() {
    CmzTypedArrayListItem* item;
    if (this->_size > 0) {
        item = (CmzTypedArrayListItem*)this->_items[this->_size-1];
        return item->pointer;
    }
    
    return 0;
}

CmzString CmzTypedArrayList::getTypeAtLast() {
    CmzTypedArrayListItem* item;
    if (this->_size > 0) {
        item = (CmzTypedArrayListItem*)this->_items[this->_size-1];
        return item->type;
    }
    
    return COMMON_EMPTY_STRING;
}

CmzTypedArrayListItem* CmzTypedArrayList::pop() {
    CmzTypedArrayListItem* item;
    
    if (this->_size < 1) {
        return NULL;
    }
    
    item = (CmzTypedArrayListItem*)this->_items[this->_size - 1];
    this->_items[this->_size - 1] = NULL;
    this->_size--;
    
    return item;
}

VOID CmzTypedArrayList::pop(LPVOID& data) {
    CmzTypedArrayListItem* item;
    
    if (this->_size < 1) {
        data = 0;
        return;
    }
    
    item = (CmzTypedArrayListItem*)this->_items[this->_size - 1];
    this->_items[this->_size - 1] = NULL;
    this->_size--;
    
    data = item->pointer;
    delete item;
}

VOID CmzTypedArrayList::pop(CmzString& type) {
    CmzTypedArrayListItem* item;
    
    if (this->_size < 1) {
        type = COMMON_EMPTY_STRING;
        return;
    }
    
    item = (CmzTypedArrayListItem*)this->_items[this->_size - 1];
    this->_items[this->_size - 1] = NULL;
    this->_size--;
    
    type = item->type;
    delete item;
}

VOID CmzTypedArrayList::pop(LPVOID& data, CmzString& type) {
    CmzTypedArrayListItem* item;
    
    if (this->_size < 1) {
        data = 0;
        type = COMMON_EMPTY_STRING;
        return;
    }
    
    item = (CmzTypedArrayListItem*)this->_items[this->_size - 1];
    this->_items[this->_size - 1] = NULL;
    this->_size--;
    
    data = item->pointer;
    type = item->type;
    delete item;
}

}
