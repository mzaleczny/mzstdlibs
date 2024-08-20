#ifndef __MZSTDGRAPHICS_CURSOR_H__
#define __MZSTDGRAPHICS_CURSOR_H__

#include "mzstdgraphicsCommon.h"
#include "mzObject.h"

namespace mzstd {

class DECLSPEC_MZSTDGRAPHICS CmzCursor : public CmzObject {
public:
	WINAPI CmzCursor();
	virtual WINAPI ~CmzCursor();

    VOID WINAPI setSystemCursor(LONG type);
    inline VOID WINAPI setNormalCursor();
    inline VOID WINAPI setIBeamCursor();
    inline VOID WINAPI setWaitCursor();
    inline VOID WINAPI setCrossCursor();
    inline VOID WINAPI setUpArrowCursor();
    inline VOID WINAPI setSizeCursor();
    inline VOID WINAPI setSizeAllCursor();
    inline VOID WINAPI setSizeNWSECursor();
    inline VOID WINAPI setSizeNESWCursor();
    inline VOID WINAPI setSizeWECursor();
    inline VOID WINAPI setSizeNSCursor();
    inline VOID WINAPI setWinLogoCursor();
    inline VOID WINAPI setNoCursor();
    inline VOID WINAPI setHandCursor();
    inline VOID WINAPI setAppStartingCursor();
    inline VOID WINAPI setHelpCursor();

protected:
    // uchwyt na aktualnie wyświetlany kursor
    HCURSOR _hCursor;
    // informuje, czy aktualnie wyświetlany kursor jest kursorem systemowym czy nie. Jeśli
    // ta flaga ma wartość FALSE, to przy każdorazowej zmianie wyświetlanego kursora oraz
    // przy usunięciu obiektu tej klasy zwalniane są zasoby (jest wywoływana funkcja
    // DestroyCursor(this->hCursor)).
    BOOL _systemCursor;
    LONG _type;

    VOID WINAPI _freeCurrentCursor();
};

}

#endif
