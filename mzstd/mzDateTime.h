#ifndef __MZSTD_DATETIME_H__
#define __MZSTD_DATETIME_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzDateTime : public CmzObject {
public:
	WINAPI CmzDateTime();
    virtual WINAPI ~CmzDateTime();
    
    static CmzString date(WCHAR* format);
};

}

#endif
