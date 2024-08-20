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
#include "pch.h"
#include "mzRpnParser.h"
#include "mzString.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"
#include "mzMath.h"

using namespace mzstd;


VOID CRpnParser::_commonInit() {
    this->setRealClassName(L"CRpnParser");
    this->_tokens = NULL;
    this->_initTokensArray();
}

CRpnParser::CRpnParser() {
    this->_commonInit();
}


CRpnParser::~CRpnParser() {
    if (this->_tokens) {
        this->_cleanupTokensArray(this->_tokens);
    }
}

VOID CRpnParser::_initTokensArray() {
    this->_growTokensArray();
}

VOID CRpnParser::_growTokensArray() {
    LONG count = 50;

    if (!this->_tokens) {
        this->_tokens = new CmzArrayList(count);
    }

    CRpnToken* token;
    for (LONG i = 0; i < count; i++) {
        token = new CRpnToken();
        this->_tokens->append((LONGLONG)token);
    }
}

VOID CRpnParser::_cleanupTokensArray(CmzArrayList* array, BOOL deleteArray) {
    if (array) {
        CRpnToken* token;
        ULONGLONG size = array->size();
        for (ULONGLONG i = 0; i < size; i++) {
            token = (CRpnToken*)array->getAt(i);
            if (token) {
                delete token;
            }
        }
        if (deleteArray) {
            delete array;
            if (array == this->_tokens) {
                this->_tokens = NULL;
                this->_tokensCount = 0;
            }
        }
    }
}

double CRpnParser::calculate(CmzString expression, CmzMap* variables, CmzString* rpnNotation) {
    double result = 0.0;
    if (rpnNotation) {
        *rpnNotation = COMMON_EMPTY_STRING;
    }
    
    CmzArrayList* rpnTokens = new CmzArrayList();
    if (!rpnTokens) {
        return result;
    }
    
    CmzString rpn = this->infixToPostfix(expression, FALSE, rpnTokens);
    if (rpnNotation) {
        *rpnNotation = rpn;
    }
    
    if (!this->_error && rpnTokens && rpnTokens->size() > 0) {
        ULONGLONG size = rpnTokens->size();
        CmzArrayList* stack = new CmzArrayList(size);
        if (stack) {
            CRpnToken* token;
            CRpnToken* tmpToken;
            double a, b;
            
            for (ULONGLONG i = 0; i < size; i++) {
                token = (CRpnToken*)rpnTokens->getAt(i);
                if (token->type == RPN_TOKEN_TYPE_CONSTANT) {
                    stack->append((LONGLONG)token);
                    rpnTokens->setAt(i, NULL);
                } else if (token->type == RPN_TOKEN_TYPE_VARIABLE) {
                    token->type = RPN_TOKEN_TYPE_CONSTANT;
                    token->fValue = 0.0; // by default 0.0 (if value of variable not given)
                    if (variables) {
                        CmzString value = variables->get(token->value);
                        if (value != COMMON_EMPTY_STRING) {
                            token->fValue = value.toDouble();
                        }
                    }
                    stack->append((LONGLONG)token);
                    rpnTokens->setAt(i, NULL);
                } else if (token->type == RPN_TOKEN_TYPE_OPERATOR) {
                    a = 0.0;
                    b = 0.0;
                    tmpToken = (CRpnToken*)stack->pop();
                    if (tmpToken) {
                        if (tmpToken->type == RPN_TOKEN_TYPE_CONSTANT) {
                            a = tmpToken->fValue;
                        }
                        delete tmpToken;
                    }
                    tmpToken = (CRpnToken*)stack->getLast();
                    if (tmpToken) {
                        if (tmpToken->type == RPN_TOKEN_TYPE_CONSTANT) {
                            b = tmpToken->fValue;
                        }
                    }
                    
                    if (token->value == L'+') {
                        tmpToken->fValue = b + a;
                    } else if (token->value == L'-') {
                        tmpToken->fValue = b - a;
                    } else if (token->value == L'*') {
                        tmpToken->fValue = b * a;
                    } else if (token->value == L'/') {
                        tmpToken->fValue = b / a;
                    } else if (token->value == L'%') {
                        tmpToken->fValue = (LONG)b % (LONG)a;
                    } else if (token->value == L'^') {
                        tmpToken->fValue = pow(b, a);
                    }
                }
            }
            
            if (stack->size() > 1) {
                ErrorBox(L"Too many items on the stack.");
            } else {
                tmpToken = (CRpnToken*)stack->pop();
                result = tmpToken->fValue;
            }
            
            this->_cleanupTokensArray(rpnTokens, TRUE);
            this->_cleanupTokensArray(stack, TRUE);
            rpnTokens = NULL;
            stack = NULL;
        }
    }
    
    return result;
}

