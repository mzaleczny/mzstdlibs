#include "pch.h"
#include "mzJsonParser.h"
#include "mzFileUtils.h"
#include "mzMessageBox.h"

namespace mzstd {

CmzJsonParser::CmzJsonParser() {
    this->setRealClassName(L"CmzJsonParser");
    this->_retval = NULL;
    this->_current_token_buf_size = CMZJSONPARSER_GROW_CHUNK;
    this->_current_token = new CHAR[this->_current_token_buf_size];
    this->_current_token[0] = NULL;
    this->_current_token_length = 0;
    this->_current_object = NULL;
    this->_current_state = NULL;
    this->_retvalInternalCreated = FALSE;
}

CmzJsonParser::~CmzJsonParser() {
    if (this->_current_token) {
        delete[] this->_current_token;
    }
    this->_deleteCurrentObjectArray();
    this->_deleteCurrentStateArray();
    this->_cleanRetvalObject();
}

VOID CmzJsonParser::_deleteCurrentObjectArray() {
    ULONGLONG i;
    CmzJsonParserEntity *entity;

    if (this->_current_object) {
        for (i = 0; i < this->_current_object->size(); i++) {
            entity = (CmzJsonParserEntity*)this->_current_object->getAt(i);
            if (entity) {
                delete entity;
            }
        }
        this->_current_object->clear();
        delete this->_current_object;
        this->_current_object = NULL;
    }
}

VOID CmzJsonParser::_deleteCurrentStateArray() {
    if (this->_current_state) {
        this->_current_state->clear();
        delete this->_current_state;
        this->_current_state = NULL;
    }
}

VOID CmzJsonParser::_initParsingProcess(CmzStdObject* destination) {
    this->_cleanRetvalObject();
    if (this->_current_object) {
        this->_deleteCurrentObjectArray();
    }
    this->_current_object = new CmzPointerList();
    if (this->_current_state) {
        this->_deleteCurrentStateArray();
    }
    this->_current_state = new CmzArrayList();
    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY);
    if (!this->_current_token) {
        this->_current_token_buf_size = CMZJSONPARSER_GROW_CHUNK;
        this->_current_token = new CHAR[this->_current_token_buf_size];
    }
    this->_current_token[0] = NULL;
    this->_current_token_length = 0;
    
    if (destination) {
        this->_retval = destination;
        this->_retvalInternalCreated = FALSE;
    } else {
        this->_retval = new CmzStdObject();
        this->_retvalInternalCreated = TRUE;
    }
    
    CmzJsonParserEntity *entity = new CmzJsonParserEntity(this->_retval, CMZJSONPARSER_CURRENT_ITEM_OBJECT);
    this->_current_object->push(entity);
    this->_current_object_attribute_name = "root";
    this->_char_number_in_file = -1;
    this->_char_number_in_line = -1;
}

CmzStdObject* CmzJsonParser::parseFile(CmzString fname, CmzStdObject* destination) {
    this->_error = 0;
    this->_error_msg = "";
    this->_line_number = 1;

    HANDLE fp = CreateFileW(fname.getBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
        this->_error = 1;
        this->_error_msg = CmzString(L"Nie można otworzyć pliku: ") + fname;
		return NULL;
	}

    this->_initParsingProcess(destination);
    
	DWORD read;
    DWORD buf_size = 8192;
	CHAR *buf = new CHAR[buf_size];
    while (1) {
		::ReadFile(fp, buf, buf_size, &read, NULL);
        if (read < buf_size) {
            break;
        }
        this->_processBuffer(buf, read);
        if (this->_error) {
            break;
        }
    }

    if (!this->_error && read > 0) {
        this->_processBuffer(buf, read);
    }

	CloseHandle(fp);

    if (!this->_error && this->_current_state->size() > 1 && this->_current_state->getLast() != CMZJSONPARSER_STATE_STOP_PARSING) {
        this->_error = 1;
        this->_error_msg = CmzString("Error at line ") + this->_line_number + CmzString(". Unexpected end of data.");
    }

	delete[] buf;

    CmzStdObject *retval = this->_retval;
    if (this->_error) {
        this->_cleanRetvalObject();
        return NULL;
    }
    this->_retval = NULL;
    this->_retvalInternalCreated = FALSE;
	return retval;
}

