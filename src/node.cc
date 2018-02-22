#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include "node.h"
#include "symtab.h"
#include "y.tab.h"
using namespace std;

const char * nTypeName[] = {
    "N_PROG"
    , "N_INT"
    , "N_REAL"
    , "N_STRING"
    , "N_OP"
    , "N_SIGN"
    , "N_NOT"
    , "N_SUBPROGRAM"
    , "N_ARRAY"
    , "N_EXPRESSION_LIST"
    , "N_TAIL"
    , "N_VAR"
    , "N_VAR_REF"
    , "N_VAR_OR_SUBPROGRAM"
    , "N_WHILE"
    , "N_IF"
    , "N_BLOCK"
    , "N_ASSIGN"
    , "N_STATEMENT_LIST"
    , "N_PARAMETER"
    , "N_PARAMETER_LIST"
    , "N_ARGUMENT_LIST"
    , "N_SUBPROGRAM_HEAD"
    , "N_PROCEDURE_HEAD"
    , "N_FUNCTION_HEAD"
    , "N_SUBPROGRAM_DECL"
    , "N_SUBPROGRAM_DECL_LIST"
    , "N_TYPE_INT"
    , "N_TYPE_REAL"
    , "N_TYPE_STRING"
    , "N_TYPE_ARRAY"
    , "N_TYPE_FILE"
    , "N_TYPE_CLASS"
    , "N_VAR_DECL"
    , "N_VAR_DECL_LIST"
    , "N_ID"
    , "N_ID_LIST"
    , "N_NULL"
    , "N_ARGV"
    , "N_JFUNCTION_DECL"
    , "N_JBYTECODE"
    , "N_MODULE"
    , "N_CLASS"
    , "N_IMPORT_LIST"
    , "N_IMPORT"
    , "N_MOD_ID"
    , "N_REPEAT"
    , "N_FOR"
    , "N_IF_TERNARY"
    , "N_AND"
    , "N_OR"
    , "N_BREAK"
    , "N_CONTINUE"
    , "N_TYPE_CONVERT"
    , "N_RETURN"
    , "N_USE_LIST"
    , "N_USE"
};

Location::Location(YYLTYPE loc):line(loc.first_line), column(loc.first_column) {};

Node* makeNode (Location loc, int nodeType, int nChildren, ...)  {
    va_list ap;
    Node * n = new Node;
    n->loc = Location(loc);
    n->nodeType = nodeType;
    n->sval = NULL;
    n->parent = NULL;
    n->child = NULL;
    n->lsibling = n;
    n->rsibling = n;
    n->op = OP_NULL;

    va_start(ap, nChildren);
    for (int i = 0; i < nChildren; i++) {
        appendChild(n, va_arg(ap, Node *), 0);
    }
    va_end(ap);
    return n;
}

Node* appendChild (Node *n, Node *c, int appendL) {
    if (c == NULL) {
        return n;
    }

    c->parent = n;

    if (n->child == NULL) {
        n->child = c;
    } else {
        c->lsibling = n->child->lsibling;
        c->rsibling = n->child;
        n->child->lsibling->rsibling = c;
        n->child->lsibling = c;
    }

    if (appendL) {
        n->child = c;
    }
    return n;
}

Node* insertParent (Node *n, nType t) {
    Node *newParent = makeNode(n->loc, t, 0);
    n->rsibling->lsibling = newParent;
    n->lsibling->rsibling = newParent;
    n->parent->child = n->parent->child == n ? newParent : n->parent->child;
    newParent->rsibling = n->rsibling;
    newParent->lsibling = n->lsibling;
    newParent->parent = n->parent;
    n->rsibling = n->lsibling = n;
    appendChild(newParent, n, 0);
    return newParent;
}

void deleteNode (Node * n) {
    // keep
}

Node::~Node() {
    delete valueType;
}

Node* Node::nthChild(int x) const{
    Node *p = child, *pp = child;
    if (child == NULL) {
        return NULL;
    }

    for (int i = 1; i < x; i++) {
        p = p->rsibling;
        if (p == pp && i > 1) {
            return NULL;
        }
    }
    if (p == pp && x > 1)
        return NULL;
    return p;
}

enum nStdValueType Node::stdType() const{
    switch (nodeType) {
        case N_STRING:
        case N_TYPE_STRING:
            return TYPE_STRING;
        case N_INT:
        case N_TYPE_INT:
            return TYPE_INT;
        case N_REAL:
        case N_TYPE_REAL:
            return TYPE_REAL;
        case N_TYPE_ARRAY:
            return TYPE_ARRAY;
        case N_PROCEDURE_HEAD:
        case N_FUNCTION_HEAD:
            return TYPE_FUNCTION;
        case N_MODULE:
            return TYPE_MODULE;
        case N_CLASS:
            return TYPE_MODULE_CLASS;
        case N_TYPE_CLASS:
            return TYPE_CLASS;
        case N_TYPE_FILE:
            return TYPE_FILE;
        case N_NULL:
            return TYPE_NULL;
        default:
            return TYPE_NULL;
    }
}

