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
#include "mzMath.h"
#include "vmath.h"

using namespace mzstd;

//---------------------------------------------------------------------------
// Zamienia ona liczbę na jej wyrażenie słowne
CmzString WINAPI SlowTrzy(LONG i) {
    CmzString tmp;
    CmzString licz;

    if (i > 999 || i < 0) { //Jeśli błędna wartość nic nie rób
        return L"";
    }

    if (i == 0) { //jeśli zero, to zwróć >>zero<<
        return L"zero";
    }

    licz = i;
    // Ustaw tak wartość zmiennej licz, aby zawsze była 3-cyfrowa
    switch(licz.length())
    {
    case 1: licz = CmzString(L"00") + licz;  break;
    case 2: licz = CmzString(L"0") + licz;   break;
    }//switch
    tmp = L"";

    // Ustal ilość setek i wpisz odpowiednią nazwę do zmiennej tmp
    if (licz[0] == L'1') {
        tmp = L"sto";
    } else if (licz[0] == L'2') {
        tmp = L"dwieście";
    } else if (licz[0] == L'3') {
        tmp = L"trzysta";
    } else if (licz[0] == L'4') {
        tmp = L"czterysta";
    } else if (licz[0] == L'5') {
        tmp = L"pięćset";
    } else if (licz[0] == L'6') {
        tmp = L"sześćset";
    } else if (licz[0] == L'7') {
        tmp = L"siedemset";
    } else if (licz[0] == L'8') {
        tmp = L"osiemset";
    } else if (licz[0] == L'9') {
        tmp = L"dziewięćset";
    } else if (licz[0] == L'0') {
        tmp = L"";
    }
    // Jeśli tmp nie jest już puste, to na końcu dopisz spację
    if (tmp != L"") {
        tmp += L" ";
    }

    // Ustal ilość dziesiątek i dopisz odpowiednią ilość dziesiątek do tmp
    if( licz[1] == L'1' )
    {
        //dla 1 dziesiątki będą brzmieć inaczej :
        if (licz[2] == L'0') {
            tmp += L"dziesięć";
        } else if (licz[2] == L'1') {
            tmp += L"jedenaście";
        } else if (licz[2] == L'2') {
            tmp += L"dwanaście";
        } else if(licz[2] == L'3') {
            tmp += L"trzynaście";
        } else if (licz[2] == L'4') {
            tmp += L"czternaście";
        } else if (licz[2] == L'5') {
            tmp += L"piętnaście";
        } else if (licz[2] == L'6') {
            tmp += L"szesnaście";
        } else if (licz[2] == L'7') {
            tmp += L"siedemnaście";
        } else if (licz[2] == L'8') {
            tmp += L"osiemnaście";
        } else if (licz[2] == L'9') {
            tmp += L"dziewiętnaście";
        }
        return tmp;
    }//if
    else if (licz[1] == L'2') {
        tmp += L"dwadzieścia";
    } else if (licz[1] == L'3') {
        tmp += L"trzydzieści";
    } else if (licz[1] == L'4') {
        tmp += L"czterdzieści";
    } else if (licz[1] == L'5') {
        tmp += L"pięćdziesiąt";
    } else if (licz[1] == L'6') {
        tmp += L"sześćdziesiąt";
    } else if (licz[1] == L'7') {
        tmp += L"siedemdziesiąt";
    } else if (licz[1] == L'8') {
        tmp += L"osiemdziesiąt";
    } else if (licz[1] == L'9') {
        tmp += L"dziewięćdziesiąt";
    } else if (licz[1] == L'0') {
        tmp += L"";
    }
    
    // Jeśli tmp nie jest puste i nie kończy się spacją, to dopisz spację
    if (tmp != L"" && tmp[2] != L'0') {
        tmp += L" ";
    }
    
    // Ustal cyfrę jedności
    if (licz[2] == L'1') {
        tmp += L"jeden";
    } else if (licz[2] == L'2') {
        tmp += L"dwa";
    } else if (licz[2] == L'3') {
        tmp += L"trzy";
    } else if (licz[2] == L'4') {
        tmp += L"cztery";
    } else if (licz[2] == L'5') {
        tmp += L"pięć";
    } else if (licz[2] == L'6') {
        tmp += L"sześć";
    } else if (licz[2] == L'7') {
        tmp += L"siedem";
    } else if (licz[2] == L'8') {
        tmp += L"osiem";
    } else if (licz[2] == L'9') {
        tmp += L"dziewięć";
    } else if (licz[2] == L'0') {
        tmp += L"";
    }
    tmp.trim();

    return tmp;
}//SlowTrzy

