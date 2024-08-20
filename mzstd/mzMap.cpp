/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstd
 * Version: 1.07
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstd library
 * 
 * mzstd library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstd library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */
#include "pch.h"
#include "mzMap.h"
#include "mzstdGlobals.h"

using namespace mzstd;

// ***********************************************************************************************
// FUNKCJE
// ***********************************************************************************************
VOID WINAPI mzstd::swap(LONGLONG& val1, LONGLONG& val2) {
    LONGLONG tmp = val1;
    val1 = val2;
    val2 = tmp;
}

VOID WINAPI mzstd::swap(BOOL& val1, BOOL& val2) {
    BOOL tmp = val1;
    val1 = val2;
    val2 = tmp;
}

VOID WINAPI mzstd::swap(CPair* pair1, CPair* pair2) {
    pair1->swap(pair2);
}

VOID WINAPI mzstd::swap(TPairExtValueType& val1, TPairExtValueType& val2) {
    TPairExtValueType tmp = val1;
    val1 = val2;
    val2 = tmp;
}

VOID WINAPI mzstd::deleteArrayListOfPairExt(CmzArrayList *lst) {
    ULONGLONG size = lst->size();
    if (!lst || size < 1) {
        return;
    }

    LONGLONG* items = lst->getItemsBuf();
    CPairExt* p;

    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p) {
            delete p;
        }
    }

    lst->clear();
    delete lst;
}
// ***********************************************************************************************
// KONIEC FUNKCJI
// ***********************************************************************************************




// ***********************************************************************************************
// Klasa CPair
// ***********************************************************************************************
VOID CPair::_commonInit() {
    this->setRealClassName(L"CPair");
}

CPair::CPair()
	: first(0), second(0)
{
    this->_commonInit();
}

CPair::CPair(const CPair& pair)
	: first(pair.first), second(pair.second)
{
    this->_commonInit();
}

CPair::CPair(CPair* pair)
	: first(pair->first), second(pair->second)
{
    this->_commonInit();
}

// konstruktor z podanych wartości
CPair::CPair(LONGLONG first, LONGLONG second)
	: first(first), second(second)
{
    this->_commonInit();
}

CPair::~CPair() {
}


VOID CPair::swap(CPair* pair) {
    if (this != pair) {
        ::swap(this->first, pair->first);
        ::swap(this->second, pair->second);
    }
}

BOOL CPair::equals(CmzObject* obj) {
    if (CmzObject::equals(obj)) {
        return TRUE;
    }

    CPair* p = dynamic_cast<CPair*>(obj);
    if (p) {
        return this->equals(p);
    }

    return FALSE;
}

BOOL CPair::equals(CPair* pair) {
    // jeśli porównujemy te same wskaźniki, to obiekty są równe
    if (this == pair) {
        return TRUE;
    }
    // jeśli są to dwa różne obiekty o tych samych wartościach first i second, to też są równe
    if (this->first == pair->first && this->second == pair->second) {
        return TRUE;
    }

    return FALSE;
}

BOOL CPair::equals(CObjectPair* pair) {
    return this->equals((CPair*)pair);
}

BOOL CPair::equals(CPairExt* pair) {
    return this->equals((CPair*)pair);
}

CPair& CPair::operator=(CPair& pair) {
    this->first = pair.first;
    this->second = pair.second;
    return (*this);
}

BOOL CPair::operator==(CPair& pair) {
    return this->equals(&pair);
}

BOOL CPair::operator!=(CPair& pair) {
    return !this->equals(&pair);
}
// ***********************************************************************************************
// Koniec klasy CPair
// ***********************************************************************************************









// ***********************************************************************************************
// Klasa CObjectPair
// ***********************************************************************************************
VOID CObjectPair::_commonInit() {
    this->setRealClassName(L"CObjectPair");
}

CObjectPair::CObjectPair()
    : CPair()
{
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = FALSE;
}

CObjectPair::CObjectPair(const CObjectPair& pair) {
    this->_commonInit();
    this->first = pair.first;
    this->second = pair.second;
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = FALSE;
}

// konstruktor z podanych wartości
CObjectPair::CObjectPair(CmzObject* first, CmzObject* second, BOOL destroyFirstOnObjectDestroy, BOOL destroySecondOnObjectDestroy)
{
    this->_commonInit();
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)second;
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
}

CObjectPair::~CObjectPair() {
    this->_cleanup();
}

VOID CObjectPair::_cleanup() {
    if (this->_destroyFirstOnObjectDestroy) {
        if (this->first) {
            delete (CmzObject*)this->first;
            this->first = NULL;
        }
    }

    if (this->_destroySecondOnObjectDestroy) {
        if (this->second) {
            delete (CmzObject*)this->second;
            this->second = NULL;
        }
    }
}


VOID CObjectPair::swap(CObjectPair* pair) {
    if (this != pair) {
        ::swap(this->first, pair->first);
        ::swap(this->second, pair->second);
        ::swap(this->_destroyFirstOnObjectDestroy, pair->_destroyFirstOnObjectDestroy);
        ::swap(this->_destroySecondOnObjectDestroy, pair->_destroySecondOnObjectDestroy);
    }
}

BOOL CObjectPair::equals(CPair* pair) {
    return CPair::equals(pair);
}

