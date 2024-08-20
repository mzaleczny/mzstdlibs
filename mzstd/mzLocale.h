#ifndef __MZSTD_LOCALE_H__
#define __MZSTD_LOCALE_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzLocale : public CmzObject {
public:
	WINAPI CmzLocale(WCHAR* localeName = nullptr);
	virtual WINAPI ~CmzLocale();

	TCHAR locale_sdecimal[4];

private:
	CmzString localeName;
};

extern CmzLocale* userLocale;

}

#endif
