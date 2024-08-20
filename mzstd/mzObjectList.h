#ifndef __MZSTD_OBJECT_LIST_H__
#define __MZSTD_OBJECT_LIST_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzPointerList.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzObjectList : public CmzPointerList {
public:
    WINAPI CmzObjectList(ULONGLONG capacity = 50, BOOL deleteObjects = TRUE);
    virtual WINAPI ~CmzObjectList();

    VOID WINAPI append(CmzObject* data);
    VOID WINAPI append(CmzObjectList* lst);
    inline VOID WINAPI push(CmzObject* data) { this->append(data); };
    VOID WINAPI addAsFirst(CmzObject* data);
    VOID WINAPI insert(ULONGLONG pos, CmzObject* data);
    // Removes objects from array and deletes it.
    ULONGLONG WINAPI del(CmzObject* data);
    // Removes first object from array and deletes it.
    BOOL WINAPI delFirst(CmzObject* data);
    // Removes object from array and deletes it.
    BOOL WINAPI delAt(ULONGLONG pos);
    // Removes objects from array.
    ULONGLONG WINAPI remove(CmzObject* data);
    // Removes first object from array. Returns the object.
    BOOL WINAPI removeFirst(CmzObject* data);
    // Removes object from array. Returns the object.
    CmzObject* WINAPI removeAt(ULONGLONG pos);
    LONGLONG WINAPI pos(CmzObject* data);
    BOOL WINAPI contains(CmzObject* data);
    inline CmzObject* WINAPI getAt(ULONGLONG pos) { if (pos < this->_size) return (CmzObject*)this->_items[pos]; return NULL; };
    VOID WINAPI setAt(ULONGLONG pos, CmzObject* data);
	inline CmzObject* WINAPI getLast() { if (this->_size > 0) return (CmzObject*)this->_items[this->_size - 1]; return NULL; };
    CmzObject* WINAPI pop();
    // removes all object in the list and delete them
    virtual VOID WINAPI clear();
    // removes all object in the list but do not delete them
    virtual VOID WINAPI empty();

    virtual CmzString WINAPI toString();
    
protected:
    // Sometimes we hold the same objects (pointers to them) in a few arrays and do not want release them
    // when the array is deleted.
    BOOL _deleteObjectsInDestructor;
};

}

#endif