string Node::nodeTypeName(int t) const {
    char str[256];
    if (t == -1)
        t = nodeType;
    switch (t) {
        case N_INT:
            sprintf(str, "%s (%d)", nTypeName[t], ival);
            break;
        case N_REAL:
            sprintf(str, "%s (%f)", nTypeName[t], rval);
            break;
        case N_STRING:
        case N_ID:
            sprintf(str, "%s (%s)", nTypeName[t], sval);
            break;
        default:
            strcpy(str, nTypeName[t]);
            break;
    }
    return string(str);
}

const char * getNodeTokenName (int tokenType) {
    switch (tokenType) {
        case Tok_ARRAY          : return "ARRAY";
        case Tok_DO             : return "DO";
        case Tok_ELSE           : return "ELSE";
        case Tok_END            : return "END";
        case Tok_FUNCTION       : return "FUNCTION";
        case Tok_IF             : return "IF";
        case Tok_NOT            : return "NOT";
        case Tok_OF             : return "OF";
        case Tok_PBEGIN         : return "PBEGIN";
        case Tok_PROCEDURE      : return "PROCEDURE";
        case Tok_PROGRAM        : return "PROGRAM";
        case Tok_THEN           : return "THEN";
        case Tok_VAR            : return "VAR";
        case Tok_WHILE          : return "WHILE";
        case Tok_INTEGER        : return "INTEGER";
        case Tok_REAL           : return "REAL";
        case Tok_STRING         : return "STRING";
        case Tok_IDENTIFIER     : return "IDENTIFIER";
        case Tok_ASSIGNMENT     : return "ASSIGNMENT";
        case Tok_COLON          : return "COLON";
        case Tok_COMMA          : return "COMMA";
        case Tok_DOT            : return "DOT";
        case Tok_DOTDOT         : return "DOTDOT";
        case Tok_EQUAL          : return "EQUAL";
        case Tok_notEQUAL       : return "notEQUAL";
        case Tok_GE             : return "GE";
        case Tok_GT             : return "GT";
        case Tok_LBRAC          : return "LBRAC";
        case Tok_LE             : return "LE";
        case Tok_LPAREN         : return "LPAREN";
        case Tok_LT             : return "LT";
        case Tok_MINUS          : return "MINUS";
        case Tok_PLUS           : return "PLUS";
        case Tok_RBRAC          : return "RBRAC";
        case Tok_RPAREN         : return "RPAREN";
        case Tok_SEMICOLON      : return "SEMICOLON";
        case Tok_SLASH          : return "SLASH";
        case Tok_STAR           : return "STAR";
        case Tok_INTNUM         : return "INTNUM";
        case Tok_REALNUM        : return "REALNUM";
        case Tok_STRINGNUM      : return "STRINGNUM";
        case Tok_JFUNCTION      : return "JFUNCTION";
        case Tok_JBC            : return "JBC";
        case Tok_MODULE         : return "MODULE";
        case Tok_IMPORT         : return "IMPORT";
        case Tok_FROM           : return "FROM";
        case Tok_REPEAT         : return "REPEAT";
        case Tok_UNTIL          : return "UNTIL";
        case Tok_FOR            : return "FOR";
        case Tok_TO             : return "TO";
        case Tok_DOWN           : return "DOWN";
        case Tok_QUESTION       : return "QUESTION";
        case Tok_AND            : return "AND";
        case Tok_OR             : return "OR";
        case Tok_BREAK          : return "BREAK";
        case Tok_CONTINUE       : return "CONTINUE";
        case Tok_RETURN         : return "RETURN";
        case Tok_ADD_ASSIGN     : return "ADD_ASSIGN";
        case Tok_MINUS_ASSIGN   : return "MINUS_ASSIGN";
        case Tok_STAR_ASSIGN    : return "STAR_ASSIGN";
        case Tok_SLASH_ASSIGN   : return "SLASH_ASSIGN";
        case Tok_MOD_IDENTIFIER : return "MOD_IDENTIFIER";
        case Tok_LOOP_LABEL     : return "LOOP_LABEL";
        case Tok_USE            : return "USE";
        case Tok_CLASS          : return "CLASS";
        default :
            return NULL;
    }
}

bool isConditionOp(char c) {
    switch (c) {
        case OP_GT:
        case OP_LT:
        case OP_EQ:
        case OP_GE:
        case OP_LE:
        case OP_NE:
            return true;
        default:
            return false;
    }
}


void printTree (Node* n, int deep) {
    char *str;

    if (n != NULL) {
        printf("%2d ", deep);
        for (int i = 0; i < deep; i++)
            printf("_-");
        //printf("%d\n", n->nodeType);
        printf("%s\n", n->nodeTypeName().c_str());

        Node *lmp = n->child, *p = n->child;

        if (n->child != NULL) {
            do {
                printTree(p, deep + 1);
                p = p->rsibling;
            } while(p != lmp);
        }
    }
}
