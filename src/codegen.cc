#include "codegen.h"
#include "node.h"
#include "symtab.h"
#include "logger.h"
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <list>
#include <map>
#include <unordered_map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <boost/filesystem.hpp>
using namespace std;
namespace fs = boost::filesystem;

#define printf(...) fprintf (src, __VA_ARGS__)

extern fs::path tmpWorkSpace;
extern int globalLogLevel;

FILE* src;
static char tmpStr[256];
static Logger logger("[Codegen]", &globalLogLevel);

void CodeGen::wHeader () {
    printf(".class public %s\n", javaClassName.c_str());
    printf(".super java/lang/Object\n");
    printf("\n");
}
void CodeGen::wTail () {
    // keep
}

void CodeGen::wConst (int n) {
    static const int mask0 = 0;
    static const int mask8 = 0xFF;
    static const int mask16 = 0xFFFF;
    static const int mask32 = 0xFFFFFFFF;

    if (n <= 127 && n >= -128)
        printf("bipush %d\n", n);
    else if (n <= 32767 && n >= -32768)
        printf("sipush %d\n", n);
    else
        printf("ldc %d\n", n);
}

void CodeGen::wConst (double n) {
    printf("ldc %.5f\n", n);
}

void CodeGen::wConst (string s) {
    printf("ldc \"%s\"\n", s.c_str());
}

void CodeGen::wTypeConvert (Node *n) {
    wBody(n->child);
    switch (n->valueType->stdType) {
        case TYPE_INT:
            switch (n->child->valueType->stdType) {
                case TYPE_REAL:
                    printf("f2i\n");
                    return;
                case TYPE_STRING:
                    printf("invokestatic java/lang/Double/parseDouble(Ljava/lang/String;)D\n");
                    printf("d2i\n");
                    return;
            }
            return;
        case TYPE_REAL:
            switch (n->child->valueType->stdType) {
                case TYPE_INT:
                    printf("i2f\n");
                    return;
                case TYPE_STRING:
                    printf("invokestatic java/lang/Float/parseFloat(Ljava/lang/String;)F\n");
                    return;
            }
            return;
        case TYPE_STRING:
            switch (n->child->valueType->stdType) {
                case TYPE_INT:
                    printf("invokestatic java/lang/String/valueOf(I)Ljava/lang/String;\n");
                    return;
                case TYPE_REAL:
                    printf("invokestatic java/lang/String/valueOf(F)Ljava/lang/String;\n");
                    return;
            }
            return;
    }
}

