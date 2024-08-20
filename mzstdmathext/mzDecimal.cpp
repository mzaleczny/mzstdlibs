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
#include "pch.h"
#include "mzDecimal.h"
#include "mzString.h"
#include <wchar.h>
#include "mzFileUtils.h"

using namespace mzstd;

VOID CDecimal::_commonInit() {
    this->setRealClassName(L"CDecimal");
}

CDecimal::CDecimal(USHORT precision)
    : _integerPart(0), _fractionPart(0), _precision(precision), _sign(FALSE)
{
    this->_commonInit();
}

CDecimal::CDecimal(LONG integerPart, ULONG fractionPart, USHORT precision)
    : _fractionPart(fractionPart), _precision(precision)
{
    this->_commonInit();
    if (integerPart >= 0) {
        this->_integerPart = integerPart;
        this->_sign = FALSE;
    } else {
        this->_integerPart = -integerPart;
        this->_sign = TRUE;
    }
}

CDecimal::CDecimal(const CDecimal& d) {
    this->_commonInit();
    this->_integerPart = d._integerPart;
    this->_fractionPart = d._fractionPart;
    this->_precision = d._precision;
    this->_sign = d._sign;
}

CDecimal::CDecimal(CDecimal* d) {
    this->_commonInit();
    this->_integerPart = d->_integerPart;
    this->_fractionPart = d->_fractionPart;
    this->_precision = d->_precision;
    this->_sign = d->_sign;
}

CDecimal::CDecimal(LONG value, USHORT precision) {
    this->_commonInit();
    if (value >= 0) {
        this->_integerPart = value;
        this->_sign = FALSE;
    } else {
        this->_integerPart = -value;
        this->_sign = TRUE;
    }

    this->_fractionPart = 0;
    this->_precision = precision;
}

CDecimal::CDecimal(double value, USHORT precision) {
    this->_commonInit();
    CmzString s((LONG)value, 12);
    s.rtrim(L'0');
    *this = this->_fromStr(s.getBuffer(), precision);
}

CDecimal::CDecimal(WCHAR* fromStr, USHORT precision) {
    this->_commonInit();
    *this = this->_fromStr(fromStr, precision);
}

CDecimal::~CDecimal() {
}

CDecimal CDecimal::_fromStr(WCHAR* fromStr, USHORT precision) {
    size_t len = wcslen(fromStr);
    size_t buf_size = len + 1;
    CDecimal result;
    BOOL negative = fromStr[0] == L'-';
    if (negative) {
        fromStr++;
        len -= 1;
    }

    WCHAR* buf = new WCHAR[buf_size];
    if (!buf) {
        return result;
    }

    if (isDouble(fromStr)) {
        wcscpy_s(buf, buf_size, fromStr);
        for (LONG i = 0; i < len; i++) {
            if (buf[i] == L',') {
                buf[i] = L'.';
            }
        }

        WCHAR* dot = wcsstr(buf, L".");
        if (dot) {
            dot[0] = NULL;
            result._integerPart = _wtoi(buf);
            if (dot[1] != NULL) {
                result._fractionPart = _wtoi(&dot[1]);
                if (precision == 0) {
                    result._precision = static_cast<USHORT>(wcslen(&dot[1]));
                } else {
                    result._precision = precision;
                }
            } else {
                result._fractionPart = 0;
                result._precision = precision;
            }
        } else {
            result._integerPart = _wtoi(buf);
            result._fractionPart = 0;
            result._precision = 0;
        }
    } else {
        //fwprintf(stderr, L"ERROR: Invalid parameter: '%s'. Not a float number.\n", str);
        //exit(1);
        result._integerPart = 0;
        result._fractionPart = 0;
        result._precision = precision;
        result._sign = FALSE;
    }

    delete[] buf;

    result._sign = negative;
    return result;
}


