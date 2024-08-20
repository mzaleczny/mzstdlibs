// mzstdwin32.h: główny plik nagłówka dla biblioteki DLL mzstdwin32
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// Cmzstdwin32App
// Aby uzyskać implementację klasy, zobacz mzstdwin32.cpp
//

class Cmzstdwin32App : public CWinApp
{
public:
	Cmzstdwin32App();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
