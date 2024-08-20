#include "pch.h"
#include "mzstdsystemCommon.h"
#include "mzstdsystemGlobals.h"
#include "mzTooltip.h"
#include "mzMessageBox.h"

namespace mzstd {

VOID CmzTooltip::_commonInit(HWND hwndOwner, CmzString message) {
	/*
    this->setRealClassName(L"CmzTooltip");
    this->_hwndOwner = hwndOwner;
    this->_message = message;

    this->_icex.dwSize = sizeof(this->_icex);
    this->_icex.dwICC  = ICC_BAR_CLASSES;
	this->_hwnd = ::CreateWindowW(TOOLTIPS_CLASSW, this->_message.toWCHAR(), WS_POPUP,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL, (HMENU)NULL, GetInstance(), NULL);
                               
    if (this->_hwnd) {
        // Prepare TOOLINFO structure for use as tracking ToolTip.
        this->_tinfo.cbSize = sizeof(TOOLINFO);
        this->_tinfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
        this->_tinfo.hwnd   = hwndOwner;
        this->_tinfo.uId    = (UINT)this->_hwndOwner;
        this->_tinfo.hinst  = NULL;
        this->_tinfo.lpszText  = LPSTR_TEXTCALLBACK;
        this->_tinfo.rect.left = this->_tinfo.rect.top = this->_tinfo.rect.bottom = this->_tinfo.rect.right = 0;
        if (SendMessage(this->_hwnd, TTM_ADDTOOL, 0, (LPARAM)&this->_tinfo)) {
            return;
        }
        ShowDetailedLastSystemError();
        DestroyWindow(this->_hwnd);
    }
    
    ErrorBox(L"Couldn't create the ToolTip control.");
	*/
}

CmzTooltip::CmzTooltip(HWND hwndOwner, CmzString message) {
    this->_commonInit(hwndOwner, message);
}

CmzTooltip::~CmzTooltip() {
    this->_cleanup();
}

VOID CmzTooltip::_cleanup() {
    if (this->_hwnd) {
        DestroyWindow(this->_hwnd);
        this->_hwnd = NULL;
    }
}

VOID CmzTooltip::show() {
    if (this->_hwnd) {
        ShowWindow(this->_hwnd, SW_SHOW);
    }
}

}
