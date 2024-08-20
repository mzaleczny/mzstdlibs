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

#ifndef __MZSTD_MAP_H__
#define __MZSTD_MAP_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzArrayList.h"
#include "mzStringList.h"

namespace mzstd {

    class CPair;
    class CObjectPair;
    enum TPairExtValueType;
    class CPairExt;

    DECLSPEC_MZSTD VOID WINAPI swap(LONGLONG& val1, LONGLONG& val2);
    DECLSPEC_MZSTD VOID WINAPI swap(BOOL& val1, BOOL& val2);
    DECLSPEC_MZSTD VOID WINAPI swap(CPair* pair1, CPair* pair2);
    DECLSPEC_MZSTD VOID WINAPI swap(TPairExtValueType& val1, TPairExtValueType& val2);

    // Funkcja zwalnia wszystkie elementy listy lst zakładając, że każdy z nich jest wskaźnikiem na
    // obiekt CPairExt. Na koniec czyści listę i zwalnia samą listę.
    DECLSPEC_MZSTD VOID WINAPI deleteArrayListOfPairExt(CmzArrayList* lst);


    // Klasa CPair
    class DECLSPEC_MZSTD CPair : public CmzObject {
    public:
        LONGLONG first;	// pierwsza przechowywana wartość
        LONGLONG second;	// druga przechowywana wartość

        // konstruktor domyślny
        WINAPI CPair();
        // konstruktor kopiujący
        WINAPI CPair(const CPair& pair);
        // konstruktor ze wskaźnika
        WINAPI CPair(CPair* pair);
        // konstruktor z podanych wartości
        WINAPI CPair(LONGLONG first, LONGLONG second);
        virtual WINAPI ~CPair();

        // Wymienia zawartość tego obiektu z podanym w parametrze obiektem.
        VOID WINAPI swap(CPair* pair);
        // Zwraca TRUE jeśli ten obiekt i obiekt podany w parametrze wskazują na ten sam obiekt,
        // lub gdy są to dwa różne obiekty ale o takich samych wartościach _first i _second
        // odpowiednio.
        virtual BOOL WINAPI equals(CPair* pair);
        virtual BOOL WINAPI equals(CObjectPair* pair);
        virtual BOOL WINAPI equals(CPairExt* pair);
        virtual BOOL WINAPI equals(CmzObject* obj);


        inline virtual CmzObject* WINAPI getFirstAsObject() { return NULL; }
        inline virtual CPair* WINAPI getFirstAsPair() { return NULL; }
        inline virtual CObjectPair* WINAPI getFirstAsObjectPair() { return NULL; }
        inline virtual CPairExt* WINAPI getFirstAsPairExt() { return NULL; }
        inline virtual CmzString* WINAPI getFirstAsString() { return NULL; }

        inline virtual CmzObject* WINAPI getSecondAsObject() { return NULL; }
        inline virtual CPair* WINAPI getSecondAsPair() { return NULL; }
        inline virtual CObjectPair* WINAPI getSecondAsObjectPair() { return NULL; }
        inline virtual CPairExt* WINAPI getSecondAsPairExt() { return NULL; }
        inline virtual CmzString* WINAPI getSecondAsString() { return NULL; }

        CPair& WINAPI operator=(CPair& pair);
        BOOL WINAPI operator==(CPair& pair);
        BOOL WINAPI operator!=(CPair& pair);

    protected:
        VOID WINAPI _commonInit();
    }; // class CPair
    typedef CPair* PPair;



    // Klasa CObjectPair
    class DECLSPEC_MZSTD CObjectPair : public CPair {
    public:
        // konstruktor domyślny
        WINAPI CObjectPair();
        // konstruktor kopiujący
        WINAPI CObjectPair(const CObjectPair& pair);
        // konstruktor z podanych wartości
        WINAPI CObjectPair(CmzObject* first, CmzObject* second, BOOL destroyFirstOnObjectDestroy = TRUE, BOOL destroySecondOnObjectDestroy = TRUE);
        virtual WINAPI ~CObjectPair();