void CodeGen::wVarStore (Node *n1, Node *n2, bool writeN2) {
    wVarLoad(n1);
    if (n1->classEntry != NULL) {
        if (writeN2)
            wBody(n2);
        if (n1->memberEntry->valueType->stdType == TYPE_ARRAY && n1->child != NULL) {
            switch (n2->valueType->stdType) {
                case TYPE_INT:
                    printf("iastore\n");
                    break;
                case TYPE_REAL:
                    printf("fastore\n");
                    break;
                case TYPE_STRING:
                    printf("aastore\n");
                    break;
                case TYPE_ARRAY:
                    printf("aastore\n");
                    break;
            }
        } else {
            printf("putfield %s/%s %s\n", n1->classEntry->valueType->className.c_str(),
                n1->memberEntry->name.c_str(), n1->memberEntry->valueType->jType().c_str());
        }
    } else if (n1->entry->valueType->stdType == TYPE_ARRAY && n1->child != NULL) {
        if (writeN2)
            wBody(n2);
        switch (n2->valueType->stdType) {
            case TYPE_INT:
                printf("iastore\n");
                break;
            case TYPE_REAL:
                printf("fastore\n");
                break;
            case TYPE_STRING:
                printf("aastore\n");
                break;
            case TYPE_ARRAY:
                printf("aastore\n");
                break;
        }
    } else if (isClass) {
        switch (n1->entry->valueType->stdType) {
            case TYPE_INT:
                if (n1->entry->scopeId == 0) {
                    printf("aload_0\n");
                    if (writeN2)
                        wBody(n2);
                    printf("putfield %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str());
                } else {
                    if (writeN2)
                        wBody(n2);
                    printf("istore %d\n", n1->entry->varId);
                }
                break;
            case TYPE_REAL:
                if (n1->entry->scopeId == 0) {
                    printf("aload_0\n");
                    if (writeN2)
                        wBody(n2);
                    printf("putfield %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str());
                } else {
                    if (writeN2)
                        wBody(n2);
                    printf("fstore %d\n", n1->entry->varId);
                }
                break;
            case TYPE_STRING:
                if (n1->entry->scopeId == 0) {
                    printf("aload_0\n");
                    if (writeN2)
                        wBody(n2);
                    printf("putfield %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str());
                } else {
                    if (writeN2)
                        wBody(n2);
                    printf("astore %d\n", n1->entry->varId);
                }
                break;
            case TYPE_ARRAY:
                if (n1->entry->scopeId == 0) {
                    printf("aload_0\n");
                    if (writeN2)
                        wBody(n2);
                    printf("putfield %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str());
                } else {
                    if (writeN2)
                        wBody(n2);
                    printf("astore %d\n", n1->entry->varId);
                }
                break;
        }
    } else {
        if (writeN2)
            wBody(n2);
        switch (n1->entry->valueType->stdType) {
            case TYPE_INT:
                n1->entry->scopeId == 0
                    ? printf("putstatic %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str())
                    : printf("istore %d\n", n1->entry->varId);
                break;
            case TYPE_REAL:
                n1->entry->scopeId == 0
                    ? printf("putstatic %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str())
                    : printf("fstore %d\n", n1->entry->varId);
                break;
            case TYPE_STRING:
                n1->entry->scopeId == 0
                    ? printf("putstatic %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str())
                    : printf("astore %d\n", n1->entry->varId);
                break;
            case TYPE_ARRAY:
            case TYPE_CLASS:
                n1->entry->scopeId == 0
                    ? printf("putstatic %s/%s %s\n", javaClassName.c_str(), n1->entry->name.c_str(), n1->entry->valueType->jType().c_str())
                    : printf("astore %d\n", n1->entry->varId);
                break;
        }
    }
}

void CodeGen::wVarLoad (Node *n) {
    string base, id;
    if (n->nodeType != N_VAR_REF) {
        if (isClass) {
            switch (n->entry->valueType->stdType) {
                case TYPE_INT:
                    if (n->entry->scopeId == 0) {
                        printf("aload_0\n");
                        printf("getfield %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                    } else {
                        printf("iload %d\n", n->entry->varId);
                    }
                    break;
                case TYPE_REAL:
                    if (n->entry->scopeId == 0) {
                        printf("aload_0\n");
                        printf("getfield %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                    } else {
                        printf("fload %d\n", n->entry->varId);
                    }
                    break;
                case TYPE_STRING:
                    if (n->entry->scopeId == 0) {
                        printf("aload_0\n");
                        printf("getfield %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                    } else {
                        printf("aload %d\n", n->entry->varId);
                    }
                    break;
                case TYPE_ARRAY: {
                    if (n->entry->scopeId == 0) {
                        printf("aload_0\n");
                        printf("getfield %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                    } else {
                        printf("aload %d\n", n->entry->varId);
                    }

                    const ArrayInfo* arrInf = n->entry->valueType->arrayInfo;
                    auto sidxIt = arrInf->sidx.begin();
                    Node *idxNode = n->child;
                    do {
                        if (idxNode == NULL)
                            break;
                        wBody(idxNode);
                        wConst(*sidxIt);
                        printf("isub\n");
                        idxNode = idxNode->rsibling;
                        sidxIt++;
                        if (sidxIt != arrInf->sidx.end())
                            printf("aaload\n");
                    } while(idxNode != n->child);
                    if (idxNode != NULL && sidxIt == arrInf->sidx.end()) {
                        switch (arrInf->baseType.stdType){
                            case TYPE_INT:
                                printf("iaload\n");
                                break;
                            case TYPE_REAL:
                                printf("faload\n");
                                break;
                            case TYPE_STRING:
                                printf("aaload\n");
                                break;
                        }
                    }
                    break;
                }
                case TYPE_FUNCTION:

                    printf("aload_0\n");
                    if (n->child != NULL)
                        wBody(n->child);
                    printf("invokevirtual %s/%s(", javaClassName.c_str(), n->entry->name.c_str());
                    for (size_t i = 0; i < n->entry->funcInfo->params.size(); i++){
                        TypeInfo t = n->entry->funcInfo->params[i].second;
                        printf("%s", t.jType().c_str());
                    }
                    printf(")%s\n", n->entry->funcInfo->returnType.jType().c_str());
                    break;
            }
        } else {
            // Program
            switch (n->entry->valueType->stdType) {
                case TYPE_INT:
                    if (n->classEntry != NULL) {
                        if (n->classEntry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                             n->classEntry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->classEntry->varId);
                        }
                        printf("getfield %s/%s %s\n", n->classEntry->valueType->className.c_str(),
                            n->memberEntry->name.c_str(), n->memberEntry->valueType->jType().c_str());
                    } else {
                        if (n->entry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                        } else {
                            printf("iload %d\n", n->entry->varId);
                        }
                    }
                    break;
                case TYPE_REAL:
                    if (n->classEntry != NULL) {
                        if (n->classEntry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                             n->classEntry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->classEntry->varId);
                        }
                        printf("getfield %s/%s %s\n", n->classEntry->valueType->className.c_str(),
                            n->memberEntry->name.c_str(), n->memberEntry->valueType->jType().c_str());
                    } else {
                        if (n->entry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                        } else {
                            printf("fload %d\n", n->entry->varId);
                        }
                    }
                    break;
                case TYPE_STRING:
                    if (n->classEntry != NULL) {
                        if (n->classEntry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                             n->classEntry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->classEntry->varId);
                        }
                        printf("getfield %s/%s %s\n", n->classEntry->valueType->className.c_str(),
                            n->memberEntry->name.c_str(), n->memberEntry->valueType->jType().c_str());
                    } else {
                        if (n->entry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->entry->varId);
                        }
                    }
                    break;
                case TYPE_ARRAY: {
                    if (n->classEntry != NULL) {
                        if (n->classEntry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                             n->classEntry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->classEntry->varId);
                        }
                        printf("getfield %s/%s %s\n", n->classEntry->valueType->className.c_str(),
                            n->memberEntry->name.c_str(), n->memberEntry->valueType->jType().c_str());
                    } else {
                        if (n->entry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->entry->varId);
                        }
                    }

                    const ArrayInfo* arrInf = n->entry->valueType->arrayInfo;
                    auto sidxIt = arrInf->sidx.begin();
                    Node *idxNode = n->child;
                    do {
                        if (idxNode == NULL)
                            break;
                        wBody(idxNode);
                        wConst(*sidxIt);
                        printf("isub\n");
                        idxNode = idxNode->rsibling;
                        sidxIt++;
                        if (sidxIt != arrInf->sidx.end())
                            printf("aaload\n");
                    } while(idxNode != n->child);
                    if (idxNode != NULL && sidxIt == arrInf->sidx.end()) {
                        switch (arrInf->baseType.stdType){
                            case TYPE_INT:
                                printf("iaload\n");
                                break;
                            case TYPE_REAL:
                                printf("faload\n");
                                break;
                            case TYPE_STRING:
                                printf("aaload\n");
                                break;
                        }
                    }
                    break;
                }
                case TYPE_CLASS: {
                    if (n->classEntry != NULL) {
                        if (n->classEntry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                             n->classEntry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->classEntry->varId);
                        }
                        printf("getfield %s/%s %s\n", n->classEntry->valueType->className.c_str(),
                            n->memberEntry->name.c_str(), n->memberEntry->valueType->jType().c_str());
                    } else {
                        if (n->entry->scopeId == 0) {
                            printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str());
                        } else {
                            printf("aload %d\n", n->entry->varId);
                        }
                    }
                    break;
                }
                case TYPE_FUNCTION:
                    if (n->classEntry != NULL) {
                        if (n->classEntry->valueType->stdType == TYPE_MODULE) {
                            if (n->child != NULL)
                                wBody(n->child);

                            printf("invokestatic %s/%s(", n->classEntry->name.c_str(),
                                n->memberEntry->name.c_str());
                        }  else {
                            if (n->classEntry->scopeId == 0) {
                                printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                                 n->classEntry->valueType->jType().c_str());
                            } else {
                                printf("aload %d\n", n->classEntry->varId);
                            }

                            if (n->child != NULL)
                                wBody(n->child);
                            printf("invokevirtual %s/%s(", n->classEntry->valueType->className.c_str(),
                                n->memberEntry->name.c_str());
                        }
                    } else {
                        if (n->child != NULL)
                            wBody(n->child);
                        printf("invokestatic %s/%s(", javaClassName.c_str(), n->entry->name.c_str());
                    }
                    for (size_t i = 0; i < n->entry->funcInfo->params.size(); i++){
                        TypeInfo t = n->entry->funcInfo->params[i].second;
                        printf("%s", t.jType().c_str());
                    }

                    if (n->classEntry != NULL) {
                        printf(")%s\n", n->memberEntry->funcInfo->returnType.jType().c_str());
                    } else {
                        printf(")%s\n", n->entry->funcInfo->returnType.jType().c_str());
                    }
                    break;
            }
        }

    // end if (n->nodeType != N_VAR_REF)
    } else {
        // if (n->nodeType == N_VAR_REF)
        if (n->classEntry != NULL) {
            if (n->classEntry->scopeId == 0) {
                printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                 n->classEntry->valueType->jType().c_str());
            } else {
                printf("aload %d\n", n->classEntry->varId);
            }

            if (n->memberEntry->valueType->stdType == TYPE_ARRAY && n->child != NULL) {
                printf("getfield %s/%s %s\n", n->classEntry->valueType->className.c_str(),
                    n->memberEntry->name.c_str(), n->memberEntry->valueType->jType().c_str());

                const ArrayInfo* arrInf = n->memberEntry->valueType->arrayInfo;
                auto sidxIt = arrInf->sidx.begin();
                Node *idxNode = n->child;
                do {
                    if (sidxIt != arrInf->sidx.begin())
                        printf("aaload\n");
                    if (idxNode == NULL)
                        break;
                    wBody(idxNode);
                    wConst(*sidxIt);
                    printf("isub\n");
                    idxNode = idxNode->rsibling;
                    sidxIt++;
                } while(idxNode != n->child);
            }
        } else if (n->entry->valueType->stdType == TYPE_ARRAY && n->child != NULL) {
            n->entry->scopeId == 0
                ? printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->entry->name.c_str(), n->entry->valueType->jType().c_str())
                : printf("aload %d\n", n->entry->varId);

            // if (isClass) {
            //     n->entry->scopeId == 0
            //         ? printf("getfield %s/%s %s\n", javaClassName.c_str(), n->className.c_str(), n->entry->valueType->jType().c_str())
            //         : printf("aload %d\n", n->entry->varId);
            // } else {
            //     if (n->classEntry != NULL) {
            //         printf("getfield %s/%s %s\n", javaClassName.c_str(), n->className.c_str(), n->entry->valueType->jType().c_str())
            //     } else {
                    // if (n->classEntry->scopeId == 0) {
                    //     printf("getstatic %s/%s %s\n", javaClassName.c_str(), n->classEntry->name.c_str(),
                    //      n->classEntry->valueType->jType().c_str());
                    // } else {
                    //     printf("aload %d\n", n->entry->varId);
                    // }
                // }
            // }
            const ArrayInfo* arrInf = n->entry->valueType->arrayInfo;
            auto sidxIt = arrInf->sidx.begin();
            Node *idxNode = n->child;
            do {
                if (sidxIt != arrInf->sidx.begin())
                    printf("aaload\n");
                if (idxNode == NULL)
                    break;
                wBody(idxNode);
                wConst(*sidxIt);
                printf("isub\n");
                idxNode = idxNode->rsibling;
                sidxIt++;
            } while(idxNode != n->child);
        }
    }
}