CmzString CRpnParser::infixToPostfix(CmzString expression, BOOL showMessageOnError, CmzArrayList* outputTokenArray) {
    CmzString result;
    CmzString stringToken;
    ULONG i;
    WCHAR ch;
    size_t len;
    CRpnToken* token = nullptr, rpnToken;
    LONG parenthesisOpened, priority;
    CmzString space(L" ");

    expression.remove(L' ');
    expression.remove(L'\t');
    expression.remove(L'\n');
    expression.toLowercase();
    parenthesisOpened = 0;
    this->_error = 0;
    this->_errorMessage = L"ERROR_SUCCESS";
    this->_tokensCount = 0;
    this->_parenthesisOpened = 0;
    len = expression.length();
    if (len == 0) {
        if (outputTokenArray) {
            this->_cleanupTokensArray(outputTokenArray, FALSE);
        }
        return COMMON_EMPTY_STRING;
    }
        
    i = 0;
    while (i < len) {
        ch = expression[i];
        i++;
        if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z')) {
            stringToken += ch;
        } else {
            if (ch == L'(') {
                if (stringToken != COMMON_EMPTY_STRING) {
                    result += space + stringToken;
                    if (outputTokenArray) {
                        if (!this->_pushToken(outputTokenArray, stringToken)) {
                            this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + stringToken + L"] to output token array", showMessageOnError);
                            return COMMON_EMPTY_STRING;
                        }
                    }
                    stringToken = COMMON_EMPTY_STRING;
                }
                this->_pushToken(ch);
            } else if (ch == L')') {
                LONG pos = this->_tokensCount - 1;
                if (this->_parenthesisOpened == 0 || pos == 0) {
                    this->_handleError(1, CmzString(L"expression: ") + expression + L"\nUnmatched closing parenthesis ')' at char " + i, showMessageOnError);
                    return COMMON_EMPTY_STRING;
                }
                if (stringToken != COMMON_EMPTY_STRING) {
                    result += space + stringToken;
                    if (outputTokenArray) {
                        if (!this->_pushToken(outputTokenArray, stringToken)) {
                            this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + stringToken + L"] to output token array", showMessageOnError);
                            return COMMON_EMPTY_STRING;
                        }
                    }
                    stringToken = COMMON_EMPTY_STRING;
                }
                this->_parenthesisOpened--;
                while (this->_tokensCount > 0) {
                    token = (CRpnToken*)this->_tokens->getAt(this->_tokensCount-1);
                    if (token->type == RPN_TOKEN_TYPE_OPERATOR) {
                        result += space + token->value;
                        if (outputTokenArray) {
                            if (!this->_pushToken(outputTokenArray, token->value)) {
                                this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + token->value + L"] to output token array", showMessageOnError);
                                return COMMON_EMPTY_STRING;
                            }
                        }
                        this->_tokensCount--;
                        continue;
                    }
                    break;
                }
                if (this->_tokensCount == 0 || (token && token->type != RPN_TOKEN_TYPE_PARENTHESIS)) {
                    this->_handleError(1, CmzString(L"expression: ") + expression + L"\nUnmatched closing parenthesis ')' at char " + i, showMessageOnError);
                    return COMMON_EMPTY_STRING;
                } else {
                    this->_tokensCount--;
                }
            } else if (this->isOperator(ch)) {
                if (stringToken != COMMON_EMPTY_STRING) {
                    result += space + stringToken;
                    if (outputTokenArray) {
                        if (!this->_pushToken(outputTokenArray, stringToken)) {
                            this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + stringToken + L"] to output token array", showMessageOnError);
                            return COMMON_EMPTY_STRING;
                        }
                    }
                    stringToken = COMMON_EMPTY_STRING;
                } else {
                    WCHAR pch = 0;
                    if (i > 1) {
                        pch = expression[i-2];
                    }
                    if (i == 0 || this->isOperator(pch)) {
                        this->_handleError(1, CmzString(L"expression: ") + expression + L"\nUnexpected operator encountered at char " + i, showMessageOnError);
                        return COMMON_EMPTY_STRING;
                    }
                }
                priority = this->_getOperatorPriority(ch);
                if (this->_tokensCount == 0 || priority > this->_getLastTokenPriority()) {
                    this->_pushToken(ch);
                } else {
                    while (this->_tokensCount > 0) {
                        token = (CRpnToken*)this->_tokens->getAt(this->_tokensCount-1);
                        if (token->type == RPN_TOKEN_TYPE_OPERATOR) {
                            if (this->_getLastTokenPriority() >= priority) {
                                result += space + token->value;
                                if (outputTokenArray) {
                                    if (!this->_pushToken(outputTokenArray, token->value)) {
                                        this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + token->value + L"] to output token array", showMessageOnError);
                                        return COMMON_EMPTY_STRING;
                                    }
                                }
                                this->_tokensCount--;
                                continue;
                            }
                            break;
                        } else {
                            break;
                        }
                    }
                    this->_pushToken(ch);
                }
            }
        }
    }

    if (stringToken != COMMON_EMPTY_STRING) {
        result += space + stringToken;
        if (outputTokenArray) {
            if (!this->_pushToken(outputTokenArray, stringToken)) {
                this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + stringToken + L"] to output token array", showMessageOnError);
                return COMMON_EMPTY_STRING;
            }
        }
    }
    while (this->_tokensCount > 0) {
        token = (CRpnToken*)this->_tokens->getAt(this->_tokensCount-1);
        if (token->type == RPN_TOKEN_TYPE_PARENTHESIS) {
            this->_handleError(1, CmzString(L"expression: ") + expression + L"\nUnmatched opening parenthesis [" + token->value + L"]", showMessageOnError);
            return COMMON_EMPTY_STRING;
        }
        result += space + token->value;
        this->_tokensCount--;
        if (outputTokenArray) {
            if (!this->_pushToken(outputTokenArray, token->value)) {
                this->_handleError(1, CmzString(L"expression: ") + expression + L"\nCannot add token [" + token->value + L"] to output token array", showMessageOnError);
                return COMMON_EMPTY_STRING;
            }
        }
    }
    
    result.trim();
    return result;
}