BOOL CObjectPair::equals(CObjectPair* pair) {
    if ((LONGLONG)this == (LONGLONG)pair) {
        return TRUE;
    }

    CmzObject* first = (CmzObject*)this->first;
    CmzObject* second = (CmzObject*)this->second;

    return first->equals((CmzObject*)pair->first) && second->equals((CmzObject*)pair->second);
}

BOOL CObjectPair::equals(CPairExt* pair) {
    if ((LONGLONG)this == (LONGLONG)pair) {
        return TRUE;
    }

    // jeśli przekazana para zawiera jedną z wartości typu LONGLONG, to zwracamy FALSE
    if (pair->firstType == TPairExtValueType_LONGLONG || pair->secondType == TPairExtValueType_LONGLONG) {
        return FALSE;
    }

    CmzObject* first = (CmzObject*)this->first;
    CmzObject* second = (CmzObject*)this->second;

    return first->equals((CmzObject*)pair->first) && second->equals((CmzObject*)pair->second);
}

BOOL CObjectPair::equals(CmzObject* obj) {
    if (CmzObject::equals(obj)) {
        return TRUE;
    }

    CPairExt *pe = dynamic_cast<CPairExt*>(obj);
    if (pe) {
        return this->equals(pe);
    }
    CObjectPair *op = dynamic_cast<CObjectPair*>(obj);
    if (op) {
        return this->equals(op);
    }
    CPair *p = dynamic_cast<CPair*>(obj);
    if (p) {
        return this->equals(p);
    }
    return FALSE;
}

CObjectPair& CObjectPair::operator=(const CObjectPair& pair) {
    // poniższa metoda uwzględnia ustawienia atrybutów this->_destroy[First|Second]OnObjectDestroy
    this->_cleanup();

    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = FALSE;
    this->first = pair.first;
    this->second = pair.second;

    return *this;
}

BOOL CObjectPair::operator==(const CObjectPair& pair) {
    return this->equals((CObjectPair*)&pair);
}

BOOL CObjectPair::operator!=(const CObjectPair& pair) {
    return !this->equals((CObjectPair*)&pair);
}
// ***********************************************************************************************
// Koniec klasy CObjectPair
// ***********************************************************************************************










// ***********************************************************************************************
// Klasa CPairExt
// ***********************************************************************************************
VOID CPairExt::_commonInit() {
    this->setRealClassName(L"CPairExt");
}

// konstruktor domyślny
CPairExt::CPairExt() : CPair() {
    this->_commonInit();
    this->firstType = TPairExtValueType_LONGLONG;
    this->secondType = TPairExtValueType_LONGLONG;
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = FALSE;
}

// konstruktor kopiujący
CPairExt::CPairExt(const CPairExt& pair) {
    this->_commonInit();
    this->first = pair.first;
    this->second = pair.second;
    this->firstType = pair.firstType;
    this->secondType = pair.secondType;
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = FALSE;
}

// konstruktory z podanych wartości
CPairExt::CPairExt(LONGLONG first, LONGLONG second)
    : CPair(first, second)
{
    this->_commonInit();
    this->firstType = TPairExtValueType_LONGLONG;
    this->secondType = TPairExtValueType_LONGLONG;
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = FALSE;
}

WINAPI CPairExt::CPairExt(LONGLONG first, CPair* second, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = first;
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_LONGLONG;
    this->secondType = TPairExtValueType_PPair;
}

WINAPI CPairExt::CPairExt(CPair* first, LONGLONG second, BOOL destroyFirstOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = FALSE;
    this->first = (LONGLONG)first;
    this->second = second;
    this->firstType = TPairExtValueType_PPair;
    this->secondType = TPairExtValueType_LONGLONG;
}

WINAPI CPairExt::CPairExt(CPair* first, CPair* second, BOOL destroyFirstOnObjectDestroy, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_PPair;
    this->secondType = TPairExtValueType_PPair;
}

CPairExt::CPairExt(LONGLONG first, CmzObject* second, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = first;
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_LONGLONG;
    this->secondType = TPairExtValueType_PObject;
}

CPairExt::CPairExt(CmzObject* first, LONGLONG second, BOOL destroyFirstOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = FALSE;
    this->first = (LONGLONG)first;
    this->second = second;
    this->firstType = TPairExtValueType_PObject;
    this->secondType = TPairExtValueType_LONGLONG;
}

CPairExt::CPairExt(CmzObject* first, CmzObject* second, BOOL destroyFirstOnObjectDestroy, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_PObject;
    this->secondType = TPairExtValueType_PObject;
}

CPairExt::CPairExt(CPair* first, CmzObject* second, BOOL destroyFirstOnObjectDestroy, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_PPair;
    this->secondType = TPairExtValueType_PObject;
}

CPairExt::CPairExt(CmzObject* first, CPair* second, BOOL destroyFirstOnObjectDestroy, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_PObject;
    this->secondType = TPairExtValueType_PPair;
}

CPairExt::CPairExt(LONGLONG first, CmzString second) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = FALSE;
    this->_destroySecondOnObjectDestroy = TRUE;
    this->first = first;
    this->second = (LONGLONG)(new CmzString(second));
    this->firstType = TPairExtValueType_LONGLONG;
    this->secondType = TPairExtValueType_PString;
}

CPairExt::CPairExt(CmzString first, LONGLONG second) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = TRUE;
    this->_destroySecondOnObjectDestroy = FALSE;
    this->first = (LONGLONG)(new CmzString(first));
    this->second = second;
    this->firstType = TPairExtValueType_PString;
    this->secondType = TPairExtValueType_LONGLONG;
}

