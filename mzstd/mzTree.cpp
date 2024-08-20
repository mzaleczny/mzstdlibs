#include "pch.h"
#include "mzTree.h"
#include "mzString.h"
#include "mzArrayList.h"
#include "mzStringList.h"
#include "mzMath.h"

namespace mzstd {

CmzTreeNode::CmzTreeNode() {
    this->data = 0;
    this->sdata = COMMON_EMPTY_STRING;
    this->id = COMMON_EMPTY_STRING;
    this->nodes = NULL;
};

CmzTreeNode::CmzTreeNode(const WCHAR* id, const WCHAR* sdata, LONGLONG data) {
    this->data = data;
    this->sdata = sdata;
    this->id = id;
    this->nodes = NULL;
};

CmzTreeNode::~CmzTreeNode() {
    if (this->nodes) {
        CmzTreeNode* node;
        for (ULONGLONG i = 0; i < this->nodes->size(); i++) {
            node = (CmzTreeNode*)this->nodes->getAt(i);
            delete node;
        }
        this->nodes->clear();
        delete this->nodes;
        this->nodes = NULL;
    }
}



CmzTree::CmzTree() {
    this->setRealClassName(L"CmzTree");
    this->_root = new CmzTreeNode();
    this->_root->id = "root";
}

CmzTree::~CmzTree() {
    this->clear();
    this->_root = NULL;
}

CmzTreeNode *CmzTree::getNode(CmzString path) {
    LONGLONG length = 0;
    if (path.length() > 1) {
        path.rtrim(L'/');
    }
    CmzStringList* nodes = path.explode('/');
    if (!nodes || nodes->size() < 1) {
        return NULL;
    }

    length = nodes->size();
    CmzString current;
    ULONGLONG j;

    CmzTreeNode *node = this->_root;
    CmzTreeNode *tmpNode;

    for (LONGLONG i = 1; i < length; i++) {
        current = nodes->getAt(i);
        if (current == "") {
            break;
        }
        ULONGLONG currentNodeSize = node->nodes->size();
        if (!node->nodes || currentNodeSize < 1) {
            node = NULL;
            break;
        }
        for (j = 0; j < currentNodeSize; j++) {
            tmpNode = (CmzTreeNode*)node->nodes->getAt(j);
            if (tmpNode->id == current) {
                node = tmpNode;
                break;
            }
        }
        // jeśli nie znaleziono odpowiedniego noda, to kończymy
        // poszukiwanie
        if (j == currentNodeSize) {
            node = NULL;
            break;
        }
    }

    return node;
}


CmzTreeNode *CmzTree::getParentNode(CmzString path) {
    if (path.length() > 1) {
        path.rtrim(L'/');
    }
    if (path == "/") {
        return NULL;
    }

    CmzTreeNode* node = NULL;
    CmzString parentPath = path;
    parentPath.cutAtLast('/');
    if (parentPath == "") {
        parentPath = "/";
    }

    node = this->getNode(parentPath);
    return node;
}

CmzTreeNode* CmzTree::appendNode(CmzString path, LONGLONG data, const WCHAR* id, const WCHAR* sdata) {
    CmzTreeNode *node = this->getNode(path);
    return this->appendNode(node, data, id, sdata);
}

CmzTreeNode* CmzTree::addAsFirst(CmzString path, LONGLONG data, const WCHAR* id, const WCHAR* sdata) {
    CmzTreeNode *node = this->getNode(path);
    return this->addAsFirst(node, data, id, sdata);
}

CmzTreeNode* CmzTree::insertNode(CmzString path, ULONGLONG pos, LONGLONG data, const WCHAR* id, const WCHAR* sdata) {
    CmzTreeNode *node = this->getNode(path);
    return this->insertNode(node, pos, data, id, sdata);
}

CmzTreeNode* CmzTree::appendNode(CmzTreeNode* parent, LONGLONG data, const WCHAR* id, const WCHAR* sdata) {
    CmzTreeNode *node = parent;

    if (node) {
        if (!node->nodes) {
            node->nodes = new CmzPointerList();
        }
        CmzString sID = id;
        if (sID.length() == 0) {
            ULONGLONG randomValue = random();
            sID = L"Node";
            sID += randomValue;
        }
        CmzTreeNode *newNode = new CmzTreeNode(sID.getBuffer(), sdata, data);
        node->nodes->append(newNode);
        return newNode;
    }

    return NULL;
}

CmzTreeNode* CmzTree::addAsFirst(CmzTreeNode* parent, LONGLONG data, const WCHAR* id, const WCHAR* sdata) {
    CmzTreeNode *node = parent;

    if (node) {
        if (!node->nodes) {
            node->nodes = new CmzPointerList();
        }
        CmzString sID = id;
        if (sID.length() == 0) {
            ULONGLONG randomValue = random();
            sID = L"Node";
            sID += randomValue;
        }
        CmzTreeNode *newNode = new CmzTreeNode(sID.getBuffer(), sdata, data);
        node->nodes->addAsFirst(newNode);
        return newNode;
    }

    return NULL;
}

CmzTreeNode* CmzTree::insertNode(CmzTreeNode* parent, ULONGLONG pos, LONGLONG data, const WCHAR* id, const WCHAR* sdata) {
    CmzTreeNode *node = parent;

    if (node) {
        if (!node->nodes) {
            node->nodes = new CmzPointerList();
        }
        CmzString sID = id;
        if (sID.length() == 0) {
            ULONGLONG randomValue = random();
            sID = L"Node";
            sID += randomValue;
        }
        CmzTreeNode *newNode = new CmzTreeNode(sID.getBuffer(), sdata, data);
        node->nodes->insert(pos, newNode);
        return newNode;
    }

    return NULL;
}

VOID CmzTree::delNode(CmzString path) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *parentNode = this->getParentNode(path);