void CodeGen::wLabel(int n) {
    printf("L%d:\n", n);
}

void CodeGen::wJmpLabel(int n) {
    printf("goto L%d\n", n);
}

void CodeGen::wFunctionWriteln () {
    printf(".method public static writeln(I)V\n");
    printf("iload 0");
	printf("invokestatic java/lang/String/valueOf(I)Ljava/lang/String;\n");
	printf("invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n");
    printf("return");
    printf(".end method\n");
    printf("\n");
    printf(".method public static writeln(F)V\n");
    printf("fload 0");
	printf("invokestatic java/lang/String/valueOf(F)Ljava/lang/String;\n");
	printf("invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n");
    printf("return");
    printf(".end method\n");
    printf("\n");
}

void CodeGen::wJMethodLimits (int locals, int stacks) {
    printf(".limit locals %d\n", locals);
    printf(".limit stack %d\n", stacks);
}

void CodeGen::wProgramVars (Node *n) {
    if (n->nodeType == N_ID) {
        SymTableEntry* p = n->entry;
        switch (p->valueType->stdType) {
            case TYPE_INT:
            case TYPE_REAL:
            case TYPE_ARRAY:
            case TYPE_STRING:
            case TYPE_CLASS:
                printf(".field public static %s %s\n", p->name.c_str(), p->valueType->jType().c_str());
                break;
        }
    } else {
        TRV_CHILDREN(n, wProgramVars);
    }
}

