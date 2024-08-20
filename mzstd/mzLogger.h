#ifndef __MZSTD_LOGGER_H__
#define __MZSTD_LOGGER_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzFileUtils.h"

namespace mzstd {

#define DEFAULT_LOGGER_FILENAME                         L"trace.log"

#define LOG(MESSAGE)                                    WriteLogMessage((DWORD)__LINE__, (CHAR*)__FILE__, ENC_UTF8, (MESSAGE));
#define LOGFMT1(FORMAT, PARAM1)                         WriteLogMessage((DWORD)__LINE__, (CHAR*)__FILE__, ENC_UTF8, FormatMessage((WCHAR*)(FORMAT), (PARAM1)).toWCHAR());
#define LOGFMT2(FORMAT, PARAM1, PARAM2)                 WriteLogMessage((DWORD)__LINE__, (CHAR*)__FILE__, ENC_UTF8, FormatMessage((WCHAR*)(FORMAT), (PARAM1), (PARAM2)).toWCHAR());
#define LOGFMT3(FORMAT, PARAM1, PARAM2, PARAM3)         WriteLogMessage((DWORD)__LINE__, (CHAR*)__FILE__, ENC_UTF8, FormatMessage((WCHAR*)(FORMAT), (PARAM1), (PARAM2), (PARAM3)).toWCHAR());
#define LOGMEM(MEMORY, SIZE)                            WriteMemoryLogMessage((DWORD)__LINE__, (CHAR*)__FILE__, ENC_UTF8, (MEMORY), (SIZE));


class DECLSPEC_MZSTD CmzLogger : public CmzObject {
public:
	WINAPI CmzLogger(CmzString filename);
    virtual WINAPI ~CmzLogger();

    VOID __cdecl log(WCHAR* format, va_list paramList);
	VOID __cdecl log(WCHAR* format, ...);
	VOID __cdecl log(WCHAR* message, BOOL appendEOL = FALSE);
    VOID WINAPI close();
    VOID WINAPI reopen();
    VOID WINAPI flush();
    BOOL WINAPI isReady() { return this->_fh != INVALID_HANDLE_VALUE; }
    
    inline CmzString WINAPI getFilename() { return this->_filename; }

protected:
    HANDLE _fh;
    CmzString _filename;
    
private:
    ULONGLONG _writeOperations;
};
typedef CmzLogger* PLogger;

DECLSPEC_MZSTD BOOL WINAPI InitLogger(CmzString fname = DEFAULT_LOGGER_FILENAME);
DECLSPEC_MZSTD VOID WINAPI CleanupLogger();
DECLSPEC_MZSTD BOOL __cdecl WriteLogMessage(DWORD lineNumber, CHAR* sourceFilename, LONGLONG encoding, WCHAR *message, ...);
DECLSPEC_MZSTD BOOL __cdecl WriteLogMessage(WCHAR *message, ...);
DECLSPEC_MZSTD BOOL WINAPI WriteMemoryLogMessage(DWORD lineNumber, CHAR* sourceFilename, LONGLONG encoding, LPVOID memory, ULONGLONG size);
DECLSPEC_MZSTD VOID WINAPI FlushLogger();

}

#endif
