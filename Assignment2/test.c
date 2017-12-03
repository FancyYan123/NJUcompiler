#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "symbolTable.h"
#include <string.h>

FieldList* createBasicNode(char* name, int basic){
	FieldList* node = malloc(sizeof(FieldList));
	node->type = malloc(sizeof(Type));
//	printf("%s\n", name);
	strcpy(node->name, name);
//	printf("here1\n");
	node->type->kind = BASIC;
	node->type->u.basic = basic;
	return node;	
}

int main(){
	initVarTable();

	FieldList* node1 = createBasicNode("aa", 1);
	FieldList* node2 = createBasicNode("ab", 1);	
	FieldList* node3 = createBasicNode("aa", 1);
	FieldList* node4 = createBasicNode("ab", 0);

	insertVarTable(node1);
	insertVarTable(node2);
	insertVarTable(node3);
	insertVarTable(node4);

	FieldList* temp = findVar(node1->name);
	FieldList* temp2 = findVar(node2->name);
	
	printFieldList(temp);
	printf("\n");
	printFieldList(temp2);
	printf("\n");
	printVarTable();
	return 0;
}
