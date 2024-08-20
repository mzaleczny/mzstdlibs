// mzstdgraphics.h: główny plik nagłówka dla biblioteki DLL mzstdgraphics
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CmzstdgraphicsApp
// Aby uzyskać implementację klasy, zobacz mzstdgraphics.cpp
//

class CmzstdgraphicsApp : public CWinApp
{
public:
	CmzstdgraphicsApp();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
