#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <utility>
#include <tuple>
#include <map>
#include <set>
#include <list>
#include "symtab.h"
#include "node.h"
#include "codegen.h"
#include "logger.h"
using namespace std;

extern int globalLogLevel;
extern bool disAST;
extern bool disSymTab;

static char tmpStr[256];
static Logger log("[Semantic]", &globalLogLevel);
// static Logger log("[Semantic]", LOG_LEVEL_ERROR);

// To use macro MKNx
static Location yylloc(-1, -1);

unordered_map<string, Node*> modules;
unordered_map<string, SymTable*> modulesSymTab;

SymTable::SymTable () {
    isClass = false;
    loopLabelCount = 0;
    scopeCount = -1;
    hashTable.clear();
    while (!scopeStack.empty()) {
        scopeStack.pop();
    }
}

void SymTable::openScope () {
    scopeCount++;
    if (isClass) {
        scopeStack.push(make_tuple(scopeCount, (SymTableEntry*)(NULL), 1));
    } else {
        scopeStack.push(make_tuple(scopeCount, (SymTableEntry*)(NULL), 0));
    }
}

void SymTable::closeScope () {
    int scopeId, varCount;
    SymTableEntry* p;
    tie(scopeId, p, varCount) = scopeStack.top();

    scopeStack.pop();

    vector<SymTableEntry*> vars;
    while (p != NULL){
        vars.push_back(p);

        if (p->scopeId != scopeId) {
            log.error("Unexpected error: Symbol Table Error");
            exit(-1);
        }

        if (scopeId != 0){
            if (p->prevVar == NULL) {
                hashTable[p->name] = p->var;
            } else {
                p->prevVar = p->var;
            }
        }

        SymTableEntry* oldp = p;
        p = p->level;
        // delete oldp;
    }
    for (size_t i = 0; i < vars.size()>>1; i++)
        tie(vars[i], vars[vars.size()-i-1]) = make_tuple(vars[vars.size()-i-1], vars[i]);

    scopeVars.insert(make_pair(scopeId, vars));
    // fprintf(stderr, "Close Scope\n");
    // dumpScope(scopeId);
}


SymTableEntry* SymTable::findFuncSymbol(string s, FunctionInfo *funcInfo) {
    if (hashTable.find(s) != hashTable.end()) {
        SymTableEntry *p = hashTable[s];
        while (p != NULL) {
            if (p->valueType->stdType == TYPE_FUNCTION && *(p->funcInfo) == *funcInfo) {
                return p;
            }
            p = p->var;
        }
        return NULL;
    } else {
        return NULL;
    }
}

SymTableEntry* SymTable::findFuncSymbol(string s, vector<TypeInfo*> *paramsType) {
    if (hashTable.find(s) != hashTable.end()) {
        SymTableEntry *p = hashTable[s];
        while (p != NULL) {
            if (p->valueType->stdType == TYPE_FUNCTION && *(p->funcInfo) == *paramsType) {
                return p;
            }
            p = p->var;
        }
        return NULL;
    } else {
        return NULL;
    }
}

SymTableEntry* SymTable::findSymbol(string s) {
    if (hashTable.find(s) != hashTable.end()) {
        return hashTable[s];
    } else {
        return NULL;
    }
}

SymTableEntry* SymTable::addVariable(Node *n, string s, Node *typeNode, FunctionInfo *funcInfo) {
    int stVarCount, stScopeId;
    SymTableEntry *stLevel;
    tie(stScopeId, stLevel, stVarCount) = scopeStack.top();

    SymTableEntry* sp = findSymbol(s);
    if (TypeInfo(typeNode).stdType == TYPE_FUNCTION) {
        SymTableEntry* spit = sp;
        while (spit != NULL) {
            if (spit->valueType->stdType == TYPE_FUNCTION && findFuncSymbol(s, funcInfo) != NULL) {
                return NULL;
            }
            spit = spit->var;
        }
    } else if (TypeInfo(typeNode).stdType == TYPE_CLASS) {
        SymTableEntry* sp = findSymbol(typeNode->sval);
        if (sp == NULL || sp->valueType->stdType != TYPE_MODULE_CLASS) {
            return (SymTableEntry*)-1;
        }
    } else if(sp != NULL && sp->scopeId == stScopeId) {
        return NULL;
    }

    SymTableEntry* p = new SymTableEntry(s, typeNode, stScopeId, sp);
    if (TypeInfo(typeNode).stdType == TYPE_FUNCTION) {
        p->varId = -1;
        *p->funcInfo = *funcInfo;
    } else {
        p->varId = stVarCount++;
    }

    p->level = stLevel;
    stLevel = p;

    scopeStack.top() = make_tuple(stScopeId, stLevel, stVarCount);
    if (sp != NULL) {
        sp -> prevVar = p;
    }
    hashTable[p->name] = p;
    p->n = n;
    return p;
}