    if (node) {
        if (parentNode) {
            parentNode->nodes->delFirst(node);
        }
        delete node;
    }
}

LONGLONG CmzTree::delSubNodeWithData(CmzString path, LONGLONG data) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG removed = 0;

    if (node) {
        if (node->nodes) {
            for (LONGLONG i = (LONGLONG)node->nodes->size() - 1; i >= 0; i--) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->data == data) {
                    node->nodes->delAt(i);
                    delete tmpNode;
                    removed++;
                }
            }
        }
    }

    return removed;
}

LONGLONG CmzTree::delSubNodeWithId(CmzString path, CmzString id) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG removed = 0;

    if (node) {
        if (node->nodes) {
            for (LONGLONG i = (LONGLONG)node->nodes->size() - 1; i >= 0; i--) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->id == id) {
                    node->nodes->delAt(i);
                    delete tmpNode;
                    removed++;
                }
            }
        }
    }

    return removed;
}

BOOL CmzTree::delFirstNode(CmzString path, LONGLONG data) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG removed = 0;

    if (node) {
        if (node->nodes) {
            for (LONGLONG i = (LONGLONG)node->nodes->size() - 1; i >= 0; i--) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->data == data) {
                    node->nodes->delAt(i);
                    delete tmpNode;
                    removed++;
                    break;
                }
            }
        }
    }

    return removed == 1;
}

BOOL CmzTree::delFirstNodeWithId(CmzString path, CmzString id) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG removed = 0;

    if (node) {
        if (node->nodes) {
            for (LONGLONG i = (LONGLONG)node->nodes->size() - 1; i >= 0; i--) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->id == id) {
                    node->nodes->delAt(i);
                    delete tmpNode;
                    removed++;
                    break;
                }
            }
        }
    }

    return removed == 1;
}

