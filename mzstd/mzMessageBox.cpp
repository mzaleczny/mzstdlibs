#include "pch.h"
#include "mzMessageBox.h"
#include "mzstdGlobals.h"

namespace mzstd {

LONGLONG WINAPI MessageBox(CmzString caption, CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), caption.getBuffer(), MB_OK | MB_ICONINFORMATION);
    return result;
}

LONGLONG WINAPI AlertBox(CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Uwaga", MB_OK | MB_ICONWARNING);
    return result;
}

LONGLONG WINAPI WarningBox(CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Uwaga", MB_OK | MB_ICONWARNING);
    return result;
}

LONGLONG WINAPI QuestionBox(CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Pytanie", MB_YESNO | MB_ICONQUESTION);
    return result;
}

LONGLONG WINAPI QuestionCancelBox(CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Pytanie", MB_YESNOCANCEL | MB_ICONQUESTION);
    return result;
}

LONGLONG WINAPI ErrorBox(CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Błąd", MB_OK | MB_ICONERROR);
    return result;
}

LONGLONG WINAPI InformationBox(CmzString message) {
	LONGLONG result = ::MessageBoxW(GetActiveWindow(), message.getBuffer(), L"Informacja", MB_OK | MB_ICONINFORMATION);
    return result;
}

DECLSPEC_MZSTD BOOL WINAPI isMessageBoxOpened()
{
    return FALSE;
}

VOID WINAPI ShowLastSystemError(WCHAR* prependString) {
    ShowSystemError(GetLastError(), prependString);
}

VOID WINAPI ShowSystemError(DWORD errorCode, WCHAR* prependString) {
	ErrorBox(GetSystemErrorMessage(errorCode));
}

VOID WINAPI ShowDetailedLastSystemError(WCHAR* prependString) {
    ShowDetailedSystemError(GetLastError(), prependString);
}

VOID WINAPI ShowDetailedSystemError(DWORD errorCode, WCHAR* prependString) {
	ErrorBox(GetDetailedSystemErrorMessage(errorCode, prependString));
}

}
