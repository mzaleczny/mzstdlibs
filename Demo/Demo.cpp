// Demo.cpp : Definiuje punkt wejścia dla aplikacji.
//

#include "framework.h"
#include "Demo.h"
#include "mzString.h"
#include "mzStringList.h"
#include "mzApplication.h"
#include "mzWindow.h"
#include "mzStyledWindow.h"
#include "mzPanel.h"
#include "mzButton.h"
#include "mzCheckbox.h"
#include "mzScrollbarHorizontal.h"
#include "mzScrollbarVertical.h"
#include "mzLabel.h"
#include "mzTextField.h"
#include "mzToolbar.h"
#include "mzListbox.h"
#include "mzGrid.h"
#include "mzMenu.h"
#include "mzTimer.h"
#include <string>
using namespace std;

#define MAX_LOADSTRING 100

// Zmienne globalne:
HWND hWnd;
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego
CmzImageList* ImageList = new CmzImageList();
CmzImageList* ImageList16 = new CmzImageList();

// ---------------------------------------------------------------------------------------------------------------------------------------
int Counter1, Counter2, Counter3;
class CSomeClass {
    void OnTimer1(DWORD dwTime)
    {
        Counter1++;
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    void OnTimer2(DWORD dwTime)
    {
        Counter2++;
        if (Counter2 > 10)
        {
            CTimer::KillTimer(Timer2_PTR);
        }
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    void OnTimer3(DWORD dwTime)
    {
        Counter3++;
        InvalidateRect(hWnd, nullptr, FALSE);
    }

    UINT_PTR Timer1_PTR;
    UINT_PTR Timer2_PTR;
    UINT_PTR Timer3_PTR;
public:
    ~CSomeClass()
    {
        CTimer::CleanupTimers();
    }
    void DoTimerStuff() {
        Timer1_PTR = CTimer::AddTimer(new CTimer::SpecialisedCallback<CSomeClass>(this, &CSomeClass::OnTimer1), 100);
        Timer2_PTR = CTimer::AddTimer(new CTimer::SpecialisedCallback<CSomeClass>(this, &CSomeClass::OnTimer2), 300);
        Timer3_PTR = CTimer::AddTimer(new CTimer::SpecialisedCallback<CSomeClass>(this, &CSomeClass::OnTimer3), 500);
    }
};
CSomeClass Test;



class CApplicationWindow : public CStyledWindow
{
public:
    CApplicationWindow(CApplication* app, CWindow* parentWindow, mzstd::CmzString name, mzstd::CmzString layoutFilename, CWindowClass* wc = NULL, HWND hWnd = NULL) : CStyledWindow(app, parentWindow, name, layoutFilename, wc, hWnd) {
        SetWindowPos(this->_hWnd, NULL, 150, 50, 1250, 700, 0);
    }
    virtual LRESULT WINAPI onLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) override;
    virtual VOID WINAPI onMenuItemClicked(mzstd::CmzStdObject* item) override;
    virtual VOID WINAPI onToolbarItemClicked(mzstd::CmzStdObject* item) override;
};



void BuildLayout(CStyledWindow* sw);

// ---------------------------------------------------------------------------------------------------------------------------------------

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: W tym miejscu umieść kod.

    // Inicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DEMO, szWindowClass, MAX_LOADSTRING);
    /*
    MyRegisterClass(hInstance);

    // Wykonaj inicjowanie aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    */
    CApplication* Application = new CApplication(hInstance, hPrevInstance, lpCmdLine, nCmdShow, L"L.K. Progmar Marcin Załęczny", L"Demo", L".");
    if (Application)
    {
        CWindow* Window = new CWindow(Application, nullptr, L"DemoWindow");
        if (Window)
        {
            CApplicationWindow* ApplicationWindow = new CApplicationWindow(Application, Window, L"DemoWindow", COMMON_EMPTY_STRING);
            if (ApplicationWindow)
            {
                BuildLayout(ApplicationWindow);
                Application->run(ApplicationWindow);
            }
        }
    }
    
    if (ImageList)
    {
        delete ImageList;
        ImageList = nullptr;
    }

    if (ImageList16)
    {
        delete ImageList16;
        ImageList16 = nullptr;
    }
    /*
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO));

    MSG msg;

    Test.DoTimerStuff();
    // Główna pętla komunikatów:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    */
    return 0;
}