void CodeGen::wProgramVarInit(Node *n) {
    if (n->nodeType == N_ID) {
        SymTableEntry* p = n->entry;
        switch (p->valueType->stdType) {
            case TYPE_INT:
                wConst(0);
                break;
            case TYPE_REAL:
                wConst(0.0);
                break;
            case TYPE_STRING:
                wConst("");
                break;
            case TYPE_ARRAY: {
                auto &dims = p->valueType->arrayInfo->dim;
                for (auto d = dims.begin(); d != dims.end(); d++)
                    wConst(*d);
                printf("multianewarray %s %d\n", p->valueType->jType().c_str(), int(dims.size()));
                break;
            }
            case TYPE_CLASS:
                printf("new %s\n", p->valueType->className.c_str());
                printf("dup\n");
                printf("invokespecial %s/<init>()V\n", p->valueType->className.c_str());
        }

        switch (p->valueType->stdType) {
            case TYPE_INT:
            case TYPE_REAL:
            case TYPE_STRING:
            case TYPE_ARRAY:
            case TYPE_CLASS:
                printf("putstatic %s/%s %s\n", javaClassName.c_str(), p->name.c_str(), p->valueType->jType().c_str());
                break;
        }
    } else {
        TRV_CHILDREN(n, wProgramVarInit);
    }
}

void CodeGen::wJStandardInit() {
    printf(".method public <init>()V\n");
	printf("aload_0\n");
	printf("invokenonvirtual java/lang/Object/<init>()V\n");
	printf("return\n");
    printf(".end method\n");
    printf("\n");
}