CPairExt::CPairExt(CmzString first, CmzString second) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = TRUE;
    this->_destroySecondOnObjectDestroy = TRUE;
    this->first = (LONGLONG)(new CmzString(first));
    this->second = (LONGLONG)(new CmzString(second));
    this->firstType = TPairExtValueType_PString;
    this->secondType = TPairExtValueType_PString;
}

CPairExt::CPairExt(CPair* first, CmzString second, BOOL destroyFirstOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = TRUE;
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)(new CmzString(second));
    this->firstType = TPairExtValueType_PPair;
    this->secondType = TPairExtValueType_PString;
}

CPairExt::CPairExt(CmzString first, CPair* second, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = TRUE;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = (LONGLONG)(new CmzString(first));
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_PString;
    this->secondType = TPairExtValueType_PPair;
}

CPairExt::CPairExt(CmzObject* first, CmzString second, BOOL destroyFirstOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    this->_destroySecondOnObjectDestroy = TRUE;
    this->first = (LONGLONG)first;
    this->second = (LONGLONG)(new CmzString(second));
    this->firstType = TPairExtValueType_PObject;
    this->secondType = TPairExtValueType_PString;
}

CPairExt::CPairExt(CmzString first, CmzObject* second, BOOL destroySecondOnObjectDestroy) {
    this->_commonInit();
    this->_destroyFirstOnObjectDestroy = TRUE;
    this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    this->first = (LONGLONG)(new CmzString(first));
    this->second = (LONGLONG)second;
    this->firstType = TPairExtValueType_PString;
    this->secondType = TPairExtValueType_PObject;
}


// destruktor
CPairExt::~CPairExt() {
    this->_cleanup();
}

VOID CPairExt::_cleanup() {
    this->deleteFirst();
    this->deleteSecond();
}

VOID CPairExt::deleteFirst() {
    if (this->_destroyFirstOnObjectDestroy) {
        if (this->first && this->firstType != TPairExtValueType_LONGLONG) {
            delete (CmzObject*)this->first;
        }
    }
    this->first = (LONGLONG)NULL;
    this->firstType = TPairExtValueType_Null;
}

VOID CPairExt::deleteSecond() {
    if (this->_destroySecondOnObjectDestroy) {
        if (this->second && this->secondType != TPairExtValueType_LONGLONG) {
            delete (CmzObject*)this->second;
        }
    }
    this->second = (LONGLONG)NULL;
    this->secondType = TPairExtValueType_Null;
}

VOID CPairExt::setFirst(LONGLONG first) {
    this->deleteFirst();
    this->first = first;
    this->firstType = TPairExtValueType_LONGLONG;
    this->_destroyFirstOnObjectDestroy = FALSE;
}

VOID CPairExt::setFirst(CmzObject* first, BOOL destroyFirstOnObjectDestroy) {
    if (!first) {
        this->setFirst((LONGLONG)0);
        return;
    }

    this->deleteFirst();

    CmzString* first_as_string = dynamic_cast<CmzString*>(first);
    CPair* first_as_pair = dynamic_cast<CPair*>(first);

    this->first = (LONGLONG)first;

    if (first_as_string) {
        this->firstType = TPairExtValueType_PString;
        this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    }

    else if (first_as_pair) {
        this->firstType = TPairExtValueType_PPair;
        this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    }

    else {
        this->firstType = TPairExtValueType_PObject;
        this->_destroyFirstOnObjectDestroy = destroyFirstOnObjectDestroy;
    }
}

VOID CPairExt::setFirst(CmzString first) {
    this->deleteFirst();
    this->first = (LONGLONG)(new CmzString(first.getBuffer()));
    this->firstType = TPairExtValueType_PString;
    this->_destroyFirstOnObjectDestroy = FALSE;
}

VOID CPairExt::setSecond(LONGLONG second) {
    this->deleteSecond();
    this->second = second;
    this->secondType = TPairExtValueType_LONGLONG;
    this->_destroySecondOnObjectDestroy = FALSE;
}

VOID CPairExt::setSecond(CmzObject* second, BOOL destroySecondOnObjectDestroy) {
    if (!second) {
        this->setSecond((LONGLONG)0);
        return;
    }

    this->deleteSecond();

    CmzString* second_as_string = dynamic_cast<CmzString*>(second);
    CPair* second_as_pair = dynamic_cast<CPair*>(second);

    this->second = (LONGLONG)second;

    if (second_as_string) {
        this->secondType = TPairExtValueType_PString;
        this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    }

    else if (second_as_pair) {
        this->secondType = TPairExtValueType_PPair;
        this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    }

    else {
        this->secondType = TPairExtValueType_PObject;
        this->_destroySecondOnObjectDestroy = destroySecondOnObjectDestroy;
    }
}
VOID CPairExt::setSecond(CmzString second) {
    this->deleteSecond();
    this->second = (LONGLONG)(new CmzString(second.getBuffer()));
    this->secondType = TPairExtValueType_PString;
    this->_destroySecondOnObjectDestroy = FALSE;
}

VOID CPairExt::swap(CPairExt* pair) {
    if (this != pair) {
        ::swap(this->first, pair->first);
        ::swap(this->firstType, pair->firstType);
        ::swap(this->second, pair->second);
        ::swap(this->secondType, pair->secondType);
        ::swap(this->_destroyFirstOnObjectDestroy, pair->_destroyFirstOnObjectDestroy);
        ::swap(this->_destroySecondOnObjectDestroy, pair->_destroySecondOnObjectDestroy);
    }
}

