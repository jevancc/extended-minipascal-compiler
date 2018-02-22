#include "logger.h"
#include "node.h"
#include "symtab.h"
#include "codegen.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <tuple>
using namespace std;

extern int globalLogLevel;
static char tmpStr[256];
static Logger log("[Semantic]", &globalLogLevel);
// static Logger log("[Semantic]", LOG_LEVEL_ERROR);


bool FunctionInfo::operator==(const FunctionInfo &f1) const {
    if (f1.params.size() != params.size()) {
        return false;
    } else {
        for (size_t i = 0; i < params.size(); i++) {
            // if (params[i].second.stdType != f1.params[i].second.stdType)
            if (params[i].second != f1.params[i].second)
                return false;
        }
        return true;
    }
}

bool FunctionInfo::operator==(const vector<TypeInfo*> &pst) const {
    if (params.size() != pst.size()) {
        return false;
    } else {
        for (size_t i = 0; i < params.size(); i++) {
            if (!typeEq(&params[i].second, pst[i]))
                return false;
        }
        return true;
    }
}

TypeInfo::~TypeInfo() {
    if (arrayInfo  != NULL) {
        // delete arrayInfo;
    }
}
TypeInfo::TypeInfo(Node *tn)
    : stdType(tn->stdType()), node(tn), arrayInfo(NULL) {
        arrayInfoInit();
        if (stdType == TYPE_CLASS) {
            className = tn->sval;
        }
    }
TypeInfo::TypeInfo(const TypeInfo &t)
    : stdType(t.stdType), className(t.className), node(t.node), arrayInfo(NULL) {
        arrayInfoInit();
    }

bool typeEq(const TypeInfo *t1, const TypeInfo *t2) {
    return *t1 == *t2;
}

bool TypeInfo::operator== (const TypeInfo &t) const {
    bool isEq = (stdType == t.stdType) && (stdType != TYPE_FUNCTION);
    if (isEq && stdType == TYPE_ARRAY) {
        return *(this->arrayInfo) == *(t.arrayInfo);
    } else if (isEq && stdType == TYPE_CLASS) {
        return className == t.className;
    }
    return isEq;
}

bool TypeInfo::operator!= (const TypeInfo &t) const {
    return !(*this == t);
}

string TypeInfo::stdTypeName() const {
    static char const *stdValueTypeString[] = {
        "TYPE_BOOL"
        , "TYPE_STRING"
        , "TYPE_REAL"
        , "TYPE_INT"
        , "TYPE_ARRAY"
        , "TYPE_CLASS"
        , "TYPE_FUNCTION"
        , "TYPE_PROCEDURE"
        , "TYPE_NULL"
        , "TYPE_FILE"
        , "TYPE_MODULE"
        , "TYPE_MODULE_CLASS"
    };
    return string(stdValueTypeString[this->stdType]);
}

string TypeInfo::jType() const {
    string r;
    switch (stdType) {
        case TYPE_INT:
            return "I";
        case TYPE_REAL:
            return "F";
        case TYPE_STRING:
            return "Ljava/lang/String;";
        case TYPE_ARRAY:
            return arrayInfo->jType;
        case TYPE_CLASS:
            r = className;
            for (auto &c : r)
                if (c == '.') c = '/';
            return "L" + r + ";";
        case TYPE_NULL:
            return "V";
        default:
            return "";
    }
    return "";
}

void TypeInfo::arrayInfoInit() {
    int sidx, eidx;
    Node *p;

    if (stdType != TYPE_ARRAY) {
        this->arrayInfo = NULL;
        return;
    }

    if (this->arrayInfo == NULL) {
        this->arrayInfo = new ArrayInfo;

        p = node->child;
        sidx = p->ival;
        p = p->rsibling;
        eidx = p->ival;
        p = p->rsibling;

        TypeInfo ti(p);
        ArrayInfo* ai = this->arrayInfo;
        if (ti.stdType == TYPE_ARRAY) {
            ai->dim = ti.arrayInfo->dim;
            ai->sidx = ti.arrayInfo->sidx;
            ai->baseType = ti.arrayInfo->baseType;
        } else {
            ai->baseType = ti;
        }
        ai->jType = "[" + ti.jType();
        ai->dim.push_front(eidx - sidx + 1);
        ai->sidx.push_front(sidx);
    }
}

tuple<int, int, TypeInfo> TypeInfo::arrayBase() const {
    int sidx, eidx;
    nStdValueType t;

    if (stdType != TYPE_ARRAY)
        return make_tuple(0, 0, TypeInfo());

    Node *p = node->child;
    sidx = p->ival;
    p = p->rsibling;
    eidx = p->ival;
    p = p->rsibling;

    return make_tuple(sidx, eidx, TypeInfo(p));
}

static inline TypeInfo* min(TypeInfo* a, TypeInfo* b) { return a->stdType < b->stdType ? a : b; }
static inline TypeInfo* max(TypeInfo* a, TypeInfo* b) { return a->stdType > b->stdType ? a : b; }

TypeInfo* commonValueType(TypeInfo *t1, TypeInfo*t2) {
    if (typeEq(t1, t2) && t1->stdType <= TYPE_CLASS) {
        return t1;
    }

    TypeInfo* typemin = min(t1, t2);
    TypeInfo* typemax = max(t1, t2);

    if (typemin->stdType != TYPE_BOOL && typemax->stdType <= TYPE_INT) {
        return typemin;
    } else {
        return NULL;
    }
}

Node* insertTypeConvertParent(Node *n, TypeInfo *tn) {
    Node *conv;
    if (typeEq(n->valueType, tn)) {
        return NULL;
    }

    conv = insertParent(n, N_TYPE_CONVERT);
    conv->valueType = new TypeInfo(tn);
}

bool isModEntry(SymTableEntry *entry) {
    switch (entry->valueType->stdType) {
        case TYPE_CLASS:
        case TYPE_MODULE:
            return true;
        default:
            return false;
    }
}

tuple<string, string> splitModId(string s) {
    size_t p = s.find('.');
    if (p != string::npos)
        return make_tuple(s.substr(0, p), s.substr(p + 1));
    else
        return make_tuple("", s);
}

string replaceClassDot(string s) {
    string r;
    for (auto &c : s) {
        r += c == '.' ? '/' : c;
    }
    return r;
}
