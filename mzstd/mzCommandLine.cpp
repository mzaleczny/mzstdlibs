#include "pch.h"
#include "mzstdCommon.h"
#include "mzCommandLine.h"
#include "mzstd.h"

namespace mzstd {

#define PARAM_NAME_STARTING_CHARS       L"-/"


VOID CmzCommandLine::_commonInit(WCHAR* commandLine) {
    this->setRealClassName(L"CKeyboard");
    
    if (commandLine) {
        ULONGLONG i;
        size_t len = wcslen(commandLine);
        BOOL LONGLONGparam_started = FALSE;
        WCHAR cur_ch;
        CmzString param;
        
        for (i = 0; i < len; i++) {
            cur_ch = commandLine[i];
            if (cur_ch == L'"') {
                if (!LONGLONGparam_started) {
                    LONGLONGparam_started = TRUE;
                } else {
                    if (param.length() > 0) {
                        this->append(param);
                    }
                    param = COMMON_EMPTY_STRING;
                    LONGLONGparam_started = FALSE;
                }
                continue;
            }
            
            if (isCharSpace(cur_ch)) {
                if (LONGLONGparam_started) {
                    param += cur_ch;
                } else {
                    if (param.length() > 0) {
                        this->append(param);
                    }
                    param = COMMON_EMPTY_STRING;
                }
                continue;
            }
            
            param += cur_ch;
        }
        
        if (param.length() > 0) {
            this->append(param);
        }
    }
}

CmzCommandLine::CmzCommandLine() {
    this->_commonInit(NULL);
}

CmzCommandLine::CmzCommandLine(LONGLONG argc, WCHAR** argv) {
    CmzString commandLine;
    for (LONGLONG i = 1; i < argc; i++) { // we skip program name in first array element
        CmzString param(argv[i]);
        if (param.containsWhiteSpace()) {
            commandLine += CmzString(L"\"") + argv[i] + L"\"";
        } else {
            commandLine += argv[i];
        }
        if (i < argc-1) {
            commandLine += L" ";
        }
    }
    
    this->_commonInit(commandLine.getBuffer());
}

CmzCommandLine::CmzCommandLine(WCHAR* commandLine) {
    this->_commonInit(commandLine);
}

CmzCommandLine::~CmzCommandLine() {
}

LONGLONG CmzCommandLine::getParamPos(WCHAR* param) {
    if (!param || wcslen(param) < 1) {
        return -1;
    }
    
    CmzString sparam(param);
    CmzString cparam;
    sparam.ltrimAllChars(PARAM_NAME_STARTING_CHARS);
    
    ULONGLONG size = this->size();
    for (ULONGLONG i = 0; i < size; i++) {
        cparam = this->getAt(i);
        cparam.ltrimAllChars(PARAM_NAME_STARTING_CHARS);
        if (cparam == sparam) {
            return i;
        }
    }
    
    return -1;
}

BOOL CmzCommandLine::hasParam(WCHAR* param) {
    return this->getParamPos(param) != -1;
}

CmzString CmzCommandLine::getParam(ULONGLONG pos) {
    if (pos < this->size()) {
        return this->getAt(pos);
    }
    return COMMON_EMPTY_STRING;
}

CmzString CmzCommandLine::getParamValue(WCHAR* param) {
    LONGLONG paramPos = this->getParamPos(param);
    if (paramPos == -1 || paramPos + 1 == this->size()) {
        return COMMON_EMPTY_STRING;
    }
    
    return this->getAt(paramPos+1);
}

VOID CmzCommandLine::removeParam(WCHAR* param) {
    LONGLONG paramPos = this->getParamPos(param);
    if (paramPos == -1) {
        return;
    }
    
    this->delAt(paramPos);
    while (paramPos < (LONGLONG)this->size() && this->getAt(paramPos).startsWithOneOfChars(PARAM_NAME_STARTING_CHARS)) {
        this->delAt(paramPos);
    }
}

ULONGLONG CmzCommandLine::getParamsCount() {
    if (this->size() < 1) {
        return 0;
    }
    
    CmzString cparam;    
    ULONGLONG size = this->size();
    ULONGLONG count = 0;
    for (ULONGLONG i = 0; i < size; i++) {
        cparam = this->getAt(i);
        if (cparam.startsWithOneOfChars(PARAM_NAME_STARTING_CHARS)) {
            count++;
        }
    }
    
    return count;
}

ULONGLONG CmzCommandLine::getItemsCount() {
    return this->size();
}

}
