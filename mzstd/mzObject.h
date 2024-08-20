#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "mzstdCommon.h"
#include "mzString.h"

namespace mzstd {

class CmzObject;
DECLSPEC_MZSTD VOID WINAPI swap(CmzObject** val1, CmzObject** val2);

class DECLSPEC_MZSTD CmzObject {
protected:
    // WARNING!
    // this two protcted attributes must be first in class structure, to allow proper CObject* class
    // recognition.
    wchar_t* _originClassName;
    wchar_t* _realClassName;

public:    
    // konstruktor domyślny
    WINAPI CmzObject();
    // konstruktor kopiujący
    WINAPI CmzObject(const CmzObject& object);
    virtual WINAPI ~CmzObject();
    void WINAPI setRealClassName(const wchar_t* realClassName);
    wchar_t* WINAPI getOriginClassName() { return this->_originClassName; }
    wchar_t* WINAPI getRealClassName() { return this->_realClassName; }

    virtual BOOL WINAPI equals(CmzObject* obj);
	virtual CmzString WINAPI toString();
    virtual inline bool WINAPI isString() { return false; }

    virtual WINAPI operator wchar_t*();

private:
    LONGLONG _objectId;
    
    void WINAPI _commonInit();
};

}

#endif