        // Wymienia zawartość tego obiektu z podanym w parametrze obiektem.
        VOID WINAPI swap(CObjectPair* pair);
        // Zwraca TRUE jeśli ten obiekt i obiekt podany w parametrze wskazują na ten sam obiekt,
        // lub gdy są to dwa różne obiekty ale o takich samych wartościach _first i _second
        // odpowiednio (tzn., gdy _first i _second wskazują odpowiednio na te same obiekty).
        virtual BOOL WINAPI equals(CPair* pair);
        virtual BOOL WINAPI equals(CObjectPair* pair);
        virtual BOOL WINAPI equals(CPairExt* pair);
        virtual BOOL WINAPI equals(CmzObject* obj);


        inline virtual CmzObject* WINAPI getFirstAsObject() { return (CmzObject*)this->first; }
        inline virtual CmzObject* WINAPI getSecondAsObject() { return (CmzObject*)this->second; }

        CObjectPair& WINAPI operator=(const CObjectPair& pair);
        BOOL WINAPI operator==(const CObjectPair& pair);
        BOOL WINAPI operator!=(const CObjectPair& pair);

    protected:
        VOID WINAPI _commonInit();

    private:
        // atrybut informuje, czy wywołać delete na wartości first w destruktorze
        BOOL _destroyFirstOnObjectDestroy;
        // atrybut informuje, czy wywołać delete na wartości second w destruktorze
        BOOL _destroySecondOnObjectDestroy;

        // jeśli ustwiono atrybut this->_destroyValuesOnObjectDestroy na TRUE, to zwalnia oba
        // przechowywane obiekty (delete)
        VOID WINAPI _cleanup();

    }; // class CObjectPair
    typedef CObjectPair* PObjectPair;



    // Klasa CPairExt, której atrybuty: this->first i this->second mogą przechowywać wartości (jako
    // typ LONGLONG) następujących typów:
    //      LONGLONG
    //      CmzObject*
    //      CPair*
    //      CmzString*
    // informacja o aktualnie przechowywanym typie odpowiedniej wartości znajduje się w atrybucie:
    // this->_firstType i this->_secondType.
    // Oba z nich mogą przyjąć jedną z następujących wartości:
    enum TPairExtValueType {
        TPairExtValueType_Null = 0,
        TPairExtValueType_LONGLONG = 1,
        TPairExtValueType_PObject,
        TPairExtValueType_PPair,
        TPairExtValueType_PString
    }; // TPairExtType

    class DECLSPEC_MZSTD CPairExt : public CPair {
    public:
        TPairExtValueType firstType;
        TPairExtValueType secondType;

        // konstruktor domyślny
        WINAPI CPairExt();