CmzStdObject* CmzJsonParser::parse(CmzString content, CmzStdObject* destination) {
    this->_error = 0;
    this->_error_msg = "";
    this->_line_number = 1;

    this->_initParsingProcess();
    size_t buf_size = content.length() + 1;
    ULONGLONG len;
	CHAR* buf = new CHAR[buf_size];
    if (buf) {
        len = content.toAnsi(buf, buf_size);
        if (len == buf_size-1) {
            this->_processBuffer(buf, static_cast<DWORD>(len));
        } else {
            this->_error = 1;
            this->_error_msg = CmzString(L"Error during parser initialization. Unsuccessfull converting WCHAR* to CHAR*.");
        }
        delete[] buf;
        buf = NULL;
    }

    if (!this->_error && this->_current_state->size() > 1 && this->_current_state->getLast() != CMZJSONPARSER_STATE_STOP_PARSING) {
        this->_error = 1;
        this->_error_msg = CmzString("Error at line ") + this->_line_number + CmzString(". Unexpected end of data.");
    }

    CmzStdObject *retval = this->_retval;
    if (this->_error) {
        this->_cleanRetvalObject();
        return NULL;
    }
    this->_retval = NULL;
    this->_retvalInternalCreated = FALSE;
	return retval;
}

VOID CmzJsonParser::_processBuffer(CHAR *buf, DWORD buf_len) {
    DWORD i = -1;
    CHAR ch;
    LONGLONG current_state;

    while (i + 1 < buf_len) {
        this->_char_number_in_line++;
        this->_char_number_in_file++;
        i++;
        ch = buf[i];
        if (ch == '\n') {
            this->_char_number_in_line = -1;
            this->_line_number++;
        }

        current_state = this->_current_state->getLast();
        switch (current_state) {
        case CMZJSONPARSER_STATE_STOP_PARSING:
            // sprawdzamy, czy w aktualnym buforze są jakieś znaki
            // różne od białych znaków, jeśli tak to błąd
            if (!this->_isWhiteSpace(ch)) {
                this->_error = 1;
                this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Unexpected data at end of file.");
                return;
            }
            break;

        case CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY:
            if (ch != '{' && ch != '[') {
                if (this->_isWhiteSpace(ch)) {
                    continue;
                } else {
                    this->_error = 1;
                    this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Object or array expected.");
                    return;
                }
            } else {
                if (ch == '[') {
                    this->_onBeginArray();
                } else if (ch == '{') {
                    this->_onBeginObject();
                }
            }
            break;




        case CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY:
            if (ch != '{' && ch != '[' && ch != '\'' && ch != '"' && ch != ']') {
                if (this->_isWhiteSpace(ch)) {
                    continue;
                } else {
                    this->_error = 1;
                    this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Object, array or scalar value expected.");
                    return;
                }
            } else {
                CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
                if (entity->entity_type != CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
                    this->_error = 1;
                    this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". INTERNAL ERROR. Array expected.");
                    return;
                }

                switch (ch) {
                case '[':
                    this->_onBeginArray();
                    break;
                case '{':
                    this->_onBeginObject();
                    break;
                case ']':
                    this->_current_state->pop();
                    delete (CmzJsonParserEntity*)this->_current_object->pop();
                    this->_onCloseSquare();
                    break;
                case '"':
                    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE);
                    this->_current_token[0] = NULL;
                    this->_current_token_length = 0;
                    break;
                case '\'':
                    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_SINGLE_QUOTE);
                    this->_current_token[0] = NULL;
                    this->_current_token_length = 0;
                    break;
                }
            }
            break;



        case CMZJSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_DOUBLE_QUOTE:
            this->_onExpectArrayValueInQuote('"', ch);
            if (this->_error) {
                return;
            }
            break;



        case CMZJSONPARSER_STATE_EXPECT_ARRAY_VALUE_IN_SINGLE_QUOTE:
            this->_onExpectArrayValueInQuote('\'', ch);
            if (this->_error) {
                return;
            }
            break;



        case CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE:
            if (this->_isWhiteSpace(ch)) {
                continue;
            }
            if (ch == ',') {
                this->_current_state->pop();
                break;
            }
            if (ch == ']') {
                CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
                this->_current_state->pop();
                if (this->_current_state->getLast() == CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY) {
                    this->_current_state->pop();
                }
                delete (CmzJsonParserEntity*)this->_current_object->pop();
                if (this->_current_state->size() == 1) {
                    this->_current_state->append(CMZJSONPARSER_STATE_STOP_PARSING);
                    break;
                } else {
                    CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
                    if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
                        this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
                    } else if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_OBJECT) {
                        this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                    }
                }
                break;
            }
            // jeśli tu jesteśmy to błąd
            this->_error = 1;
            this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Comma or bracket ] expected.");
            return;




        case CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE:
            if (this->_isWhiteSpace(ch)) {
                continue;
            }
            if (ch == ',') {
                this->_current_state->pop();
                break;
            }
            if (ch == '}') {
                CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
                this->_current_state->pop();
                if (this->_current_state->getLast() == CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME) {
                    this->_current_state->pop();
                }
                delete (CmzJsonParserEntity*)this->_current_object->pop();
                if (this->_current_state->size() == 1) {
                    this->_current_state->append(CMZJSONPARSER_STATE_STOP_PARSING);
                    break;
                } else {
                    CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
                    if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
                        this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
                    } else if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_OBJECT) {
                        this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
                    }
                }
                break;
            }

            // jeśli tu jesteśmy to błąd
            this->_error = 1;
            this->_error_msg = CmzString("Błąd w linii ") + this->_line_number + CmzString(". Oczekiwano przecinka lub nawiasu }.");
            return;



        case CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME:
            if (this->_current_token_length == 0 && this->_isWhiteSpace(ch)) {
                continue;
            }
            if (ch == '\'' && this->_current_token_length == 0 && this->_object_property_name_quote == '~') {
                this->_object_property_name_quote = '\'';
                break;
            }
            if (ch == '"' && this->_current_token_length == 0 && this->_object_property_name_quote == '~') {
                this->_object_property_name_quote = '"';
                break;
            }
            if (
                (ch == '\'' && this->_object_property_name_quote == '\'')
                ||
                (ch == '"' && this->_object_property_name_quote == '"')
            ) {
                // jeśli bieżący token jest pusty, to błąd
                if (this->_current_token_length < 1) {
                    this->_error = 1;
                    this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Empty value for object attribute name.");
                    return;
                }
                this->_onCompleteReadObjectAttributeName();
                this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON);
                break;
            }

            if (ch == ':') {
                // jeśli bieżący token jest pusty, to błąd
                if (this->_current_token_length < 1) {
                    this->_error = 1;
                    this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Empty value for object attribute name.");
                    return;
                }
                this->_onCompleteReadObjectAttributeName();
                this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE);
                break;
            }

            // jeśli mamy biały znak i otwarty jakiś cudzysłów to błąd
            if (this->_object_property_name_quote != '~' && this->_isWhiteSpace(ch)) {
                this->_error = 1;
                this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Unallowed whitespace in object attribute name.");
                return;
            }

            if (this->_object_property_name_quote == '~' && this->_isWhiteSpace(ch)) {
                // jeśli bieżący token jest pusty, to błąd
                if (this->_current_token_length < 1) {
                    this->_error = 1;
                    this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Empty value for object attribute name.");
                    return;
                }
                this->_onCompleteReadObjectAttributeName();
                this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON);
                break;
            }

            // jeśli mamy zamykający nawias klamrowy, to
            // koniec pobierania obiektu
            if (ch == '}') {
                delete (CmzJsonParserEntity*)this->_current_object->pop();
                this->_current_state->pop();
                if (this->_current_state->getLast() == CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE) {
                    this->_current_state->pop();
                }
                this->_onCloseSquare();
                break;
            }

            // tutaj dodajemy znak do tokena
            this->_appendCharToCurrentToken(ch);

            break;




        case CMZJSONPARSER_STATE_EXPECT_OBJECT_SEMICOLON:
            if (this->_isWhiteSpace(ch)) {
                continue;
            }
            if (ch == ':') {
                this->_current_state->pop();
                this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE);
                break;
            }
            // jeśli tu jesteśmy, to błąd
            this->_error = 1;
            this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Colon expected ':'.");
            return;



        case CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_VALUE:
            if (this->_current_token_length == 0) {
                if (this->_isWhiteSpace(ch)) {
                    continue;
                }
                if (ch == '{') {
                    this->_current_state->pop();
                    this->_onBeginObject();
                    break;
                }
                if (ch == '[') {
                    this->_current_state->pop();
                    this->_onBeginArray();
                    break;
                }
                if (ch == '\'') {
                    this->_current_state->pop();
                    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_SINGLE_QUOTE);
                    break;
                }
                if (ch == '"') {
                    this->_current_state->pop();
                    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE);
                    break;
                }

                // jeśli mamy inny znak, to błąd - wartość powinna być
                // w cudzysłowach
                this->_error = 1;
                this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Scalar value of object attribute [") + this->_current_object_attribute_name + L"] should be quoted.";
                return;
            }

            // jeśli tu jesteśmy, to błąd
            this->_error = 1;
            this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Scalar value of object attribute should be quoted.");
            return;



        case CMZJSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_DOUBLE_QUOTE:
            this->_onExpectObjectValueInQuote('"', ch);
            if (this->_error) {
                return;
            }
            break;



        case CMZJSONPARSER_STATE_EXPECT_OBJECT_VALUE_IN_SINGLE_QUOTE:
            this->_onExpectObjectValueInQuote('\'', ch);
            if (this->_error) {
                return;
            }
            break;
        }

        if (this->_current_state->size() < 1) {
            break;
        }
    }
}


