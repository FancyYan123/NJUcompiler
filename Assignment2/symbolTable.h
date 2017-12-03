#ifndef __SYMBOLTABLE__
#define __SYMBOLTABLE__

#include<stdio.h>
#include<math.h>

#define INTTYPE		0
#define FLOATTYPE		1
#define HASH_LENGTH		256
#define MAX_EMBEDDED	30	

typedef enum {false, true} bool;

//typedef struct FieldList_* FieldList;
//typedef struct Type_* Type;
struct FieldList_;
struct Structure_;

typedef struct Structure_{
	char name[32];
	struct FieldList_* inList;
}Structure;

typedef struct Type_{
	enum {BASIC, ARRAY, STRUCTURE, UNKNOWN} kind;
	union
	{
		int basic;
		struct {
			struct Type_ *elem;			//link subarray;
			int size;
		}array;
		Structure structure;	//link other structure variables;		
	}u;
	enum {LEFT, RIGHT, BOTH} assign;
}Type;

typedef struct FieldList_{
	char name[32];
	enum {VARIABLE, STRUCTURETYPE} kind;

	Type* type;							//type of the area;
	struct FieldList_ *pre;			//for convenience of delete
	struct FieldList_ *next;	        //next node of hash table;
	struct FieldList_ *nextStack;			//next node of the same embedded level
}FieldList;		//the element of crossing hash table

typedef struct FuncDef_{
//	enum {DEF, DEC} kind;
	char name[50];
	Type* rtn;
	FieldList* param;
	bool is_define;
	int line;
	struct FuncDef_* next;
}FuncDef;	//use a linklist to store defination or declaration of function

//defination of global variables: 
extern FieldList* VarTable[HASH_LENGTH];
extern FuncDef* FuncList;
extern FieldList* Stack[MAX_EMBEDDED];
extern int StackLevel;

bool compareFunc(FuncDef *func1, FuncDef *func2);
bool compareFieldList(FieldList* node1, FieldList* node2);
bool compareType(Type* node1, Type* node2);
bool Compare_StructureType(Type* node1, Type* node2);
int calcHashIndex(char* str);
void freeFieldList(FieldList* head);
FieldList* getFieldListTail(FieldList* head);
//to find if the structure contains field named 'name'
Type* checkStructInlist(FieldList* s, char* name);

//interface of VarTable:
void initVarTable();
int insertVarTable(FieldList* node);
FieldList* findVar(char* varName);	//find the node named varName

//interface of FuncList
void insertFunc(FuncDef* func);
FuncDef* findFunc(char* FuncName);

//debug functions, output hashtable and linkedlist information:
void printFieldList(FieldList* var);
void printVarTable();
void printFuncDef(FuncDef* f);
void printFuncList();
void printInList(FieldList* inList);

//interface of Stack:
void initStack();
void insertStackTail(FieldList* node);	//insert the node to the tail of stack top; 
FieldList* getStackTail();	//get the last node linked to the top of stack;
FieldList* push();		//start a new embedded level
void pop();			//delete the top linkedlist of the stack to leave the embeded level

#endif
