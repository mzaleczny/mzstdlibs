#include "pch.h"
#include "mzObject.h"
using namespace std;


namespace mzstd {

VOID WINAPI swap(CmzObject** object1, CmzObject** object2) {
    CmzObject* tmp = *object1;
    *object1 = *object2;
    *object2 = tmp;
}




CmzObject::CmzObject() {
    this->_commonInit();
}

CmzObject::CmzObject(const CmzObject& object) {
    this->_commonInit();
    this->setRealClassName(object._realClassName);
}

VOID WINAPI CmzObject::_commonInit() {
    static LONGLONG id = 0;
    
    id += 1;
    
    size_t bufsize = wcslen(::cmzObjectClassName) + 1;
    this->_originClassName = new WCHAR[bufsize];
    if (this->_originClassName) {
        wcscpy(this->_originClassName, ::cmzObjectClassName);
    }
    
    this->_realClassName = new WCHAR[64];
    this->setRealClassName(::cmzObjectClassName);

    this->_objectId = id;
}


CmzObject::~CmzObject() {
    if (this->_originClassName) {
        delete[] this->_originClassName;
    }
    if (this->_realClassName) {
        delete[] this->_realClassName;
    }
}


BOOL CmzObject::equals(CmzObject* obj) {
    return this == obj;
}

CmzString CmzObject::toString() {
    return CmzString("CmzObject [id: ") + this->_objectId + "]; originClassName: " + this->_originClassName + L"; realClassName: " + this->_realClassName;
}

CmzObject::operator WCHAR*() {
    return NULL;
}

VOID CmzObject::setRealClassName(const wchar_t* realClassName) {
    if (this->_realClassName) {
        wcsncpy(this->_realClassName, realClassName, 64);
    }
}

}