VOID CmzJsonParser::_onExpectArrayValueInQuote(CHAR quoteType, CHAR ch) {
    if (ch == quoteType) {
        CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
        if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
            CmzString v = this->_getStringFromCurrentToken();
            this->_current_token[0] = NULL;
            this->_current_token_length = 0;
            CmzPointerList *a = (CmzPointerList*)entity->entity;
            CmzStdObjectProperty *p = nullptr;
			BOOL isInt, isDouble;
			if (v.isNumeric(&isInt, &isDouble)) {
				if (isInt) {
					p = new CmzStdObjectProperty(CmzString(a->size()), static_cast<LONGLONG>(v.toInt()));
				} else if (isDouble) {
					p = new CmzStdObjectProperty(CmzString(a->size()), v.toDouble());
				} else {
					this->_error = 1;
					this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Improper numerical array value (not LONGLONG and not DOUBLE).");
				}
			} else {
				p = new CmzStdObjectProperty(CmzString(a->size()), v);
			}
            a->append(p);
            this->_current_state->pop();
            this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
            return;
        } else {
            this->_error = 1;
            this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". INTERNAL ERROR. Array expected.");
            return;
        }
    } else {
        this->_appendCharToCurrentToken(ch);
    }
}

VOID CmzJsonParser::_onExpectObjectValueInQuote(CHAR quoteType, CHAR ch) {
    if (ch == quoteType) {
        this->_onCompleteReadObjectAttributeSimpleValue();
        this->_current_state->pop();
        this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
    } else {
        this->_appendCharToCurrentToken(ch);
    }
}

