#ifndef __COMMON_H__
#define __COMMON_H__

#define STRING2(x)  #x
#define STRING(x)   STRING2(x)

// if we compile dll based project
#ifdef MZSTD_EXPORTS
    // if we compile dll library
    #ifdef _USRDLL
        #define DECLSPEC_MZSTD __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTD __declspec(dllimport)
    #endif
#endif

// if we compile static lib based project
#ifdef MZSTD_STATIC
    #ifdef MZSTD_INTERNAL
        #define DECLSPEC_MZSTD __declspec(dllexport)
    #else
        #define DECLSPEC_MZSTD
    #endif
#endif

#ifndef DECLSPEC_MZSTD
    #define DECLSPEC_MZSTD __declspec(dllimport)
#endif

#ifndef COMMON_MEASURE_STRING
#define COMMON_MEASURE_STRING   L"~!@#$%^&*()_+|`1234567890-=\\QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm{}[]:\";'<>?,./ĄŚĘŁÓŃŹŻąśęłóńćźż"
#endif

#ifndef COMMON_EMPTY_STRING
#define COMMON_EMPTY_STRING   L""
#endif


extern const wchar_t* const cmzObjectClassName;
extern const wchar_t* const cmzStringClassName;

#endif
