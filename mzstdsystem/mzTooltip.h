#ifndef __MZSTDSYSTEM_TOOLTIP_H__
#define __MZSTDSYSTEM_TOOLTIP_H__

#include "mzstdsystemCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include <commctrl.h>

namespace mzstd {

class DECLSPEC_MZSTDSYSTEM CmzTooltip : public CmzObject {
public:
    WINAPI CmzTooltip(HWND hwndOwner, CmzString message);
	virtual WINAPI ~CmzTooltip();
    VOID WINAPI show();
protected:
    HWND _hwndOwner;        // HWND of parent window
    HWND _hwnd;         // HWND of created tooltip window
    CmzString _message;   // tooltip message
    INITCOMMONCONTROLSEX _icex;
    TOOLINFO _tinfo;
    
    VOID WINAPI _commonInit(HWND hwndOwner, CmzString message);
    VOID WINAPI _cleanup();
    WINAPI CmzTooltip() {this->_commonInit(NULL, COMMON_EMPTY_STRING);};
};

}

#endif
