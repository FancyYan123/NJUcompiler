#ifndef __SYNTAXTREE__
#define __SYNTAXTREE__

#include<stdio.h>

typedef struct Node_t{
	/*TODO: add defined more data here.*/

	/*type value*/
	char value[32];

	/*type name*/
	char type[30];

	/*token's position*/
	int line;

	/*left child right sibling tree structure*/
	struct Node_t* Child;
	struct Node_t* Sibling;
}Node;

extern int yylineno;

Node* root;

Node* initNode(char* typeName, char* valueStr);
Node* getFirstChild(Node* parent);
Node* getSibling(Node* bro);
void insert(Node* parent, Node* child);
void traversal(Node* root, int space);

#endif