//
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_DEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna głównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno główne
//  WM_DESTROY  - opublikuj komunikat o wyjściu i wróć
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizuj zaznaczenia menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Tutaj dodaj kod rysujący używający elementu hdc...
            RECT rc;
            GetClientRect(hWnd, &rc);
            FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            wstring s = L"Counter1: " + to_wstring(Counter1);
            TextOut(hdc, 10, 10, s.c_str(), static_cast<INT>(s.length()));
            s = L"Counter2: " + to_wstring(Counter2);
            TextOut(hdc, 10, 40, s.c_str(), static_cast<INT>(s.length()));
            s = L"Counter3: " + to_wstring(Counter3);
            TextOut(hdc, 10, 70, s.c_str(), static_cast<INT>(s.length()));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Procedura obsługi komunikatów dla okna informacji o programie.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void BuildLayout(CStyledWindow* sw)
{
    CSprite* MainPanel = sw->getSpriteByName(L"mainPanel");
    ImageList->load(L".\\resources\\Images");
    ImageList16->load(L".\\resources\\Images\\Toolbar16");

    mzstd::CMenu* MainMenu = new mzstd::CMenu(MainPanel, L"MainMenu", nullptr, 1, ImageList16);
    MainMenu->loadMenuFromFile(L".\\resources\\MainMenu.json");
    MainPanel->addChild(MainMenu);

    int X = 10;
    int Y = MainMenu->height;

    CToolbar* tb = new CToolbar(MainPanel, L"Toolbar", L".\\resources\\Images\\Toolbar16\\AppToolbar16.json", ImageList16, 16, 0, Y);
    tb->height = 32;
    MainPanel->addChild(tb);
    Y += tb->height + 20;

    int StartY = Y + 10;
    Y = StartY;

    CmzCheckbox* chk = new CmzCheckbox(MainPanel, L"chkCheckbox", L"Checkbox zaznaczony", L"Checkbox odznaczony", FALSE, X, Y, 160);
    chk->setTabStop(FALSE);
    MainPanel->addChild(chk);
    Y += 30;

    CScrollbarHorizontal* hscrlbar = new CScrollbarHorizontal(MainPanel, L"HorizontalScrollBar", X, Y);
    hscrlbar->setSize(200, 24);
    hscrlbar->setThumbSize(24);
    hscrlbar->setTabStop(FALSE);
    MainPanel->addChild(hscrlbar);
    Y += 30;

    CScrollbarVertical* vscrlbar = new CScrollbarVertical(MainPanel, L"VertcialScrollBar", X + 250, StartY);
    vscrlbar->setSize(24, 350);
    vscrlbar->setThumbSize(24);
    vscrlbar->setTabStop(FALSE);
    MainPanel->addChild(vscrlbar);

    CLabel* lbl = new CLabel(MainPanel, L"MyLabel", X, Y, L"Zwykła etykieta wielolinijkowa\nLinijka 222\nLinijka 333", 200, 40);
    lbl->setBgColor(0x404040, FALSE);
    lbl->setTabStop(FALSE);
    MainPanel->addChild(lbl);
    Y += 50;

    CTextField* tf = new CTextField(MainPanel, L"TextField", X, Y, 200);
    tf->setText(L"Some Text", FALSE);
    MainPanel->addChild(tf);
    Y += 30;
    tf = new CTextField(MainPanel, L"TextField", X, Y, 200);
    MainPanel->addChild(tf);
    tf->setText(L"Some Other Text", FALSE);
    tf->setSelection(5, 5, FALSE);
    tf->focus();
    Y += 30;

    static mzstd::CmzStringList StringList;
    StringList.append(L"Wiedźmin część pierwsza");
    StringList.append(L"Wiedźmin część druga");
    StringList.append(L"Wiedźmin część trzecia");
    StringList.append(L"Cyberpunk 2077");
    StringList.append(L"Baldur's Gate część trzecia");
    CListbox* lb = new CListbox(MainPanel, L"Listbox", &StringList, X, Y);
    MainPanel->addChild(lb);
    X = 10;
    Y = lb->y + lb->height + 20;
    lb->setTabStop(false);

    CmzButton* btn = new CmzButton(MainPanel, L"btnOk", L"OK", X, Y);
    btn->setImage(ImageList, 0);
    btn->setTabStop(FALSE);
    MainPanel->addChild(btn);
    btn = new CmzButton(MainPanel, L"btnAnuluj", L"Anuluj", btn->x + btn->width + 15, Y);
    btn->setImage(ImageList, 1);
    btn->setTabStop(FALSE);
    MainPanel->addChild(btn);
    btn = new CmzButton(MainPanel, L"btnZamknij", L"Zamknij", btn->x + btn->width + 15, Y);
    btn->setTabStop(FALSE);
    MainPanel->addChild(btn);

    Y = 60;
    CGrid* grid = new CGrid(MainPanel, L"Grid", 40, 20, X + 290, StartY, 900, 385);
    MainPanel->addChild(grid);
    grid->setTabStop(false);

    sw->getCaret()->setColor(0x00ffffff);
}

LRESULT WINAPI CApplicationWindow::onLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam)
{
    CStyledWindow::onLButtonMouseDown(hWnd, message, wParam, lParam);

    CSprite* control = this->getActiveControl();
    if (control)
    {
        if (control->name == L"btnZamknij")
        {
            DestroyWindow(this->_hWnd);
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

VOID WINAPI CApplicationWindow::onMenuItemClicked(mzstd::CmzStdObject* item)
{
    if (item->get(L"id")->getAsString() == L"btnClose")
    {
        DestroyWindow(this->_hWnd);
    }
}

VOID WINAPI CApplicationWindow::onToolbarItemClicked(mzstd::CmzStdObject* item)
{
    if (item->get(L"id")->getAsString() == L"btnClose")
    {
        DestroyWindow(this->_hWnd);
    }
}
