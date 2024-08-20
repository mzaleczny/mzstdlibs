/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdgui
 * Version: 1.07
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdgui library
 * 
 * mzstdgui library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdgui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */

#ifndef __MZSTDGUI_GRID_H__
#define __MZSTDGUI_GRID_H__

#include "mzstdguiCommon.h"
#include "mzString.h"
#include "mzGuiContainer.h"
#include "mzFont.h"
#include "mzMap.h"

#define CGRID_DATA_FILE_EXTENSION                              L"mzd"


#define CGRID_STATE_NORMAL                                     1L
#define CGRID_STATE_FOCUSED                                    2L
#define CGRID_STATE_DISABLED                                   4L
#define CGRID_STATE_UPDATE_SELECTION_ACCORDING_MOUSE_POS       8L

#define CGRID_DEFAULT_COLUMN_COUNT                            10L
#define CGRID_DEFAULT_ROW_COUNT                               10L
#define CGRID_DEFAULT_COLUMN_WIDTH                            90L
#define CGRID_DEFAULT_LEFT_HEADER_WIDTH                       30L

class CmzImageList;
class mzstd::CmzArrayList;
class mzstd::CmzStringList;


class DECLSPEC_MZSTDGUI CGridCell : public mzstd::CmzObject {
public:
    mzstd::CmzString value;
    BOOL selected;
    
    WINAPI CGridCell(mzstd::CmzString value, BOOL selected = FALSE);
    virtual WINAPI ~CGridCell();
};


class DECLSPEC_MZSTDGUI CGridHeaderCell : public mzstd::CmzObject {
public:
    // width dla komórki nagłówka górnego, height dla komórki nagłówka lewego
    LONG size;
    // czy komórka nagłówka jest zaznaczona
    BOOL selected;
    mzstd::CmzString text;
    
    WINAPI CGridHeaderCell(LONG size, BOOL selected = FALSE);
    WINAPI CGridHeaderCell(mzstd::CmzString text, LONG size, BOOL selected = FALSE);
    virtual WINAPI ~CGridHeaderCell();
};

class DECLSPEC_MZSTDGUI CGrid : public CGuiContainer {
public:
	WINAPI CGrid(CSprite *parent, mzstd::CmzString name,
                    LONG columnCount = CGRID_DEFAULT_COLUMN_COUNT, LONG rowCount = CGRID_DEFAULT_ROW_COUNT,
                    LONG x = 0, LONG y = 0, LONG width = 400, LONG height = 220, BOOL editable = TRUE);
	WINAPI CGrid(CSprite *parent, mzstd::CmzString name, LONG columnCount, LONG rowCount, LONG x, LONG y,
                    LONG width, LONG height, BOOL editable,
                    BOOL showLeftHeader, BOOL showTopHeader, BOOL entireRowSelect, BOOL entireColumnSelect,
                    BOOL drawVerticalLines, BOOL drawHorizontalLines);
	virtual WINAPI ~CGrid();

    // Returns CmzFont* out of the this->_cachedFonts attriubute. If fontName == "", the default font for grid
    // defined in current theme is returned. Else if fontName != "" and font does not exists in this->_cachedFonts
    // then it is created added tho the attribute and returned.
    CmzFont* WINAPI getFont(const WCHAR* fontName = L"", LONG fontSize = 0, LONG fontColor = 0);
    VOID WINAPI cleanCachedFonts();
    VOID WINAPI cleanData();

    // Umieszcza focus na tej kontrolce a dokładnie:
    //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
    //      ParentWnd->setActiveControl(this) tego okna.
    //      Przełącza stan kontrolki w tryb CGRID_STATE_FOCUSED.
    virtual VOID WINAPI focus();
    // Usuwa focus z tej kontrolki a dokładnie:
    //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
    //      Przełącza kontrolkę w stan CGRID_STATE_NORMAL
    //      w zależności od aktualnego położenia myszy.
    virtual VOID WINAPI looseFocus();
    // funkcja obsługi kliknięcia
//    virtual VOID WINAPI onClick();

    // Funkcje obsługi zdarzeń
    virtual VOID WINAPI onDraw(CmzBitmap *dest);

    // Funkcje obsługi kliknięć
    virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
    virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
    virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

