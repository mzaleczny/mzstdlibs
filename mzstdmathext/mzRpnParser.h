/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2011 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdmathext
 * Version: 1.05
 * First release date:   2011-03-04
 * Current release date: 2011-03-04
 * Additional info:      Algorithms used according to wiki description of "Odwrotna notacja polska" at 2011-03-02
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

#ifndef __MZ_RPNPARSER_H__
#define __MZ_RPNPARSER_H__

#include "mzstdmathextCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzArrayList.h"
#include "mzMap.h"

#define     RPN_TOKEN_TYPE_CONSTANT             1
#define     RPN_TOKEN_TYPE_VARIABLE             2
#define     RPN_TOKEN_TYPE_PARENTHESIS          3
#define     RPN_TOKEN_TYPE_OPERATOR             4


class DECLSPEC_MZSTDMATHEXT CRpnToken {
public:
    LONG type;
    mzstd::CmzString value; // applied for tokens of type: RPN_TOKEN_TYPE_VARIABLE, RPN_TOKEN_TYPE_PARENTHESIS
    double fValue;  // applied for tokens of type: RPN_TOKEN_TYPE_CONSTANT
};


// Reverse Polish Notation parser and calculator.
class DECLSPEC_MZSTDMATHEXT CRpnParser : public mzstd::CmzObject {
public:
	WINAPI CRpnParser();
	virtual WINAPI ~CRpnParser();
    
    double WINAPI calculate(mzstd::CmzString expression, mzstd::CmzMap* variables = NULL, mzstd::CmzString* rpnNotation = NULL);

    // Conversion from infix notation (ex. a+b*c) to postfix (RPN) notation (ex. abc*+)
    // If error occured during conversion then returned is empty string, method isError() returns TRUE
    // and method getErrorMessage returns appropriate message.
    // If outputTokenArray is not NULL and no error occured during parsing then we get in outputTokenArray
    // array of tokens in RPN (besides returned string).
    mzstd::CmzString WINAPI infixToPostfix(mzstd::CmzString expression, BOOL showMessageOnError = FALSE, mzstd::CmzArrayList* outputTokenArray = NULL);
    inline BOOL WINAPI isError() {return this->_error != 0; };
    inline mzstd::CmzString WINAPI getErrorMessage() {return this->_errorMessage; };
    inline BOOL WINAPI isOperator(WCHAR ch) {
        return ch == L'+' || ch == L'-' || ch == L'*' || ch == L'/' || ch == L'%' || ch == L'^';
    }
    
protected:
    // stack of tokens for usage by infixToPostfix method (to speed up)
    mzstd::CmzArrayList* _tokens;
    // count of initialised tokens
    LONG _tokensCount;
    
    LONG _error;
    mzstd::CmzString _errorMessage;
    LONG _parenthesisOpened;

    VOID WINAPI _commonInit();
    // By default initializes token array with 50 empty tokens
    VOID WINAPI _initTokensArray();
    // grows tokens array to twice the size and creates new uninitialised tokens
    VOID WINAPI _growTokensArray();
    VOID WINAPI _cleanupTokensArray(mzstd::CmzArrayList* array, BOOL deleteArray = TRUE);
    VOID WINAPI _pushToken(WCHAR ch);
    BOOL WINAPI _pushToken(mzstd::CmzArrayList* tokens, mzstd::CmzString token);
    LONG WINAPI _getOperatorPriority(WCHAR ch);
    LONG WINAPI _getLastTokenPriority();
    VOID WINAPI _handleError(LONG errorCode, mzstd::CmzString errorMessage, BOOL showDialogBox);
};
typedef CRpnParser* PRpnParser;

#endif
