#ifndef __MZSTD_COMMAND_LINE_H__
#define __MZSTD_COMMAND_LINE_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzStringList.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzCommandLine : public CmzStringList {
public:
    WINAPI CmzCommandLine();
    WINAPI CmzCommandLine(LONGLONG argc, WCHAR** argv);
    WINAPI CmzCommandLine(WCHAR* argv);
	virtual WINAPI ~CmzCommandLine();

    LONGLONG WINAPI getParamPos(WCHAR* param);
    BOOL WINAPI hasParam(WCHAR* param);
    CmzString WINAPI getParam(ULONGLONG pos);
    CmzString WINAPI getParamValue(WCHAR* param);
    VOID WINAPI removeParam(WCHAR* param);
    ULONGLONG WINAPI getParamsCount();
    ULONGLONG WINAPI getItemsCount();

protected:
    VOID WINAPI _commonInit(WCHAR* commandLine);
};

}

#endif;
