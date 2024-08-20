#include "pch.h"
#include "mzLocale.h"

namespace mzstd {

CmzLocale* userLocale = NULL;

CmzLocale::CmzLocale(WCHAR* localeName) {
    this->setRealClassName(L"CmzLocale");
	LCID lName = NULL;

	if (!localeName)
	{
		this->localeName = L"LOCALE_USER_DEFAULT";
	}

	if (wcscmp(localeName, L"LOCALE_SYSTEM_DEFAULT") == 0) {
		lName = LOCALE_SYSTEM_DEFAULT;
	} else if (wcscmp(localeName, L"LOCALE_USER_DEFAULT") == 0) {
		lName = LOCALE_USER_DEFAULT;
	}

	GetLocaleInfo((LCID)lName, LOCALE_SDECIMAL, &this->locale_sdecimal[0], 4);
}

CmzLocale::~CmzLocale() {
}

}