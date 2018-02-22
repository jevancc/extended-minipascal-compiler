#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "node.h"
#include "symtab.h"
#include <string>
#include <stack>
using namespace std;

#define TRV_CHILDREN(n, x) { \
    Node *_ccc = n->child; \
    if(_ccc != NULL) { \
        do { \
            x(_ccc); \
            _ccc = _ccc->rsibling; \
        } while(_ccc != n->child); } \
    }

class CodeGen {
private:
    SymTable symtab;
    Node* ast;
    string javaClassName;
    bool isClass;
    bool reNot;

    unordered_map<string, int> breakLabelTable;
    unordered_map<string, int> continueLabelTable;
    int labelCount;
    int getLabel() { return labelCount++; }

    void wConst (int);
    void wConst (double);
    void wConst (string);

    void wTypeConvert(Node *n);

    void wVarStore (Node *n1, Node *n2, bool writeN2 = true);
    void wVarLoad (Node *n);
    void wLabel(int n);
    void wJmpLabel(int n);

    void wJMethodLimits (int locals = 100, int stacks = 100);
    void wJStandardInit();

    void wHeader();
    void wTail();

    void wProgram(Node *n);
    void wProgramVars(Node *n);
    void wProgramVarInit(Node *n);

    void wFunction(Node *n);
    void wFunctionInit(Node *n);
    void wFunctionWriteln();
    void wReturn(Node *n);

    void wModule(Node *n);
    void wModuleClass(Node *n);

    void wClassInit (Node *n);
    void wClassVars(Node *n);
    void wClassVarInit (Node *n);

    void wAssign(Node *n);

    void wOp(Node *n);
    void wIfOp(Node *n, int l);
    void wIfNotOp(Node *n, int l);
    void wOr(Node*, int, int);
    void wAnd(Node*, int, int);
    void wCondition(Node*, int, int);

    void wSign(Node *n);

    void wIf(Node *n);
    void wWhile(Node *n);
    void wFor(Node *n);
    void wRepeat(Node *n);

    void wBody(Node *n);
public:
    CodeGen(Node* n, SymTable s, string c = "Main", bool isc = false)
        : javaClassName(c), symtab(s), ast(n), labelCount(0), isClass(isc), reNot(false) {}
    void dump ();
};

#endif
