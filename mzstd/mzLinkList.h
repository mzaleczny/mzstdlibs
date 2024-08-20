#ifndef __MZSTD_LINK_LIST_H__
#define __MZSTD_LINK_LIST_H__

#include "mzstdCommon.h"
#include "mzObject.h"

namespace mzstd {

class DECLSPEC_MZSTD CmzLinkListNode : public CmzObject {
public:
    LONGLONG data;
    CmzLinkListNode* next;
};
typedef CmzLinkListNode* PLinkListNode;

class DECLSPEC_MZSTD CmzLinkList : public CmzObject {
public:
    WINAPI CmzLinkList();
    WINAPI ~CmzLinkList();

    VOID WINAPI append(LONGLONG data);
    inline VOID WINAPI push(LONGLONG data) { this->append(data); }
    LONGLONG WINAPI pop();
    VOID WINAPI addAsFirst(LONGLONG data);
    VOID WINAPI addAfter(ULONGLONG pos, LONGLONG data);
    ULONGLONG WINAPI del(LONGLONG data);
    BOOL WINAPI delFirst(LONGLONG data);
    BOOL WINAPI del(ULONGLONG pos);
    LONGLONG WINAPI getAt(ULONGLONG pos);
    LONGLONG WINAPI pos(LONGLONG data);
    BOOL WINAPI contains(LONGLONG data);
    inline ULONGLONG WINAPI size() { return this->_size; };
    inline CmzLinkListNode* WINAPI first() { return this->_head; }
    inline CmzLinkListNode* WINAPI lastNode() { return this->_last; }
    VOID WINAPI clear();
private:
    CmzLinkListNode *_head;
    CmzLinkListNode *_last;
    ULONGLONG _size;
};

}

#endif
