/*
 * ************************************************************************************************************
 * Author: Marcin Załęczny
 * Copyright (C)2010 Marcin Załęczny <mzaleczny@hotmail.com>
 * License: The GNU Lesser General Public License, version 3.0 (LGPLv3)
 * Library Name: mzstdwin32
 * Version: 1.05
 * First release date:   2010-11-08
 * Current release date: 2011-03-04
 *
 *
 * This file is part of mzstdwin32 library
 * 
 * mzstdwin32 library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mzstdwin32 library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * aLONGLONG with this program. If not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************************************************
 */
#include "pch.h"
#include "mzstdwin32Common.h"
#include "mzRegistry.h"
#include "mzMessageBox.h"
#include "mzTree.h"
using namespace mzstd;

DWORD WINAPI WriteRegistryBinaryData(HKEY hkeyRoot, WCHAR* keyName, WCHAR* valueName, BYTE* data, DWORD dataSize) {
    DWORD   dwReturnValue = ERROR_SUCCESS;
    DWORD   dwDisposition = 0;
    HKEY    hkeyReg       = NULL;

    dwReturnValue = RegCreateKeyEx(hkeyRoot, keyName, 0, L"", 0, KEY_ALL_ACCESS, NULL, &hkeyReg, &dwDisposition);
    if (dwReturnValue != ERROR_SUCCESS) {
        return dwReturnValue;
    }

    dwReturnValue = RegSetValueEx(hkeyReg, valueName, 0, REG_BINARY, data, dataSize);
    
    RegCloseKey(hkeyReg);
    return dwReturnValue;
}

DWORD WINAPI ReadRegistryBinaryData(HKEY hkeyRoot, WCHAR* keyName, WCHAR* valueName, BYTE* data, DWORD* dataSize) {
    DWORD   dwReturnValue = ERROR_SUCCESS;
    DWORD   dwDisposition = 0;
    HKEY    hkeyReg       = NULL;

    dwReturnValue = RegCreateKeyEx(hkeyRoot, keyName, 0, L"", 0, KEY_ALL_ACCESS, NULL, &hkeyReg, &dwDisposition);
    if (dwReturnValue != ERROR_SUCCESS) {
        return dwReturnValue;
    }

    DWORD dataType;
    dwReturnValue = RegQueryValueEx(hkeyReg, valueName, NULL, &dataType, data, dataSize);
    
    RegCloseKey(hkeyReg);
    return dwReturnValue;
}

DWORD WINAPI WriteRegistryStringValuesInternal(HKEY hOpenedKey, CmzArrayList* values) {
    DWORD dwReturnValue = ERROR_SUCCESS, result;
    DWORD dwDisposition;
    HKEY hKey;
    CmzTreeNode* value, subValues;
            
    if (values && values->size() > 0) {
        for (ULONGLONG i = 0; i < values->size(); i++) {
            value = (CmzTreeNode*)values->getAt(i);
            if (value->nodes && value->nodes->size() > 0) {
                // Here we have a registry "folder" value
                result = RegCreateKeyEx(hOpenedKey, value->id.getBuffer(), 0, L"", 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
                if (result == ERROR_SUCCESS) {
                    result = ERROR_SUCCESS;// WriteRegistryStringValuesInternal(hKey, value->nodes);
                    if (result != ERROR_SUCCESS) {
                        dwReturnValue = result;
                    }
                    RegCloseKey(hKey);
                } else {
                    dwReturnValue = result;
                }
            } else {
                // Here we have a registry node value
                result = RegSetValueEx(hOpenedKey, value->id.getBuffer(), 0, REG_SZ, (BYTE*)value->sdata.getBuffer(), static_cast<DWORD>(value->sdata.length()*2));
                if (result != ERROR_SUCCESS) {
                    dwReturnValue = result;
                }
            }
        }
    }
    
    return dwReturnValue;
}

DWORD WINAPI WriteRegistryStringValues(HKEY hkeyRoot, WCHAR* keyName, CmzTree* values) {
    DWORD   dwReturnValue = ERROR_SUCCESS;
    DWORD   dwDisposition = 0;
    HKEY    hKey          = NULL;

    dwReturnValue = RegCreateKeyEx(hkeyRoot, keyName, 0, L"", 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (dwReturnValue != ERROR_SUCCESS) {
        return dwReturnValue;
    }

    CmzTreeNode* node = values->getNode(L"/");
    dwReturnValue = ERROR_SUCCESS;// WriteRegistryStringValuesInternal(hKey, node->nodes);
    
    RegCloseKey(hKey);
    return dwReturnValue;
}

DWORD WINAPI ReadRegistryStringValues(HKEY hkeyRoot, WCHAR* keyName, CmzTree* values) {
    return 0;
}
