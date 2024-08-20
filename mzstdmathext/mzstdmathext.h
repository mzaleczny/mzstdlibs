// mzstdmathext.h: główny plik nagłówka dla biblioteki DLL mzstdmathext
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CmzstdmathextApp
// Aby uzyskać implementację klasy, zobacz mzstdmathext.cpp
//

class CmzstdmathextApp : public CWinApp
{
public:
	CmzstdmathextApp();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
