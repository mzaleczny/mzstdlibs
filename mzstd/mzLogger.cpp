#include "pch.h"
#include "mzstdCommon.h"
#include "mzFileUtils.h"
#include "mzLogger.h"
#include "mzMemoryView.h"

namespace mzstd {

CmzLogger* logger = NULL;

CmzLogger::CmzLogger(CmzString filename) {
    this->setRealClassName(L"CmzLogger");
    this->_fh = INVALID_HANDLE_VALUE;
    this->_filename = filename;
    this->_writeOperations = 0;
    if (FileExists(this->_filename)) {
        DeleteFileW(this->_filename.toWCHAR());
    }
}

CmzLogger::~CmzLogger() {
    this->close();
}

VOID CmzLogger::close() {
    if (this->_fh != INVALID_HANDLE_VALUE) {
        CloseHandle(this->_fh);
        this->_fh = INVALID_HANDLE_VALUE;
    }
}

VOID CmzLogger::reopen() {
    this->close();
    CmzString filename = this->_filename;
    
    this->_fh = CreateFileW(filename.getBuffer(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (this->_fh != INVALID_HANDLE_VALUE) {
        this->_filename = filename;
        this->_writeOperations = 0;
	}
}

VOID CmzLogger::log(WCHAR* format, va_list paramList) {
    if (this->_fh == INVALID_HANDLE_VALUE) {
        this->reopen();
    }    
    if (this->_fh != INVALID_HANDLE_VALUE) {
        CmzString fmt(format);
        fmt.prepend(L"%dtm ");
        CmzString s = FormatMessage(fmt.getBuffer(), paramList) + L"\n";
        AppendToFile(this->_fh, s.getBuffer(), ENC_UTF8);
        this->_writeOperations += 1;
        this->flush();
    }
}

VOID CmzLogger::log(WCHAR* format, ...) {
    if (this->_fh == INVALID_HANDLE_VALUE) {
        this->reopen();
    }
    if (this->_fh != INVALID_HANDLE_VALUE) {
        va_list paramList;
        va_start(paramList, format);
        this->log(format, paramList);
        va_end(paramList);
    }
}

VOID CmzLogger::log(WCHAR* message, BOOL appendEOL) {
    if (this->_fh == INVALID_HANDLE_VALUE) {
        this->reopen();
    }
    if (this->_fh != INVALID_HANDLE_VALUE) {
        CmzString fmt(L"%dtm ");
        CmzString logmessage;
        if (appendEOL) {
            fmt += CmzString(message) + L"\r\n";
        } else {
            fmt += message;
        }
        
        logmessage = FormatMessage(fmt.getBuffer());
        AppendToFile(this->_fh, logmessage.getBuffer(), ENC_UTF8);
        this->_writeOperations += 1;
        this->flush();
    }
}

VOID CmzLogger::flush() {
    if (this->_fh != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(this->_fh);
    }
}

BOOL WINAPI InitLogger(CmzString filename) {
    logger = new CmzLogger(filename);
    if (logger) {
        return TRUE;
    }
    
    return FALSE;
}

VOID WINAPI CleanupLogger() {
    if (logger) {
        CmzString fn = logger->getFilename();
        delete logger;
        logger = NULL;
        // if log file is empty then delete it
        if (FileExists(fn)) {
            if (GetFileSize(fn) == 0) {
                DeleteFileW(fn.getBuffer());
            }
        }
    }
}

VOID WINAPI FlushLogger() {
    if (logger) {
        logger->flush();
    }
}

BOOL __cdecl WriteLogMessage(DWORD lineNumber, CHAR* sourceFilename, LONGLONG encoding, WCHAR *message, ...) {
    if (!logger) {
        InitLogger(DEFAULT_LOGGER_FILENAME);
    }
    
    if (logger) {
        va_list paramList;
        va_start(paramList, message);
        CmzString log_message = FormatMessage(L"SOURCE_FILE: %sc, SOURCE_LINE: %d, MESSAGE:", sourceFilename, lineNumber) + FormatMessage(message, paramList) + L"\n";
        logger->log(log_message.getBuffer(), FALSE);
        va_end(paramList);
        return TRUE;
    }
    
    return FALSE;
}

DECLSPEC_MZSTD BOOL __cdecl WriteLogMessage(WCHAR *message, ...) {
    if (!logger) {
        InitLogger(DEFAULT_LOGGER_FILENAME);
    }
    
    if (logger) {
        va_list paramList;
        va_start(paramList, message);
        logger->log(message, paramList);
        va_end(paramList);
        return TRUE;
    }
    
    return FALSE;
}

BOOL WINAPI WriteMemoryLogMessage(DWORD lineNumber, CHAR* sourceFilename, LONGLONG encoding, LPVOID memory, ULONGLONG size) {
    if (!logger) {
        InitLogger(DEFAULT_LOGGER_FILENAME);
    }
    
    if (logger) {
        CmzMemoryView mv(memory, static_cast<DWORD>(size), (LONGLONG)memory);
        CmzString log_message = FormatMessage(L"SOURCE_FILE: %sc, SOURCE_LINE: %d, MESSAGE:\n    Memory address: %p  memory size: %ddw\n    Memory content: %s\n", sourceFilename, lineNumber,
            memory, size, mv.getDump(L"", TRUE, FALSE, FALSE).toWCHAR());
        logger->log(log_message.toWCHAR(), FALSE);
        return TRUE;
    }
    
    return FALSE;
}

}