BOOL CPairExt::equals(CPair* pair) {
    return CPair::equals(pair);
}

BOOL CPairExt::equals(CObjectPair* pair) {
    if ((LONGLONG)this == (LONGLONG)pair) {
        return TRUE;
    }

    // jeśli przekazana para zawiera jedną z wartości typu LONGLONG, to zwracamy FALSE
    if (this->firstType == TPairExtValueType_LONGLONG || this->secondType == TPairExtValueType_LONGLONG) {
        return FALSE;
    }

    CmzObject* first = (CmzObject*)pair->first;
    CmzObject* second = (CmzObject*)pair->second;

    return first->equals((CmzObject*)this->first) && second->equals((CmzObject*)this->second);
}

BOOL CPairExt::equals(CPairExt* pair) {
    if (this == pair) {
        return TRUE;
    }

    BOOL firstElemsEquals = FALSE;
    BOOL secondElemsEquals = FALSE;

    if (this->firstType == pair->firstType) {
        if (this->firstType == TPairExtValueType_LONGLONG) {
            firstElemsEquals = this->first == pair->first;
        } else if (this->firstType == TPairExtValueType_PObject) {
            CmzObject* this_first = (CmzObject*)this->first;
            CmzObject* pair_first = (CmzObject*)pair->first;
            firstElemsEquals = this_first->equals(pair_first);
        } else if (this->firstType == TPairExtValueType_PPair) {
            CPair* this_first = (CPair*)this->first;
            CPair* pair_first = (CPair*)pair->first;
            firstElemsEquals = this_first->equals(pair_first);
        } else if (this->firstType == TPairExtValueType_PString) {
            CmzString* this_first = (CmzString*)this->first;
            CmzString* pair_first = (CmzString*)pair->first;
            firstElemsEquals = this_first->equals(pair_first);
        }
    }

    if (!firstElemsEquals) {
        return FALSE;
    }


    if (this->secondType == pair->secondType) {
        if (this->secondType == TPairExtValueType_LONGLONG) {
            secondElemsEquals = this->second == pair->second;
        } else if (this->secondType == TPairExtValueType_PObject) {
            CmzObject* this_second = (CmzObject*)this->second;
            CmzObject* pair_second = (CmzObject*)pair->second;
            secondElemsEquals = this_second->equals(pair_second);
        } else if (this->secondType == TPairExtValueType_PPair) {
            CPair* this_second = (CPair*)this->second;
            CPair* pair_second = (CPair*)pair->second;
            secondElemsEquals = this_second->equals(pair_second);
        } else if (this->secondType == TPairExtValueType_PString) {
            CmzString* this_second = (CmzString*)this->second;
            CmzString* pair_second = (CmzString*)pair->second;
            secondElemsEquals = this_second->equals(pair_second);
        }
    }

    if (!secondElemsEquals) {
        return FALSE;
    }

    return TRUE;
}

BOOL CPairExt::equals(CmzObject* obj) {
    if (CmzObject::equals(obj)) {
        return TRUE;
    }

    CPair* p = dynamic_cast<CPair*>(obj);
    return this->equals(p);
}

CmzObject* CPairExt::getFirstAsObject() {
    if (this->firstType == TPairExtValueType_PObject) {
        return (CmzObject*)this->first;
    }
    return NULL;
}
CPair* CPairExt::getFirstAsPair() {
    if (this->firstType == TPairExtValueType_PPair) {
        return (CPair*)this->first;
    }
    return NULL;
}
CObjectPair* CPairExt::getFirstAsObjectPair() {
    if (this->firstType == TPairExtValueType_PPair) {
        return (CObjectPair*)this->first;
    }
    return NULL;
}
CPairExt* CPairExt::getFirstAsPairExt() {
    if (this->firstType == TPairExtValueType_PPair) {
        return (CPairExt*)this->first;
    }
    return NULL;
}
CmzString* CPairExt::getFirstAsString() {
    if (this->firstType == TPairExtValueType_PString) {
        return (CmzString*)this->first;
    }
    return NULL;
}


CmzObject* CPairExt::getSecondAsObject() {
    if (this->secondType == TPairExtValueType_PObject) {
        return (CmzObject*)this->second;
    }
    return NULL;
}
CPair* CPairExt::getSecondAsPair() {
    if (this->secondType == TPairExtValueType_PPair) {
        return (CPair*)this->second;
    }
    return NULL;
}
CObjectPair* CPairExt::getSecondAsObjectPair() {
    if (this->secondType == TPairExtValueType_PPair) {
        return (CObjectPair*)this->second;
    }
    return NULL;
}
CPairExt* CPairExt::getSecondAsPairExt() {
    if (this->secondType == TPairExtValueType_PPair) {
        return (CPairExt*)this->second;
    }
    return NULL;
}
CmzString* CPairExt::getSecondAsString() {
    if (this->secondType == TPairExtValueType_PString) {
        return (CmzString*)this->second;
    }
    return NULL;
}

