#include "pch.h"
#include "mzLinkList.h"

namespace mzstd {

CmzLinkList::CmzLinkList() {
    this->setRealClassName(L"CLinkedList");
    this->_head = NULL;
    this->_last = NULL;
    this->_size = 0;
}

VOID CmzLinkList::append(LONGLONG data) {
    CmzLinkListNode *q, *t;

    if (this->_head == NULL) {
        this->_head = new CmzLinkListNode;
        this->_head->data = data;
        this->_head->next = NULL;
        this->_last = this->_head;
        this->_size = 1;
    }
    else
    {
        q = this->_last;

        t = new CmzLinkListNode;
        t->data = data;
        t->next = NULL;
        q->next = t;

        this->_last = t;
        this->_size += 1;
    }
}

VOID CmzLinkList::addAsFirst(LONGLONG data) {
    CmzLinkListNode *q;

    q = new CmzLinkListNode;
    q->data = data;
    q->next = this->_head;
    this->_head = q;

    if (this->_last == NULL) {
        this->_last = q;
    }

    this->_size += 1;
}

VOID CmzLinkList::addAfter(ULONGLONG pos, LONGLONG data) {
    CmzLinkListNode *q, *t;
    ULONGLONG i;

    if (this->_head == NULL) {
        this->append(data);
        return;
    }

    for(i = 0, q = this->_head; i < pos; i++) {
        q = q->next;
        if (q == NULL) {
            this->append(data);
            return;
        }
    }

    t = new CmzLinkListNode;
    t->data = data;
    t->next = q->next;
    q->next = t;

    if (t->next == NULL) {
        this->_last = t;
    }
    this->_size += 1;
}

ULONGLONG CmzLinkList::del(LONGLONG data) {
    CmzLinkListNode *q, *r;
    ULONGLONG removed = 0;
    q = this->_head;

    if (this->_head == NULL) {
        return 0;
    }

    while (q && q->data == data) {
        this->_head = q->next;
        if (this->_head == NULL) {
            this->_last = NULL;
        } else {
            if (this->_head->next == NULL) {
                this->_last = this->_head;
            }
        }
        delete q;
        q = this->_head;
        this->_size -= 1;
        removed += 1;
    }

    if (q == NULL) {
        return removed;
    }

    r = q;
    q = q->next;
    while (q != NULL) {
        if (q->data == data) {
            r->next = q->next;
            delete q;
            if (r->next == NULL) {
                this->_last = r;
            }
            q = r->next;
            this->_size -= 1;
            removed += 1;
            continue;
        }

        r = q;
        q = q->next;
    }

    // element nie został znaleziony
    return removed;
}

BOOL CmzLinkList::delFirst(LONGLONG data) {
    CmzLinkListNode *q, *r;
    q = this->_head;

    if (this->_head == NULL) {
        return FALSE;
    }

    if (q->data == data) {
        this->_head = q->next;
        if (this->_head == NULL) {
            this->_last = NULL;
        } else {
            if (this->_head->next == NULL) {
                this->_last = this->_head;
            }
        }
        delete q;
        this->_size -= 1;
        return TRUE;
    }

    r = q;
    q = q->next;
    while (q != NULL) {
        if (q->data == data) {
            r->next = q->next;
            delete q;
            if (r->next == NULL) {
                this->_last = r;
            }
            this->_size -= 1;
            return TRUE;
        }

        r = q;
        q = q->next;
    }

    // element nie został znaleziony
    return FALSE;
}

BOOL CmzLinkList::del(ULONGLONG pos) {
    if (pos >= this->_size) {
        return FALSE;
    }

    CmzLinkListNode *q, *r;
    q = this->_head;

    if (this->_head == NULL) {
        return FALSE;
    }

    ULONGLONG c = 0;
    if (pos == 0) {
        this->_head = q->next;
        if (this->_head == NULL) {
            this->_last = NULL;
        } else {
            if (this->_head->next == NULL) {
                this->_last = this->_head;
            }
        }
        delete q;
        this->_size -= 1;
        return TRUE;
    }

    r = q;
    q = q->next;
    c += 1;
    while (c < pos) {
        if (q == NULL) {
            return FALSE;
        }
        r = q;
        q = q->next;
        c++;
    }

    if (q != NULL) {
        r->next = q->next;
        if (r->next == NULL) {
            this->_last = r;
        }
        this->_size -= 1;
        return TRUE;
    }

    // element nie został znaleziony
    return FALSE;
}

LONGLONG CmzLinkList::getAt(ULONGLONG pos) {
    if (pos >= this->_size) {
        return 0;
    }

    CmzLinkListNode *q;
    q = this->_head;

    if (this->_head == NULL) {
        return 0;
    }

    ULONGLONG c = 0;
    while (c < pos) {
        if (q == NULL) {
            return 0;
        }
        q = q->next;
        c++;
    }

    if (q != NULL) {
        return q->data;
    }

    // element nie został znaleziony
    return 0;
}

VOID CmzLinkList::clear() {
    CmzLinkListNode *q;

    if (this->_head == NULL) {
        this->_last = NULL;
        this->_size = 0;
        return;
    }

    while (this->_head != NULL) {
        q = this->_head->next;
        delete this->_head;
        this->_head = q;
    }

    this->_last = NULL;
    this->_size = 0;
}

CmzLinkList::~CmzLinkList() {
    this->clear();
}

LONGLONG CmzLinkList::pos(LONGLONG data) {
    CmzLinkListNode *q;
    LONGLONG retval = 0;

    if (this->_head == NULL) {
        return -1;
    }

    q = this->_head;
    while (q != NULL) {
        if (q->data == data) {
            return retval;
        }
        q = q->next;
        retval++;
    }

    return -1;
}

BOOL CmzLinkList::contains(LONGLONG data) {
    return this->pos(data) >= 0;
}

LONGLONG CmzLinkList::pop() {
    if (this->_size < 1) {
        return 0;
    }

    ULONGLONG pos = this->_size - 1;
    LONGLONG retval = this->getAt(pos);
    this->del(pos);

    return retval;
}

}
