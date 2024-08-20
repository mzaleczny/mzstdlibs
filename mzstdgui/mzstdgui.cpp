﻿// mzstdgui.cpp: Definiuje procedury inicjowania biblioteki DLL.
//

#include "pch.h"
#include "framework.h"
#include "mzstdgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: Jeśli ta biblioteka DLL jest połączona dynamicznie z bibliotekami DLL MFC,
//		wszystkie funkcje wyeksportowane z tej biblioteki DLL, które wywołują
//		muszą mieć makro AFX_MANAGE_STATE dodane w
//		na samym początku funkcji.
//
//		Na przykład:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// zwykłe ciało funkcji
//		}
//
//		Jest bardzo ważnym, żeby to makro pojawiło się w każdym
//		funkcję, przed dowolnymi wywołaniami MFC.  Oznacza to, że
//		musi wystąpić jako pierwsza instrukcja w obrębie
//		funkcję, nawet przed dowolnymi deklaracjami zmiennych obiektu
//		tak, jak ich konstruktory mogę generować wywołania MFC
//		biblioteka DLL.
//
//		Zobacz uwagi techniczne MFC nr 33 i 58 dla dodatkowych
//		szczegóły.
//

// CmzstdguiApp

BEGIN_MESSAGE_MAP(CmzstdguiApp, CWinApp)
END_MESSAGE_MAP()


// Konstrukcja klasy CmzstdguiApp

CmzstdguiApp::CmzstdguiApp()
{
	// TODO: W tym miejscu dodaj kod konstruktora,
	// Umieść wszystkie znaczące inicjacje w InitInstance
}


// Jedyny obiekt CmzstdguiApp

CmzstdguiApp theApp;


// Inicjowanie klasy CmzstdguiApp

BOOL CmzstdguiApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
