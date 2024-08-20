#include "pch.h"
#include "mzHashTableString.h"
#include "String.h"
#include <string.h>

namespace mzstd {

CmzHashTableString::CmzHashTableString(UINT capacity) {
    this->_size = 0;
    this->_keys = new CmzString[capacity+1];
    if (this->_keys) {
        this->_values = new CmzString[capacity+1];
        if (!this->_values) {
            delete[] this->_keys;
            this->_keys = NULL;
        } else {
            this->_capacity = capacity;
        }
    }
}

CmzHashTableString::CmzHashTableString(const CmzHashTableString& obj) {
}

CmzHashTableString::~CmzHashTableString() {
    if (this->_keys) {
        delete[] this->_keys;
    }
    if (this->_values) {
        delete[] this->_values;
    }
}

void CmzHashTableString::put(CmzString key, CmzString value) {
    INT index = this->_computeIndex(key);
    if (index >= 0) {
        this->_keys[index] = key;
        this->_values[index] = value;
        this->_size++;
    }
}

CmzString CmzHashTableString::get(CmzString key) {
    INT index = this->_computeIndex(key);
    if (index >= 0) {
        if (this->_keys[index] == key) {
            return this->_values[index];
        }
    }
    return "";
}

void CmzHashTableString::remove(CmzString key) {
    INT index = this->_computeIndex(key);
    if (index >= 0) {
        this->_keys[index] = "";
        this->_values[index] = "";
        this->_size--;
    }
}

void CmzHashTableString::clear() {
    for (UINT i = 0; i < this->_capacity; i++) {
        this->_keys[i] = "";
        this->_values[i] = "";
    }
    this->_size = 0;
}

CmzString CmzHashTableString::toString() {
    return "";
}

void CmzHashTableString::print() {
}

void CmzHashTableString::println() {
}

CmzString& CmzHashTableString::operator [] (CmzString& indexKey) {
    INT index = this->_computeIndex(indexKey);
    if (index >= 0) {
        if (this->_keys[index] == "") {
            this->_size++;
            this->_keys[index] = indexKey;
        }
        return this->_values[index];
    }
    // return empty value
    return this->_values[this->_capacity];
}

INT CmzHashTableString::_computeIndex(CmzString& key) {
    INT index = -1;
    INT len = (INT)key.length();
    INT value;
    
    for (INT i = 0; i < len; i++) {
        value = (INT)key[i];
        value *= 1 << ((i*2)%32);
        index += value;
    }
    index = index % this->_capacity;
    if (this->_keys[index] != "" && this->_keys[index] != key) {
        index = 0;
    }
    
    while (this->_keys[index] != "") {
        if (this->_keys[index] == key) {
            break;
        } else {
            index++;
        }
    }
    if (index == (INT)this->_capacity) {
        return -1;
    }
    return index;
}

}