CPairExt& CPairExt::operator=(const CPairExt& pair) {
    // poniższa metoda uwzględnia ustawienia atrybutów this->_destroy[First|Second]OnObjectDestroy
    this->_cleanup();

    if (pair.firstType == TPairExtValueType_PString) {
        this->_destroyFirstOnObjectDestroy = TRUE;
        this->first = (LONGLONG)(new CmzString(((CmzString*)pair.first)->getBuffer()));
        this->firstType = TPairExtValueType_PString;
    } else {
        this->_destroyFirstOnObjectDestroy = FALSE;
        this->first = pair.first;
        this->firstType = pair.firstType;
    }

    if (pair.secondType == TPairExtValueType_PString) {
        this->_destroySecondOnObjectDestroy = TRUE;
        this->second = (LONGLONG)(new CmzString(((CmzString*)pair.second)->getBuffer()));
        this->secondType = TPairExtValueType_PString;
    } else {
        this->_destroySecondOnObjectDestroy = FALSE;
        this->second = pair.second;
        this->secondType = pair.secondType;
    }

    return *this;
}

BOOL CPairExt::operator==(const CPairExt& pair) {
    return this->equals((CPairExt*)&pair);
}

BOOL CPairExt::operator!=(const CPairExt& pair) {
    return !this->equals((CPairExt*)&pair);
}

CPairExt* CPairExt::clone() {
    CPairExt* result = new CPairExt((LONGLONG)0, (LONGLONG)0);
    if (result) {
        *result = *this;
    }
    return result;
}
// ***********************************************************************************************
// ***********************************************************************************************










// ***********************************************************************************************
// Klasa CmzMap
// ***********************************************************************************************
VOID CmzMap::_commonInit() {
    this->setRealClassName(L"CmzMap");
}

// konstruktor domyślny
CmzMap::CmzMap(ULONGLONG capacity) {
    this->_commonInit();
    if (capacity > 0) {
        this->_items = new CmzArrayList(capacity);
    } else {
        this->_items = new CmzArrayList();
    }
}

// konstruktor kopiujący - zablokowany
CmzMap::CmzMap(const CmzMap& map) {
    this->_commonInit();
}

CmzMap::~CmzMap() {
    if (this->_items) {
        CPairExt* pair;
        ULONGLONG size = this->_items->size();
        LONGLONG* items = this->_items->getItemsBuf();
        for (ULONGLONG i = 0; i < size; i++) {
            pair = (CPairExt*)items[i];
            delete pair;
            items[i] = (LONGLONG)NULL;
        }
        delete this->_items;
    }
}

VOID CmzMap::append(CPairExt* pair) {
    this->_items->append((LONGLONG)pair);
}

VOID CmzMap::push(CPairExt* pair) {
    this->_items->append((LONGLONG)pair);
}

VOID CmzMap::addAsFirst(CPairExt* pair) {
    this->_items->addAsFirst((LONGLONG)pair);
}

VOID CmzMap::insert(ULONGLONG pos, CPairExt* pair) {
    this->_items->insert(pos, (LONGLONG)pair);
}

ULONGLONG CmzMap::del(CPairExt* pair) {
    // Musimy ręcznie iterować po tablicy, bo oprócz wyrzucenia z niej wskaźnika trzeba
    // jeszcze zrobić delete na obiekcie na który wskazuje
    LONGLONG pair_LONGLONG = (LONGLONG)pair;
    LONGLONG p;
    ULONGLONG deleted = 0;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        return 0;
    }
    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = items[i];
        if (p == pair_LONGLONG) {
            delete (CPairExt*)p;
            deleted++;
            items[i] = (LONGLONG)NULL;
        }
    }
    if (deleted > 0) {
        // usuwamy z tablicy wszystkie NULL-e
        this->_items->del((LONGLONG)NULL);
    }
    return deleted;
}

BOOL CmzMap::delFirst(CPairExt* pair) {
    LONGLONG p;
    LONGLONG pair_LONGLONG = (LONGLONG)pair;
    ULONGLONG size = this->_items->size();
    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = items[i];
        if (p == pair_LONGLONG) {
            delete (CPairExt*)p;
            this->_items->delAt(i);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CmzMap::delAt(ULONGLONG pos) {
    CPairExt* p = (CPairExt*)this->_items->getAt(pos);
    if (p) {
        delete p;
        return this->_items->delAt(pos);
    }
    return FALSE;
}

LONGLONG CmzMap::pos(CPairExt* pair) {
    LONGLONG p;
    LONGLONG pair_LONGLONG = (LONGLONG)pair;
    ULONGLONG size = this->_items->size();
    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = items[i];
        if (p == pair_LONGLONG) {
            return i;
        }
    }
    return -1;
}

CPairExt* CmzMap::getAt(ULONGLONG pos) {
    if (pos < this->_items->size())
        return (CPairExt*)this->_items->getAt(pos);

    return NULL;
}

CPairExt* CmzMap::getFirst() {
    if (this->_items->size() > 0)
        return (CPairExt*)this->_items->getAt(0);

    return NULL;
}

CPairExt* CmzMap::getLast() {
    ULONGLONG size = this->_items->size();
    if (size > 0) {
        return (CPairExt*)this->_items->getAt(size - 1);
    }

    return NULL;
}

CPairExt* CmzMap::pop() {
    return (CPairExt*)this->_items->pop();
}

VOID CmzMap::clear() {
    if (this->_items) {
        CPairExt* p;
        ULONGLONG size = this->_items->size();
        LONGLONG* items = this->_items->getItemsBuf();

        for (ULONGLONG i = 0; i < size; i++) {
            p = (CPairExt*)items[i];
            delete p;
            items[i] = (LONGLONG)NULL;
        }
        this->_items->clear();
    }
}

CPairExt* CmzMap::findFirstItemByFirst(LONGLONG first, LONGLONG* foundPosition) {
    CPairExt* p;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        *foundPosition = -1;
        return NULL;
    }
    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p->first == first && p->firstType == TPairExtValueType_LONGLONG) {
            *foundPosition = i;
            return p;
        }
    }

    *foundPosition = -1;
    return NULL;
}