VOID CmzJsonParser::_onBeginArray() {
    CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
    CmzPointerList *newA = new CmzPointerList();
    if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
        CmzPointerList *a = (CmzPointerList*)entity->entity;
        CmzStdObjectProperty *p = new CmzStdObjectProperty(CmzString(a->size()), newA);
        a->append(p);
    } else if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_OBJECT) {
        CmzStdObject *o = (CmzStdObject*)entity->entity;
        o->set(this->_current_object_attribute_name.getBuffer(), newA);
    }
    CmzJsonParserEntity *new_entity = new CmzJsonParserEntity(newA, CMZJSONPARSER_CURRENT_ITEM_ARRAY);
    this->_current_object->push(new_entity);
    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_OR_ARRAY_OR_VALUE_IN_ARRAY);
    this->_current_token[0] = NULL;
    this->_current_token_length = 0;
}

VOID CmzJsonParser::_onBeginObject() {
    CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
    CmzStdObject *newO = new CmzStdObject();
    if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
        CmzPointerList *a = (CmzPointerList*)entity->entity;
        CmzStdObjectProperty *p = new CmzStdObjectProperty(CmzString(a->size()), newO);
        a->append(p);
    } else if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_OBJECT) {
        CmzStdObject *o = (CmzStdObject*)entity->entity;
        o->set(this->_current_object_attribute_name.getBuffer(), newO);
    }
    CmzJsonParserEntity *new_entity = new CmzJsonParserEntity(newO, CMZJSONPARSER_CURRENT_ITEM_OBJECT);
    this->_current_object->push(new_entity);
    this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_OBJECT_PROPERTY_NAME);
    this->_current_token[0] = NULL;
    this->_current_token_length = 0;
    this->_object_property_name_quote = '~';
}

