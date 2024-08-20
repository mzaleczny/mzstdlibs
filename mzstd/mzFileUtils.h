#ifndef __MZSTD_FILE_UTILS_H__
#define __MZSTD_FILE_UTILS_H__

#include "mzstdCommon.h"
#include "mzString.h"
namespace mzstd {

class CmzStringList;

DECLSPEC_MZSTD DWORD WINAPI GetFileSize(CmzString fname);
DECLSPEC_MZSTD CmzString WINAPI ReadFile(CmzString fname, LONGLONG encoding = 0);
DECLSPEC_MZSTD LONGLONG WINAPI ReadBinaryFile(CmzString fname, BYTE* buffer);
DECLSPEC_MZSTD DWORD WINAPI AppendToFile(HANDLE fh, WCHAR* content, LONGLONG encoding = ENC_UTF8);
DECLSPEC_MZSTD DWORD WINAPI AppendToFile(HANDLE fh, BYTE* data, ULONGLONG datasize);
DECLSPEC_MZSTD BOOL WINAPI WriteToFile(CmzString fname, CmzString content, LONGLONG encoding = ENC_UTF8, DWORD creationDisposition = CREATE_ALWAYS);
DECLSPEC_MZSTD BOOL WINAPI WriteFile(CmzString fname, CmzString content, LONGLONG encoding = ENC_UTF8);
DECLSPEC_MZSTD BOOL WINAPI AppendToFile(CmzString fname, CmzString content, LONGLONG encoding = ENC_UTF8);
DECLSPEC_MZSTD BOOL WINAPI WriteFile(CmzString fname, CmzStringList *lst, LONGLONG encoding = ENC_UTF8);
DECLSPEC_MZSTD BOOL WINAPI WriteFile(CmzString fname, BYTE* data, DWORD dataLength);
DECLSPEC_MZSTD BOOL WINAPI DirectoryExists(CmzString dirname);
DECLSPEC_MZSTD BOOL WINAPI FileExists(CmzString fname);
DECLSPEC_MZSTD CmzString WINAPI GetCurrentDirectory();
DECLSPEC_MZSTD CmzStringList* WINAPI GetDirectoryContent(CmzString dir);
DECLSPEC_MZSTD CmzStringList* WINAPI GetDirectoryDirs(CmzString dir);
DECLSPEC_MZSTD BOOL WINAPI CreateDirectory(CmzString dir, BOOL createAll = FALSE);
// Warning!!!
// Function below removes recursively all directory without prompting. If read-only or system files encounters
// then removes these attributes and deletes this files. Cannot delete files if opened by another app or
// directories that are in use.
DECLSPEC_MZSTD BOOL WINAPI DeleteDirectory(CmzString dir);
// W poniższej funkcji rozszerzenie podajemy bez kropki
DECLSPEC_MZSTD CmzStringList* WINAPI GetDirectoryFiles(CmzString dir, CmzString ext = L"");
// Returns path extension without '.' (string after last '.')
DECLSPEC_MZSTD CmzString WINAPI GetExt(CmzString path);
// Returns only last path part ex. only filename (with extension) or last directory.
DECLSPEC_MZSTD CmzString WINAPI GetPathname(CmzString path);
// Returns only last path part ex. only filename or last directory without specified extension.
DECLSPEC_MZSTD CmzString WINAPI GetPathname(CmzString path, CmzString rtrimExt);
// Returns only last path part ex. only filename or last directory without any extension.
DECLSPEC_MZSTD CmzString WINAPI GetPathnameNoExt(CmzString path);
DECLSPEC_MZSTD BOOL WINAPI PathEndsWithPathSeparator(CmzString& path);
DECLSPEC_MZSTD BOOL WINAPI PathStartsWithPathSeparator(CmzString& path);
// adds extra path part to the specified path. If path not ends with PathSeparator, then the PathSeparator
// is inserted.
DECLSPEC_MZSTD VOID WINAPI AddPathPart(CmzString& path, CmzString partPathname);
// Splits path into array of directories. No matter if path exist.
DECLSPEC_MZSTD CmzStringList* WINAPI GetPathnameParts(CmzString path);
DECLSPEC_MZSTD VOID WINAPI TruncateEndingPathSeparator(CmzString& path);
DECLSPEC_MZSTD VOID WINAPI TruncateStartingPathSeparator(CmzString& path);
DECLSPEC_MZSTD VOID WINAPI TrimPathSeparators(CmzString& path);
// Returns full directory name of the path. If path is the file then full parent directory is returned. Else
// RealPath(path) is returned.
DECLSPEC_MZSTD CmzString WINAPI GetBasename(CmzString path);
// Returns full parent directory name of the path. If path is the file then full parent directory is returned.
// Else RealPath('path\\..') is returned.
DECLSPEC_MZSTD CmzString WINAPI GetParentDir(CmzString path);
// if path is not a full path ex. 'c:\test.txt' but a partial path ex. 'test.txt' or '..\\..\\dir\\test.txt' then
// returned value is: GetCurrentDirectory() + parse_and_apply('..') + if_required(\\) + path,
// else path is returned
DECLSPEC_MZSTD CmzString WINAPI RealPath(CmzString path);
DECLSPEC_MZSTD CmzString WINAPI GetWindowsDirectory();
DECLSPEC_MZSTD CmzString WINAPI GetSystemDirectory();

}

#endif
