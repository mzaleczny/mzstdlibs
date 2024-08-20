#include "pch.h"
#include "mzGraphicsUtils.h"
#include "mzString.h"

namespace mzstd {

VOID WINAPI SaveBitmapToFile(CmzString fname, HBITMAP hBitmap) {
    if (!hBitmap) {
        return;
    }
    //ShowBitmapDlg(hBitmap);

    // najpierw określamy rozmiar bitmapy
    BITMAP bm;
    LONG bmWidth = 0;
    LONG bmHeight = 0;
    LONG retval = GetObject(hBitmap, sizeof(bm), &bm);
    if (retval) {
        bmWidth = bm.bmWidth;
        bmHeight = bm.bmHeight;
    } else {
        return;
    }

    if (bmWidth < 1 || bmHeight < 1) {
        return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    LONG bmpFileHeaderSize = sizeof(BITMAPFILEHEADER);
    LONG bmpInfoHeaderSize = sizeof(BITMAPINFOHEADER);
    
    LONG bitmapContentBytes = (((24 * bmWidth + 31) & (~31)) / 8) * bmHeight;
    LONG bitmapDataBytes = bitmapContentBytes + bmpFileHeaderSize + bmpInfoHeaderSize;
    HANDLE heap = GetProcessHeap();
    BYTE* bitmapBinaryData = (BYTE*)HeapAlloc(heap, 0, bitmapDataBytes);


    ZeroMemory(&fileHeader, bmpFileHeaderSize);
    ZeroMemory(&infoHeader, bmpInfoHeaderSize);
    
    BYTE* magicNumber = (BYTE*)&fileHeader.bfType; // 0x4d42
    magicNumber[0] = 0x42; // 'B'
    magicNumber[1] = 0x4d; // 'M'
    fileHeader.bfOffBits   = bmpFileHeaderSize + bmpInfoHeaderSize;

    infoHeader.biSize          = bmpInfoHeaderSize;
    infoHeader.biWidth         = bmWidth;
    infoHeader.biHeight        = bmHeight;
    infoHeader.biPlanes        = 1;
    infoHeader.biBitCount      = 24;
    infoHeader.biCompression   = BI_RGB;

    CopyMemory(bitmapBinaryData, &fileHeader, bmpFileHeaderSize);
    CopyMemory(&(bitmapBinaryData[bmpFileHeaderSize]), &infoHeader, bmpInfoHeaderSize);

    BITMAPINFO info;
    CopyMemory(&info.bmiHeader, &infoHeader, bmpInfoHeaderSize);

    // save dibsection data
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP hOrigBmp = (HBITMAP)SelectObject(hdc, hBitmap);
    BYTE* bmpBits = &bitmapBinaryData[bmpFileHeaderSize + bmpInfoHeaderSize];
    retval = GetDIBits(hdc, hBitmap, 0, bmHeight, bmpBits, &info, DIB_RGB_COLORS);
    SelectObject(hdc, hOrigBmp);
    DeleteDC(hdc);

	DWORD dwPos;
    DWORD bytesWritten;
	HANDLE fp;

    fp = CreateFileW(fname.getBuffer(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		return;
	}    
    dwPos = SetFilePointer(fp, 0, NULL, FILE_END);
	LockFile(fp, dwPos, 0, bitmapDataBytes, 0);
	WriteFile(fp, bitmapBinaryData, bitmapDataBytes, &bytesWritten, NULL);
	UnlockFile(fp, dwPos, 0, bitmapDataBytes, 0);
    CloseHandle(fp);

    HeapFree(heap, 0, bitmapBinaryData);
}

VOID WINAPI GetBitmapSize(HBITMAP hBitmap, SIZE &size) {
    size.cx = 0;
    size.cy = 0;

    BITMAP bm;
    LONG retval = GetObject(hBitmap, sizeof(bm), &bm);
    if (retval) {
        size.cx = bm.bmWidth;
        size.cy = bm.bmHeight;
    }
}

COLORREF WINAPI HexToRGB(CmzString hex_color) {
    size_t len = hex_color.length();
    COLORREF def_color = RGB(0, 0, 0);

    if (len < 3) {
        return def_color;
    }

    WCHAR *buf = hex_color.getBuffer();
    if (len == 3 && buf[0] == L'#') {
        return def_color;
    }

    LONG pos = 0;
    if (buf[0] == L'#') {
        pos = 1;
    }

    WCHAR rHex[3];
    WCHAR gHex[3];
    WCHAR bHex[3];
    // jeśli mamy skrócony zapis heksowy, np: #fff, 456, #789
    if ((pos == 0 && len == 3) || (pos == 1 && len == 4)) {
        rHex[0] = buf[pos];
        rHex[1] = buf[pos];
        rHex[2] = NULL;

        gHex[0] = buf[pos+1];
        gHex[1] = buf[pos+1];
        gHex[2] = NULL;

        bHex[0] = buf[pos+2];
        bHex[1] = buf[pos+2];
        bHex[2] = NULL;
    } else if ((pos == 0 && len == 6) || (pos == 1 && len == 7)) {
        rHex[0] = buf[pos];
        rHex[1] = buf[pos+1];
        rHex[2] = NULL;

        gHex[0] = buf[pos+2];
        gHex[1] = buf[pos+3];
        gHex[2] = NULL;

        bHex[0] = buf[pos+4];
        bHex[1] = buf[pos+5];
        bHex[2] = NULL;
    } else {
        return def_color;
    }

    LONG r = static_cast<LONG>(hexToInt(rHex));
    LONG g = static_cast<LONG>(hexToInt(gHex));
    LONG b = static_cast<LONG>(hexToInt(bHex));

    return RGB(r, g, b);
}

}