CPairExt* CmzMap::findFirstItemByFirst(CmzString key, LONGLONG* foundPosition) {
    CPairExt* p;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        *foundPosition = -1;
        return NULL;
    }
    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p->firstType == TPairExtValueType_PString) {
            CmzString* s = (CmzString*)p->first;
            if (s && *s == key) {
                *foundPosition = i;
                return p;
            }
        }
    }

    *foundPosition = -1;
    return NULL;
}

CmzArrayList* CmzMap::findAllItemsByFirst(LONGLONG first) {
    CPairExt* p;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        return NULL;
    }
    CmzArrayList* result = NULL;
    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p->first == first && p->firstType == TPairExtValueType_LONGLONG) {
            if (!result) {
                result = new CmzArrayList(this->size());
            }
            result->append((LONGLONG)p);
        }
    }

    return result;
}

CPairExt* CmzMap::findFirstItemByFirst(CmzObject* first, LONGLONG* foundPosition) {
    CPairExt* p;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        *foundPosition = -1;
        return NULL;
    }

    CmzObject* first_as_object = first;
    CPair* first_as_pair = dynamic_cast<CPair*>(first);
    CmzString* first_as_string = dynamic_cast<CmzString*>(first);

    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (first_as_string && p->firstType == TPairExtValueType_PString && first_as_string->equals((CmzObject*)p->first)) {
            *foundPosition = i;
            return p;
        }
        else if (first_as_pair && p->firstType == TPairExtValueType_PPair) {
            CPair* p_first_as_pair = dynamic_cast<CPair*>((CmzObject*)p->first);
            CObjectPair* p_first_as_objectpair = dynamic_cast<CObjectPair*>((CmzObject*)p->first);
            CPairExt* p_first_as_pairext = dynamic_cast<CPairExt*>((CmzObject*)p->first);

            if (p_first_as_pairext && first_as_pair->equals(p_first_as_pairext)) {
                *foundPosition = i;
                return p;
            }
            else if (p_first_as_objectpair && first_as_pair->equals(p_first_as_objectpair)) {
                *foundPosition = i;
                return p;
            }
            else if (p_first_as_pair && first_as_pair->equals(p_first_as_pair)) {
                *foundPosition = i;
                return p;
            }
        }
        else if (first_as_object && p->firstType == TPairExtValueType_PObject && first_as_object->equals((CmzObject*)p->first)) {
            *foundPosition = i;
            return p;
        }
    }

    *foundPosition = -1;
    return NULL;
}

CmzArrayList* CmzMap::findAllItemsByFirst(CmzObject* first) {
    CPairExt* p;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        return NULL;
    }

    CmzArrayList* result = NULL;
    CmzObject* first_as_object = first;
    CPair* first_as_pair = dynamic_cast<CPair*>(first);
    CmzString* first_as_string = dynamic_cast<CmzString*>(first);

    LONGLONG* items = this->_items->getItemsBuf();

    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (first_as_string && p->firstType == TPairExtValueType_PString && first_as_string->equals((CmzObject*)p->first)) {
            if (!result) {
                result = new CmzArrayList(this->size());
            }
            result->append((LONGLONG)p);
            continue;
        }
        else if (first_as_pair && p->firstType == TPairExtValueType_PPair) {
            CPair* p_first_as_pair = dynamic_cast<CPair*>((CmzObject*)p->first);
            CObjectPair* p_first_as_objectpair = dynamic_cast<CObjectPair*>((CmzObject*)p->first);
            CPairExt* p_first_as_pairext = dynamic_cast<CPairExt*>((CmzObject*)p->first);

            if (p_first_as_pairext && first_as_pair->equals(p_first_as_pairext)) {
                if (!result) {
                    result = new CmzArrayList(this->size());
                }
                result->append((LONGLONG)p);
                continue;
            }
            else if (p_first_as_objectpair && first_as_pair->equals(p_first_as_objectpair)) {
                if (!result) {
                    result = new CmzArrayList(this->size());
                }
                result->append((LONGLONG)p);
                continue;
            }
            else if (p_first_as_pair && first_as_pair->equals(p_first_as_pair)) {
                if (!result) {
                    result = new CmzArrayList(this->size());
                }
                result->append((LONGLONG)p);
                continue;
            }
        }
        else if (first_as_object && p->firstType == TPairExtValueType_PObject && first_as_object->equals((CmzObject*)p->first)) {
            if (!result) {
                result = new CmzArrayList(this->size());
            }
            result->append((LONGLONG)p);
            continue;
        }
    }

    return result;
}

BOOL CmzMap::erase(LONGLONG first) {
    LONGLONG position;
    CPairExt* p = this->findFirstItemByFirst(first, &position);
    if (p) {
        return this->delAt(position);
    }

    return FALSE;
}

BOOL CmzMap::erase(CmzObject* first) {
    LONGLONG position;
    CPairExt* p = this->findFirstItemByFirst(first, &position);
    if (p) {
        return this->delAt(position);
    }

    return FALSE;
}

