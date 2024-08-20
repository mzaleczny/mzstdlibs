#include "pch.h"
#include "mzStdObject.h"
#include "mzJsonParser.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"



namespace mzstd {

VOID CmzStdObjectProperty::_commonInit() {
    this->setRealClassName(L"CmzStdObjectProperty");
}

CmzStdObjectProperty::CmzStdObjectProperty(CmzString name, CmzString value) {
    this->_commonInit();
    this->type = STDOBJ_PROPERTY_VALUE;
    this->name = name;
    this->value = value;
    this->aValue = NULL;
    this->oValue = NULL;
	this->extraData = NULL;
}

CmzStdObjectProperty::CmzStdObjectProperty(CmzString name, LONGLONG value) {
    this->_commonInit();
    this->type = STDOBJ_PROPERTY_VALUE_INT;
    this->name = name;
    this->value = "";
	this->iValue = value;
    this->aValue = NULL;
    this->oValue = NULL;
	this->extraData = NULL;
}

CmzStdObjectProperty::CmzStdObjectProperty(CmzString name, double value) {
    this->_commonInit();
    this->type = STDOBJ_PROPERTY_VALUE_DOUBLE;
    this->name = name;
    this->value = "";
	this->fValue = value;
    this->aValue = NULL;
    this->oValue = NULL;
	this->extraData = NULL;
}

CmzStdObjectProperty::CmzStdObjectProperty(CmzString name, CmzPointerList *value_array) {
    this->_commonInit();
    this->type = STDOBJ_PROPERTY_ARRAY;
    this->name = name;
    this->value = "";
    this->aValue = value_array;
    this->oValue = NULL;
	this->extraData = NULL;
}

CmzStdObjectProperty::CmzStdObjectProperty(CmzString name, CmzStdObject *value_object) {
    this->_commonInit();
    this->type = STDOBJ_PROPERTY_OBJECT;
    this->name = name;
    this->value = "";
    this->aValue = NULL;
    this->oValue = value_object;
	this->extraData = NULL;
}

CmzStdObjectProperty::CmzStdObjectProperty(CmzStdObjectProperty& copyObj) {
    this->_commonInit();
    
    this->type = copyObj.type;
    this->name = copyObj.name;
    this->value = copyObj.value;
    if (copyObj.aValue) {
        ULONGLONG size = copyObj.aValue->size();
        this->aValue = new CmzPointerList(size);
        LPVOID* copyItems = copyObj.aValue->getItemsBuf();
        CmzStdObjectProperty* copyItem;
        for (ULONGLONG i = 0; i < size; i++) {
            copyItem = (CmzStdObjectProperty*)copyItems[i];
            this->aValue->insert(i, copyItem->clone());
        }
    } else {
        this->aValue = NULL;
    }
    if (copyObj.oValue) {
        this->oValue = copyObj.oValue->clone();
    } else {
        this->oValue = NULL;
    }
    if (copyObj.extraData) {
        this->extraData = copyObj.extraData->clone();
    } else {
        this->extraData = NULL;
    }
}

CmzStdObjectProperty::~CmzStdObjectProperty() {
    this->clearValue();
}

VOID CmzStdObjectProperty::clearValue() {
    CmzStdObjectProperty *p;
    if (this->aValue) {
        for (ULONGLONG i = 0; i < this->aValue->size(); i++) {
            p = (CmzStdObjectProperty*)this->aValue->getAt(i);
            delete p;
        }
        this->aValue->clear();
        delete this->aValue;
        this->aValue = NULL;
    }
    if (this->oValue) {
        delete this->oValue;
        this->oValue = NULL;
    }
	if (this->extraData) {
        delete this->extraData;
        this->extraData = NULL;
	}
    this->value = "";
    this->type = STDOBJ_PROPERTY_NONE;
}

CmzStdObjectProperty* CmzStdObjectProperty::clone() {
    CmzStdObjectProperty* newValue = new CmzStdObjectProperty(*this);
    return newValue;
}

CmzString CmzStdObjectProperty::getAsString() {
    switch (this->type) {
        case STDOBJ_PROPERTY_VALUE:
            return this->value;
        case STDOBJ_PROPERTY_VALUE_INT:
            return CmzString(this->iValue);
        case STDOBJ_PROPERTY_VALUE_DOUBLE:
            return CmzString(this->fValue, 2L);
    }
    return COMMON_EMPTY_STRING;
}

BOOL CmzStdObjectProperty::hasOnlyScalars() {
    if (this->type == STDOBJ_PROPERTY_VALUE || this->type == STDOBJ_PROPERTY_VALUE_INT || this->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
        return TRUE;
    }

    if (this->type == STDOBJ_PROPERTY_ARRAY) {
        return this->_arrayHasOnlyScalars();
    }

    if (this->type == STDOBJ_PROPERTY_OBJECT) {
        return this->_objectHasOnlyScalars();
    }

    return FALSE;
}

BOOL CmzStdObjectProperty::_arrayHasOnlyScalars() {
    BOOL onlyScalars = TRUE;
    if (!this->aValue || this->aValue->size() < 1) {
        return FALSE;
    }

    CmzStdObjectProperty *item;
    for (ULONGLONG i = 0; i < this->aValue->size(); i++) {
        item = (CmzStdObjectProperty*)this->aValue->getAt(i);
        if (item->type == STDOBJ_PROPERTY_ARRAY || item->type == STDOBJ_PROPERTY_OBJECT) {
            onlyScalars = FALSE;
            break;
        }
    }

    return onlyScalars;
}

BOOL CmzStdObjectProperty::_objectHasOnlyScalars() {
    return FALSE;
}








VOID CmzStdObject::_commonInit() {
    this->setRealClassName(L"CmzStdObject");
}

CmzStdObject::CmzStdObject() {
    this->_commonInit();
    this->_properties = new CmzPointerList();
}

CmzStdObject::CmzStdObject(CmzStdObject& copyObj) {
    this->_commonInit();
    
    ULONGLONG size = copyObj.itemsCount();
    this->_properties = new CmzPointerList(size);
    LPVOID* copyItems = copyObj._properties->getItemsBuf();
    CmzStdObjectProperty* copyItem;
    for (ULONGLONG i = 0; i < size; i++) {
        copyItem = (CmzStdObjectProperty*)copyItems[i];
        this->_properties->insert(i, copyItem->clone());
    }
}

CmzStdObject::~CmzStdObject() {
    CmzStdObjectProperty *p;
    if (this->_properties) {
        for (ULONGLONG i = 0; i < this->_properties->size(); i++) {
            p = (CmzStdObjectProperty*)this->_properties->getAt(i);
            delete p;
        }
        this->_properties->clear();
        delete this->_properties;
    }
}

CmzStdObject* CmzStdObject::clone() {
    CmzStdObject* newValue = new CmzStdObject(*this);
    return newValue;
}

VOID CmzStdObject::set(WCHAR* name, CmzString value) {
    CmzStdObjectProperty *p;

    p = this->get(name);
    if (p) {
        p->clearValue();
        p->type = STDOBJ_PROPERTY_VALUE;
        p->value = value;
    } else {
        p = new CmzStdObjectProperty(name, value);
        this->_properties->append(p);
    }
}

VOID CmzStdObject::set(WCHAR* name, LONGLONG value) {
    CmzStdObjectProperty *p;

    p = this->get(name);
    if (p) {
        p->clearValue();
        p->type = STDOBJ_PROPERTY_VALUE_INT;
        p->iValue = value;
    } else {
        p = new CmzStdObjectProperty(name, value);
        this->_properties->append(p);
    }
}

VOID CmzStdObject::set(WCHAR* name, double value) {
    CmzStdObjectProperty *p;

    p = this->get(name);
    if (p) {
        p->clearValue();
        p->type = STDOBJ_PROPERTY_VALUE_DOUBLE;
        p->fValue = value;
    } else {
        p = new CmzStdObjectProperty(name, value);
        this->_properties->append(p);
    }
}

VOID CmzStdObject::set(WCHAR* name, CmzPointerList *aValue) {
    CmzStdObjectProperty *p;

    p = this->get(name);
    if (p) {
        p->clearValue();
        p->type = STDOBJ_PROPERTY_ARRAY;
        p->aValue = aValue;
    } else {
        p = new CmzStdObjectProperty(name, aValue);
        this->_properties->append(p);
    }
}

VOID CmzStdObject::set(WCHAR* name, CmzStdObject *oValue) {
    CmzStdObjectProperty *p;

    p = this->get(name);
    if (p) {
        p->clearValue();
        p->type = STDOBJ_PROPERTY_OBJECT;
        p->oValue = oValue;
    } else {
        p = new CmzStdObjectProperty(name, oValue);
        this->_properties->append(p);
    }
}

CmzStdObjectProperty* CmzStdObject::get(WCHAR *name) {
    CmzStdObjectProperty *p;

    if (!this->_properties) {
        return NULL;
    }
    
    for (ULONGLONG i = 0; i < this->_properties->size(); i++) {
        p = (CmzStdObjectProperty*)this->_properties->getAt(i);
        if (p->name == name) {
            return p;
        }
    }

    return NULL;
}

CmzString CmzStdObject::getAsString(WCHAR *name) {
    CmzStdObjectProperty* property = this->get(name);
    if (property) {
        switch (property->type) {
            case STDOBJ_PROPERTY_VALUE:
                return property->value;
            case STDOBJ_PROPERTY_VALUE_INT:
                return CmzString(property->iValue);
            case STDOBJ_PROPERTY_VALUE_DOUBLE:
                return CmzString(property->fValue);
        }
    }
    return COMMON_EMPTY_STRING;
}

LONGLONG CmzStdObject::getAsInt(WCHAR *name) {
    CmzStdObjectProperty* property = this->get(name);
    if (property) {
        switch (property->type) {
            case STDOBJ_PROPERTY_VALUE:
                return property->value.toInt();
            case STDOBJ_PROPERTY_VALUE_INT:
                return property->iValue;
            case STDOBJ_PROPERTY_VALUE_DOUBLE:
                return (LONGLONG)property->fValue;
        }
    }
    return 0;
}

double CmzStdObject::getAsDouble(WCHAR *name) {
    CmzStdObjectProperty* property = this->get(name);
    if (property) {
        switch (property->type) {
            case STDOBJ_PROPERTY_VALUE:
                return property->value.toDouble();
            case STDOBJ_PROPERTY_VALUE_DOUBLE:
                return property->fValue;
            case STDOBJ_PROPERTY_VALUE_INT:
                return static_cast<double>(property->iValue);
        }
    }
    return 0;
}

CmzPointerList* CmzStdObject::getAsArray(WCHAR *name) {
    CmzStdObjectProperty* property = this->get(name);
    if (property && property->type == STDOBJ_PROPERTY_ARRAY) {
        return property->aValue;
    }
    return NULL;
}

CmzStdObject* CmzStdObject::getAsObject(WCHAR *name) {
    CmzStdObjectProperty* property = this->get(name);
    if (property && property->type == STDOBJ_PROPERTY_OBJECT) {
        return property->oValue;
    }
    return NULL;
}

CmzStdObjectProperty* CmzStdObject::operator[](WCHAR *name) {
    return this->get(name);
}

CmzString CmzStdObject::toJson(CmzString quote) {
    CmzString json = "";
    CmzStdObjectProperty *root;

    this->_quote = quote;
    if (this->_properties && this->_properties->size() > 0) {
        root = this->get(L"root");
        if (root) {
            json += this->_propertyToJson(root, "");
        }
    }

    return json;
}

CmzString CmzStdObject::_propertyToJson(CmzStdObjectProperty *p, CmzString indent) {
    CmzString json = "";

    if (p->type == STDOBJ_PROPERTY_VALUE || p->type == STDOBJ_PROPERTY_VALUE_INT || p->type == STDOBJ_PROPERTY_VALUE_DOUBLE) {
        return this->_valueToJson(p, "");
    }

    if (p->type == STDOBJ_PROPERTY_ARRAY) {
        return this->_arrayToJson(p, "");
    }

    if (p->type == STDOBJ_PROPERTY_OBJECT) {
        return this->_objectToJson(p, "");
    }

    return "WEWNĘTRNY BŁĄD. Ta wartość nie powinna zostać zwrócona";
}

CmzString CmzStdObject::_valueToJson(CmzStdObjectProperty *p, CmzString indent, BOOL withName) {
    if (withName) {
        return indent + p->name + ": " + this->_quote + p->value + this->_quote;
    }

    return indent + this->_quote + p->value + this->_quote;
}

CmzString CmzStdObject::_arrayToJson(CmzStdObjectProperty *p, CmzString indent, BOOL withName) {
    CmzStdObjectProperty *item;
    CmzPointerList *a = p->aValue;
    BOOL onlyScalars;
    CmzString json;

    if (a && a->size() > 0) {
        // najpierw sprawdzamy czy ta tablica zawiera same wartości
        // skalarne
        onlyScalars = p->hasOnlyScalars();

        if (withName) {
            json = indent + p->name + ": [";
        } else {
            json = indent + "[";
        }
        // jeśli mamy same skalary, to je drukujemy w rządku
        if (onlyScalars) {
            ULONGLONG len = a->size();
            for (ULONGLONG i = 0; i < len; i++) {
                item = (CmzStdObjectProperty*)a->getAt(i);
                json += this->_valueToJson(item, "");
                if (i < len - 1) {
                    json += ", ";
                }
            }
            json += "]";
        } else {
            ULONGLONG len = a->size();
            for (ULONGLONG i = 0; i < len; i++) {
                item = (CmzStdObjectProperty*)a->getAt(i);
                if (item->type == STDOBJ_PROPERTY_VALUE) {
                    json += CmzString("\n") + indent + STDOBJ_DEFAULT_INDENT + this->_valueToJson(item, "");
                } else if (item->type == STDOBJ_PROPERTY_ARRAY) {
                    json += CmzString("\n") + this->_arrayToJson(item, indent + STDOBJ_DEFAULT_INDENT);
                } else if (item->type == STDOBJ_PROPERTY_OBJECT) {
                    json += CmzString("\n") + this->_objectToJson(item, indent + STDOBJ_DEFAULT_INDENT);
                }
                if (i < len - 1) {
                    json += ",";
                }
            }
            json += "\n";
            json += indent + "]";
        }
        return json;
    } else {
        if (withName) {
            return indent + p->name + ": []";
        }
        return indent + "[]";
    }
}

CmzString CmzStdObject::_objectToJson(CmzStdObjectProperty *p, CmzString indent, BOOL withName) {
    CmzString json = "";

    if (withName) {
        json = indent + p->name + ": {";
    } else {
        json = indent + "{";
    }
    CmzPointerList *a = p->oValue->_properties;
    ULONGLONG len = a->size();
    if (len == 0) {
        json += "}";
        return json;
    }
    CmzStdObjectProperty *item;
    for (ULONGLONG i = 0; i < len; i++) {
        item = (CmzStdObjectProperty*)a->getAt(i);
        if (item->type == STDOBJ_PROPERTY_VALUE) {
            json += CmzString("\n") + indent + STDOBJ_DEFAULT_INDENT + this->_valueToJson(item, "", TRUE);
        } else if (item->type == STDOBJ_PROPERTY_ARRAY) {
            json += CmzString("\n") + this->_arrayToJson(item, indent + STDOBJ_DEFAULT_INDENT, TRUE);
        } else if (item->type == STDOBJ_PROPERTY_OBJECT) {
            json += CmzString("\n") + this->_objectToJson(item, indent + STDOBJ_DEFAULT_INDENT, TRUE);
        }
        if (i < len - 1) {
            json += ",";
        }
    }
    json += "\n";
    json += indent + "}";

    return json;
}

BOOL CmzStdObject::loadFromFile(CmzString fname) {
    if (FileExists(fname)) {
        CmzJsonParser parser;
        this->clean();
        CmzStdObject* data = parser.parseFile(fname, this);
        if (data) {
            return TRUE;
        } else {
            if (!parser.isError()) {
                AlertBox(CmzString(L"No json objects found in file [") + fname + L"]");
            } else {
                ErrorBox(parser.getErrorMessage());
            }
        }
    }
    return FALSE;
}

VOID CmzStdObject::clean() {
    CmzStdObjectProperty *p;
    if (this->_properties) {
        for (ULONGLONG i = 0; i < this->_properties->size(); i++) {
            p = (CmzStdObjectProperty*)this->_properties->getAt(i);
            if (p) {
                delete p;
            }
        }
        this->_properties->clear();
    }
}

}
