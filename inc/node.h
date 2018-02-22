#ifndef _NODE_H_
#define _NODE_H_

class TypeInfo;
class SymTableEntry;
class ModuleInfo;

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include "symtab.h"
using namespace std;

enum nType {
    N_PROG
    , N_INT
    , N_REAL
    , N_STRING
    , N_OP
    , N_SIGN
    , N_NOT
    , N_SUBPROGRAM
    , N_ARRAY
    , N_EXPRESSION_LIST
    , N_TAIL
    , N_VAR
    , N_VAR_REF
    , N_VAR_OR_SUBPROGRAM
    , N_WHILE
    , N_IF
    , N_BLOCK
    , N_ASSIGN
    , N_STATEMENT_LIST
    , N_PARAMETER
    , N_PARAMETER_LIST
    , N_ARGUMENT_LIST
    , N_SUBPROGRAM_HEAD
    , N_PROCEDURE_HEAD
    , N_FUNCTION_HEAD
    , N_SUBPROGRAM_DECL
    , N_SUBPROGRAM_DECL_LIST
    , N_TYPE_INT
    , N_TYPE_REAL
    , N_TYPE_STRING
    , N_TYPE_ARRAY
    , N_TYPE_FILE
    , N_TYPE_CLASS
    , N_VAR_DECL
    , N_VAR_DECL_LIST
    , N_ID
    , N_ID_LIST
    , N_NULL
    , N_ARGV
    , N_JFUNCTION_DECL
    , N_JBYTECODE
    , N_MODULE
    , N_CLASS
    , N_IMPORT_LIST
    , N_IMPORT
    , N_MOD_ID
    , N_REPEAT
    , N_FOR
    , N_IF_TERNARY
    , N_AND
    , N_OR
    , N_BREAK
    , N_CONTINUE
    , N_TYPE_CONVERT
    , N_RETURN
    , N_USE_LIST
    , N_USE
};

enum nOperator {
    OP_NULL
    , OP_ADD
    , OP_SUB
    , OP_MUL
    , OP_DIV
    , OP_GT
    , OP_LT
    , OP_EQ
    , OP_GE
    , OP_LE
    , OP_NE
    , OP_NOT
};

struct YYLTYPE;
class Location {
public:
    int line;
    int column;
    Location(int l = -1, int c = -1):line(l), column(c){};
    Location(YYLTYPE loc);
};

class Node {
public:
    Location loc;

    int nodeType;
    Node *parent;
    Node *child;
    Node *lsibling;
    Node *rsibling;

    string className;
    SymTableEntry* classEntry;
    SymTableEntry* memberEntry;

    int op;
    union {
        int ival;
        double rval;
        char *sval;
    };

    TypeInfo *valueType;
    SymTableEntry *entry;
    ModuleInfo *modInfo;
    char valueValid;

    Node* nthChild(int x) const;
    enum nStdValueType stdType() const;

    string nodeTypeName(int t = -1) const;

    Node(): child(NULL), classEntry(NULL), memberEntry(NULL){}
    ~Node ();
};

Node* makeNode (Location, int nodeType, int nChildren, ...);
#define MKN0(A) makeNode(yylloc, A, 0)
#define MKN1(A, B) makeNode(yylloc, A, 1, B)
#define MKN2(A, B, C) makeNode(yylloc, A, 2, B, C)
#define MKN3(A, B, C, D) makeNode(yylloc, A, 3, B, C, D)
#define MKN4(A, B, C, D, E) makeNode(yylloc, A, 4, B, C, D, E)
#define MKN5(A, B, C, D, E, F) makeNode(yylloc, A, 5, B, C, D, E, F)
#define MKN6(A, B, C, D, E, F, G) makeNode(yylloc, A, 6, B, C, D, E, F, G)
#define MKN7(A, B, C, D, E, F, G, H) makeNode(yylloc, A, 7, B, C, D, E, F, G, H)

Node* appendChild (Node *n, Node *c, int appendL);
#define APC(A, B) appendChild(A, B, 0)
#define APCR(A, B) appendChild(A, B, 0)
#define APCL(A, B) appendChild(A, B, 1)

void deleteNode (Node * n);
#define DN(A) deleteNode(A)

Node* insertParent (Node *n, nType t);

const char * getNodeTokenName (int tokenType);
void printTree (Node* n, int deep);

extern const char * nTypeName[];

bool isConditionOp(char c);

#endif