ULONGLONG CmzMap::remove(LONGLONG first) {
    CPairExt* p;
    ULONGLONG deleted = 0;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        return 0;
    }
    LONGLONG* items = this->_items->getItemsBuf();
    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p && p->first == first) {
            delete p;
            deleted++;
            items[i] = 0;
        }
    }
    if (deleted > 0) {
        // usuwamy z tablicy wszystkie NULL-e
        this->_items->del(0);
    }
    return deleted;
}

ULONGLONG CmzMap::remove(CmzString second) {
    CPairExt* p;
    ULONGLONG deleted = 0;
    ULONGLONG size = this->_items->size();
    if (size < 1) {
        return 0;
    }
    LONGLONG* items = this->_items->getItemsBuf();
    for (ULONGLONG i = 0; i < size; i++) {
        p = (CPairExt*)items[i];
        if (p->secondType == TPairExtValueType_PString && p->getSecondAsString()->equals(&second)) {
            delete p;
            deleted++;
            items[i] = 0;
        }
    }
    if (deleted > 0) {
        // usuwamy z tablicy wszystkie NULL-e
        this->_items->del(0);
    }
    return deleted;
}

LONGLONG CmzMap::get(LONGLONG first, CPairExt** pair) {
    if (pair) {
        *pair = NULL;
    }

    if (this->_items) {
        CPairExt* p;
        ULONGLONG size = this->_items->size();
        LONGLONG* items = this->_items->getItemsBuf();

        for (ULONGLONG i = 0; i < size; i++) {
            p = (CPairExt*)items[i];
            if (p->first == first) {
                if (pair) {
                    *pair = p;
                }
                return p->second;
            }
        }
    }

    return MININT;
}

CmzObject* CmzMap::getAsObject(LONGLONG first, CPairExt** pair) {
    if (pair) {
        *pair = NULL;
    }
    if (!this->_items) {
        return NULL;
    }

    CPairExt* pairOut = NULL;
    LONGLONG second = this->get(first, &pairOut);
    if (pair) {
        *pair = pairOut;
    }
    if (second != MININT) {
        if (pairOut && pairOut->secondType == TPairExtValueType_PObject) {
            return (CmzObject*)second;
        }
    }

    return NULL;
}

CPair* CmzMap::getAsPair(LONGLONG first, CPairExt** pair) {
    if (pair) {
        *pair = NULL;
    }
    if (!this->_items) {
        return NULL;
    }

    CPairExt* pairOut = NULL;
    LONGLONG second = this->get(first, &pairOut);
    if (pair) {
        *pair = pairOut;
    }
    if (second != MININT) {
        if (pairOut && pairOut->secondType == TPairExtValueType_PPair) {
            return (CPair*)second;
        }
    }

    return NULL;
}

CObjectPair* CmzMap::getAsObjectPair(LONGLONG first, CPairExt** pair) {
    if (pair) {
        *pair = NULL;
    }
    if (!this->_items) {
        return NULL;
    }

    CPairExt* pairOut = NULL;
    LONGLONG second = this->get(first, &pairOut);
    if (pair) {
        *pair = pairOut;
    }
    if (second != MININT) {
        if (pairOut && pairOut->secondType == TPairExtValueType_PPair) {
            CObjectPair* p_obj = dynamic_cast<CObjectPair*>((CPair*)second);
            if (p_obj) {
                return p_obj;
            }
        }
    }

    return NULL;
}

CPairExt* CmzMap::getAsPairExt(LONGLONG first, CPairExt** pair) {
    CPairExt* pairOut = NULL;
    LONGLONG second = this->get(first, &pairOut);
    if (pair) {
        *pair = pairOut;
    }
    if (second != MININT) {
        if (pairOut && pairOut->secondType == TPairExtValueType_PPair) {
            CPairExt* p_ext = dynamic_cast<CPairExt*>((CPair*)second);
            if (p_ext) {
                return p_ext;
            }
        }
    }

    return NULL;
}

LONGLONG CmzMap::get(CmzObject* first, CPairExt** pair) {
    LONGLONG position;
    CPairExt* pair_out = this->findFirstItemByFirst(first, &position);
    if (pair_out) {
        if (pair) {
            *pair = pair_out;
        }
        return pair_out->second;
    }

    if (pair) {
        *pair = NULL;
    }
    return MININT;
}

CmzObject* CmzMap::getAsObject(CmzObject* first, CPairExt** pair) {
    LONGLONG position;
    CPairExt* pair_out = this->findFirstItemByFirst(first, &position);
    if (pair_out) {
        if (pair) {
            *pair = pair_out;
        }
        return pair_out->getSecondAsObject();
    }

    if (pair) {
        *pair = NULL;
    }
    return NULL;
}

CPair* CmzMap::getAsPair(CmzObject* first, CPairExt** pair) {
    LONGLONG position;
    CPairExt* pair_out = this->findFirstItemByFirst(first, &position);
    if (pair_out) {
        if (pair) {
            *pair = pair_out;
        }
        return pair_out->getFirstAsPair();
    }

    if (pair) {
        *pair = NULL;
    }
    return NULL;
}

CObjectPair* CmzMap::getAsObjectPair(CmzObject* first, CPairExt** pair) {
    LONGLONG position;
    CPairExt* pair_out = this->findFirstItemByFirst(first, &position);
    if (pair_out) {
        if (pair) {
            *pair = pair_out;
        }
        return pair_out->getSecondAsObjectPair();
    }

    if (pair) {
        *pair = NULL;
    }
    return NULL;
}