void CodeGen::dump () {
    src = fopen((tmpWorkSpace/fs::path(javaClassName+".j")).generic_string().c_str(), "w");
    if (ast->nodeType == N_MODULE || ast->nodeType == N_CLASS) {
        wModuleClass(ast);
    } else {
        wProgram(ast);
    }
    fflush(src);
    fclose(src);
}

void CodeGen::wOp(Node *n) {
    Node *n1 = n->nthChild(1);
    Node *n2 = n->nthChild(2);
    switch (n->op) {
        case OP_ADD: {
            switch(n1->valueType->stdType) {
                case TYPE_INT:
                    wBody(n1);
                    wBody(n2);
                    printf("iadd\n");
                    break;
                case TYPE_REAL:
                    wBody(n1);
                    wBody(n2);
                    printf("fadd\n");
                    break;
                case TYPE_STRING:
                    printf("new java/lang/StringBuilder\n");
                    printf("dup\n");
                    printf("invokespecial java/lang/StringBuilder/<init>()V\n");
                    wBody(n1);
                    printf("invokevirtual java/lang/StringBuilder/append(Ljava/lang/String;)Ljava/lang/StringBuilder;\n");
                    wBody(n2);
                    printf("invokevirtual java/lang/StringBuilder/append(Ljava/lang/String;)Ljava/lang/StringBuilder;\n");
                    printf("invokevirtual java/lang/StringBuilder/toString()Ljava/lang/String;\n");
                    break;
            }
            break;
        }
        case OP_SUB: {
            switch(n1->valueType->stdType) {
                case TYPE_INT:
                    wBody(n1);
                    wBody(n2);
                    printf("isub\n");
                    break;
                case TYPE_REAL:
                    wBody(n1);
                    wBody(n2);
                    printf("fsub\n");
                    break;
            }
            break;
        }
        case OP_MUL: {
            switch(n1->valueType->stdType) {
                case TYPE_INT:
                    wBody(n1);
                    wBody(n2);
                    printf("imul\n");
                    break;
                case TYPE_REAL:
                    wBody(n1);
                    wBody(n2);
                    printf("fmul\n");
                    break;
            }
            break;
        }
        case OP_DIV: {
            switch(n1->valueType->stdType) {
                case TYPE_INT:
                    wBody(n1);
                    wBody(n2);
                    printf("idiv\n");
                    break;
                case TYPE_REAL:
                    wBody(n1);
                    wBody(n2);
                    printf("fdiv\n");
                    break;
            }
            break;
        }
    }
}

void CodeGen::wIfOp(Node *n, int l) {
    Node *n1 = n->nthChild(1);
    Node *n2 = n->nthChild(2);
    wBody(n1);
    wBody(n2);
    switch (n1->valueType->stdType) {
        case TYPE_INT: {
            switch (n->op) {
                case OP_GT:
                    printf("if_icmpgt L%d\n", l);
                    break;
                case OP_LT:
                    printf("if_icmplt L%d\n", l);
                    break;
                case OP_EQ:
                    printf("if_icmpeq L%d\n", l);
                    break;
                case OP_GE:
                    printf("if_icmpge L%d\n", l);
                    break;
                case OP_LE:
                    printf("if_icmple L%d\n", l);
                    break;
                case OP_NE:
                    printf("if_icmpne L%d\n", l);
                    break;
            }
            break;
        }
        case TYPE_REAL: {
            switch (n->op) {
                case OP_GT:
                    printf("fcmpl\n");
                    printf("ifgt L%d\n", l);
                    break;
                case OP_LT:
                    printf("fcmpl\n");
                    printf("iflt L%d\n", l);
                    break;
                case OP_EQ:
                    printf("fcmpl\n");
                    printf("ifeq L%d\n", l);
                    break;
                case OP_GE:
                    printf("fcmpl\n");
                    printf("ifge L%d\n", l);
                    break;
                case OP_LE:
                    printf("fcmpl\n");
                    printf("ifle L%d\n", l);
                    break;
                case OP_NE:
                    printf("fcmpl\n");
                    printf("ifne L%d\n", l);
                    break;
            }
            break;
        }
    }
}

void CodeGen::wIfNotOp(Node *n, int l) {
    char op = n->op;
    switch (n->op) {
        case OP_GT:
            n->op = OP_LE;
            break;
        case OP_LT:
            n->op = OP_GE;
            break;
        case OP_EQ:
            n->op = OP_NE;
            break;
        case OP_GE:
            n->op = OP_LT;
            break;
        case OP_LE:
            n->op = OP_GT;
            break;
        case OP_NE:
            n->op = OP_EQ;
            break;
    }
    wIfOp(n, l);
    n->op = op;
}

void CodeGen::wSign(Node *n) {
    wBody(n->nthChild(1));
    switch (n->op) {
        case OP_SUB:
            switch (n->nthChild(1)->valueType->stdType) {
                case TYPE_INT:
                    wConst(-1);
                    printf("imul\n");
                    break;
                case TYPE_REAL:
                    wConst(-1.0);
                    printf("fmul\n");
                    break;
            }
            break;
        case OP_ADD:
            break;
    }
}

