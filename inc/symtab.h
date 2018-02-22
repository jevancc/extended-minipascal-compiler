#ifndef _SYMTAB_H_
#define _SYMTAB_H_

enum nStdValueType {
    TYPE_BOOL
    , TYPE_STRING
    , TYPE_REAL
    , TYPE_INT
    , TYPE_ARRAY
    , TYPE_CLASS
    , TYPE_FUNCTION
    , TYPE_PROCEDURE
    , TYPE_NULL
    , TYPE_FILE
    , TYPE_MODULE
    , TYPE_MODULE_CLASS
};

#include <vector>
#include <stack>
#include <unordered_map>
#include <map>
#include <string>
#include <utility>
#include <tuple>
#include <list>
#include "node.h"
using namespace std;

class Node;
class ArrayInfo;
class SymTable;

class ModuleInfo {
public:
    string name;

    bool importAll;
    vector<string> memberImport;
    SymTable* symtab;
};

class TypeInfo {
private:
    void arrayInfoInit();
public:
    enum nStdValueType stdType;
    Node *node;
    ArrayInfo *arrayInfo;
    string className;

    TypeInfo(): stdType(TYPE_NULL), node(NULL), arrayInfo(NULL) { arrayInfoInit(); }
    TypeInfo(Node *tn);
    TypeInfo(TypeInfo *tp)
        : stdType(tp->stdType), className(tp->className), node(tp->node), arrayInfo(NULL) { arrayInfoInit(); }
    TypeInfo(const TypeInfo &t);
    TypeInfo(nStdValueType t): stdType(t), node(NULL), arrayInfo(NULL) { arrayInfoInit(); }

    string jType() const;
    string stdTypeName() const;
    tuple<int, int, TypeInfo> arrayBase() const;

    bool operator== (const TypeInfo &t) const;
    bool operator!= (const TypeInfo &t) const;
    ~TypeInfo();
};
bool typeEq(const TypeInfo *t1, const TypeInfo *t2);

class ArrayInfo {
public:
    string jType;
    list<int> dim;
    list<int> sidx;
    TypeInfo baseType;
    bool operator==(const ArrayInfo ai) const {
        return dim == ai.dim && sidx == ai.sidx && baseType == ai.baseType;
    }
};

#define NAME first
#define TYPE second
class FunctionInfo {
public:
    TypeInfo returnType;
    vector< pair<string, TypeInfo> > params;
    bool operator==(const FunctionInfo &f1) const;
    bool operator==(const vector<TypeInfo*> &pst) const;
};

class SymTableEntry {
public:
    Node *n;
    string name;
    int scopeId;
    int varId;
    TypeInfo *valueType;
    FunctionInfo *funcInfo;
    SymTable *symtab;

    SymTableEntry *prevVar;
    SymTableEntry *var;
    SymTableEntry *level;

    SymTableEntry () {}
    SymTableEntry (
        string s
        , Node *tn
        , int sid
        , SymTableEntry* v = NULL
        , SymTableEntry* pv = NULL
    ):name(s), scopeId(sid), var(v), prevVar(pv), level(NULL), funcInfo(NULL) {
        valueType = new TypeInfo(tn);
        if (valueType->stdType == TYPE_FUNCTION)
            funcInfo = new FunctionInfo;
    };

    ~SymTableEntry() {
        delete valueType;
        if (funcInfo)
            delete funcInfo;
    }
};


class SymTable {
public:
    int scopeCount;
    stack< tuple<int, SymTableEntry*, int> > scopeStack;
    unordered_map< string, SymTableEntry* > hashTable;
    map< int, vector<SymTableEntry*> > scopeVars;

    unordered_map<string, SymTableEntry*> publicVars;
    SymTableEntry* returnFuncEntry;

    int loopLabelCount;
    stack<string> loopLabelStack;
    unordered_map<string, Node*> loopLabelTable;
    vector<Node*> importFuncNode;

    bool isClass;
public:
    SymTable();
    void openScope();
    void closeScope();
    SymTableEntry* findSymbol(string s);
    SymTableEntry* findFuncSymbol(string s, vector<TypeInfo*> *paramsTyp = NULL);
    SymTableEntry* findFuncSymbol(string s, FunctionInfo *funcInfo);
    SymTableEntry* addVariable(Node *n, string s, Node *typeNode, FunctionInfo *funcInfo = NULL);

    bool semanticCheck(Node *n);
    void dumpScope(int x);
    void dumpAll();
};

extern Node* parseModule (string, string);
TypeInfo* commonValueType(TypeInfo*, TypeInfo*);
Node* insertTypeConvertParent(Node*, TypeInfo*);
bool isModEntry(SymTableEntry *entry);
tuple<string, string> splitModId(string s);
#endif