BOOL CmzTree::delNodeAt(CmzString path, ULONGLONG pos) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG removed = 0;

    if (node) {
        if (node->nodes && pos < node->nodes->size()) {
            tmpNode = (CmzTreeNode*)node->nodes->getAt(pos);
            node->nodes->delAt(pos);
            delete tmpNode;
            removed++;
        }
    }

    return removed == 1;
}

LONGLONG CmzTree::pos(CmzString path, LONGLONG data) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG foundPos = -1;

    if (node) {
        if (node->nodes) {
            for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->data == data) {
                    foundPos = i;
                    break;
                }
            }
        }
    }

    return foundPos;
}

LONGLONG CmzTree::posWithId(CmzString path, CmzString id) {
    CmzTreeNode *node = this->getNode(path);
    CmzTreeNode *tmpNode;
    LONGLONG foundPos = -1;

    if (node) {
        if (node->nodes) {
            for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->id == id) {
                    foundPos = i;
                    break;
                }
            }
        }
    }

    return foundPos;
}

CmzString CmzTree::getNodePath(LONGLONG data, CmzTreeNode *startAtNode) {
    CmzTreeNode *node;
    CmzString path = "";
    if (startAtNode) {
        node = startAtNode;
    } else {
        node = this->_root;
    }
    CmzTreeNode *tmpNode;
    CmzString p;

    if (node) {
        if (node->nodes) {
            for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->data == data) {
                    path += tmpNode->id;
                    if (!startAtNode) {
                        path.prepend("/");
                    }
                    return path;
                }
                p = this->getNodePath(data, tmpNode);
                if (p.length() > 0) {
                    path += tmpNode->id + "/" + p;
                    if (!startAtNode) {
                        path.prepend("/");
                    }
                    return path;
                }
            }
        }
    }

    return path;
}

CmzString CmzTree::getNodeWithIdPath(CmzString id, CmzTreeNode *startAtNode) {
    CmzTreeNode *node;
    CmzString path = "";
    if (startAtNode) {
        node = startAtNode;
    } else {
        node = this->_root;
    }
    CmzTreeNode *tmpNode;
    CmzString p;

    if (node) {
        if (node->nodes) {
            for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->id == id) {
                    path += tmpNode->id;
                    if (!startAtNode) {
                        path.prepend("/");
                    }
                    return path;
                }
                p = this->getNodeWithIdPath(id, tmpNode);
                if (p.length() > 0) {
                    path += tmpNode->id + "/" + p;
                    if (!startAtNode) {
                        path.prepend("/");
                    }
                    return path;
                }
            }
        }
    }

    return path;
}

BOOL CmzTree::contains(LONGLONG data) {
    CmzString path = this->getNodePath(data);
    return path.length() > 0;
}

BOOL CmzTree::contains(CmzString id) {
    CmzString path = this->getNodeWithIdPath(id);
    return path.length() > 0;
}

BOOL CmzTree::contains(CmzString path, LONGLONG data) {
    CmzTreeNode* node = this->getNode(path);
    CmzTreeNode* tmpNode;

    if (node) {
        if (node->nodes) {
            for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->data == data) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL CmzTree::containsNodeWithId(CmzString path, CmzString id) {
    CmzTreeNode* node = this->getNode(path);
    CmzTreeNode* tmpNode;

    if (node) {
        if (node->nodes) {
            for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
                tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
                if (tmpNode->id == id) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

VOID CmzTree::clear() {
    delete this->_root;
}

CmzString CmzTree::simplePrintTree() {
    return this->simplePrintNode(this->_root, "");
}

CmzString CmzTree::simplePrintNode(CmzTreeNode *node, CmzString indent) {
    CmzString retval = indent + node->id + "\n";

    if (!node->nodes || node->nodes->size() < 1) {
        return retval;
    }

    CmzTreeNode *tmpNode;
    for (ULONGLONG i = 0; i < node->nodes->size(); i++) {
        tmpNode = (CmzTreeNode*)node->nodes->getAt(i);
        retval += this->simplePrintNode(tmpNode, indent + "    ");
    }

    return retval;
}

}