#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int StackLevel = 0;
FuncDef* FuncList = NULL;
FieldList* VarTable[HASH_LENGTH];
FieldList* Stack[MAX_EMBEDDED];

void initVarTable(){
	int i;
	for(i=0; i<HASH_LENGTH; i++)
		VarTable[i]=NULL;
}

int calcHashIndex(char* str){
	int no=0, i=0;
	char ch;
	for(ch=str[i++]; ch!='\0'; ch=str[i++])
		no += (int)ch;

	no %= HASH_LENGTH;
	return no;
}

int insertVarTable(FieldList* node){
	if(node==NULL)
		return -1;
	
	int no = calcHashIndex(node->name);	
	
	//insert from head:
	FieldList* head=VarTable[no];
	VarTable[no]=node;
	if(head==NULL){
		head = node;
		head->pre = head->next = NULL;
	}
	else{
		head->pre=node;
		node->next=head;
		node->pre=NULL;
	}

	//maintain the stack information:
	insertStackTail(node);
}

bool compareFieldList(FieldList* node1, FieldList* node2){
	if(node1==NULL || node2==NULL)
		return false;
	if(strcmp(node1->name, node2->name)!=0)
		return false;
	if(node1->type->kind!=node2->type->kind)
		return false;	

	return true;
}

FieldList* findVar(char* varName){
	int no=calcHashIndex(varName);
	FieldList* index = VarTable[no];
	while(index!=NULL){
		if(strcmp(varName, index->name)==0)
			return index;
		index=index->next;
	}
	return NULL;
}


void insertFunc(FuncDef* func){
	assert(func!=NULL);

	if(FuncList==NULL){
		FuncList=func;
		func->next=NULL;
	}
	else{
		func->next = FuncList;
		FuncList = func;
	}
}

FuncDef* findFunc(char* FuncName){
	FuncDef* temp = FuncList;
	while(temp!=NULL){
		if(strcmp(temp->name, FuncName)==0)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

void initStack(){
	int i;
	for(i=0; i<MAX_EMBEDDED; i++){
		Stack[i]=NULL;
	}
}

void insertStackTail(FieldList* node){
	node->nextStack = NULL;
	FieldList* tail = getStackTail();
	if(tail==NULL)
		Stack[StackLevel]=node;
	else
		tail->nextStack = node;
}

FieldList* getStackTail(){
	FieldList* last=Stack[StackLevel];
	if(last==NULL)
		return NULL;
	while(last->nextStack!=NULL)
		last = last->nextStack;

	return last;
}

FieldList* push(){
//	assert(node!=NULL);
	
	assert(StackLevel < MAX_EMBEDDED);	
	StackLevel++;
//	Stack[StackLevel] = node;
//	node->nextStack = NULL;
}

void pop(){
	FieldList* temp=Stack[StackLevel];
	while(temp!=NULL){
		//if temp is the head node:
		if(temp->pre == NULL){
			int no = calcHashIndex(temp->name);
			VarTable[no]=temp->next;
			if(temp->next!=NULL){
				temp->next->pre = NULL;
			}
		}
		else{
			if(temp->next!=NULL)
				temp->next->pre = temp->pre;
			if(temp->pre!=NULL)
				temp->pre->next = temp->next;
		}

		FieldList* to_be_free = temp;
		temp = temp->nextStack;
		free(to_be_free->type);
		free(to_be_free);
	}
	if(StackLevel > 0)
		StackLevel--;
}

bool compareType(Type* node1, Type* node2){
	if(node1==NULL||node2==NULL)
		return false;
	if(node1->kind != node2->kind)
		return false;
	if(node1->kind==BASIC){
		if(node1->u.basic != node2->u.basic)
			return false;
	}
	else if(node1->kind==ARRAY){
		if(node1->u.array.size!=node2->u.array.size)
			return false;
		return compareType(node1->u.array.elem, node2->u.array.elem);
	}
	else{
		if(strcmp(node1->u.structure.name, node2->u.structure.name)!=0)
			return false;
	}
	return true;
}

void freeFieldList(FieldList* head){
	while(head!=NULL){
		FieldList* temp = head;
		head = head->next;
		free(temp);
	}
}

FieldList* getFieldListTail(FieldList* head){
	if(head==NULL)
		return NULL;
	FieldList* temp = head;
	while(temp->next!=NULL){
		temp = temp->next;
	}
	return temp;
}

Type* checkStructInlist(Structure* s, char* name){
	//check if the field contained in structure
	FieldList* head = s->inList;
	while(head!=NULL){
		if(strcmp(head->name, name)==0)
			return head->type;
		head = head->next;
	}
	return NULL;
}
