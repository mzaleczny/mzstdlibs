#include "pch.h"
#include "mzIcon.h"
#include "mzFileUtils.h"

namespace mzstd {

CmzIcon::CmzIcon(HINSTANCE hInst, CmzString fname, LONG size) {
    this->setRealClassName(L"CmzIcon");
    this->fname = fname;
    this->_width = size;
    this->_height = size;

    if (FileExists(fname)) {
        this->hIcon = (HICON)LoadImageW(hInst, fname.getBuffer(), IMAGE_ICON, this->_width, this->_height, LR_LOADFROMFILE);
    }
}

CmzIcon::~CmzIcon() {
    if (this->hIcon) {
        DestroyIcon(this->hIcon);
    }
    this->hIcon = 0;
}

}
