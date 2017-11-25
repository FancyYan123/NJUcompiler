#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "symbolTable.h"
#include <string.h>

FieldList* createBasicNode(char* name, int basic){
	FieldList* node = malloc(sizeof(FieldList));
	printf("%s\n", name);
	strcpy(node->name, name);
	printf("here1\n");
	node->type.kind = BASIC;
	node->type.u.basic = basic;
	return node;	
}

int main(){
	initVarTable();
	initStack();

	FieldList* node1 = createBasicNode("aa", 1);
	FieldList* node2 = createBasicNode("ab", 1);	
	printf("point0\n");
	FieldList* node3 = createBasicNode("aa", 1);
	printf("%s\n", node3->name);
	FieldList* node4 = createBasicNode("ab", 0);
	printf("%s\n", node4->name);

	insertVarTable(node1);
	insertVarTable(node2);
	printf("point1\n");
	push();
	insertVarTable(node3);
	insertVarTable(node4);
	printf("point2\n");

	FieldList* temp = findVar(node1->name);
	printf("%s\n", temp->name);
	FieldList* temp2 = findVar(node2->name);
	printf("%s\n", temp2->name);
	
	printf("before pop\n");
	pop();
	printf("after pop\n");
//	assert(node2==NULL);
	assert(findVar("aa")!=NULL);
	assert(findVar("ab")!=NULL);
	pop();
	assert(findVar("ab")==NULL);
	assert(findVar("aa")==NULL);

	return 0;
}
