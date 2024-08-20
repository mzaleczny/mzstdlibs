#ifndef __MZSTD_TREE_H__
#define __MZSTD_TREE_H__

#include "mzstdCommon.h"
#include "mzObject.h"
#include "mzString.h"
#include "mzPointerList.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzTreeNode {
public:
    WINAPI CmzTreeNode();
    WINAPI CmzTreeNode(const WCHAR* id, const WCHAR* sdata, LONGLONG data);
    virtual WINAPI ~CmzTreeNode();
    
    CmzString id;
    CmzString sdata;
    LONGLONG data;
    CmzPointerList *nodes;
};
typedef CmzTreeNode* PTreeNode;


class DECLSPEC_MZSTD CmzTree : public CmzObject {
public:
    WINAPI CmzTree();
    virtual WINAPI ~CmzTree();

    CmzTreeNode* WINAPI getNode(CmzString path);
    CmzTreeNode* WINAPI getParentNode(CmzString path);
    CmzTreeNode* WINAPI appendNode(CmzString path, LONGLONG data, const WCHAR* id = L"", const WCHAR* sdata = L"");
    CmzTreeNode* WINAPI addAsFirst(CmzString path, LONGLONG data, const WCHAR* id = L"", const WCHAR* sdata = L"");
    CmzTreeNode* WINAPI insertNode(CmzString path, ULONGLONG pos, LONGLONG data, const WCHAR* id = L"", const WCHAR* sdata = L"");
    CmzTreeNode* WINAPI appendNode(CmzTreeNode* parent, LONGLONG data, const WCHAR* id = L"", const WCHAR* sdata = L"");
    CmzTreeNode* WINAPI addAsFirst(CmzTreeNode* parent, LONGLONG data, const WCHAR* id = L"", const WCHAR* sdata = L"");
    CmzTreeNode* WINAPI insertNode(CmzTreeNode* parent, ULONGLONG pos, LONGLONG data, const WCHAR* id = L"", const WCHAR* sdata = L"");
    VOID WINAPI delNode(CmzString path);
    LONGLONG WINAPI delSubNodeWithData(CmzString path, LONGLONG data);
    LONGLONG WINAPI delSubNodeWithId(CmzString path, CmzString id);
    BOOL WINAPI delFirstNode(CmzString path, LONGLONG data);
    BOOL WINAPI delFirstNodeWithId(CmzString path, CmzString id);
    BOOL WINAPI delNodeAt(CmzString path, ULONGLONG pos);
    LONGLONG WINAPI pos(CmzString path, LONGLONG data);
    LONGLONG WINAPI posWithId(CmzString path, CmzString id);
    CmzString WINAPI getNodePath(LONGLONG data, CmzTreeNode *startAtNode = NULL);
    CmzString WINAPI getNodeWithIdPath(CmzString id, CmzTreeNode *startAtNode = NULL);
    BOOL WINAPI contains(LONGLONG data);
    BOOL WINAPI contains(CmzString id);
    BOOL WINAPI contains(CmzString path, LONGLONG data);
    BOOL WINAPI containsNodeWithId(CmzString path, CmzString id);
    VOID WINAPI clear();
    CmzString WINAPI simplePrintTree();
private:
    CmzTreeNode* _root;
    CmzString WINAPI simplePrintNode(CmzTreeNode *node, CmzString indent);
};

}

#endif
