#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include <stdlib.h>
#include "symbolTable.h"
#include "syntaxTree.h"

/*functions for semantic checking*/
void Program(Node* root);
void ExtDefList(Node* node);
void ExtDef(Node* node);

Type* Specifier(Node* node);
Type* StructSpecifier(Node* node);

void ExtDecList(Node* node, Type* varType);

FieldList* VarDec(Node* node, Type* varType, int from);
FieldList* DefList(Node* node, int from);
FuncDef* FunDec(Node* node, Type* rtn);
FieldList* VarList(Node* node);
FieldList* ParamDec(Node* node);
Type* CompSt(Node* node, Type* rtnType);

Type* StmtList(Node* node, Type* rtnType);
Type* Stmt(Node* node, Type* rtnType);

//when from equals 1, we should create 2 variable nodes, one for varTable, the another for structure type.
FieldList* DefList(Node* node, int from);
FieldList* Def(Node* node, int from);
FieldList* DecList(Node* node, Type* varType, int from);
FieldList* Dec(Node* node, Type* varType, int from);

Type* Exp(Node* node);
bool Args(Node* node, FieldList* param);

//......

#endif