CPairExt* CmzMap::getAsPairExt(CmzObject* first, CPairExt** pair) {
    LONGLONG position;
    CPairExt* pair_out = this->findFirstItemByFirst(first, &position);
    if (pair_out) {
        if (pair) {
            *pair = pair_out;
        }
        return pair_out->getSecondAsPairExt();
    }

    if (pair) {
        *pair = NULL;
    }
    return NULL;
}

BOOL CmzMap::set(LONGLONG first, LONGLONG second) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(first, &position);
    if (pair != NULL) {
        pair->setSecond(second);
        return TRUE;
    }

    pair = new CPairExt(first, second);
    if (pair) {
        this->append(pair);
        return TRUE;
    }

    return FALSE;
}

BOOL CmzMap::set(LONGLONG first, CmzObject* second) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(first, &position);
    if (pair != NULL) {
        pair->setSecond(second);
        return TRUE;
    }

    pair = new CPairExt(first, second);
    if (pair) {
        this->append(pair);
        return TRUE;
    }

    return FALSE;
}

BOOL CmzMap::set(LONGLONG first, CmzString second) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(first, &position);
    if (pair != NULL) {
        pair->setSecond(second);
        return TRUE;
    }

    pair = new CPairExt(first, second);
    if (pair) {
        this->append(pair);
        return TRUE;
    }

    return FALSE;
}

BOOL CmzMap::set(CmzObject* first, LONGLONG second) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(first, &position);
    if (pair != NULL) {
        pair->setSecond(second);
        return TRUE;
    }

    pair = new CPairExt(first, second);
    if (pair) {
        this->append(pair);
        return TRUE;
    }

    return FALSE;
}

BOOL CmzMap::set(CmzObject* first, CmzObject* second) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(first, &position);
    if (pair != NULL) {
        pair->setSecond(second);
        return TRUE;
    }

    pair = new CPairExt(first, second);
    if (pair) {
        this->append(pair);
        return TRUE;
    }

    return FALSE;
}

BOOL CmzMap::set(CmzObject* first, CmzString second) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(first, &position);
    if (pair != NULL) {
        pair->setSecond(second);
        return TRUE;
    }

    pair = new CPairExt(first, second);
    if (pair) {
        this->append(pair);
        return TRUE;
    }

    return FALSE;
}

LONGLONG CmzMap::count(LONGLONG first) {
    LONGLONG result = 0;
    CmzArrayList* found_items = this->findAllItemsByFirst(first);
    if (found_items) {
        result = found_items->size();
        deleteArrayListOfPairExt(found_items);
        found_items = NULL;
    }
    return result;
}

LONGLONG CmzMap::count(CPair* first) {
    LONGLONG result = 0;
    CmzArrayList* found_items = this->findAllItemsByFirst(first);
    if (found_items) {
        result = found_items->size();
        deleteArrayListOfPairExt(found_items);
        found_items = NULL;
    }
    return result;
}

VOID CmzMap::set(CmzString key, CmzString value) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(key, &position);
    if (pair != NULL) {
        pair->setSecond(value);
        return;
    }

    pair = new CPairExt(key, value);
    if (pair) {
        this->append(pair);
    }
}

CmzString CmzMap::get(CmzString key) {
    LONGLONG position;
    CPairExt* pair = this->findFirstItemByFirst(key, &position);
    if (pair != NULL) {
        CmzString* s = pair->getSecondAsString();
        if (s) {
            return *s;
        }
    }

    return COMMON_EMPTY_STRING;
}

CmzStringList* CmzMap::getKeys() {
    CmzStringList* lst = new CmzStringList();
    if (lst) {
        CPairExt* p;
        ULONGLONG size = this->_items->size();
        if (size < 1) {
            return lst;
        }
        
        LONGLONG* items = this->_items->getItemsBuf();
        CmzString* s;
        for (ULONGLONG i = 0; i < size; i++) {
            p = (CPairExt*)items[i];
            if (p->firstType == TPairExtValueType_PString) {
                s = (CmzString*)p->first;
                lst->append(*s);
            }
        }
    }
    
    return lst;
}

CmzStringList* CmzMap::getValues() {
    CmzStringList* lst = new CmzStringList();
    if (lst) {
        CPairExt* p;
        ULONGLONG size = this->_items->size();
        if (size < 1) {
            return lst;
        }
        
        LONGLONG* items = this->_items->getItemsBuf();
        CmzString* s;
        for (ULONGLONG i = 0; i < size; i++) {
            p = (CPairExt*)items[i];
            if (p->secondType == TPairExtValueType_PString) {
                s = (CmzString*)p->second;
                lst->append(*s);
            }
        }
    }
    
    return lst;
}

CmzMap* CmzMap::clone() {
    CmzMap* retval = new CmzMap();
    if (retval) {
        CPairExt* p;
        CPairExt* clonedP;
        ULONGLONG size = this->_items->size();
        if (size > 0) {
            LONGLONG* items = this->_items->getItemsBuf();
            for (ULONGLONG i = 0; i < size; i++) {
                p = (CPairExt*)items[i];
                if (p) {
                    clonedP = p->clone();
                    retval->_items->append((LONGLONG)clonedP);
                }
            }
        }
    }
    
    return retval;
}

// ***********************************************************************************************
// ***********************************************************************************************
