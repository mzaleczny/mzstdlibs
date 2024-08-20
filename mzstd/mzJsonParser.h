#ifndef __MZSTD_JSON_PARSER_H__
#define __MZSTD_JSON_PARSER_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzStdObject.h"
#include "mzArrayList.h"
#include "mzPointerList.h"

namespace mzstd {

#define     CMZJSONPARSER_GROW_CHUNK                                                  8192

#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY                                1
#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY              2
#define     CMZJSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE                    3
#define     CMZJSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_SINGLE_QUOTE                    4
#define     CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE                     5
#define     CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE                    6
#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME                           7
#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON                               8
#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE                          9
#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE                   10
#define     CMZJSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_SINGLE_QUOTE                   11
#define     CMZJSONPARSER_STATE_STOP_PARSING                                          100

#define     CMZJSONPARSER_CURRENT_ITEM_UNDEFINED                                      0
#define     CMZJSONPARSER_CURRENT_ITEM_ARRAY                                          1
#define     CMZJSONPARSER_CURRENT_ITEM_OBJECT                                         2

class CmzJsonParserEntity : public CmzObject {
public:
    WINAPI CmzJsonParserEntity(VOID *entity, LONGLONG entity_type) {
        this->entity = entity;
        this->entity_type = entity_type;
    };
    // wskaźnik na obiekt
    VOID *entity;
    // typ obiektu:
    //  0 - nieokreślony
    //  1 - tablica (CmzPointerList)
    //  2 - obiekt  (CmzStdObject)
    LONGLONG entity_type;
private:
    WINAPI CmzJsonParserEntity() {};
};

class DECLSPEC_MZSTD CmzJsonParser : public CmzObject {
public:
    WINAPI CmzJsonParser();
    virtual WINAPI ~CmzJsonParser();

    CmzStdObject* WINAPI parseFile(CmzString fname, CmzStdObject* destination = NULL);
    CmzStdObject* WINAPI parse(CmzString content, CmzStdObject* destination = NULL);
    inline BOOL WINAPI isError() {return this->_error != 0; };
    inline CmzString WINAPI getErrorMessage() {return this->_error_msg; };

private:
    // poniższy atrybut przechowuje obiekt który zostanie zwrócony po
    // poprawnym przeparsowaniu pliku json. Ma on jeden atrybut: 'root'
    // który zawiera wskaźnik na pierwszy obiekt lub tablicę w pliku
    // json.
    CmzStdObject *_retval;
    // Flag marking if retval object was created in CParser object or if it was supplied from parser caller.
    // If it was created inside parser, then if parser error this object is deleted. Otherwise not.
    BOOL _retvalInternalCreated;
    
    // tablica zawierająca wskaźniki na aktualnie przetwarzane encje.
    // są to wskaźniki na obiekty klasy CJsonParserEntity.
    CmzPointerList *_current_object;
    // nazwa atrybutu pod jaką zostanie dodana nowa wartość do
    // aktualnie przetwarzanego obiektu
    CmzString _current_object_attribute_name;

    CHAR *_current_token;
    LONGLONG _current_token_buf_size;
    // ilość znaczących znaków w buforze tokena
    LONGLONG _current_token_length;

    // Tablica kolejnych stanów parsera. Zawiera liczby całkowite.
    CmzArrayList *_current_state;
    // poniższa zmienna informuje czy nazwę atrybutu obiektu otoczono
    // cudzysłowem czy nie a jeśli tak, to jakim. Możliwe wartości:
    //  - - nazwa atrybutu podana bez cudzysłowu
    //  ' - nazwa atrybutu podana w pojedyńczym cudzysłowiu
    //  " - nazwa atrybutu podana w podwójnym cudzysłowiu
    CHAR _object_property_name_quote;

    LONGLONG _error;
    CmzString _error_msg;
    DWORD _line_number;
    LONGLONG _char_number_in_line;
    LONGLONG _char_number_in_file;

    VOID WINAPI _processBuffer(CHAR *buf, DWORD buf_len);
    inline BOOL WINAPI _isWhiteSpace(CHAR ch) { return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t'; };
    VOID WINAPI _deleteCurrentObjectArray();
    VOID WINAPI _deleteCurrentStateArray();
    VOID WINAPI _cleanRetvalObject();
    CmzString WINAPI _getStringFromCurrentToken();
    VOID WINAPI _onExpectArrayValueInQuote(CHAR quoteType, CHAR ch);
    VOID WINAPI _onExpectObjectValueInQuote(CHAR quoteType, CHAR ch);
    VOID WINAPI _onBeginArray();
    VOID WINAPI _onBeginObject();
    VOID WINAPI _onCompleteReadObjectAttributeName();
    VOID WINAPI _onCompleteReadObjectAttributeSimpleValue();
    VOID WINAPI _appendCharToCurrentToken(CHAR ch);
    VOID WINAPI _onCloseSquare();
    VOID WINAPI _initParsingProcess(CmzStdObject* destination = NULL);
};

}

#endif
