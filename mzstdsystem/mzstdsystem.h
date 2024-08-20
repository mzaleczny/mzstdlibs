// mzstdsystem.h: główny plik nagłówka dla biblioteki DLL mzstdsystem
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CmzstdsystemApp
// Aby uzyskać implementację klasy, zobacz mzstdsystem.cpp
//

class CmzstdsystemApp : public CWinApp
{
public:
	CmzstdsystemApp();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
