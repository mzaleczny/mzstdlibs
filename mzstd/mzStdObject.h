#ifndef __MZSTD_STD_OBJECT_H__
#define __MZSTD_STD_OBJECT_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzPointerList.h"

namespace mzstd {

class CmzStdObject;

#define STDOBJ_DEFAULT_INDENT			L"    "
#define STDOBJ_PROPERTY_NONE			0
#define STDOBJ_PROPERTY_VALUE			1
#define STDOBJ_PROPERTY_VALUE_INT		2
#define STDOBJ_PROPERTY_VALUE_DOUBLE	3
#define STDOBJ_PROPERTY_ARRAY			4
#define STDOBJ_PROPERTY_OBJECT			5

class DECLSPEC_MZSTD CmzStdObjectProperty : public CmzObject {
public:

    // dostępne typy:
    //   'scalar' - pole value przechowuje wartość (typu CmzString) dla tej właściwości
    //   'scalar_int' - pole value przechowuje wartość (typu LONGLONG) dla tej właściwości
    //   'scalar_double' - pole value przechowuje wartość (typu double) dla tej właściwości
    //   'array'  - pole aValue zawiera tablicę obiektów CmzStdObjectProperty
    //   'object' - pole oValue zawiera wskaźnik na obiekt CmzStdObject
    LONGLONG type;
    CmzString name;
    CmzString value;
	LONGLONG iValue;
	double fValue;
    CmzPointerList *aValue;
    CmzStdObject *oValue;
	CmzStdObject *extraData;

    WINAPI CmzStdObjectProperty(CmzString name, CmzString value);
    WINAPI CmzStdObjectProperty(CmzString name, LONGLONG value);
    WINAPI CmzStdObjectProperty(CmzString name, double value);
    WINAPI CmzStdObjectProperty(CmzString name, CmzPointerList *value_array);
    WINAPI CmzStdObjectProperty(CmzString name, CmzStdObject *value_object);
    WINAPI CmzStdObjectProperty(CmzStdObjectProperty& copyObj);
    WINAPI ~CmzStdObjectProperty();

    VOID WINAPI clearValue();
    BOOL WINAPI hasOnlyScalars();
    CmzStdObjectProperty* WINAPI clone();
    CmzString WINAPI getAsString();

protected:
    VOID WINAPI _commonInit();

private:
    WINAPI CmzStdObjectProperty() {};
    BOOL WINAPI _arrayHasOnlyScalars();
    BOOL WINAPI _objectHasOnlyScalars();
};
typedef CmzStdObjectProperty *PStdObjectProperty;




class DECLSPEC_MZSTD CmzStdObject : public CmzObject {
public:
    WINAPI CmzStdObject();
    WINAPI CmzStdObject(CmzStdObject& copyObj);
    virtual WINAPI ~CmzStdObject();

    VOID WINAPI set(WCHAR* name, CmzString value);
    inline VOID WINAPI set(CmzString name, CmzString value) { return this->set(name.toWCHAR(), value); }
    VOID WINAPI set(WCHAR* name, LONGLONG value);
    inline VOID WINAPI set(CmzString name, LONGLONG value) { return this->set(name.toWCHAR(), value); }
    VOID WINAPI set(WCHAR* name, double value);
    inline VOID WINAPI set(CmzString name, double value) { return this->set(name.toWCHAR(), value); }
    VOID WINAPI set(WCHAR* name, CmzPointerList *aValue);
    inline VOID WINAPI set(CmzString name, CmzPointerList* aValue) { return this->set(name.toWCHAR(), aValue); }
    VOID WINAPI set(WCHAR* name, CmzStdObject *oValue);
    inline VOID WINAPI set(CmzString name, CmzStdObject* oValue) { return this->set(name.toWCHAR(), oValue); }
    CmzStdObjectProperty* WINAPI get(WCHAR *name);
    inline CmzStdObjectProperty* WINAPI get(CmzString name) { return this->get(name.toWCHAR()); }
    CmzString WINAPI getAsString(WCHAR *name);
    inline CmzString WINAPI getAsString(CmzString name) { return this->getAsString(name.toWCHAR()); }
    LONGLONG WINAPI getAsInt(WCHAR *name);
    inline LONGLONG WINAPI getAsInt(CmzString name) { return this->getAsInt(name.toWCHAR()); }
    double WINAPI getAsDouble(WCHAR *name);
    inline double WINAPI getAsDouble(CmzString name) { return this->getAsDouble(name.toWCHAR()); }
    CmzPointerList* WINAPI getAsArray(WCHAR *name);
    inline CmzPointerList* WINAPI getAsArray(CmzString name) { return this->getAsArray(name.toWCHAR()); }
    CmzStdObject* WINAPI getAsObject(WCHAR *name);
    inline CmzStdObject* WINAPI getAsObject(CmzString name) { return this->getAsObject(name.toWCHAR()); }
    CmzString WINAPI toJson(CmzString quote = "\"");
    BOOL WINAPI loadFromFile(CmzString fname);
    inline ULONGLONG WINAPI itemsCount() { return this->_properties->size(); }
    CmzStdObject* WINAPI clone();
    VOID WINAPI clean();
    
    // przeciążone operatory
    CmzStdObjectProperty* WINAPI operator[](WCHAR *name);

protected:
    VOID WINAPI _commonInit();

private:
    CmzPointerList *_properties;
    CmzString _quote;

    CmzString WINAPI _propertyToJson(CmzStdObjectProperty *p, CmzString indent);
    CmzString WINAPI _valueToJson(CmzStdObjectProperty *p, CmzString indent, BOOL withName = FALSE);
    CmzString WINAPI _arrayToJson(CmzStdObjectProperty *p, CmzString indent, BOOL withName = FALSE);
    CmzString WINAPI _objectToJson(CmzStdObjectProperty *p, CmzString indent, BOOL withName = FALSE);
};

}

#endif
