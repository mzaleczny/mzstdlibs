#include "pch.h"
#include "mzStringList.h"
#include "mzFileUtils.h"
#include "mzLogger.h"

namespace mzstd {

CmzStringList::CmzStringList(ULONGLONG capacity) {
    this->setRealClassName(L"CmzStringList");
	this->_growBy = 100;
	this->_emptyStr = "";
    this->_capacity = capacity;
    this->_size = 0;

    if (capacity > 0) {
        this->_items = this->createNullArray(capacity);
    } else {
        this->_items = NULL;
    }
}

CmzStringList::~CmzStringList() {
	CmzString *s;
    if (this->_items) {
		for (ULONGLONG i = 0; i < this->_size; i++) {
			s = (CmzString*)this->_items[i];
			delete s;
		}
        delete[] this->_items;
    }
}

LPVOID* CmzStringList::createNullArray(ULONGLONG capacity) {
    LPVOID* result = new LPVOID[capacity];
    return result;
}

VOID CmzStringList::append(CmzString data) {
    if (this->_size == this->_capacity) {
		ULONGLONG newCapacity = this->_capacity + this->_growBy;
		this->_growBy *= 2;
        LPVOID *newItems = this->createNullArray(newCapacity);
        if (this->_items) {
            memcpy(newItems, this->_items, sizeof(LPVOID) * this->_size);
            delete[] this->_items;
        }
        this->_items = newItems;
        this->_capacity = newCapacity;
    }
    this->_items[this->_size] = (LPVOID)(new CmzString(data));
    this->_size++;
}

VOID WINAPI CmzStringList::append(CmzStringList *lst) {
    if (lst && lst->size() > 0) {
        for (ULONGLONG i = 0; i < lst->size(); i++) {
            this->append(lst->getAt(i));
        }
    }
}

VOID CmzStringList::setAt(ULONGLONG pos, CmzString data) {
    if (pos >= this->_size) {
        this->append(data);
    } else {
        CmzString* s = (CmzString*)this->_items[pos];
        if (s) {
            delete s;
        }
        this->_items[pos] = (LPVOID)(new CmzString(data));
    }
}

VOID CmzStringList::addAsFirst(CmzString data) {
    if (this->_size == this->_capacity) {
		ULONGLONG newCapacity = this->_capacity + this->_growBy;
		this->_growBy *= 2;
        LPVOID *newItems = this->createNullArray(newCapacity);
        if (this->_items) {
            memcpy((newItems + sizeof(LPVOID)), this->_items, sizeof(LPVOID) * this->_size);
            delete[] this->_items;
        }
        newItems[0] = (LPVOID)(new CmzString(data));
        this->_items = newItems;
        this->_capacity = newCapacity;
        this->_size++;
    } else {
        for (LONGLONG i = this->_size; i > 0; i--) {
            this->_items[i] = this->_items[i-1];
        }
        this->_items[0] = (LPVOID)(new CmzString(data));
        this->_size++;
    }
}

VOID CmzStringList::addAsFirst(WCHAR* data) {
    CmzString s(data);
    this->addAsFirst(s);
}

VOID CmzStringList::insert(ULONGLONG pos, CmzString data) {
    if (pos < 0 || pos > this->_size) {
        return;
    }

    if (this->_size == this->_capacity) {
        ULONGLONG newCapacity = this->_capacity + this->_growBy;
		this->_growBy *= 2;
        LPVOID *newItems = this->createNullArray(newCapacity);
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
    this->_items[pos] = (LPVOID)(new CmzString(data));
    this->_size++;
}

ULONGLONG CmzStringList::del(CmzString data) {
    ULONGLONG removed = 0;
	CmzString *s;

    for (ULONGLONG i = 0; i < this->_size; i++) {
		s = (CmzString*)this->_items[i];
        if (*s == data) {
            for (ULONGLONG j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            delete s;
            this->_size--;
            removed++;
        }
    }

    return removed;
}

BOOL CmzStringList::delFirst(CmzString data) {
    BOOL removed = FALSE;
	CmzString *s;

    for (ULONGLONG i = 0; i < this->_size; i++) {
		s = (CmzString*)this->_items[i];
        if (*s == data) {
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

BOOL CmzStringList::delAt(ULONGLONG pos) {
    if (pos >= this->_size) {
        return FALSE;
    }

    CmzString* s = (CmzString*)this->_items[pos];
    delete s;
    
    for (ULONGLONG i = pos + 1; i < this->_size; i++) {
        this->_items[i - 1] = this->_items[i];
    }
    this->_size--;

    return TRUE;
}

LONGLONG CmzStringList::pos(CmzString data) {
	CmzString *s;

    for (ULONGLONG i = 0; i < this->_size; i++) {
		s = (CmzString*)this->_items[i];
        if (*s == data) {
            return i;
        }
    }

    return -1;
}

BOOL CmzStringList::contains(CmzString data) {
    return this->pos(data) >= 0;
}

VOID CmzStringList::clear() {
	CmzString *s;
    if (this->_items) {
        ULONGLONG i;
		for (i = 0; i < this->_size; i++) {
			s = (CmzString*)this->_items[i];
			delete s;
		}
        this->_items[i] = NULL;
    }
    this->_size = 0;
}

VOID CmzStringList::writeToFile(CmzString fname) {
	WriteFile(fname, this, ENC_UTF8);
}

VOID CmzStringList::readFromFile(CmzString fname) {
}

VOID CmzStringList::joinIntoStringArray(CmzString* result, WCHAR glue) {
    if (result) {
        *result = COMMON_EMPTY_STRING;
        WCHAR* item = NULL;
        for (ULONGLONG i = 0; i < this->_size; i++) {
            if (this->_items[i]) {
                item = ((CmzString*)this->_items[i])->toWCHAR();
                (*result).appendAfterEos(item, glue);
            }
        }
    }
}

CmzString CmzStringList::join(const WCHAR* glue) {
    CmzString s;
    for (ULONGLONG i = 0; i < this->_size; i++) {
        s.append(((CmzString*)this->_items[i])->getBuffer());
        if (i != this->_size - 1) {
            s.append(glue);
        }
    }
    
    return s;
}

CmzString CmzStringList::toString() {
    return this->join(L"\n");
}

CmzStringList::operator WCHAR*() {
    static CmzString s;
    s = this->toString();
    return s.getBuffer();
}

}