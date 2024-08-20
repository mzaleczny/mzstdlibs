// mzstdshell.h: główny plik nagłówka dla biblioteki DLL mzstdshell
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CmzstdshellApp
// Aby uzyskać implementację klasy, zobacz mzstdshell.cpp
//

class CmzstdshellApp : public CWinApp
{
public:
	CmzstdshellApp();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
