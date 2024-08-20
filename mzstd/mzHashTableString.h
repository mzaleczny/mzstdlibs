#ifndef __HASHTABLE_STRING_H__
#define __HASHTABLE_STRING_H__

#include "mzString.h"
#include "mzObject.h"

namespace mzstd {

class CmzHashTableString : public CmzObject {
public:
    CmzHashTableString(UINT capacity = 1000);
    CmzHashTableString(const CmzHashTableString& obj);
    virtual ~CmzHashTableString();
    
    void put(CmzString key, CmzString value);
    CmzString get(CmzString key);
    void remove(CmzString key);
    void clear();
    virtual CmzString WINAPI toString();
    inline UINT size() { return this->_size; }
    inline UINT capacity() { return this->_capacity; }
    
    void print();
    void println();
    
    CmzString& operator [] (CmzString& indexKey);
    
protected:
    UINT       _capacity;
    UINT       _size;
    CmzString*    _keys;
    CmzString*    _values;
    
    INT _computeIndex(CmzString& key);
};

}

#endif
