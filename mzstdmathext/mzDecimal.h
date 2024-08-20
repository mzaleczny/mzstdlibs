/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdmathext
 * Version: 1.05
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdmathext library
 * 
 * mzstdmathext library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdmathext library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZ_DECIMAL_H__
#define __MZ_DECIMAL_H__

#include "mzstdmathextCommon.h"
#include "mzObject.h"

class DECLSPEC_MZSTDMATHEXT CDecimal : public mzstd::CmzObject {
public:
	WINAPI CDecimal(USHORT precision = 2);
	WINAPI CDecimal(LONG integerPart, ULONG fractionPart, USHORT precision = 2);
    // copy-constructor
	WINAPI CDecimal(const CDecimal& d);
	WINAPI CDecimal(CDecimal* d);
	WINAPI CDecimal(LONG value, USHORT precision = 2);
    // Set precision below to 0 to take precision from the fromStr argument.
    // Another value sets arbitrary precision.
	WINAPI CDecimal(double value, USHORT precision = 0);
    
    // Set precision below to 0 to take precision from the fromStr argument.
    // Another value sets arbitrary precision.
    WINAPI CDecimal(WCHAR* fromStr, USHORT precision = 0);
	virtual WINAPI ~CDecimal();

    // Comparison below is stronger than operator== because it compares all state of objects
    // (includeing _precision value)
    virtual BOOL WINAPI equals(CObject* obj);
    virtual mzstd::CmzString WINAPI toString();
    inline VOID WINAPI setPrecision(USHORT precision) {
        this->_precision = precision;
    }
    
	CDecimal& WINAPI operator=(CDecimal &d);

/*
	// przeciążone operatory
    BOOL WINAPI operator==(CDecimal &d);
    BOOL WINAPI operator==(LONG value);
    BOOL WINAPI operator==(double value);
    BOOL WINAPI operator!=(CDecimal &d);
    BOOL WINAPI operator!=(LONG value);
    BOOL WINAPI operator!=(double value);
	CDecimal& WINAPI operator=(LONG value);
	CDecimal& WINAPI operator=(double value);
	CDecimal& WINAPI operator+=(CDecimal &d);
	CDecimal& WINAPI operator+=(LONG value);
	CDecimal& WINAPI operator+=(double value);
	CDecimal WINAPI operator+(CDecimal &d);
	CDecimal WINAPI operator+(LONG value);
	CDecimal WINAPI operator+(double value);
	CDecimal& WINAPI operator-=(CDecimal &d);
	CDecimal& WINAPI operator-=(LONG value);
	CDecimal& WINAPI operator-=(double value);
	CDecimal WINAPI operator-(CDecimal &d);
	CDecimal WINAPI operator-(LONG value);
	CDecimal WINAPI operator-(double value);
	CDecimal& WINAPI operator*=(LONG value);
	CDecimal WINAPI operator*(LONG value);
	BOOL WINAPI operator>(CDecimal& d);
	BOOL WINAPI operator>(LONG value);
	BOOL WINAPI operator>(double value);
	BOOL WINAPI operator>=(CDecimal& d);
	BOOL WINAPI operator>=(LONG value);
	BOOL WINAPI operator>=(double value);
	BOOL WINAPI operator<(CDecimal& d);
	BOOL WINAPI operator<(LONG value);
	BOOL WINAPI operator<(double value);
	BOOL WINAPI operator<=(CDecimal& d);
	BOOL WINAPI operator<=(LONG value);
	BOOL WINAPI operator<=(double value);

    WINAPI operator WCHAR*();
  */
public:
    LONG    _integerPart;
    ULONG   _fractionPart;
    USHORT  _precision;
    // _sign: TRUE if positive, FALSE otherwise
    BOOL    _sign;
    
    // Set precision below to 0 to take precision from the fromStr argument.
    // Another value sets arbitrary precision.
    CDecimal WINAPI _fromStr(WCHAR* fromStr, USHORT precision = 0);
    
/*
    // Returns fraction part with given precision. If this->_precision is greater than required_precision then
    // this->_fractionPart with this->_precision is returned.
    ULONG WINAPI _getFractionPartWithPrecision(USHORT required_precision);
    
    // moves specified number of units from _integerPart to _fractionPart.
    // returns TRUE if operation was successfull.
    BOOL WINAPI _moveUnitsFromIPartToFPart(USHORT units);
    
    // Return unit closure for fraction taking into account precision. ex.:
    // fraction=2, precision=1 => result = 8
    // fraction=23, precision=2 => result = 77
    ULONG WINAPI _unitClosure(ULONG fraction, USHORT precision);
    
    // if this->_fractionPart is greater than its greater possible value according to this->_precision, then
    // overflow is carried to this->_integerPart and the this->_fractionPart is appropriately decreased.
    VOID WINAPI _correctFractionPart();    
*/    
    VOID WINAPI _commonInit();
};
typedef CDecimal* PDecimal;

#endif