void CodeGen::wAssign(Node *n) {
    switch (n->op) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            n->nthChild(1)->nodeType = N_VAR;
            n->nodeType = N_OP;
            wOp(n);
            n->nthChild(1)->nodeType = N_VAR_REF;
            n->nodeType = N_ASSIGN;
            wVarStore(n->nthChild(1), n->nthChild(2), false);
            break;
        case OP_NULL:
            wVarStore(n->nthChild(1), n->nthChild(2));
            break;
    }

}

void CodeGen::wBody (Node *n) {
    switch (n->nodeType) {
        case N_ASSIGN:
            wAssign(n);
            break;
        case N_OP:
            wOp(n);
            break;
        case N_SIGN:
            wSign(n);
            break;
        case N_VAR:
        case N_VAR_OR_SUBPROGRAM:
        case N_VAR_REF:
        case N_SUBPROGRAM:
            wVarLoad(n);
            break;
        case N_INT:
            wConst(n->ival);
            break;
        case N_REAL:
            wConst(n->rval);
            break;
        case N_STRING:
            wConst(n->sval);
            break;
        case N_IF_TERNARY:
        case N_IF:
            wIf(n);
            break;
        case N_CONTINUE:
            wJmpLabel(continueLabelTable[n->child->sval]);
            break;
        case N_BREAK:
            wJmpLabel(breakLabelTable[n->child->sval]);
            break;
        case N_WHILE:
            wWhile(n);
            break;
        case N_REPEAT:
            wRepeat(n);
            break;
        case N_FOR:
            wFor(n);
            break;
        case N_RETURN:
            wReturn(n);
            break;
        case N_TYPE_CONVERT:
            wTypeConvert(n);
            break;
        default:
            TRV_CHILDREN(n, wBody);
    }
}

void CodeGen::wCondition (Node *n, int lSuccess, int lFailed) {
    switch (n->nodeType) {
        case N_NOT:
            reNot = !reNot;
            wCondition(n->child, lSuccess, lFailed);
            reNot = !reNot;
            break;
        case N_OR:
            if (reNot)
                wAnd(n, lSuccess, lFailed);
            else
                wOr(n, lSuccess, lFailed);
            break;
        case N_AND:
            if (reNot)
                wOr(n, lSuccess, lFailed);
            else
                wAnd(n, lSuccess, lFailed);
            break;
        case N_OP:
            if (lSuccess < lFailed) {
                if (reNot)
                    wIfNotOp(n, lSuccess);
                else
                    wIfOp(n, lSuccess);
            } else {
                if (reNot)
                    wIfOp(n, lFailed);
                else
                    wIfNotOp(n, lFailed);
            }
            break;
    }
}

void CodeGen::wOr(Node *n, int ls, int lf) {
    Node *c = n->child;
    do {
        int l = getLabel();
        wCondition(c, ls, l);
        wLabel(l);
        c = c->rsibling;
    } while (c != n->child);
    wJmpLabel(lf);
}

void CodeGen::wAnd(Node *n, int ls, int lf) {
    Node *c = n->child;
    do {
        int l = getLabel();
        wCondition(c, l, lf);
        wLabel(l);
        c = c->rsibling;
    } while (c != n->child);
    wJmpLabel(ls);
}

void CodeGen::wIf(Node *n) {
    int lf = getLabel();
    int ls = getLabel();
    int le = getLabel();
    wCondition(n->nthChild(1), ls, lf);
    if (n->nthChild(3) != NULL) {
        wLabel(ls);
        wBody(n->nthChild(2));
        wJmpLabel(le);
        wLabel(lf);
        wBody(n->nthChild(3));
        wLabel(le);
    } else {
        wLabel(ls);
        wBody(n->nthChild(2));
        wLabel(lf);
    }
}

void CodeGen::wWhile(Node *n) {
    int lf = getLabel();
    int ls = getLabel();
    int lb = getLabel();
    string label = n->nthChild(3)->sval;

    continueLabelTable[label] = lb;
    breakLabelTable[label] = lf;
    wLabel(lb);
    wCondition(n->nthChild(1), ls, lf);
    wLabel(ls);
    wBody(n->nthChild(2));
    wJmpLabel(lb);
    wLabel(lf);
    continueLabelTable.erase(label);
    breakLabelTable.erase(label);
}

void CodeGen::wRepeat(Node *n) {
    int lf = getLabel();
    int ls = getLabel();
    int lc = getLabel();
    string label = n->nthChild(3)->sval;

    continueLabelTable[label] = lc;
    breakLabelTable[label] = ls;
    wLabel(lf);
    wBody(n->nthChild(1));
    wLabel(lc);
    wCondition(n->nthChild(2), ls, lf);
    wLabel(ls);
    continueLabelTable.erase(label);
    breakLabelTable.erase(label);
}