    virtual BOOL WINAPI onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual BOOL WINAPI onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual BOOL WINAPI onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch);
    virtual VOID WINAPI onThumbChange(LONG oldPosition, LONG curPosition, CScrollbar* scrollbar);

    // Move active cell to the next cell to the right and returns TRUE if active cell was changed.
    BOOL WINAPI moveRight(LONG count = 1);
    // Move active cell to the next cell to the left and returns TRUE if active cell was changed.
    BOOL WINAPI moveLeft(LONG count = 1);
    // Move active cell to the next cell to the up and returns TRUE if active cell was changed.
    BOOL WINAPI moveUp(LONG count = 1);
    // Move active cell to the next cell to the down and returns TRUE if active cell was changed.
    BOOL WINAPI moveDown(LONG count = 1);
    BOOL WINAPI movePageUp();
    BOOL WINAPI movePageDown();
    BOOL WINAPI movePageLeft();
    BOOL WINAPI movePageRight();
    BOOL WINAPI moveHome();
    BOOL WINAPI moveHomeVertical();
    BOOL WINAPI moveHomeHorizontal();
    BOOL WINAPI moveEnd();
    BOOL WINAPI moveEndVertical();
    BOOL WINAPI moveEndHorizontal();
    BOOL WINAPI moveToCell(LONG coordX, LONG coordY);
    VOID WINAPI setStartCellCoordX(LONG coordX);
    VOID WINAPI setStartCellCoordY(LONG coordX);
    // Gets coords of the cell under specified mouse coordinates. Returns TRUE if cell was found.
    BOOL WINAPI getCellCoordsAtMousePos(LONG mouseX, LONG mouseY, LONG* coordX, LONG* coordY);
    // Move active cell to mouse-clicked cell returns TRUE if active cell was changed.
    BOOL WINAPI activateCellAtMousePos(LONG x, LONG y);
    // Usuwa zaznaczenie (bez usunięcia zawartości komórek, które były zaznaczone)
    // i ewentualnie odrysowuje kontrolkę
    VOID WINAPI clearSelection(BOOL redraw = TRUE);
    BOOL WINAPI isSelection();

    // Returns total summary width of all columns
    ULONG WINAPI getTotalColumnsWidth();
    // Returns total summary height of all rows
    ULONG WINAPI getTotalRowsHeight();
    // Returns number of visible columns
    LONG WINAPI getVisibleColumnCount();
    // Returns number of fully visible columns (if there is column partialy visible then it is not counted)
    LONG WINAPI getFullVisibleColumnCount();
    // Returns number of visible rows
    LONG WINAPI getVisibleRowCount();
    // Returns number of fully visible rows (if there is row partialy visible then it is not counted)
    LONG WINAPI getFullVisibleRowCount();

    virtual VOID WINAPI onEditorShow();
    virtual mzstd::CmzStdObject WINAPI getValue();
    virtual VOID WINAPI setValue(mzstd::CmzString value, BOOL redraw = TRUE);
    VOID WINAPI setCellValue(LONG cellCoordX, LONG cellCoordY, mzstd::CmzString value, BOOL redraw = TRUE);
    VOID WINAPI setCurrentCellValue(mzstd::CmzString value, BOOL redraw = TRUE);
    VOID WINAPI clearCellValue(LONG cellCoordX, LONG cellCoordY, BOOL redraw = TRUE);
    VOID WINAPI clearCurrentCellValue(BOOL redraw = TRUE);
    virtual VOID WINAPI onApplyEditorChanges(mzstd::CmzStdObject* value);

    CGridCell* WINAPI getCell(LONG cellCoordX, LONG cellCoordY);
    CGridCell* WINAPI getCurrentCell();
    CGridHeaderCell* WINAPI getHeaderCellX(LONG coordX);
    CGridHeaderCell* WINAPI getCurrentHeaderCellX();
    CGridHeaderCell* WINAPI getHeaderCellY(LONG coordY);
    CGridHeaderCell* WINAPI getCurrentHeaderCellY();

    // Zwraca wymiary podane komórki grida.
    SIZE WINAPI getCellSize(LONG cellCoordX, LONG cellCoordY);    
    // Zwraca współrzędne lewego górnego rogu komórki grida o podany współrzędnych logicznych.
    // Współrzędne te są współrzędnymi (x, y) bitmapy na której grid jest rysowany.
    VOID WINAPI cellCoordsToCanvasCoords(LONG cellCoordX, LONG cellCoordY, LONG* x, LONG* y);
    BOOL WINAPI cellVisible(LONG coordX, LONG coordY, BOOL acceptPartialVisibility = FALSE);

    VOID WINAPI showTopHeader(BOOL showTopHeader, BOOL redraw = FALSE);
    VOID WINAPI showLeftHeader(BOOL showTopHeader, BOOL redraw = FALSE);
    VOID WINAPI setGridSize(LONG columnCount, LONG rowCount);
    
    inline VOID WINAPI setEntireRowSelect(BOOL value) { this->_entireRowSelect = value; }
    inline VOID WINAPI setEntireColumnSelect(BOOL value) { this->_entireColumnSelect = value; }
    inline VOID WINAPI setDrawVerticalLines(BOOL value) { this->_drawVerticalLines = value; }
    inline VOID WINAPI setDrawHorizontalLines(BOOL value) { this->_drawHorizontalLines = value; }
    
    VOID WINAPI setLeftHeaderCaptions(mzstd::CmzStringList* captions);
    VOID WINAPI setTopHeaderCaptions(mzstd::CmzStringList* captions);

    inline VOID WINAPI setLeftHeaderWidth(LONG value) { this->_leftHeaderWidth = value; }
    inline mzstd::CmzMap* WINAPI getData() { return &this->_data; }

    BOOL WINAPI saveToFile(mzstd::CmzString fname);
    BOOL WINAPI loadFromFile(mzstd::CmzString fname);

