#include "pch.h"
#include "mzFileUtils.h"
#include "mzArrayUtils.h"
#include "mzStringList.h"
#include "mzMessageBox.h"

namespace mzstd {

DWORD WINAPI GetFileSize(CmzString fname) {
	HANDLE fp = CreateFileW(fname.getBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		return 0;
	}

	LARGE_INTEGER size;
	GetFileSizeEx(fp, &size);
	CloseHandle(fp);
	return size.LowPart;
}


CmzString WINAPI ReadFile(CmzString fname, LONGLONG encoding) {
	DWORD fsize = GetFileSize(fname);

	HANDLE fp = CreateFileW(fname.getBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		return "";
	}

	DWORD read;
	CHAR *buf = new CHAR[fsize];
	::ReadFile(fp, buf, fsize, &read, NULL);
	CloseHandle(fp);

	if (encoding == 0) {
		encoding = DetectDataEncoding((BYTE*)buf, read);
	}

	WCHAR *outbuf = NULL;
	ULONGLONG outbuf_len;

	// jeśli mamy ISO2, to konwertujemy na ANSI
	if (encoding == ENC_ISO2) {
		Iso2ToAnsi((BYTE*)buf, read);
		encoding = ENC_ANSI;
	}

	switch (encoding) {
	case ENC_UTF8_WITH_BOM:
		outbuf_len = read + 1;
		outbuf = new WCHAR[outbuf_len];
		MultiByteToWideChar(CP_UTF8, 0, &buf[3], -1, outbuf, read);
		break;
	case ENC_UTF8:
		outbuf_len = read + 1;
		outbuf = new WCHAR[outbuf_len];
		MultiByteToWideChar(CP_UTF8, 0, &buf[0], -1, outbuf, read);
		break;
	case ENC_UNICODE:
		outbuf_len = read / 2 + 1;
		if (read % 2) {
			read--;
		}
		outbuf = new WCHAR[outbuf_len];
		memcpy(outbuf, buf, read);
		break;
    default:
    case ENC_ANSI:
		outbuf_len = read + 1;
		outbuf = new WCHAR[outbuf_len];
		MultiByteToWideChar(CP_ACP, 0, &buf[0], -1, outbuf, read);
		break;
    }


	delete[] buf;
	if (outbuf) {
		outbuf[outbuf_len - 1] = NULL;
		CmzString content(outbuf, outbuf_len);
		return content;
	}

	return "";
}


LONGLONG WINAPI ReadBinaryFile(CmzString fname, BYTE* buffer) {
    if (!buffer) {
        return 0;
    }

	HANDLE fp = CreateFileW(fname.getBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		return 0;
	}

	DWORD read;
    DWORD chunk = 8192;
    DWORD offset = 0;
    do {
	    ::ReadFile(fp, &buffer[offset], chunk, &read, NULL);
        offset += read;
    } while (read == chunk);
	CloseHandle(fp);

	return offset;
}

DWORD WINAPI AppendToFile(HANDLE fh, WCHAR* content, LONGLONG encoding) {
	// ostatnie plus 3 jest na wypadek UTF8 with BOM
	size_t output_buf_len = wcslen(content) * 2 + 3;
	BYTE *output_buf = new BYTE[output_buf_len];
    if (!output_buf) {
        return 0;
    }
    
	size_t bytesToWrite;
	LPCCH defaultChar = ".";
	BOOL usedDefaultChar;

	switch (encoding) {
	case ENC_UTF8_WITH_BOM:
		output_buf[0] = (BYTE)0xef;
		output_buf[1] = (BYTE)0xbb;
		output_buf[2] = (BYTE)0xbf;
		bytesToWrite = WideCharToMultiByte(CP_UTF8, 0, content, -1, (LPSTR)&output_buf[3], (INT)output_buf_len - 3, NULL, NULL);
		bytesToWrite -= 1;
		bytesToWrite += 3;
		break;
	case ENC_UTF8:
		bytesToWrite = WideCharToMultiByte(CP_UTF8, 0, content, -1, (LPSTR)output_buf, (INT)output_buf_len, NULL, NULL);
		bytesToWrite -= 1;
		break;
	case ENC_UNICODE:
		bytesToWrite = wcslen(content) * 2;
		memcpy(output_buf, content, bytesToWrite);
		break;
	case ENC_ANSI:
		bytesToWrite = WideCharToMultiByte(CP_ACP, 0, content, -1, (LPSTR)output_buf, (INT)output_buf_len, defaultChar, &usedDefaultChar);
		bytesToWrite -= 1;
		break;
	case ENC_ISO2:
		bytesToWrite = WideCharToMultiByte(CP_ACP, 0, content, -1, (LPSTR)output_buf, (INT)output_buf_len, defaultChar, &usedDefaultChar);
		bytesToWrite -= 1;
		AnsiToIso2(output_buf, bytesToWrite);
		break;
	}

	if (bytesToWrite < 1) {
		return 0;
	}

	DWORD bytesWritten = 0;
	DWORD dwPos = SetFilePointer(fh, 0, NULL, FILE_END);
	LockFile(fh, dwPos, 0, (DWORD)bytesToWrite, 0);
	::WriteFile(fh, output_buf, (DWORD)bytesToWrite, &bytesWritten, NULL);
	UnlockFile(fh, dwPos, 0, (DWORD)bytesToWrite, 0);
    
    delete[] output_buf;
    return bytesWritten;
}

DECLSPEC_MZSTD DWORD WINAPI AppendToFile(HANDLE fh, BYTE* data, ULONGLONG datasize) {
	DWORD bytesWritten = 0;
	DWORD dwPos = SetFilePointer(fh, 0, NULL, FILE_END);
	LockFile(fh, dwPos, 0, static_cast<DWORD>(datasize), 0);
	::WriteFile(fh, data, static_cast<DWORD>(datasize), &bytesWritten, NULL);
	UnlockFile(fh, dwPos, 0, static_cast<DWORD>(datasize), 0);
    return bytesWritten;
}

BOOL WINAPI WriteToFile(CmzString fname, CmzString content, LONGLONG encoding, DWORD creationDisposition) {
	HANDLE fh = CreateFileW(fname.getBuffer(), GENERIC_WRITE, FILE_SHARE_READ, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fh == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
    DWORD bytesWritten = AppendToFile(fh, content.getBuffer(), encoding);    
	CloseHandle(fh);

	if (bytesWritten > 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL WINAPI WriteFile(CmzString fname, CmzString content, LONGLONG encoding) {
    return WriteToFile(fname, content, encoding, CREATE_ALWAYS);
}

BOOL WINAPI AppendToFile(CmzString fname, CmzString content, LONGLONG encoding) {
    return WriteToFile(fname, content, encoding, OPEN_ALWAYS);
}

BOOL WINAPI WriteFile(CmzString fname, CmzStringList *lst, LONGLONG encoding) {
	HANDLE fp = CreateFileW(fname.getBuffer(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fp == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	size_t output_buf_len = 0;
	BYTE *output_buf = NULL;
	size_t bytesToWrite;
	LPCCH defaultChar = ".";
	BOOL usedDefaultChar;
	CmzString content;
	DWORD bytesWritten;
	DWORD dwPos;

	for (ULONGLONG i = 0; i < lst->size(); i++) {
		content = lst->getAt(i);

		// ostatnie plus 3 jest na wypadek UTF8 with BOM
		if (output_buf_len < content.getBufferSize() * 2 + 3) {
			if (output_buf) {
				delete[] output_buf;
			}
			output_buf_len = content.getBufferSize() * 2 + 3;
			output_buf = new BYTE[output_buf_len];
		}


		switch (encoding) {
		case ENC_UTF8_WITH_BOM:
			output_buf[0] = (BYTE)0xef;
			output_buf[1] = (BYTE)0xbb;
			output_buf[2] = (BYTE)0xbf;
			bytesToWrite = WideCharToMultiByte(CP_UTF8, 0, content.getBuffer(), -1, (LPSTR)&output_buf[3], (INT)output_buf_len - 3, NULL, NULL);
			if (bytesToWrite > 0) {
				bytesToWrite -= 1;
			}
			bytesToWrite += 3;
			break;
		case ENC_UTF8:
			bytesToWrite = WideCharToMultiByte(CP_UTF8, 0, content.getBuffer(), -1, (LPSTR)output_buf, (INT)output_buf_len, NULL, NULL);
			if (bytesToWrite > 0) {
				bytesToWrite -= 1;
			}
			break;
		case ENC_UNICODE:
			bytesToWrite = content.length() * 2;
			memcpy(output_buf, content.getBuffer(), bytesToWrite);
			break;
		case ENC_ANSI:
			bytesToWrite = WideCharToMultiByte(CP_ACP, 0, content.getBuffer(), -1, (LPSTR)output_buf, (INT)output_buf_len, defaultChar, &usedDefaultChar);
			if (bytesToWrite > 0) {
				bytesToWrite -= 1;
			}
			break;
		case ENC_ISO2:
			bytesToWrite = WideCharToMultiByte(CP_ACP, 0, content.getBuffer(), -1, (LPSTR)output_buf, (INT)output_buf_len, defaultChar, &usedDefaultChar);
			if (bytesToWrite > 0) {
				bytesToWrite -= 1;
			}
			AnsiToIso2(output_buf, bytesToWrite);
			break;
		}

		if (bytesToWrite < 1) {
			continue;
		}


		dwPos = SetFilePointer(fp, 0, NULL, FILE_END);
		LockFile(fp, dwPos, 0, (DWORD)bytesToWrite, 0);
		::WriteFile(fp, output_buf, (DWORD)bytesToWrite, &bytesWritten, NULL);
		UnlockFile(fp, dwPos, 0, (DWORD)bytesToWrite, 0);
	}

	CloseHandle(fp);
	delete[] output_buf;

	return TRUE;
}

BOOL WINAPI WriteFile(CmzString fname, BYTE* data, DWORD dataLength) {
	HANDLE fp = CreateFileW(fname.getBuffer(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fp == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	DWORD bytesToWrite = 8192;
    if (dataLength < bytesToWrite) {
        bytesToWrite = dataLength;
    }
    DWORD offset = 0;

    DWORD bytesWritten;
	DWORD dwPos;

    while (offset < dataLength) {
        dwPos = SetFilePointer(fp, 0, NULL, FILE_END);
	    LockFile(fp, dwPos, 0, bytesToWrite, 0);
	    ::WriteFile(fp, &data[offset], bytesToWrite, &bytesWritten, NULL);
	    UnlockFile(fp, dwPos, 0, bytesToWrite, 0);
        if (bytesWritten != bytesToWrite) {
            break;
        }
        offset += bytesWritten;
        if (dataLength - offset < bytesToWrite) {
            bytesToWrite = dataLength - offset;
        }
    }
	CloseHandle(fp);

    return offset == dataLength;
}

BOOL WINAPI DirectoryExists(CmzString dirname) {
    DWORD attribs = GetFileAttributesW(dirname.getBuffer());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL WINAPI FileExists(CmzString fname) {
    DWORD attribs = GetFileAttributesW(fname.getBuffer());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (!(attribs & FILE_ATTRIBUTE_DIRECTORY));
}

CmzString WINAPI GetCurrentDirectory() {
	WCHAR buf[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH, buf);
	return buf;
}

CmzStringList* WINAPI GetDirectoryContent(CmzString dir) {
    WIN32_FIND_DATAW fd;
    HANDLE hDir = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    CmzStringList *lstDirs = NULL, *lstFiles = NULL;
    CmzStringList *lstAll = NULL;

    if (dir.length() > MAX_PATH) {
        return NULL;
    }

    if (dir.lastChar() != L'\\') {
        dir += L'\\';
    }

    if (dir.lastChar() != L'*') {
        dir += L'*';
    }

    // Find the first file in the directory.
    hDir = FindFirstFileW(dir.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        ShowLastSystemError();
        return NULL;
    }

    lstDirs = new CmzStringList();
    lstFiles = new CmzStringList();
    lstAll = new CmzStringList();
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            lstDirs->append(CmzString(fd.cFileName));
        } else {
            lstFiles->append(CmzString(fd.cFileName));
        }
    } while (FindNextFileW(hDir, &fd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        FindClose(hDir);
        ShowLastSystemError();
    }

    FindClose(hDir);

    lstAll->append(lstDirs);
    lstAll->append(lstFiles);

    delete lstDirs;
    delete lstFiles;

    return lstAll;
}

CmzStringList* WINAPI GetDirectoryDirs(CmzString dir) {
    WIN32_FIND_DATAW fd;
    HANDLE hDir = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    CmzStringList *lstDirs = NULL;

    if (dir.length() > MAX_PATH) {
        return NULL;
    }

    if (dir.lastChar() != L'\\') {
        dir += L'\\';
    }

    if (dir.lastChar() != L'*') {
        dir += L'*';
    }

    // Find the first file in the directory.
    hDir = FindFirstFileW(dir.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        ShowLastSystemError();
        return NULL;
    }

    lstDirs = new CmzStringList();
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            lstDirs->append(CmzString(fd.cFileName));
        }
    } while (FindNextFileW(hDir, &fd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        FindClose(hDir);
        ShowLastSystemError();
    }

    FindClose(hDir);

    return lstDirs;
}

CmzStringList* WINAPI GetDirectoryFiles(CmzString dir, CmzString ext) {
    WIN32_FIND_DATAW fd;
    HANDLE hDir = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    CmzStringList *lstFiles = NULL;
    BOOL checkExtension = ext.length() > 0;

    if (dir.length() > MAX_PATH) {
        return NULL;
    }

    if (dir.lastChar() != L'\\') {
        dir += L'\\';
    }

    if (dir.lastChar() != L'*') {
        dir += L'*';
    }

    // Find the first file in the directory.
    hDir = FindFirstFileW(dir.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        ShowLastSystemError();
        return NULL;
    }

    lstFiles = new CmzStringList();
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (checkExtension) {
                if (ext == CmzString(fd.cFileName).strAfterLast(L'.')) {
                    lstFiles->append(CmzString(fd.cFileName));
                }
            } else {
                lstFiles->append(CmzString(fd.cFileName));
            }
        }
    } while (FindNextFileW(hDir, &fd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        FindClose(hDir);
        ShowLastSystemError();
    }

    FindClose(hDir);

    return lstFiles;
}

CmzString WINAPI GetExt(CmzString path) {
    return path.strAfterLast(L'.');
}

CmzString WINAPI GetPathname(CmzString path) {
    // if we have only disk drive path ex. 'C:\\', then we return empty string
    if (path.length() < 4) {
        return COMMON_EMPTY_STRING;
    }

    if (path.contains(L'/') || path.contains(L'\\')) {
        return path.strAfterLastChar(L"/\\");
    }
    return path;
}

CmzString WINAPI GetPathname(CmzString path, CmzString rtrimExt) {
    CmzString result = GetPathname(path);

    if (result == rtrimExt) {
        return COMMON_EMPTY_STRING;
    }

    size_t resultLen = result.length();
    size_t rtrimextLen = rtrimExt.length();

    if (resultLen == rtrimextLen) {
        return result;
    }

    if (resultLen > rtrimextLen) {
        WCHAR* pend = result.getBuffer();
        pend = pend + resultLen - rtrimextLen;
        if (rtrimExt == pend) {
            return result.substr(0, resultLen - rtrimextLen);
        }
        return result;
    }

    return COMMON_EMPTY_STRING;
}

CmzString WINAPI GetPathnameNoExt(CmzString path) {
    CmzString result = GetPathname(path);
    CmzString rtrimExt = CmzString(L".") + GetExt(result);

    return GetPathname(result, rtrimExt);
}

CmzString WINAPI GetBasename(CmzString path) {
    CmzString realPath = RealPath(path);
    if (FileExists(realPath)) {
        realPath.cutAtLastChar(L"/\\");
        return realPath;
    } else {
        if (DirectoryExists(realPath)) {
            return realPath;
        }

        // Not existing path - if has dot then assuming file else directory
        if (realPath.contains(L".")) {
            realPath.cutAtLastChar(L"/\\");
        }

        return realPath;
    }

    // we should not be here
    return realPath;
}

CmzString WINAPI GetParentDir(CmzString path) {
    if (path == L"\\\\") {
        return COMMON_EMPTY_STRING;
    }
    size_t pathLen = path.length();
    if (pathLen == 1 || ((pathLen == 2 || pathLen == 3) && path.getCharAt(1) == L':')) {
        return COMMON_EMPTY_STRING;
    }

    CmzString realPath = RealPath(path);
    realPath.cutAtLastChar(L"/\\");
    WCHAR* buf = realPath.getBuffer();
    size_t bufLen = wcslen(buf);

    if (bufLen == 1 && buf[0] == L'\\') {
        realPath += L'\\';
        return realPath;
    }

    if (bufLen == 2 && buf[1] == L':') {
        realPath += L'\\';
        return realPath;
    }

    return realPath;
}

VOID WINAPI TruncateEndingPathSeparator(CmzString& path) {
    path.rtrimAllChars(L"/\\");
}

VOID WINAPI TruncateStartingPathSeparator(CmzString& path) {
    path.ltrimAllChars(L"/\\");
}

VOID WINAPI TrimPathSeparators(CmzString& path) {
    path.rtrimAllChars(L"/\\");
    path.ltrimAllChars(L"/\\");
}

CmzString WINAPI RealPath(CmzString path) {
    WCHAR* buf = path.getBuffer();
    if (!buf || buf[0] == NULL) {
        return path;
    }

    CmzStringList* pathParts = GetPathnameParts(path);
    ULONGLONG pathPartsCount = pathParts->size();

    CmzString resultPath = COMMON_EMPTY_STRING;
    ULONGLONG i = 0;
    CmzString ppart;

    while (i < pathPartsCount) {
        ppart = pathParts->getAt(i);
        i++;

        if (resultPath == COMMON_EMPTY_STRING) {
            if (ppart == L"\\\\") {
                resultPath = ppart;
                continue;
            }
            if (ppart.length() > 1 && ppart.getCharAt(1) == L':') {
                if (ppart.length() > 2 && ppart.getCharAt(2) == L'\\') {
                    resultPath = ppart;
                    continue;
                }
                resultPath = ppart + L"\\";
                continue;
            }

            resultPath = GetCurrentDirectory() + L"\\" + ppart;
            continue;
        }

        if (ppart == L".") {
            continue;
        }

        TruncateEndingPathSeparator(resultPath);

        if (ppart == L"..") {
            if (resultPath.containsOneOfChars(L"/\\")) {
                resultPath.cutAtLastChar(L"/\\");
            }
            continue;
        }

        resultPath += CmzString(L"\\") + ppart;
    }
    delete pathParts;

    return resultPath;
}

DECLSPEC_MZSTD CmzStringList* WINAPI GetPathnameParts(CmzString path) {
    if (path.startsWith(L"\\\\")) {
        CmzString sub = path.substr(2);
        CmzStringList* lst = sub.explodeAtChar(L"/\\");
        lst->addAsFirst(L"\\\\");
        return lst;
    }

    return path.explodeAtChar(L"/\\");
}

DECLSPEC_MZSTD BOOL WINAPI CreateDirectory(CmzString dir, BOOL createAll) {
    if (!createAll) {
        CreateDirectoryW((LPCWSTR)dir.getBuffer(), (LPSECURITY_ATTRIBUTES)NULL);
        if (!DirectoryExists(dir)) {
            return FALSE;
        }
        return TRUE;
    }

    CmzString realPath = RealPath(dir);
    CmzStringList* dirs = GetPathnameParts(realPath);
    if (!dirs) {
        return FALSE;
    }
    LONGLONG count = dirs->size();

    if (count > 0) {
        CmzString partDir = dirs->getAt(0);
        if (!PathEndsWithPathSeparator(partDir)) {
            partDir += L"\\";
        }

        for (LONGLONG i = 1; i < count; i++) {
            if (i == 1) {
                partDir += dirs->getAt(i);
            } else {
                partDir += CmzString(L"\\") + dirs->getAt(i);
            }

            if (!DirectoryExists(partDir)) {
                CreateDirectoryW((LPCWSTR)partDir.getBuffer(), (LPSECURITY_ATTRIBUTES)NULL);
                if (!DirectoryExists(partDir)) {
                    delete dirs;
                    return FALSE;
                }
            }
        }

        delete dirs;
        return TRUE;
    }

    delete dirs;
    return FALSE;
}

BOOL WINAPI DeleteDirectory(CmzString dir) {
    WIN32_FIND_DATAW fd;
    HANDLE hDir = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    BOOL result = TRUE;

    CmzString allFilesMask = dir;
    CmzString item;

    if (!DirectoryExists(dir)) {
        return FALSE;
    }

    AddPathPart(allFilesMask, L"*");
    hDir = FindFirstFileW(allFilesMask.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
                continue;
            }
            item = dir;
            AddPathPart(item, fd.cFileName);
            //wprintf(L"Deleting subdirectory: %s\n", item.getBuffer());
            DeleteDirectory(item);
        } else {
            item = dir;
            AddPathPart(item, fd.cFileName);
            SetFileAttributesW(item.getBuffer(), FILE_ATTRIBUTE_NORMAL);
            //wprintf(L"Deleting file: %s\n", item.getBuffer());
            DeleteFileW(item.getBuffer());
        }
    } while (FindNextFileW(hDir, &fd) != 0);
    FindClose(hDir);

    //wprintf(L"Deleting directory: %s\n", dir.getBuffer());
    result = RemoveDirectoryW(dir.getBuffer());
    return result;
}

BOOL WINAPI PathEndsWithPathSeparator(CmzString& path) {
    return path.endsWith(L"/\\");
}

BOOL WINAPI PathStartsWithPathSeparator(CmzString& path) {
    return path.startsWith(L"/\\");
}

VOID WINAPI AddPathPart(CmzString& path, CmzString partPathname) {
    TrimPathSeparators(partPathname);
    if (PathEndsWithPathSeparator(path)) {
        path += partPathname;
    } else {
        path += CmzString(L"\\") + partPathname;
    }
}

CmzString WINAPI GetWindowsDirectory() {
    WCHAR buf[MAX_PATH];
    ::GetWindowsDirectoryW((WCHAR*)buf, MAX_PATH);
    return buf;
}

CmzString WINAPI GetSystemDirectory() {
    WCHAR buf[MAX_PATH];
    ::GetSystemDirectoryW((WCHAR*)buf, MAX_PATH);
    return buf;
}

}