bool SymTable::semanticCheck(Node *n) {
    bool noError = true;

    sprintf(tmpStr, "Checking AST node: %s", n->nodeTypeName().c_str());
    log.info(n->loc, tmpStr);

    switch(n->nodeType) {
        case N_PROG: {
            bool noError = true;
            openScope();
            sprintf(tmpStr, "Open new scope - Program: %d", scopeCount);
            log.info(n->loc, tmpStr);
            noError &= semanticCheck(n->nthChild(1));
            noError &= semanticCheck(n->nthChild(2));
            noError &= semanticCheck(n->nthChild(3));
            noError &= semanticCheck(n->nthChild(4));
            noError &= semanticCheck(n->nthChild(5));
            noError &= semanticCheck(n->nthChild(6));
            noError &= semanticCheck(n->nthChild(7));
            closeScope();
            return noError;
        }
        case N_USE: {
            Node* moduleNode;

            if (modules[n->nthChild(1)->sval] == NULL) {
                moduleNode = parseModule(n->nthChild(1)->sval, ".pc");
                if (moduleNode == NULL) {
                    return false;
                }
                modules[n->nthChild(1)->sval] = moduleNode;
            } else {
                moduleNode = modules[n->nthChild(1)->sval];
            }

            if (moduleNode->nodeType != N_CLASS) {
                log.error(n->loc, "Use error: use file is not a class");
                return false;
            } else if(strcmp(n->nthChild(1)->sval, moduleNode->nthChild(1)->sval) != 0) {
                log.error(n->loc, "Module name should be same as the file name");
                return false;
            }

            n->modInfo = new ModuleInfo;

            if (modulesSymTab[n->nthChild(1)->sval] == NULL) {
                n->modInfo->symtab = new SymTable;
                n->modInfo->symtab->isClass = true;
                sprintf(tmpStr, "============ Checking class %s ============",
                    (string(n->nthChild(1)->sval)+".pc").c_str());
                log.info(tmpStr);
                n->modInfo->symtab->openScope();
                if (!n->modInfo->symtab->semanticCheck(moduleNode)) {
                    return false;
                }
                n->modInfo->symtab->closeScope();
                sprintf(tmpStr, "============ END %s =============",
                    (string(n->nthChild(1)->sval)+".pc").c_str());
                log.info(tmpStr);
                modulesSymTab[n->nthChild(1)->sval] = n->modInfo->symtab;
            } else {
                n->modInfo->symtab = modulesSymTab[n->nthChild(1)->sval];
            }

            CodeGen cg(moduleNode, *n->modInfo->symtab, n->nthChild(1)->sval, true);
            cg.dump();

            if (disAST) {
                fprintf(stderr, "========= AST of %s ==========\n", (string(n->nthChild(1)->sval)+".pc").c_str());
                printTree(moduleNode, 0);
                fputs("========= AST END ==========\n", stderr);
            }
            if (disSymTab) {
                fprintf(stderr, "========= SYMTAB of %s ==========\n", (string(n->nthChild(1)->sval)+".pc").c_str());
                n->modInfo->symtab->dumpAll();
                fputs("========= SYMTAB END ==========\n", stderr);
            }

            SymTableEntry *entry = addVariable(n, n->nthChild(1)->sval, moduleNode);
            entry->symtab = n->modInfo->symtab;
            return true;
        }
        case N_IMPORT: {
            Node* moduleNode;
            if (modules[n->nthChild(1)->sval] == NULL) {
                moduleNode = parseModule(n->nthChild(1)->sval, ".pm");
                if (moduleNode == NULL) {
                    return false;
                }
                modules[n->nthChild(1)->sval] = moduleNode;
            } else {
                moduleNode = modules[n->nthChild(1)->sval];
            }

            if (moduleNode->nodeType != N_MODULE) {
                log.error(n->loc, "Import error: import file is not a module");
                return false;
            } else if(strcmp(n->nthChild(1)->sval, moduleNode->nthChild(1)->sval) != 0) {
                log.error(n->loc, "Module name should be same as the file name");
                return false;
            }

            n->modInfo = new ModuleInfo;

            if (modulesSymTab[n->nthChild(1)->sval] == NULL) {
                n->modInfo->symtab = new SymTable;
                sprintf(tmpStr, "============ Checking module %s ============",
                        (string(n->nthChild(1)->sval)+".pm").c_str());
                log.info(tmpStr);
                n->modInfo->symtab->openScope();
                if (!n->modInfo->symtab->semanticCheck(moduleNode)) {
                    return false;
                }
                n->modInfo->symtab->closeScope();
                sprintf(tmpStr, "============ END %s =============",
                    (string(n->nthChild(1)->sval)+".pc").c_str());
                log.info(tmpStr);
                modulesSymTab[n->nthChild(1)->sval] = n->modInfo->symtab;
            } else {
                n->modInfo->symtab = modulesSymTab[n->nthChild(1)->sval];
            }

            Node *im = n->nthChild(2);
            if (im != NULL) {
                SymTable *symtab = n->modInfo->symtab;
                // APC(n, moduleNode);
                if (im->nodeType == N_NULL) {
                    // Import *
                    for (size_t i = 0; i < symtab->scopeVars[0].size(); i++){
                        SymTableEntry* var = symtab->scopeVars[0][i];
                        if (addVariable(var->n, var->name, var->valueType->node, var->funcInfo) == NULL) {
                            sprintf(tmpStr, "Module import failed: duplicated function name :%s", var->name.c_str());
                            log.error(n->loc, tmpStr);
                            return false;
                        }
                        importFuncNode.push_back(var->n);
                    }
                } else {
                    // Import Func1, Func2, ...
                    Node *idNode = im->child;
                    map<string, int> im_list;
                    do {
                        im_list[idNode->sval] = -1;
                        idNode = idNode->rsibling;
                    } while(idNode != im->child);

                    for (size_t i = 0; i < symtab->scopeVars[0].size(); i++) {
                        SymTableEntry* var = symtab->scopeVars[0][i];
                        if (im_list[var->name] != 0) {
                            im_list[var->name] = 1;
                            if (addVariable(var->n, var->name, var->valueType->node, var->funcInfo) == NULL) {
                                sprintf(tmpStr, "Module import failed: duplicated function name :%s", var->name.c_str());
                                log.error(n->loc, tmpStr);
                                return false;
                            }
                            importFuncNode.push_back(var->n);
                        } else {
                            var->n->nodeType = N_NULL;
                        }
                    }
                    for (auto it = im_list.begin(); it != im_list.end(); it++) {
                        if (it->second == -1) {
                            sprintf(tmpStr, "Module import failed: function not found %s", it->first.c_str());
                            log.error(n->loc, tmpStr);
                            return false;
                        }
                    }
                }
            } else {
                CodeGen cg(moduleNode, *n->modInfo->symtab, n->nthChild(1)->sval);
                cg.dump();
                if (disAST) {
                    fprintf(stderr, "========= AST of %s ==========\n", (string(n->nthChild(1)->sval)+".pm").c_str());
                    printTree(moduleNode, 0);
                    fputs("========= AST END ==========\n", stderr);
                }
                if (disSymTab) {
                    fprintf(stderr, "========= SYMTAB of %s ==========\n", (string(n->nthChild(1)->sval)+".pm").c_str());
                    n->modInfo->symtab->dumpAll();
                    fputs("========= SYMTAB END ==========\n", stderr);
                }

                SymTableEntry *entry = addVariable(n, n->nthChild(1)->sval, moduleNode);
                entry->symtab = n->modInfo->symtab;
                // n->nodeType = N_NULL;
            }
            return true;
        }

        case N_JFUNCTION_DECL: {
            Node *head = n->nthChild(1);
            Node *idNode = head->nthChild(1);
            Node *returnTypeNode = head->nthChild(3);

            // Retrieve params from the node and extract into vector
            FunctionInfo funcInfo;
            Node *params = head->nthChild(2);
            Node *param = params->nthChild(1);
            if (param != NULL) {
                do {
                    TypeInfo type(param);
                    funcInfo.params.push_back(make_pair("", type));
                    param = param->rsibling;
                } while (param != params->child);
            }

            SymTableEntry* entry = addVariable(n, idNode->sval, head, &funcInfo);
            if (entry == NULL) {
                sprintf(tmpStr, "Duplicate declaration of variable %s", idNode->sval);
                log.error(n->loc, tmpStr);
                return false;
            }
            entry->funcInfo->returnType = TypeInfo(returnTypeNode);
            // *entry->funcInfo = funcInfo;
            log.info(n->loc, tmpStr);
            sprintf(tmpStr, "Declare variable: %s %s", head->nthChild(1)->sval, head->nodeTypeName().c_str());

            openScope();
            if (entry->funcInfo->returnType.stdType != TYPE_NULL) {
                SymTableEntry *retEntry = addVariable(head->nthChild(1), entry->name, returnTypeNode);
                if (retEntry == NULL) {
                    sprintf(tmpStr, "Duplicate declaration of variable %s", idNode->sval);
                    log.error(n->loc, tmpStr);
                    noError = false;
                }
                head->nthChild(1)->entry = retEntry;
            }
            closeScope();
            n->entry = entry;
            return noError;
        }

        case N_RETURN: {
            if (!semanticCheck(n->nthChild(1))) {
                return false;
            }

            TypeInfo t(n->nthChild(1)->valueType);
            if (!typeEq(&returnFuncEntry->funcInfo->returnType, &t)) {
                TypeInfo *tn = commonValueType(&returnFuncEntry->funcInfo->returnType, &t);
                if (tn == NULL || !typeEq(tn, &returnFuncEntry->funcInfo->returnType)) {
                    log.error(n->loc, "Type mismatch for return");
                    return false;
                }
                insertTypeConvertParent(n->nthChild(1), tn);
            }
            return true;
        }

        case N_SUBPROGRAM_DECL: {
            Node *head = n->nthChild(1);
            Node *idNode = head->nthChild(1);
            Node *returnTypeNode = head->nthChild(3);

            // Retrieve params from the node and extract into vector
            FunctionInfo funcInfo;
            Node *params = head->nthChild(2);
            Node *param = params->nthChild(1);
            if (param != NULL) {
                do {
                    TypeInfo type(param->nthChild(2));
                    Node *idList = param->nthChild(1);
                    Node *idNode = idList->nthChild(1);
                    if (idNode != NULL){
                        do {
                            funcInfo.params.push_back(make_pair(idNode->sval, type));
                            idNode = idNode->rsibling;
                        } while (idNode != idList->child);
                    }
                    param = param->rsibling;
                } while (param != params->child);
            }
            SymTableEntry* entry = addVariable(n, idNode->sval, head, &funcInfo);
            if (entry == NULL) {
                sprintf(tmpStr, "Duplicate declaration of variable %s", idNode->sval);
                log.error(n->loc, tmpStr);
                return false;
            }
            entry->funcInfo->returnType = TypeInfo(returnTypeNode);

            sprintf(tmpStr, "Declare variable: %s %s", head->nthChild(1)->sval, head->nodeTypeName().c_str());
            log.info(n->loc, tmpStr);

            openScope();
            sprintf(tmpStr, "Open new scope - Function: %d", scopeCount);
            log.info(n->loc, tmpStr);

            noError &= semanticCheck(head->nthChild(2));
            if (entry->funcInfo->returnType.stdType != TYPE_NULL) {
                SymTableEntry *retEntry = addVariable(head->nthChild(1), entry->name, returnTypeNode);
                if (retEntry == NULL) {
                    sprintf(tmpStr, "Duplicate declaration of variable %s", idNode->sval);
                    log.error(n->loc, tmpStr);
                    noError = false;
                }
                head->nthChild(1)->entry = retEntry;
            }
            noError &= semanticCheck(n->nthChild(2));

            returnFuncEntry = entry;
            noError &= semanticCheck(n->nthChild(3));
            returnFuncEntry = NULL;

            sprintf(tmpStr, "Close scope - Function: %d", scopeCount);
            log.info(n->loc, tmpStr);
            closeScope();
            n->entry = entry;
            return noError;
        }

        case N_PARAMETER:
        case N_VAR_DECL: {
            /* We only implement integer and real type here,
               you should implement array type by yourself */
            Node *typeNode = n->nthChild(2);
            Node *idList = n->nthChild(1);
            Node *idNode = idList->child;

            // Check the range of type array
            for (Node *t = typeNode; t->stdType() == TYPE_ARRAY; t = t->nthChild(3)) {
                Node *sidx = t->nthChild(1);
                Node *eidx = t->nthChild(2);
                if (sidx->stdType() != TYPE_INT || eidx->stdType() != TYPE_INT) {
                    sprintf(tmpStr, "Invalid start and end index in array declaration: %s", idNode->sval);
                    log.error(n->loc, tmpStr);
                    return false;
                }
            }

            // Add variable to symbol table
            do {
                SymTableEntry *entry = addVariable(idNode, idNode->sval, typeNode);
                if (entry == NULL) {
                    sprintf(tmpStr, "Duplicate declaration of variable %s", idNode->sval);
                    log.error(idNode->loc, tmpStr);
                    return false;
                } else if (entry == (SymTableEntry*)-1) {
                    sprintf(tmpStr, "Class type not found %s", typeNode->sval);
                    log.error(typeNode->loc, tmpStr);
                    return false;
                } else if (entry->valueType->stdType == TYPE_CLASS) {
                    entry->symtab = findSymbol(typeNode->sval)->symtab;
                }

                sprintf(tmpStr, "Declare variable: %s %s", idNode->sval, typeNode->nodeTypeName().c_str());
                log.info(idNode->loc, tmpStr);

                idNode = idNode->rsibling;
                idNode->entry = entry;
            } while(idNode != idList->child);

            return true;
        }

        case N_VAR:
        case N_VAR_REF:
        case N_SUBPROGRAM:
        case N_VAR_OR_SUBPROGRAM: {
            SymTableEntry *entry;
            string base, id;

            n->classEntry = n->memberEntry = NULL;
            tie(base, id) = splitModId(n->sval);
            if (base != "") {
                entry = findSymbol(base);
                if (entry == NULL || (entry->valueType->stdType != TYPE_MODULE && entry->valueType->stdType != TYPE_CLASS) ) {
                    sprintf(tmpStr, "Unimported module %s", base.c_str());
                    log.error(n->loc, tmpStr);
                    return false;
                }
                n->classEntry = entry;

                entry = entry->symtab->findSymbol(id);
                n->memberEntry = entry;
            } else {
                entry = findSymbol(n->sval);
            }

            if(entry == NULL) {
                sprintf(tmpStr, "Undeclared variable %s", n->sval);
                log.error(n->loc, tmpStr);
                return false;
            }

            if (n->nodeType == N_SUBPROGRAM || entry->valueType->stdType == TYPE_FUNCTION) {
                Node *params = n->nthChild(1);
                if(params != NULL && !semanticCheck(params)){
                    return false;
                }
                vector<TypeInfo*> paramsType;
                if (params != NULL) {
                    Node *param = params->nthChild(1);
                    do {
                        paramsType.push_back(param->valueType);
                        param = param->rsibling;
                    } while (param != params->child);
                }

                if (base != "")
                    entry = n->classEntry->symtab->findFuncSymbol(id, &paramsType);
                else
                    entry = findFuncSymbol(n->sval, &paramsType);

                if (entry == NULL) {
                    sprintf(tmpStr, "Invalid function parameters: %s", n->sval);
                    log.error(n->loc, tmpStr);
                    return false;
                }
                n->valueType = new TypeInfo(entry->funcInfo->returnType);
            } else if (n->child != NULL) {
                if (entry->valueType->stdType != TYPE_ARRAY) {
                    sprintf(tmpStr, "Illegal qualifier: %s", n->sval);
                    log.error(n->loc, tmpStr);
                    return false;
                }

                int sidx, eidx;
                TypeInfo b(entry->valueType);
                const ArrayInfo* arrInf = entry->valueType->arrayInfo;
                auto dimIt = arrInf->dim.begin();
                Node *idxNode = n->child;
                do {
                    if (!semanticCheck(idxNode)) {
                        return false;
                    }
                    tie(sidx, eidx, b) = b.arrayBase();
                    if (dimIt == arrInf->dim.end()) {
                        sprintf(tmpStr, "Illegal qualifier: %s", n->sval);
                        log.error(n->loc, tmpStr);
                        return false;
                    }
                    if (idxNode->valueType->stdType != TYPE_INT) {
                        sprintf(tmpStr, "Illegal array index: %s", n->sval);
                        log.error(n->loc, tmpStr);
                        return false;
                    }

                    idxNode = idxNode->rsibling;
                    dimIt++;
                } while(idxNode != n->child);

                if (b.stdType == TYPE_ARRAY) {
                    n->valueType = new TypeInfo(b.node);
                } else {
                    n->valueType = new TypeInfo(arrInf->baseType);
                }
            } else {
                n->valueType = new TypeInfo(entry->valueType);
            }

            n->entry = entry;
            return true;
        }
        case N_NULL: {
            n->valueType = new TypeInfo(n);
            return true;
        }
        case N_INT: {
            n->valueType = new TypeInfo(n);
            return true;
        }
        case N_REAL: {
            n->valueType = new TypeInfo(n);
            return true;
        }
        case N_STRING: {
            n->valueType = new TypeInfo(n);
            return true;
        }

        case N_NOT: {
            Node *child = n->child;

            if (!semanticCheck(child))
                return false;

            if (child->valueType->stdType != TYPE_BOOL) {
                sprintf(tmpStr, "Operator NOT only operated on relational expression");
                log.error(n->loc, tmpStr);
                return false;
            }

            n->valueType = new TypeInfo(TYPE_BOOL);
            return true;
            // Node *child = n->nthChild(1);
            // if (!semanticCheck(child)) {
            //     return false;
            // }
            //
            // if (child->valueType->stdType != TYPE_INT) {
            //     log.error(n->loc, "Operator is not overloaded: not");
            //     return false;
            // }
            //
            // n->valueType = new TypeInfo(child->valueType);
            // return true;
        }
        case N_SIGN: {
            Node *child = n->nthChild(1);
            if (!semanticCheck(child)) {
                return false;
            }

            if (child->valueType->stdType != TYPE_INT && child->valueType->stdType != TYPE_REAL) {
                log.error(n->loc, "Operator is not overloaded: sign");
                return false;
            }

            n->valueType = new TypeInfo(child->valueType);
            return true;
        }
        case N_CONTINUE:
        case N_BREAK: {
            string label;
            if (n->child != NULL) {
                label = n->child->sval;
            } else {
                if (loopLabelStack.empty()) {
                    if (n->nodeType == N_CONTINUE)
                        log.error(n->loc, "Illegal continue statement");
                    else
                        log.error(n->loc, "Illegal break statement");
                    return false;
                }

                label = loopLabelStack.top();
                Node *labelNode = MKN0(N_ID);
                labelNode->sval = strdup(label.c_str());
                APC(n, labelNode);
            }

            if (loopLabelTable.find(label) == loopLabelTable.end()) {
                sprintf(tmpStr, "Loop label not found: %s", label.c_str());
                log.error(n->loc, tmpStr);
                return false;
            }
            return true;
        }
        case N_FOR: {
            bool noError = true;
            string label;
            noError &= semanticCheck(n->nthChild(1));
            noError &= semanticCheck(n->nthChild(2));

            if (n->nthChild(5) != NULL) {
                label = n->nthChild(5)->sval;
            } else {
                label = "__FOR_" + to_string(loopLabelCount++);
                Node *labelNode = MKN0(N_ID);
                labelNode->sval = strdup(label.c_str());
                APC(n, labelNode);
            }
            if (loopLabelTable.find(label) != loopLabelTable.end()) {
                sprintf(tmpStr, "Duplicated loop label: %s", label.c_str());
                log.error(n->loc, tmpStr);
                return false;
            } else {
                loopLabelTable[label] = n;
                loopLabelStack.push(label);
            }
            noError &= semanticCheck(n->nthChild(3));
            loopLabelStack.pop();
            loopLabelTable.erase(label);

            Node *start = n->nthChild(1);
            Node *end = n->nthChild(2);
            if (noError && start->nthChild(2)->valueType->stdType != TYPE_INT) {
                log.error(n->loc, "Illegal for variable and initial value");
                return false;
            }
            if (noError && end->valueType->stdType != TYPE_INT) {
                log.error(n->loc, "Illegal for final value");
                return false;
            }

            return noError;
        }
        case N_REPEAT: {
            bool noError = true;
            string label;

            if (n->nthChild(3) != NULL) {
                label = n->nthChild(3)->sval;
            } else {
                label = "__REPEAT_" + to_string(loopLabelCount++);
                Node *labelNode = MKN0(N_ID);
                labelNode->sval = strdup(label.c_str());
                APC(n, labelNode);
            }
            if (loopLabelTable.find(label) != loopLabelTable.end()) {
                sprintf(tmpStr, "Duplicated loop label: %s", label.c_str());
                log.error(n->loc, tmpStr);
                return false;
            } else {
                loopLabelTable[label] = n;
                loopLabelStack.push(label);
            }
            noError &= semanticCheck(n->nthChild(1));
            loopLabelStack.pop();
            loopLabelTable.erase(label);

            noError &= semanticCheck(n->nthChild(2));

            Node *rule = n->nthChild(2);
            if (noError && rule->valueType->stdType != TYPE_BOOL) {
                log.error(n->loc, "Illegal condition expression");
                return false;
            }
            return noError;
        }
        case N_WHILE: {
            bool noError = true;
            string label;
            noError &= semanticCheck(n->nthChild(1));

            if (n->nthChild(3) != NULL) {
                label = n->nthChild(3)->sval;
            } else {
                label = "__WHILE_" + to_string(loopLabelCount++);
                Node *labelNode = MKN0(N_ID);
                labelNode->sval = strdup(label.c_str());
                APC(n, labelNode);
            }
            if (loopLabelTable.find(label) != loopLabelTable.end()) {
                sprintf(tmpStr, "Duplicated loop label: %s", label.c_str());
                log.error(n->loc, tmpStr);
                return false;
            } else {
                loopLabelTable[label] = n;
                loopLabelStack.push(label);
            }
            noError &= semanticCheck(n->nthChild(2));
            loopLabelStack.pop();
            loopLabelTable.erase(label);

            Node *rule = n->nthChild(1);
            if (noError && rule->valueType->stdType != TYPE_BOOL) {
                log.error(n->loc, "Illegal condition expression");
                return false;
            }
            return noError;
        }
        case N_IF_TERNARY:
        case N_IF: {
            bool noError = true;
            noError &= semanticCheck(n->nthChild(1));
            noError &= semanticCheck(n->nthChild(2));
            if (n->nthChild(3) != NULL) {
                noError &= semanticCheck(n->nthChild(3));
            }
            Node *rule = n->nthChild(1);
            if (noError && rule->valueType->stdType != TYPE_BOOL) {
                log.error(n->loc, "Illegal condition expression");
                return false;
            }

            if (n->nodeType == N_IF_TERNARY) {
                Node *c2 = n->nthChild(2);
                Node *c3 = n->nthChild(3);
                if (!typeEq(c2->valueType, c3->valueType)) {
                    log.error(n->loc, "Type mismatch for ternary if expression");
                    return false;
                }
                n->valueType = new TypeInfo(c2->valueType);
            }
            return noError;
        }
        case N_AND:
        case N_OR: {
            bool noError = true;
            Node *child = n->child;
            do {
                noError &= semanticCheck(child);
                child = child->rsibling;
            } while(child != n->child);

            if (!noError)
                return false;

            do {
                if (child->valueType->stdType != TYPE_BOOL) {
                    sprintf(tmpStr, "Operator %s only operated on relational expressions", n->nodeType == N_AND?"AND":"OR");
                    log.error(n->loc, tmpStr);
                    return false;
                }
                child = child->rsibling;
            } while(child != n->child);
            n->valueType = new TypeInfo(TYPE_BOOL);
            return true;
        }
        case N_TYPE_CONVERT: {
            bool noError = true;

            noError &= semanticCheck(n->child);

            if (n->op == N_TYPE_INT) {
                n->valueType = new TypeInfo(TYPE_INT);
            } else if (n->op == N_TYPE_REAL) {
                n->valueType = new TypeInfo(TYPE_REAL);
            } else if (n->op == N_TYPE_STRING) {
                n->valueType = new TypeInfo(TYPE_STRING);
            } else {
                log.error(n->loc, "Invalid type conversion");
                return false;
            }
            return noError;
        }
        case N_OP:
        case N_ASSIGN: {
            Node *child1 = n->nthChild(1);
            Node *child2 = n->nthChild(2);
            noError &= semanticCheck(child1);
            noError &= semanticCheck(child2);
            if (!noError) {
                return false;
            }

            if (n->nodeType == N_ASSIGN) {
                if (child1->entry->valueType->stdType == TYPE_FUNCTION) {
                    log.error(n->loc, "Argument cannot be assigned to");
                    return false;
                }
            } else {
                if (child1->valueType->stdType == TYPE_NULL || child2->valueType->stdType == TYPE_NULL) {
                    log.error(n->loc, "Procedure can not be operated");
                    return false;
                }
                if ((child1->valueType->stdType == TYPE_STRING || child2->valueType->stdType == TYPE_STRING)
                    && n->op != OP_ADD) {
                    log.error(n->loc, "String can not be operated");
                    return false;
                }
            }

            if (!typeEq(child1->valueType, child2->valueType)) {
                if(n->nodeType == N_OP) {
                    TypeInfo *tn = commonValueType(child1->valueType, child2->valueType);
                    if (tn == NULL) {
                        log.error(n->loc, "Type mismatch for operator");
                        return false;
                    } else if (tn->stdType == TYPE_CLASS) {
                        log.error(n->loc, "Class can not be operated");
                        return false;
                    }
                    insertTypeConvertParent(child1, tn);
                    insertTypeConvertParent(child2, tn);
                } else {
                    TypeInfo *tn = commonValueType(child1->valueType, child2->valueType);
                    if (tn == NULL || !typeEq(tn, child1->valueType)) {
                        log.error(n->loc, "Type mismatch for assignment");
                        return false;
                    }
                    insertTypeConvertParent(child2, tn);
                }
            }

            if (n->nodeType == N_OP && isConditionOp(n->op))
                n->valueType = new TypeInfo(TYPE_BOOL);
            else
                n->valueType = new TypeInfo(n->nthChild(1)->valueType);

            return noError;
        }
    }

    Node *child = n->child;
    if(child != NULL) {
        do {
            noError &= semanticCheck(child);
            child = child->rsibling;
        } while(child != n->child);
    }
    return noError;
}