LONG CRpnParser::_getOperatorPriority(WCHAR ch) {
    switch (ch) {
        case L'(':
            return 0;
        case L')':
        case L'+':
        case L'-':
            return 1;
        case L'*':
        case L'/':
        case L'%':
            return 2;
        case L'^':
            return 3;
    }
    return -1;
}

LONG CRpnParser::_getLastTokenPriority() {
    if (this->_tokensCount == 0) {
        return -1;
    }
    CRpnToken* token = (CRpnToken*)this->_tokens->getAt(this->_tokensCount-1);
    if (token && token->value != COMMON_EMPTY_STRING) {
        return this->_getOperatorPriority(token->value.getCharAt(0));
    }

    return -1;
}

VOID CRpnParser::_pushToken(WCHAR ch) {
    if (!this->_tokens || this->_tokensCount == this->_tokens->capacity()) {
        this->_growTokensArray();
    }
    CRpnToken* token = (CRpnToken*)this->_tokens->getAt(this->_tokensCount);
    token->value = ch;
    switch (ch) {
        case L'(':
            token->type = RPN_TOKEN_TYPE_PARENTHESIS;
            this->_parenthesisOpened += 1;
            this->_tokensCount++;
            break;
        case L'+':
        case L'-':
        case L'*':
        case L'/':
        case L'%':
        case L'^':
            token->type = RPN_TOKEN_TYPE_OPERATOR;
            this->_tokensCount++;
            break;
    }
}

BOOL CRpnParser::_pushToken(CmzArrayList* tokens, CmzString token) {
    if (!tokens) {
        return FALSE;
    }
    
    CRpnToken* rpnToken = new CRpnToken();
    if (!rpnToken || token == COMMON_EMPTY_STRING) {
        return FALSE;
    }
    
    if (this->isOperator(token.getCharAt(0))) {
        rpnToken->type = RPN_TOKEN_TYPE_OPERATOR;
        rpnToken->value = token;
    } else {
        if (token.isNumeric()) {
            rpnToken->type = RPN_TOKEN_TYPE_CONSTANT;
            rpnToken->fValue = token.toDouble();
        } else {
            rpnToken->type = RPN_TOKEN_TYPE_VARIABLE;
        }
        rpnToken->value = token;
    }
    tokens->append((LONGLONG)rpnToken);
    
    return TRUE;
}

VOID CRpnParser::_handleError(LONG errorCode, CmzString errorMessage, BOOL showDialogBox) {
    this->_error = errorCode;
    this->_errorMessage = errorMessage;
    this->_cleanupTokensArray(this->_tokens);
    if (showDialogBox) {
        ErrorBox(this->_errorMessage);
    }
}