BOOL CDecimal::equals(CObject* obj) {
    CDecimal* d = dynamic_cast<CDecimal*>(obj);
    if (d) {
        return this->_integerPart == d->_integerPart &&
               this->_fractionPart == d->_fractionPart &&
               this->_precision == d->_precision &&
               this->_sign == d->_sign;
    }

    return FALSE;
}

CmzString CDecimal::toString() {
    LONG buf_size = 256;
    WCHAR* buf = new WCHAR[buf_size];
    if (!buf) {
        return COMMON_EMPTY_STRING;
    }

    _itow(this->_integerPart, buf, 10);
    if (this->_precision > 0) {
        wcscat(buf, L".");
        size_t len = wcslen(buf);

        WCHAR* fractionBuf = new WCHAR[256];
        if (!fractionBuf) {
            CmzString s = CmzString(buf, buf_size);
            delete[] buf;
            return s;
        }

        _itow(this->_fractionPart, fractionBuf, 10);
        size_t fractionLen = wcslen(fractionBuf);
        size_t add_zeros_count = (this->_precision > fractionLen) ? this->_precision - fractionLen : 0;

        while (add_zeros_count > 0) {
            buf[len] = L'0';
            len++;
            add_zeros_count--;
        }

        buf[len] = NULL;
        wcscat(buf, fractionBuf);
        delete[] fractionBuf;
    }

    CmzString s(buf, buf_size);
    if (this->_sign) {
        s.prepend(L'-');
    }

    return s;
}

/*
BOOL CDecimal::operator==(CDecimal &d) {
    if (this->_sign != d._sign) {
        return FALSE;
    }

    LONG maxPrecision = this->_precision > d._precision ? this->_precision : d._precision;
    ULONG fraction1 = this->_getFractionPartWithPrecision(maxPrecision);
    ULONG fraction2 = d._getFractionPartWithPrecision(maxPrecision);

    return this->_integerPart == d._integerPart &&
           fraction1 == fraction2;
}

BOOL CDecimal::operator==(LONG value) {
    return *this == CDecimal(value);
}

BOOL CDecimal::operator==(double value) {
    return *this == CDecimal(value);
}

BOOL CDecimal::operator!=(CDecimal &d) {
    if (this->_sign != d._sign) {
        return TRUE;
    }

    LONG maxPrecision = this->_precision > d._precision ? this->_precision : d._precision;
    ULONG fraction1 = this->_getFractionPartWithPrecision(maxPrecision);
    ULONG fraction2 = d._getFractionPartWithPrecision(maxPrecision);

    return this->_integerPart != d._integerPart ||
           fraction1 != fraction2;
}

BOOL CDecimal::operator!=(LONG value) {
    return *this != CDecimal(value);
}

BOOL CDecimal::operator!=(double value) {
    return *this != CDecimal(value);
}
*/

