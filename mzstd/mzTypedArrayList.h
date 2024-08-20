#ifndef __MZSTD_TYPED_ARRAY_LIST_H__
#define __MZSTD_TYPED_ARRAY_LIST_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzTypedArrayListItem {
public:
    CmzString type;
    LPVOID pointer;

    WINAPI CmzTypedArrayListItem(CmzString type, LPVOID pointer) {
        this->type = type;
        this->pointer = pointer;
    }
    virtual WINAPI ~CmzTypedArrayListItem() {}
};


class DECLSPEC_MZSTD CmzTypedArrayList : public CmzObject {
public:
    WINAPI CmzTypedArrayList(ULONGLONG capacity = 100);
    virtual WINAPI ~CmzTypedArrayList();

    VOID WINAPI append(LPVOID data, CmzString type);
    inline VOID WINAPI push(LPVOID data, CmzString type) { this->append(data, type); };
    VOID WINAPI addAsFirst(LPVOID data, CmzString type);
    VOID WINAPI insert(ULONGLONG pos, LPVOID data, CmzString type);
    ULONGLONG WINAPI del(LPVOID data, CmzString type);
    BOOL WINAPI delFirst(LPVOID data, CmzString type);
    BOOL WINAPI delAt(ULONGLONG pos);
    LONGLONG WINAPI pos(LPVOID data, CmzString type);
    BOOL WINAPI contains(LPVOID data, CmzString type);
    CmzTypedArrayListItem* WINAPI getAt(ULONGLONG pos);
    VOID WINAPI getAt(ULONGLONG pos, LPVOID& data, CmzString& type);
    LPVOID WINAPI getDataAt(ULONGLONG pos);
    CmzString WINAPI getTypeAt(ULONGLONG pos);
    CmzTypedArrayListItem* WINAPI getFirst();
    VOID WINAPI getFirst(LPVOID& data, CmzString& type);
    LPVOID WINAPI getDataAtFirst();
    CmzString WINAPI getTypeAtFirst();
    CmzTypedArrayListItem* WINAPI getLast();
    VOID WINAPI getLast(LPVOID& data, CmzString& type);
    LPVOID WINAPI getDataAtLast();
    CmzString WINAPI getTypeAtLast();
    CmzTypedArrayListItem* WINAPI pop();
    VOID WINAPI pop(LPVOID& data);
    VOID WINAPI pop(CmzString& type);
    VOID WINAPI pop(LPVOID& data, CmzString& type);
    inline ULONGLONG WINAPI size() { return this->_size; };
    VOID WINAPI clear();
    inline LPVOID* WINAPI getItemsBuf() { return this->_items; }
private:
    LPVOID* _items;
    ULONGLONG _size;
    ULONGLONG _capacity;

    LPVOID* WINAPI _createNullArray(ULONGLONG capacity);
};

}

#endif