//---------------------------------------------------------------------------
// Zamienia ona liczbę na jej wyrażenie słowne
CmzString WINAPI SlowSzesc(LONG i) {
    CmzString tmp;
    CmzString licz;
    CmzString result;
    CmzString pom;

    if( i < 0 || i > 999999 )
        return L"";
    
    if( i == 0 )
        return L"zero";
    
    licz = CmzString(i);
    switch( licz.length()) // zadbaj, aby liczba była sześciocyfrowa
    {
    case 1: licz = CmzString(L"00000") + licz;    break;
    case 2: licz = CmzString(L"0000") + licz;     break;
    case 3: licz = CmzString(L"000") + licz;      break;
    case 4: licz = CmzString(L"00") + licz;       break;
    case 5: licz = CmzString(L"0") + licz;        break;
    case 6: licz = licz;              break;
    }// switch

    // Ustal ilość tysięcy PLN
    pom = licz.substr(0, 3);
    tmp = SlowTrzy(pom.toInt());
    if (tmp == L"" || tmp == L"zero") {
        tmp = L"";
    } else {
        if (pom[1] != L'1'  && (pom[2] == L'2' || pom[2] == L'3' || pom[2] == L'4')) {
            tmp += L" tysiące";
        } else if (tmp == L"jeden") {
            tmp = L" tysiąc";
        } else {
            tmp += L" tysięcy";
        }
    }//else
    result = tmp;
    
    // Jeśli result nie jest pusty, to dodaj spację na końcu
    if (result != L"") {
        result += L" ";
    }
    
    // Ustal pozostałą ilość PLN
    tmp = SlowTrzy(licz.substr(3, 3).toInt());
    if (tmp == L"" || tmp == L"zero") {
        tmp = L"";
    }
    result += tmp;
    result.trim();
    return result;
}// SlowSzesc

//---------------------------------------------------------------------------
//Zamienia liczbę dziewięciocyfrową na opisujący ją znak
CmzString WINAPI SlowDziewiec(LONG i) {
    CmzString tmp;
    CmzString licz;
    CmzString result;
    CmzString pom;

    if (i < 0 || i > 999999999) {
        return L"";
    }

    if (i == 0) {
        return L"zero";
    }

    result = L"";
    tmp = L"";
    licz = i;

    switch (licz.length())   // zadbaj, aby liczba była dziewieciocyfrowa
    {
    case 1: licz = CmzString(L"00000000") + licz;  break;
    case 2: licz = CmzString(L"0000000") + licz;   break;
    case 3: licz = CmzString(L"000000") + licz;    break;
    case 4: licz = CmzString(L"00000") + licz;     break;
    case 5: licz = CmzString(L"0000") + licz;      break;
    case 6: licz = CmzString(L"000") + licz;       break;
    case 7: licz = CmzString(L"00") + licz;        break;
    case 8: licz = CmzString(L"0") + licz;         break;
    case 9: licz = licz;               break;
    }//switch

    // Ustal ilość milionów PLN
    pom = licz.substr(0, 3);
    tmp = SlowTrzy(pom.toInt());

    if (tmp == L"" || tmp == L"zero") {
        tmp = L"";
    } else {
        if (pom[1] != L'1' && (pom[2] == L'2' || pom[2] == L'3' || pom[2] == L'4' )) {
            tmp += L" miliony";
        } else if (tmp == L"jeden") {
            tmp.replace(L"jeden", L"", 0, 1);
            tmp += L" milion";
        } else {
          tmp += L" milionów";
        }
    }//else

    result = tmp;
    // Jeśli result nie jest pusty, to dodaj spację na końcu
    if( result != L"" ) {
        result += L" ";
    }
    
    // Ustal ilość tysięcy PLN
    pom = licz.substr(3, 3);
    tmp = SlowTrzy(pom.toInt());

    if (tmp == L"" || tmp == L"zero") {
        tmp = L"";
    } else {
        if( pom[1] != L'1' && (pom[2] == L'2' || pom[2] == L'3' || pom[2] == L'4')) {
            tmp += L" tysiące";
        } else if (tmp == L"jeden") {
            tmp.replace(L"jeden", L"", 0, 1);
            tmp += L" tysiąc";
        } else {
            tmp += " tysięcy";
        }
    }//else
    result += tmp;

    // Ježli result nie jest pusty, to dodaj spację na końcu
    if (result != L"") {
        result += L" ";
    }

// Ustal pozostałą ilość PLN
    tmp = SlowTrzy( licz.substr(6, 3).toInt());
    if (tmp == L"" || tmp == L"zero") {
        tmp = L"";
    }
    result += tmp;
    result.trim();
    return result;
}//SlowDziewiec