CmzString CmzJsonParser::_getStringFromCurrentToken() {
    LONGLONG outbuf_size = this->_current_token_length + 1;
    WCHAR *outbuf = new WCHAR[outbuf_size];
    MultiByteToWideChar(CP_UTF8, 0, &this->_current_token[0], -1, outbuf, static_cast<int>(outbuf_size));
    CmzString v(outbuf, outbuf_size);
    return v;
}

VOID CmzJsonParser::_onCompleteReadObjectAttributeName() {
    this->_current_object_attribute_name = this->_getStringFromCurrentToken();
    this->_current_token[0] = NULL;
    this->_current_token_length = 0;
    this->_object_property_name_quote = '~';
}

VOID CmzJsonParser::_onCompleteReadObjectAttributeSimpleValue() {
    CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
    if (entity->entity_type != CMZJSONPARSER_CURRENT_ITEM_OBJECT) {
        this->_error = 1;
        this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". INTERNAL ERROR. Object expected.");
        return;
    }
    CmzStdObject *o = (CmzStdObject*)entity->entity;
    CmzString v = this->_getStringFromCurrentToken();
	BOOL isInt, isDouble;
	if (v.isNumeric(&isInt, &isDouble)) {
		if (isInt) {
			o->set(this->_current_object_attribute_name.getBuffer(), static_cast<LONGLONG>(v.toInt()));
		} else if (isDouble) {
			o->set(this->_current_object_attribute_name.getBuffer(), v.toDouble());
		} else {
			this->_error = 1;
			this->_error_msg = CmzString("Error at line: ") + this->_line_number + CmzString(". Improper numerical scalar value of object attribute (not LONGLONG and not DOUBLE).");
		}
	} else {
		o->set(this->_current_object_attribute_name.getBuffer(), v);
	}
    this->_current_object_attribute_name = "";
    this->_current_token[0] = NULL;
    this->_current_token_length = 0;
    this->_object_property_name_quote = '~';
}

VOID CmzJsonParser::_appendCharToCurrentToken(CHAR ch) {
    if (this->_current_token_length >= this->_current_token_buf_size - 1) {
        LONGLONG new_size = this->_current_token_buf_size + CMZJSONPARSER_GROW_CHUNK;
        CHAR *tmp = new CHAR[new_size];
        memcpy(tmp, this->_current_token, this->_current_token_length);
        delete[] this->_current_token;
        this->_current_token = tmp;
        this->_current_token_buf_size = new_size;
    }
    this->_current_token[this->_current_token_length] = ch;
    this->_current_token_length++;
    this->_current_token[this->_current_token_length] = NULL;
}

VOID CmzJsonParser::_onCloseSquare() {
    if (this->_current_state->size() == 1) {
        this->_current_state->append(CMZJSONPARSER_STATE_STOP_PARSING);
    } else {
        CmzJsonParserEntity *entity = (CmzJsonParserEntity*)this->_current_object->getLast();
        if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_ARRAY) {
            this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_SQUARE_BRACE);
        } else if (entity->entity_type == CMZJSONPARSER_CURRENT_ITEM_OBJECT) {
            this->_current_state->append(CMZJSONPARSER_STATE_EXPECT_COMMA_OR_CLOSE_CURLY_BRACE);
        }
    }
}

VOID CmzJsonParser::_cleanRetvalObject() {
    if (this->_retval) {
        if (this->_retvalInternalCreated) {
            delete this->_retval;
        }
        this->_retval = NULL;
        this->_retvalInternalCreated = FALSE;
    }
}

}