protected:
    // Dane grida przechowywane są jako pary CPairExt(LONG, LONG). gdzie LONG definiuje
    // współrzędne w gridzie:
    //    first:
    //      HIWORD(LONG) = cellYCoord
    //      LOWORD(LONG) = cellCoordX
    //    second:
    //      LONG<=>*CGridCell obiekt komórki (wraz z zawartością).
    // Zatem przy aktualnej implementacji grid obsługuje max. siatkę o polach 1..65536 w pionie i poziomie.
    mzstd::CmzMap _data;
    // Used fonts by the grid
    mzstd::CmzArrayList* _cachedFonts;
    // Przechowuje wskaźnik na listę obrazków z której pobierane są obrazki, tgo wskaźnika nie
    // zwalniamy
    CmzImageList* _imageList;
    // if top header is visible
    BOOL _showTopHeader;
    // if left header is visible
    BOOL _showLeftHeader;
    BOOL _drawHorizontalLines;
    BOOL _drawVerticalLines;
    // do entire row selected (not just only active cell)
    BOOL _entireRowSelect;
    // do entire column selected (not just only active cell)
    BOOL _entireColumnSelect;
    
    // width of grid's canvas which contain cells with data (not headers and scrollbars)
    LONG _dataCanvasWidth;
    // height of grid's canvas which contain cells with data (not headers and scrollbars)
    LONG _dataCanvasHeight;
    
    LONG _leftHeaderWidth;
    LONG _topHeaderHeight;
    LONG _defColumnWidth;
    LONG _defRowHeight;
    // maksymalne dopuszczalne wartości dla indeksów kolumny i wiersza
    LONG _maxColumnNumber;
    LONG _maxRowNumber;
    // indeksy maksymalnej widocznej w całosci kolumny i wiersza
    LONG _maxFullVisibleColumnNumber;
    LONG _maxFullVisibleRowNumber;
    // indeksy maksymalnej widocznej przynajmniej częściowo kolumny i wiersza
    LONG _maxVisibleColumnNumber;
    LONG _maxVisibleRowNumber;
    // width in pixels of the last column's visible part
    LONG _visibleWidthOfLastColumn;
    // width in pixels of the last row's visible part
    LONG _visibleHeightOfLastRow;
    
    LONG _xPageStep;
    LONG _yPageStep;
    
    // Współrzędna od której zaczyna się indeks nagłówka górnego, liczona od 1. Wyświetlana
    // jest natomiast litera. Literze 'A' odpowiada indeks 1, literze 'B' indeks 2 itd.
    // Wyświetlane są tylko litery A-Z a po ich wyczerpaniu AA-ZZ.
    LONG _startCellCoordX;
    // Współrzędna od której zaczyna się indeks nagłówka lewego, liczony od 1. Wyświetlany
    // jest numer tej współrzednej.
    LONG _startCellCoordY;
    // max allowable value for this->_startCellCoordX
    LONG _maxAllowableStartCellCoordX;
    // max allowable value for this->_startCellCoordY
    LONG _maxAllowableStartCellCoordY;
    // Współrzędna X (1.._maxColumnNumber NOT a coord on Canvas) aktywnej komórki.
    LONG _coordX;
    // Współrzędna Y (1.._maxRowNumber NOT a coord on Canvas) aktywnej komórki.
    LONG _coordY;
    // Array containing header objects (CGridHeaderCellData*) for the columns
    mzstd::CmzArrayList *_colData;
    // Array containing header objects (CGridHeaderCellData*) for the rows
    mzstd::CmzArrayList *_rowData;

    virtual VOID WINAPI _commonInit(LONG columnCount, LONG rowCount, LONG width, LONG height, BOOL editable, BOOL showLeftHeader, BOOL showTopHeader, BOOL entireRowSelect, BOOL entireColumnSelect, BOOL drawVerticalLines, BOOL drawHorizontalLines);
    // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
    virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& redraw);

    // Funkcja zwraca string literowy odpowiadający podanemu indeksowi. Np. dla indeks=1 zwracane
    // jest 'A', dla 2 => 'B', itd.
    mzstd::CmzString WINAPI _indexToLetter(LONG index);

    // Zwraca LONGa przechowującego podane współrzędne komórki grida. Kodowanie współrzędnych
    // określone jest przy opisie danych grida.
    inline LONG WINAPI _cellCoordsToLONG(LONG x, LONG y);
    inline VOID WINAPI _LONGToCellCoords(LONG data, LONG* x, LONG* y);

    // Ustawia indeksy maksymalnych widocznych w całości/częściowo kolumn i wierszy.
    VOID WINAPI _updateGridParameters();
    VOID WINAPI _updateScrollbars();
    VOID WINAPI _cleanGridContent();
    VOID WINAPI _setHeaderCaptions(mzstd::CmzArrayList* headerCells, mzstd::CmzStringList* lst);
    // if size of grid changed, then data for cells that are outside grid col/row dimensions are deleted.
    VOID WINAPI _compactData();
    
private:
    virtual VOID WINAPI _updateCanvas();

};
typedef CGrid* PGrid;

#endif
