#include "pch.h"
#include "mzObjectList.h"
#include "mzFileUtils.h"

namespace mzstd {

CmzObjectList::CmzObjectList(ULONGLONG capacity, BOOL deleteObjects)
    : CmzPointerList(capacity)
{
    this->setRealClassName(L"CmzObjectList");
    this->_deleteObjectsInDestructor = deleteObjects;
}

CmzObjectList::~CmzObjectList() {
    if (this->_items) {
        if (this->_deleteObjectsInDestructor) {
            this->clear();
        } else {
            this->empty();
        }
    }
}

VOID CmzObjectList::append(CmzObject* data) {
    __super::append(data);
}

VOID WINAPI CmzObjectList::append(CmzObjectList *lst) {
    if (lst && lst->size() > 0) {
        for (ULONGLONG i = 0; i < lst->size(); i++) {
            this->append(lst->getAt(i));
        }
    }
}

VOID CmzObjectList::setAt(ULONGLONG pos, CmzObject* data) {
    if (pos >= this->_size) {
        this->append(data);
    } else {
        CmzObject* item = (CmzObject*)this->_items[pos];
        if (item) {
            delete item;
        }
        this->_items[pos] = data;
    }
}

VOID CmzObjectList::addAsFirst(CmzObject* data) {
    __super::addAsFirst(data);
}

VOID CmzObjectList::insert(ULONGLONG pos, CmzObject* data) {
    __super::insert(pos, data);
}

ULONGLONG CmzObjectList::del(CmzObject* data) {
    ULONGLONG removed = 0;
	CmzObject* item;

    for (size_t i = 0; i < this->_size; i++) {
		item = (CmzObject*)this->_items[i];
        if ((LPVOID)item == (LPVOID)data) {
            for (size_t j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            delete item;
            this->_size--;
            removed++;
        }
    }

    return removed;
}

BOOL CmzObjectList::delFirst(CmzObject* data) {
    if (this->removeFirst(data)) {
        delete data;
        return TRUE;
    }
    return FALSE;
}

BOOL CmzObjectList::delAt(ULONGLONG pos) {
    CmzObject* item = this->removeAt(pos);
    if (item) {
        delete item;
        return TRUE;
    }
    return FALSE;
}

ULONGLONG CmzObjectList::remove(CmzObject* data) {
    ULONGLONG removed = 0;
	CmzObject* item;

    for (size_t i = 0; i < this->_size; i++) {
		item = (CmzObject*)this->_items[i];
        if ((LPVOID)item == (LPVOID)data) {
            for (size_t j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            this->_size--;
            removed++;
        }
    }

    return removed;
}

BOOL CmzObjectList::removeFirst(CmzObject* data) {
	CmzObject* item;
    for (size_t i = 0; i < this->_size; i++) {
		item = (CmzObject*)this->_items[i];
        if ((LPVOID)item == (LPVOID)data) {
            for (size_t j = i + 1; j < this->_size; j++) {
                this->_items[j - 1] = this->_items[j];
            }
            this->_size--;
            return TRUE;
        }
    }
    return FALSE;
}

CmzObject* CmzObjectList::removeAt(ULONGLONG pos) {
    if (pos >= this->_size) {
        return FALSE;
    }
    CmzObject* item = (CmzObject*)this->_items[pos];    
    for (ULONGLONG i = pos + 1; i < this->_size; i++) {
        this->_items[i - 1] = this->_items[i];
    }
    this->_size--;
    return item;
}

LONGLONG CmzObjectList::pos(CmzObject* data) {
    return __super::pos(data);
}

BOOL CmzObjectList::contains(CmzObject* data) {
    return this->pos(data) >= 0;
}

VOID CmzObjectList::clear() {
    if (this->_items) {
        CmzObject *item;
		for (ULONGLONG i = 0; i < this->_size; i++) {
			item = (CmzObject*)this->_items[i];
			delete item;
		}
        ZeroMemory(this->_items, sizeof(LONGLONG) * this->_size);
    }
    this->_size = 0;
}

VOID CmzObjectList::empty() {
    if (this->_items) {
        ZeroMemory(this->_items, sizeof(LONGLONG) * this->_size);
    }
    this->_size = 0;
}

CmzString CmzObjectList::toString() {
    CmzString s;
    for (ULONGLONG i = 0; i < this->_size; i++) {
        s += ((CmzObject*)this->_items[i])->toString();
        s.append(L"\n");
    }    
    return s;
}

CmzObject* WINAPI CmzObjectList::pop() {
    CmzObject* retval;
    if (this->_size < 1) {
        return NULL;
    }
    retval = (CmzObject*)this->_items[this->_size - 1];
    this->_items[this->_size - 1] = NULL;
    this->_size--;
    return retval;
}

}
