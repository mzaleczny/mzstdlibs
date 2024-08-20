#include "pch.h"
#include "mzCursor.h"

using namespace mzstd;

CmzCursor::CmzCursor() {
    this->setRealClassName(L"CmzCursor");
    this->_systemCursor = TRUE;
    this->_type = OCR_NORMAL;
    this->_hCursor = GetCursor();
}

CmzCursor::~CmzCursor() {
    this->_freeCurrentCursor();
}

VOID CmzCursor::_freeCurrentCursor() {
    if (!this->_systemCursor && this->_hCursor) {
        DestroyCursor(this->_hCursor);
        this->_hCursor = NULL;
    }
}


VOID CmzCursor::setSystemCursor(LONG type) {
    if (this->_type == type) {
		CURSORINFO ci;
        ZeroMemory(&ci, sizeof(ci));
        ci.cbSize = sizeof(ci);
        GetCursorInfo(&ci);
        if (ci.hCursor == this->_hCursor) {
            return;
        }
    }

    this->_freeCurrentCursor();

    this->_systemCursor = TRUE;
    switch (type) {
    case OCR_ICON: // OBSOLETE
    case OCR_NORMAL:
        this->_type = OCR_NORMAL;
        this->_hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        break;
    case OCR_SIZE: // OBSOLETE
    case OCR_SIZEALL:
        this->_type = OCR_SIZEALL;
        this->_hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_SIZEALL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        break;
    case OCR_ICOCUR: // OBSOLETE
    case OIC_WINLOGO:
        this->_type = OIC_WINLOGO;
        this->_hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OIC_WINLOGO), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        break;
    case OCR_IBEAM:
    case OCR_WAIT:
    case OCR_CROSS:
    case OCR_UP:
    case OCR_SIZENWSE:
    case OCR_SIZENESW:
    case OCR_SIZEWE:
    case OCR_SIZENS:
    case OCR_NO:
    case OCR_HAND:
    case OCR_APPSTARTING:
        this->_type = type;
        this->_hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(type), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        break;
    default:
        this->_type = OCR_NORMAL;
        this->_hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        break;
    }

    SetCursor(this->_hCursor);
}

VOID CmzCursor::setNormalCursor()        { this->setSystemCursor(OCR_NORMAL); }
VOID CmzCursor::setIBeamCursor()         { this->setSystemCursor(OCR_IBEAM);  }
VOID CmzCursor::setWaitCursor()          { this->setSystemCursor(OCR_WAIT);   }
VOID CmzCursor::setCrossCursor()         { this->setSystemCursor(OCR_CROSS);  }
VOID CmzCursor::setUpArrowCursor()       { this->setSystemCursor(OCR_UP);     }
VOID CmzCursor::setSizeCursor()          { this->setSystemCursor(OCR_SIZEALL); }
VOID CmzCursor::setSizeAllCursor()       { this->setSystemCursor(OCR_SIZEALL); }
VOID CmzCursor::setSizeNWSECursor()      { this->setSystemCursor(OCR_SIZENWSE); }
VOID CmzCursor::setSizeNESWCursor()      { this->setSystemCursor(OCR_SIZENESW); }
VOID CmzCursor::setSizeWECursor()        { this->setSystemCursor(OCR_SIZEWE); }
VOID CmzCursor::setSizeNSCursor()        { this->setSystemCursor(OCR_SIZENS); }
VOID CmzCursor::setWinLogoCursor()       { this->setSystemCursor(OIC_WINLOGO); }
VOID CmzCursor::setNoCursor()            { this->setSystemCursor(OCR_NO); }
VOID CmzCursor::setHandCursor()          { this->setSystemCursor(OCR_HAND); }
VOID CmzCursor::setAppStartingCursor()   { this->setSystemCursor(OCR_APPSTARTING); }
VOID CmzCursor::setHelpCursor()          { this->setSystemCursor(OIC_QUES); }
