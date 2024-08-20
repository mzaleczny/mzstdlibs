// mzstd.h: główny plik nagłówka dla biblioteki DLL mzstd
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CmzstdApp
// Aby uzyskać implementację klasy, zobacz mzstd.cpp
//

class CmzstdApp : public CWinApp
{
public:
	CmzstdApp();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