CDecimal& CDecimal::operator=(CDecimal &d) {
    this->_commonInit();
    this->_integerPart = d._integerPart;
    this->_fractionPart = d._fractionPart;
    this->_precision = d._precision;
    this->_sign = d._sign;

    return *this;
}
/*
CDecimal& CDecimal::operator=(LONG value) {
    *this = CDecimal(value);
    return *this;
}

CDecimal& CDecimal::operator=(double value) {
    *this = CDecimal(value);
    return *this;
}

CDecimal CDecimal::operator+(CDecimal &d) {
    CDecimal min;
    CDecimal max;
    CDecimal result;

    if (*this <= d) {
        min = *this;
        max = d;
    } else {
        min = d;
        max = *this;
    }

    LONG integer_part;
    ULONG fraction_part;
    LONG maxPrecision = max._precision > min._precision ? max._precision : min._precision;

    if (maxPrecision == 0) {
        result._precision = 0;
        result._fractionPart = 0;
        if (min._sign == max._sign) {
            result._sign = min._sign;
            result._integerPart = min._integerPart + max._integerPart;
        } else {
            result._sign = FALSE;
            result._integerPart = max._integerPart - min._integerPart;
            if (result._integerPart < 0) {
                result._integerPart = -result._integerPart;
                result._sign = TRUE;
            }
        }
        return result;
    }

    ULONG fractionMin = min._getFractionPartWithPrecision(maxPrecision);
    ULONG fractionMax = max._getFractionPartWithPrecision(maxPrecision);

    // we add integer parts
    if (min._sign == max._sign) {
        integer_part = min._integerPart + max._integerPart;
        if (min._integerPart >= 0) {
            fraction_part = fractionMin + fractionMax;
            // below code handles eventual carry 1 to _integerPart value during adding both fraction parts
            LONG frontier_fraction_value = 10;
            for (LONG i = 1; i < maxPrecision; i++) {
                frontier_fraction_value *= 10;
            }
            if (fraction_part >= frontier_fraction_value) {
                fraction_part -= frontier_fraction_value;
                integer_part += 1;
            }
        }

        CDecimal result(integer_part, fraction_part, maxPrecision);
        result._sign = min._sign;
        return result;
    }

    result._sign = FALSE;
    // We carry 1 into max._fractionPart, to ensure that max._fractionPart - min._fractionPart will be positive
    max._moveUnitsFromIPartToFPart(1);
    fractionMax = max._getFractionPartWithPrecision(maxPrecision);
    max._fractionPart = fractionMax - fractionMin;
    max._precision = maxPrecision;
    min._fractionPart = 0;
    // below code handles eventual carry 1 to _integerPart value
    max._correctFractionPart();

    if (max._integerPart >= min._integerPart) {
        result._integerPart = max._integerPart - min._integerPart;
        result._fractionPart = max._fractionPart;
        result._precision = maxPrecision;
        result._sign = FALSE;
        return result;
    }

    fraction_part = max._fractionPart;
    min._integerPart -= max._integerPart;
    if (fraction_part > 0) {
        result._integerPart = min._integerPart - 1;
        result._fractionPart = this->_unitClosure(fraction_part, maxPrecision);
        result._precision = maxPrecision;
        result._sign = TRUE;
        return result;
    }

    result._integerPart = min._integerPart;
    result._fractionPart = 0;
    result._precision = maxPrecision;
    result._sign = TRUE;

    return result;
}

CDecimal CDecimal::operator+(LONG value) {
    CDecimal d(value);
    return *this + d;
}

CDecimal CDecimal::operator+(double value) {
    CDecimal d(value);
    return *this + d;
}

CDecimal& CDecimal::operator+=(CDecimal &d) {
    *this = *this + d;
    return *this;
}

CDecimal& CDecimal::operator+=(LONG value) {
    CDecimal d(value);
    *this += d;
    return *this;
}

CDecimal& CDecimal::operator+=(double value) {
    CDecimal d(value);
    *this += d;
    return *this;
}

CDecimal& CDecimal::operator-=(CDecimal &d) {
    CDecimal md(d);
    md._sign = !md._sign;
    *this += md;
    return *this;
}

CDecimal& CDecimal::operator-=(LONG value) {
    CDecimal md(value);
    md._sign = !md._sign;
    *this += md;
    return *this;
}

CDecimal& CDecimal::operator-=(double value) {
    CDecimal md(value);
    md._sign = !md._sign;
    *this += md;
    return *this;
}

CDecimal CDecimal::operator-(CDecimal &d) {
    CDecimal d2(d);
    d2._sign = !d2._sign;
    return *this + d2;
}

CDecimal CDecimal::operator-(LONG value) {
    CDecimal d(value);
    d._sign -= !d._sign;
    return *this + d;
}

CDecimal CDecimal::operator-(double value) {
    CDecimal d(value);
    d._sign -= !d._sign;
    return *this + d;
}

CDecimal CDecimal::operator*(LONG value) {
    CDecimal result(this);

    if (value == 0) {
        result._integerPart = 0;
        result._fractionPart = 0;
        result._sign = FALSE;
        return result;
    }

    LONG count = value > 0 ? value : -value;
    for (LONG k = 1; k < count; k++) {
        result += *this;
    }

    return result;
}

CDecimal& CDecimal::operator*=(LONG value) {
    *this = *this * value;
    return *this;
}

CDecimal::operator WCHAR*() {
    static CmzString s;
    
    s = this->toString();
    return s.getBuffer();
}

BOOL CDecimal::operator>(CDecimal& d) {
    if (this->_sign != d._sign) {
        if (d._sign) {
            return TRUE;
        }
        return FALSE;
    } else {
        BOOL thisAbsValueGreater = FALSE;

        if (this->_integerPart > d._integerPart) {
            thisAbsValueGreater = TRUE;
        } else if (this->_integerPart == d._integerPart) {
            LONG delta_precision;
            LONG maxPrecision = this->_precision > d._precision ? this->_precision : d._precision;
            ULONG fractionPart1 = this->_getFractionPartWithPrecision(maxPrecision);
            ULONG fractionPart2 = d._getFractionPartWithPrecision(maxPrecision);

            if (fractionPart1 > fractionPart2) {
                thisAbsValueGreater = TRUE;
            }
        }

        if (!this->_sign) {
            if (thisAbsValueGreater) {
                return TRUE;
            }
        } else {
            if (!thisAbsValueGreater) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL CDecimal::operator>(LONG value) {
    return *this > CDecimal(value);
}

BOOL CDecimal::operator>(double value) {
    return *this > CDecimal(value);
}

BOOL CDecimal::operator>=(CDecimal& d) {
    return *this == d || *this > d;
}

BOOL CDecimal::operator>=(LONG value) {
    return *this == value || *this > value;
}

BOOL CDecimal::operator>=(double value) {
    return *this == value || *this > value;
}

BOOL CDecimal::operator<(CDecimal& d) {
    return !(*this >= d);
}

BOOL CDecimal::operator<(LONG value) {
    return !(*this >= value);
}

BOOL CDecimal::operator<(double value) {
    return !(*this >= value);
}

BOOL CDecimal::operator<=(CDecimal& d) {
    return !(*this > d);
}

BOOL CDecimal::operator<=(LONG value) {
    return !(*this > value);
}

BOOL CDecimal::operator<=(double value) {
    return !(*this > value);
}

ULONG CDecimal::_getFractionPartWithPrecision(USHORT required_precision) {
    if (required_precision <= this->_precision) {
        return this->_fractionPart;
    }

    ULONG fraction = this->_fractionPart;
    LONG delta_precision = required_precision - this->_precision;
    LONG mul = 10;
    for (LONG i = 1; i < delta_precision; i++) {
        mul *= 10;
    }
    fraction *= mul;

    return fraction;
}

BOOL CDecimal::_moveUnitsFromIPartToFPart(USHORT units) {
    if (this->_precision == 0) {
        return FALSE;
    }

    LONG frontier_number = 10;
    for (LONG i = 1; i < this->_precision; i++) {
        frontier_number *= 10;
    }

    frontier_number *= units;
    this->_fractionPart += frontier_number;
    this->_integerPart -= units;
    if (this->_integerPart < 0) {
        this->_integerPart = -this->_integerPart;
        this->_sign = TRUE;
    }
    return TRUE;
}

ULONG CDecimal::_unitClosure(ULONG fraction, USHORT precision) {
    ULONG frontier_number = 10;
    LONG i;
    for (i = 1; i < precision; i++) {
        frontier_number *= 10;
    }
    return frontier_number - fraction;
}

VOID CDecimal::_correctFractionPart() {
    LONG frontier_number = 10;
    for (LONG i = 1; i < this->_precision; i++) {
        frontier_number *= 10;
    }

    while (this->_fractionPart >= frontier_number) {
        if (!this->_sign) {
            this->_integerPart += 1;
        } else {
            this->_integerPart -= 1;
            if (this->_integerPart < 0) {
                this->_integerPart = -this->_integerPart;
                this->_sign = FALSE;
            }
        }
        this->_fractionPart -= frontier_number;
    }
}
*/