void CodeGen::wFor(Node *n) {
    int l1 = getLabel(), l2 = getLabel();
    Node *idNode = n->nthChild(1)->nthChild(1);
    string label = n->nthChild(5)->sval;

    continueLabelTable[label] = l1;
    breakLabelTable[label] = l2;
    wBody(n->nthChild(1));
    wLabel(l1);
    idNode->entry->scopeId == 0
        ? printf("getstatic %s/%s %s\n", javaClassName.c_str(),
                idNode->entry->name.c_str(),
                idNode->entry->valueType->jType().c_str())
        : printf("iload %d\n", idNode->entry->varId);
    wBody(n->nthChild(2));

    if (n->nthChild(4)->op == OP_ADD) {
        printf("if_icmpgt L%d\n", l2);
    } else {
        printf("if_icmplt L%d\n", l2);
    }

    wBody(n->nthChild(3));
    if (idNode->entry->scopeId == 0) {
        printf("getstatic %s/%s %s\n", javaClassName.c_str(),
                idNode->entry->name.c_str(),
                idNode->entry->valueType->jType().c_str());
        wConst(1);
        if (n->nthChild(4)->op == OP_ADD) {
            printf("iadd\n");
        } else {
            printf("isub\n");
        }
        printf("putstatic %s/%s %s\n", javaClassName.c_str(),
                idNode->entry->name.c_str(),
                idNode->entry->valueType->jType().c_str());
    } else {
        if (n->nthChild(4)->op == OP_ADD) {
            printf("iinc %d 1\n", idNode->entry->varId);
        } else {
            printf("iinc %d -1\n", idNode->entry->varId);
        }
    }
    wJmpLabel(l1);
    wLabel(l2);
    continueLabelTable.erase(label);
    breakLabelTable.erase(label);
}

void CodeGen::wModule(Node *n) {
    if (n->nodeType == N_JFUNCTION_DECL || n->nodeType == N_SUBPROGRAM_DECL) {
        wFunction(n);
    } else if (n->nodeType == N_NULL) {
        return;
    } else {
        TRV_CHILDREN(n, wModule);
    }
}

void CodeGen::wReturn (Node *n) {
    wBody(n->child);
    switch (n->child->valueType->stdType) {
        case TYPE_INT:
            printf("ireturn\n");
            break;
        case TYPE_REAL:
            printf("freturn\n");
            break;
        case TYPE_STRING:
            printf("areturn\n");
            break;
        case TYPE_NULL:
            printf("return\n");
            break;
    }
}

void CodeGen::wFunction (Node *n) {
    if (isClass) {
        printf(".method public %s(", n->entry->name.c_str());
    } else {
        printf(".method public static %s(", n->entry->name.c_str());
    }
    for (size_t i = 0; i < n->entry->funcInfo->params.size(); i++){
        TypeInfo t = n->entry->funcInfo->params[i].second;
        printf("%s", t.jType().c_str());
    }
    printf(")%s\n", n->entry->funcInfo->returnType.jType().c_str());

    if (n->nodeType == N_JFUNCTION_DECL) {
        printf("%s\n", n->nthChild(2)->sval);
    } else {
        Node *head = n->nthChild(1);
        Node *vars = n->nthChild(2);
        Node *body = n->nthChild(3);
        wJMethodLimits();
        if (n->entry->funcInfo->returnType.stdType != TYPE_NULL)
            wFunctionInit(head->nthChild(1));
        wFunctionInit(vars);
        wBody(body);

        switch (n->entry->funcInfo->returnType.stdType) {
            case TYPE_INT:
                wVarLoad(head->nthChild(1));
                printf("ireturn\n");
                break;
            case TYPE_REAL:
                wVarLoad(head->nthChild(1));
                printf("freturn\n");
                break;
            case TYPE_STRING:
                wVarLoad(head->nthChild(1));
                printf("areturn\n");
                break;
            case TYPE_NULL:
                printf("return\n");
                break;
        }
    }

    printf(".end method\n");
    printf("\n");
}

void CodeGen::wFunctionInit(Node *n) {
    if (n->nodeType == N_ID) {
        SymTableEntry* p = n->entry;
        switch (p->valueType->stdType) {
            case TYPE_INT:
                wConst(0);
                break;
            case TYPE_REAL:
                wConst(0.0);
                break;
            case TYPE_STRING:
                wConst("");
                break;
            case TYPE_ARRAY: {
                auto &dims = p->valueType->arrayInfo->dim;
                for (auto d = dims.begin(); d != dims.end(); d++)
                    wConst(*d);
                printf("multianewarray %s %d\n", p->valueType->jType().c_str(), int(dims.size()));
                break;
            }
            case TYPE_CLASS:
                printf("new %s\n", p->valueType->className.c_str());
                printf("dup\n");
                printf("invokespecial %s/<init>()V\n", p->valueType->className.c_str());
                break;
        }

        switch (p->valueType->stdType) {
            case TYPE_INT:
                printf("istore %d\n", p->varId);
                break;
            case TYPE_REAL:
                printf("fstore %d\n", p->varId);
                break;
            case TYPE_STRING:
            case TYPE_ARRAY:
            case TYPE_CLASS:
                printf("astore %d\n", p->varId);
                break;
        }
    } else {
        TRV_CHILDREN(n, wFunctionInit);
    }
}