        // konstruktor kopiujący - zabraniamy używać
        WINAPI CPairExt(const CPairExt& pair);
        // konstruktory z podanych wartości
        WINAPI CPairExt(LONGLONG first, LONGLONG second);
        WINAPI CPairExt(LONGLONG first, CPair* second, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(CPair* first, LONGLONG second, BOOL destroyFirstOnObjectDestroy = TRUE);
        WINAPI CPairExt(CPair* first, CPair* second, BOOL destroyFirstOnObjectDestroy = TRUE, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(LONGLONG first, CmzObject* second, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(CmzObject* first, LONGLONG second, BOOL destroyFirstOnObjectDestroy = TRUE);
        WINAPI CPairExt(CmzObject* first, CmzObject* second, BOOL destroyFirstOnObjectDestroy = TRUE, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(CPair* first, CmzObject* second, BOOL destroyFirstOnObjectDestroy = TRUE, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(CmzObject* first, CPair* second, BOOL destroyFirstOnObjectDestroy = TRUE, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(LONGLONG first, CmzString second);
        WINAPI CPairExt(CmzString first, LONGLONG second);
        WINAPI CPairExt(CmzString first, CmzString second);
        WINAPI CPairExt(CPair* first, CmzString second, BOOL destroyFirstOnObjectDestroy = TRUE);
        WINAPI CPairExt(CmzString first, CPair* second, BOOL destroySecondOnObjectDestroy = TRUE);
        WINAPI CPairExt(CmzObject* first, CmzString second, BOOL destroyFirstOnObjectDestroy = TRUE);
        WINAPI CPairExt(CmzString first, CmzObject* second, BOOL destroySecondOnObjectDestroy = TRUE);
        // destruktor
        virtual WINAPI ~CPairExt();
        // Usuwa atrybut this->first. Jeśli jest ustawiony atrybut this->_destroyFirstOnObjectDestroy
        // i przechowywana w atrybucie this->first wartość jest wskaźnikiem na obiekt, to obiekt ten
        // jest zwalniany (delete). Na koniec atrybut this->first jest ustawiany na 0 a atrybut
        // this->firstType jest ustawiany na TPairExtValueType_Null.
        VOID WINAPI deleteFirst();
        // Usuwa atrybut this->second. Jeśli jest ustawiony atrybut this->_destroySecondOnObjectDestroy
        // i przechowywana w atrybucie this->second wartość jest wskaźnikiem na obiekt, to obiekt ten
        // jest zwalniany (delete). Na koniec atrybut this->second jest ustawiany na 0 a atrybut
        // this->secondType jest ustawiany na TPairExtValueType_Null.
        VOID WINAPI deleteSecond();
        // Ustawia wartość atrybutu this->first oraz odpowiednio ustawia wartości atrybutów:
        // this->firstType i this->_destroyFirstOnObjectDestroy. Jeśli trzeba, to zwalnia
        // dotychczasowy obiekt this->first.
        VOID WINAPI setFirst(LONGLONG first);
        // Ustawia wartość atrybutu this->first oraz odpowiednio ustawia wartości atrybutów:
        // this->firstType i this->_destroyFirstOnObjectDestroy. Rozpoznaje przy tym rzeczywisty typ
        // obiektu first którym może być (CmzString*, CPair*, CObjectPair*, CPairExt*). Jeśli trzeba,
        // to zwalnia dotychczasowy obiekt this->first.
        VOID WINAPI setFirst(CmzObject* first, BOOL destroyFirstOnObjectDestroy = TRUE);
        VOID WINAPI setFirst(CmzString first);
        // Ustawia wartość atrybutu this->second oraz odpowiednio ustawia wartości atrybutów:
        // this->secondType i this->_destroySecondOnObjectDestroy. Jeśli trzeba, to zwalnia
        // dotychczasowy obiekt this->second.
        VOID WINAPI setSecond(LONGLONG second);
        // Ustawia wartość atrybutu this->second oraz odpowiednio ustawia wartości atrybutów:
        // this->secondType i this->_destroySecondOnObjectDestroy. Rozpoznaje przy tym rzeczywisty
        // typ obiektu second którym może być (CmzString*, CPair*, CObjectPair*, CPairExt*). Jeśli
        // trzeba, to zwalnia dotychczasowy obiekt this->second.
        VOID WINAPI setSecond(CmzObject* second, BOOL destroySecondOnObjectDestroy = TRUE);
        VOID WINAPI setSecond(CmzString second);

        VOID WINAPI swap(CPairExt* pair);
        // Zwraca TRUE jeśli ten obiekt i obiekt podany w parametrze wskazują na ten sam obiekt,
        // lub gdy są to dwa różne obiekty ale o takich samych wartościach _first i _second
        // odpowiednio (tzn., gdy _first i _second wskazują odpowiednio na te same obiekty albo
        // są odpowiednio równe jako obiekty CPair).
        virtual BOOL WINAPI equals(CPair* pair);
        virtual BOOL WINAPI equals(CObjectPair* pair);
        virtual BOOL WINAPI equals(CPairExt* pair);
        virtual BOOL WINAPI equals(CmzObject* obj);


        virtual CmzObject* WINAPI getFirstAsObject();
        virtual CPair* WINAPI getFirstAsPair();
        virtual CObjectPair* WINAPI getFirstAsObjectPair();
        virtual CPairExt* WINAPI getFirstAsPairExt();
        virtual CmzString* WINAPI getFirstAsString();

        virtual CmzObject* WINAPI getSecondAsObject();
        virtual CPair* WINAPI getSecondAsPair();
        virtual CObjectPair* WINAPI getSecondAsObjectPair();
        virtual CPairExt* WINAPI getSecondAsPairExt();
        virtual CmzString* WINAPI getSecondAsString();

        CPairExt& WINAPI operator=(const CPairExt& pair);
        BOOL WINAPI operator==(const CPairExt& pair);
        BOOL WINAPI operator!=(const CPairExt& pair);

        CPairExt* WINAPI clone();

    protected:
        VOID WINAPI _commonInit();

    private:
        // atrybut informuje, czy wywołać delete na wartości first w destruktorze
        BOOL _destroyFirstOnObjectDestroy;
        // atrybut informuje, czy wywołać delete na wartości second w destruktorze
        BOOL _destroySecondOnObjectDestroy;
        // jeśli ustwiono atrybut this->_destroyValuesOnObjectDestroy na TRUE, to zwalnia oba
        // przechowywane obiekty (delete)
        VOID WINAPI _cleanup();

    }; // class CPairExt
    typedef CPairExt* PPairExt;




    // Klasa CmzMap - przechowuje ArrayList wskaźników na obiekty CPairExt
    class DECLSPEC_MZSTD CmzMap : public CmzObject {
    public:
        // konstruktor domyślny
        WINAPI CmzMap(ULONGLONG capacity = 100L);
        virtual WINAPI ~CmzMap();

        // Dodaje podaną parę do mapy
        VOID WINAPI append(CPairExt* pair);
        // Dodaje podaną parę do mapy - alias dla metody this->append
        VOID WINAPI push(CPairExt* pair);
        // Dodaje podaną parę do mapy na pierwszej pozycji wewnętrznej tablicy
        VOID WINAPI addAsFirst(CPairExt* pair);
        // Dodaje podaną parę do mapy na podanej pozycji wewnętrznej tablicy
        VOID WINAPI insert(ULONGLONG pos, CPairExt* pair);
        // Usuwa wszystkie wystąpienia podanej pary, tzn. wszystkich elementów wewnętrznej tablicy
        // wskazujących na ten sam obiekt co podany parametr. Przy usuwaniu zwalniany jest wskazywany
        // obiekt oraz usuwany jest wskaźnik z wewnętrznej tablicy.
        ULONGLONG WINAPI del(CPairExt* pair);
        // Usuwa pierwsze wystąpienie podanej pary, tzn. element wewnętrznej tablicy wskazujący
        // na ten sam obiekt co podany parametr. Przy usuwaniu zwalniany jest wskazywany
        // obiekt oraz usuwany jest wskaźnik z wewnętrznej tablicy.
        BOOL WINAPI delFirst(CPairExt* pair);
        // Usuwa parę znajdującą się na podanej pozycji w wewnętrznej tablicy. Przy usuwaniu zwalniany
        // jest wskazywany obiekt oraz usuwany jest wskaźnik z wewnętrznej tablicy.
        inline BOOL WINAPI delAt(ULONGLONG pos);
        // Zwraca numer pozycji podanej pary w wewnętrznej tablicy. Jeśli tej pary nie ma, to zwracane
        // jest -1.
        LONGLONG WINAPI pos(CPairExt* pair);
        inline BOOL WINAPI contains(CPairExt* pair) { return this->pos(pair) >= 0; }
        // Zwraca parę (wskaźnik na nią) znajdującą się na podanej pozycji w wewnętrznej tablicy.
        CPairExt* WINAPI getAt(ULONGLONG pos);
        // Zwraca parę (wskaźnik na nią) znajdującą się na pierwszej pozycji.
        CPairExt* WINAPI getFirst();
        // Zwraca parę (wskaźnik na nią) znajdującą się na ostatniej pozycji.
        CPairExt* WINAPI getLast();
        // Zwraca parę (wskaźnik na nią) znajdującą się na ostatniej pozycji i usuwa ją z wewnętrznej
        // tablicy (ale nie robie delete na wskazywanym obiekcie).
        CPairExt* WINAPI pop();
        inline ULONGLONG WINAPI size() const { return this->_items->size(); };
        // Usuwa wszystkie pary z listy (zwalniając przydzieloną im pamięć) oraz zwalnia wskazywane
        // przez nie obiekty (delete na tych obiektach przed delete na parze)
        VOID WINAPI clear();
        // Znajduje pierwszą parę, dla której element first jest typu TPairExtValueType_LONGLONG
        // i jest równy podanej wartości first. W zmiennej foundPosition zwracany jest indeks tej
        // pary w wewnętrznej tablicy. Jeśli pary nie znaleziono, to zwrcany jest NULL i indeks -1.
        CPairExt* findFirstItemByFirst(LONGLONG first, LONGLONG* foundPosition);
        // Searches first CPairExt object which as first element has pointer to CmzString equals to given key.
        CPairExt* findFirstItemByFirst(CmzString key, LONGLONG* foundPosition);
        // J.w. ale zwraca wszystkie pasujące obiekty w CmzArrayList (zawiera wskaźniki na obiekty
        // CPairExt). Jeśli nie znaleziono żadnej pasującej pary, to zwracane jest NULL.
        CmzArrayList* findAllItemsByFirst(LONGLONG first);
        // Znajduje pierwszą parę, dla której element first jest równy podanemu obiektowi.
        // Przy porównywaniu funkcja rozpoznaje sobie najpierw odpowiedni typ obiektu first i na tej
        // podstawie dokonuje odpowiedniego porównania.
        CPairExt* findFirstItemByFirst(CmzObject* first, LONGLONG* foundPosition);
        // J.w. ale zwraca wszystkie pasujące obiekty w CmzArrayList (zawiera wskaźniki na obiekty
        // CPairExt). Jeśli nie znaleziono żadnej pasującej pary, to zwracane jest NULL.
        CmzArrayList* findAllItemsByFirst(CmzObject* first);
        // Usuwa pierwszą znalezioną parę, dla której element first jest typu TPairExtValueType_LONGLONG
        // i jest równy podanej wartości. Zwraca TRUE jeśli taką parę znaleziono i usunięto.
        BOOL WINAPI erase(LONGLONG first);
        // Usuwa pierwszą znalezioną parę, dla której element first jest równy podanemu obiektowi.
        // Przy porównywaniu funkcja rozpoznaje sobie najpierw odpowiedni typ obiektu first i na tej
        // podstawie dokonuje odpowiedniego porównania.
        // Memory occupied by removed pair is deleted.
        BOOL WINAPI erase(CmzObject* first);
        // Usuwa wszystkie znalezione pary, dla której element first jest równy podanemu obiektowi.
        // Przy porównywaniu funkcja nie rozpoznaje typu obiektu first - porównuje liczbowo elementy first.
        // After removing pairs found, there are removed all NULL pointers from the buffer.
        // Memory occupied by removed pairs is deleted.
        ULONGLONG WINAPI remove(LONGLONG first);
        ULONGLONG WINAPI remove(CmzString second);
        // Zwraca wartość dla podanego indeksu. Czyli zwraca wartość second pierwszej pary:
        // this->_items[foundPos] dla której this->_items[foundPos]->_first == first.
        // Jeśli taka wartość, nie została znaleziona, to zwracane jest MININT.
        // Jeśli w drugim argumencie podano nie NULL-ową wartość, to jest do niej przypisywany
        // wskaźnik na obiekt CPairExt, w którym znaleziono wartość _second (czyli this->_items[i]).
        LONGLONG WINAPI get(LONGLONG first, CPairExt** pair = NULL);
        CmzObject* WINAPI getAsObject(LONGLONG first, CPairExt** pair = NULL);
        CPair* WINAPI getAsPair(LONGLONG first, CPairExt** pair = NULL);
        CObjectPair* WINAPI getAsObjectPair(LONGLONG first, CPairExt** pair = NULL);
        CPairExt* WINAPI getAsPairExt(LONGLONG first, CPairExt** pair = NULL);
        LONGLONG WINAPI get(CmzObject* first, CPairExt** pair = NULL);
        CmzObject* WINAPI getAsObject(CmzObject* first, CPairExt** pair = NULL);
        CPair* WINAPI getAsPair(CmzObject* first, CPairExt** pair = NULL);
        CObjectPair* WINAPI getAsObjectPair(CmzObject* first, CPairExt** pair = NULL);
        CPairExt* WINAPI getAsPairExt(CmzObject* first, CPairExt** pair = NULL);
        // Ustawia pierwszemu znalezionemu elementowi o wartości first podaną wartość elementu second.
        // Zwraca TRUE jeśli znaleziono odpowiedni element i ustawiono mu żądaną wartość.
        BOOL WINAPI set(LONGLONG first, LONGLONG second);
        BOOL WINAPI set(LONGLONG first, CmzObject* second);
        BOOL WINAPI set(LONGLONG first, CmzString second);
        // Ustawia pierwszemu znalezionemu elementowi o wartości first podaną wartość elementu second.
        // Zwraca FALSE, jeśli nie znalezionego odpowiedniego elementu.
        // Metoda rozpoznaje rzeczywiste typy obu elementów (mogą to być wskaźniki na obiekty:
        // CmzString, CObject, CPair, CObjectPair, CPairExt) i odpowiednio wyszukuje oraz ustawia
        // wartości.
        BOOL WINAPI set(CmzObject* first, LONGLONG second);
        BOOL WINAPI set(CmzObject* first, CmzObject* second);
        BOOL WINAPI set(CmzObject* first, CmzString second);

        VOID WINAPI set(CmzString key, CmzString value);
        CmzString WINAPI get(CmzString key);
        // String list obtained should be released.
        CmzStringList* WINAPI getKeys();
        // String list obtained should be released.
        CmzStringList* WINAPI getValues();

        // Zwraca ilość elementów o podanym indeksie. Czyli zwraca ilość elementów, dla których
        // this->_items[i]->_first == first.
        LONGLONG WINAPI count(LONGLONG first);
        // Zwraca ilość elementów o podanym indeksie. Czyli zwraca ilość elementów, dla których
        // this->_items[i]->_first == first (przy czym jeśli this->_items[i]->_first jest obiektem
        // CPair, to brane jest pod uwagę porównanie this->_items[i]->_first->equals(first).
        LONGLONG WINAPI count(CPair* first);
        // Zwraca bufor przechowujący wskaźniki na poszczególne pary CPairExt.
        LONGLONG* WINAPI getItemsBuf() { return this->_items->getItemsBuf(); }
        CmzMap* WINAPI clone();

    protected:
        VOID WINAPI _commonInit();

    private:
        // atrybut this->_items przechowuje ArrayList wskaźników na obiekty CPairExt
        CmzArrayList* _items;
        // konstruktor kopiujący - nie zezwalamy na stosowanie go
        WINAPI CmzMap(const CmzMap& map);
    }; // class CmzMap
    typedef CmzMap* PMap;

}

#endif