void SymTable::dumpScope(int x) {
    printf("#### Scope %3d\n", x);
    const auto &vars = scopeVars[x];

    if (vars.size() != 0) {
        printf("%15s\t%15s\t%3s\t%s\n", "ID", "Type", "Scope", "Remark");
        for (size_t i = 0; i < vars.size(); i++) {
            SymTableEntry *p = vars[i];
            printf("%d ", p->varId);
            if ( p->valueType->stdType == TYPE_FUNCTION) {
                if (p->funcInfo->returnType.stdType == TYPE_NULL) {
                    printf("%15s\t%15s\t%3d\n", p->name.c_str(), "TYPE_PROCEDURE", x);
                } else {
                    sprintf(tmpStr, "Return type: %s", p->funcInfo->returnType.stdTypeName().c_str());
                    printf("%15s\t%15s\t%3d\t%s\n", p->name.c_str(), p->valueType->stdTypeName().c_str(), x, tmpStr);
                }
            }
            else {
                printf("%15s\t%15s\t%3d\n", p->name.c_str(), p->valueType->stdTypeName().c_str(), x);
            }
        }
    }
    printf("---- END: Scope %3d\n", x);
}

void SymTable::dumpAll() {
    printf("*******************************************************\n");
    printf("*                                                     *\n");
    printf("*                  Dump all variables                 *\n");
    printf("*                                                     *\n");
    printf("*******************************************************\n");
    for (auto it = scopeVars.begin(); it != scopeVars.end(); it++) {
        dumpScope(it->first);
    }
}