void CodeGen::wProgram(Node *n) {
    wHeader();
    wProgramVars(n->nthChild(5));
    printf("\n");

    printf(".method public static vinit()V\n");
    wJMethodLimits();
    wProgramVarInit(n->nthChild(5));
    printf("return\n");
    printf(".end method\n");
    printf("\n");

    wJStandardInit();

    //wModule(n->nthChild(3));
    for (size_t i = 0; i < symtab.importFuncNode.size(); i++) {
        wModule(symtab.importFuncNode[i]);
    }

    Node *funcs = n->nthChild(6);
    Node *func = funcs->child;
    if (func != NULL) {
        do {
            wFunction(func);
            func = func->rsibling;
        } while (func != funcs->child);
    }

    printf(".method public static main([Ljava/lang/String;)V\n");
    wJMethodLimits();
    printf("invokestatic %s/vinit()V\n", javaClassName.c_str());
    wBody(n->nthChild(7));
    printf("return\n");
    printf(".end method\n");
}

void CodeGen::wClassVars (Node *n) {
    if (n->nodeType == N_ID) {
        SymTableEntry* p = n->entry;
        switch (p->valueType->stdType) {
            case TYPE_INT:
            case TYPE_REAL:
            case TYPE_ARRAY:
            case TYPE_STRING:
                printf(".field public %s %s\n", p->name.c_str(), p->valueType->jType().c_str());
                break;
        }
    } else {
        TRV_CHILDREN(n, wClassVars);
    }
}

void CodeGen::wClassInit (Node *n) {
    wClassVars(n);

    printf(".method public <init>()V\n");
    wJMethodLimits();
	printf("aload_0\n");
	printf("invokenonvirtual java/lang/Object/<init>()V\n");
    wClassVarInit(n);
    printf("return\n");
    printf(".end method\n");
    printf("\n");
}

void CodeGen::wClassVarInit (Node *n) {
    if (n->nodeType == N_ID) {
        SymTableEntry* p = n->entry;
        printf("aload_0\n");
        switch (p->valueType->stdType) {
            case TYPE_INT:
                wConst(0);
                break;
            case TYPE_REAL:
                wConst(0.0);
                break;
            case TYPE_STRING:
                wConst("");
                break;
            case TYPE_ARRAY: {
                auto &dims = p->valueType->arrayInfo->dim;
                for (auto d = dims.begin(); d != dims.end(); d++)
                    wConst(*d);
                printf("multianewarray %s %d\n", p->valueType->jType().c_str(), int(dims.size()));
                break;
            }
            case TYPE_CLASS:
                printf("new %s\n", p->valueType->className.c_str());
                printf("dup\n");
                printf("invokespecial %s/<init>()V\n", p->valueType->className.c_str());
                break;
        }

        switch (p->valueType->stdType) {
            case TYPE_INT:
            case TYPE_REAL:
            case TYPE_STRING:
            case TYPE_ARRAY:
            case TYPE_CLASS:
                printf("putfield %s/%s %s\n", javaClassName.c_str(), p->name.c_str(), p->valueType->jType().c_str());
                break;
        }
    } else {
        TRV_CHILDREN(n, wClassVarInit);
    }
}

void CodeGen::wModuleClass(Node *n) {
    wHeader();

    if (n->nodeType == N_MODULE) {
        wJStandardInit();
        Node *funcs = n->nthChild(2);
        Node *func = funcs->child;
        if (func != NULL) {
            do {
                wFunction(func);
                func = func->rsibling;
            } while (func != funcs->child);
        }
    } else {
        wClassInit(n->nthChild(3));

        Node *funcs = n->nthChild(4);
        Node *func = funcs->child;
        if (func != NULL) {
            do {
                wFunction(func);
                func = func->rsibling;
            } while (func != funcs->child);
        }
    }

    // wProgramVars(n->nthChild(4));
    // printf("\n");

    // printf(".method public static vinit()V\n");
    // wJMethodLimits();
    // wProgramVarInit(n->nthChild(4));
    // printf("return\n");
    // printf(".end method\n");
    // printf("\n");

    // wModule(n->nthChild(3));

    // printf(".method public static main([Ljava/lang/String;)V\n");
    // wJMethodLimits();
    // printf("invokestatic %s/vinit()V\n", javaClassName.c_str());
    // wBody(n->nthChild(6));
    // printf("return\n");
    // printf(".end method\n");
}
