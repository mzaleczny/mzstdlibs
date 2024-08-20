// mzstdgui.h: główny plik nagłówka dla biblioteki DLL mzstdgui
//

#pragma once

#ifndef __AFXWIN_H__
	#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CmzstdguiApp
// Aby uzyskać implementację klasy, zobacz mzstdgui.cpp
//

class CmzstdguiApp : public CWinApp
{
public:
	CmzstdguiApp();

// Przesłania
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
