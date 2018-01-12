// #include "syntaxTree.h"
// #include <stdlib.h>
// #include <assert.h>
// #include <string.h>

#include "common.h"


Node* getFirstChild(Node* parent){
	if(parent==NULL)
		return NULL;

	return parent->Child;
}

Node* getSibling(Node* bro){
	if(bro==NULL)
		return NULL;

	return bro->Sibling;
}

void insert(Node* parent, Node* child){
	assert(parent!=NULL);
	
	if(child==NULL)
		return;

	//set the line of parent's according to the child(always choose the nearest)
	if(parent->line==-1)
		parent->line = child->line;

	Node* temp = getFirstChild(parent);
	
	if(temp == NULL)
		parent->Child = child;

	else{
		while(temp->Sibling != NULL)
			temp = temp->Sibling;

		temp->Sibling = child;
	}
}

Node* initNode(char* typeName, char* valueStr) {
	Node *temp = malloc(sizeof(Node));
	
	temp->Child = NULL;
	temp->Sibling = NULL;

	strcpy(temp->value, valueStr);
	strcpy(temp->type, typeName);
	
	//if the node is a non-terminal
	if(strlen(valueStr)==0)
		temp->line = -1;
	else
		temp->line = yylineno;

	return temp;
}

/*TODO: add travel function here to visit every node in syntax tree.*/

void traversal(Node* root, int space){
	int i;
	if(root!=NULL){
		//actions before traversal:
		for(i=0; i<space; i++)
			printf("**");
		if(strlen(root->value)==0)
			printf("%s (%d)\n", root->type, root->line);
		else
			printf("%s: %s\n", root->type, root->value);
		
		traversal(root->Child, space+1);
		traversal(root->Sibling, space);
	}
}
