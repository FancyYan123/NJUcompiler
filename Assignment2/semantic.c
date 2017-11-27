#include "semantic.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>

void Program(Node* root){
	if(root!=NULL){
		ExtDefList(root->Child);
	}
	//TODO: YOU MAY NEED TO CHECK FUNCLIST HERE.
	
}

void ExtDefList(Node* node){
	if(node!=NULL){
		if(node->Child!=NULL){
			ExtDef(node->Child);
			//TODO: TO DEBUG
			printf("ExtDefList1\n");
			ExtDefList(node->Child->Sibling);
		}
	}
}

void ExtDef(Node* node){
	assert(node!=NULL);
	Node* child = node->Child;
	
	Type* varType = Specifier(child);
	//TODO:TO DEBUG
	printf("ExtDef0\n");

	child = child->Sibling;
	
	//struct defination:
	if(strcmp(child->type, "SEMI")==0)
		return;
	
	//variables defination:
	else if(strcmp(child->type, "ExtDecList")==0){
		if(varType!=NULL){
			ExtDecList(child, varType);
			assert(strcmp(child->Sibling->type, "SEMI")==0);
		}
		return;
	}

	//functions defination:
	else if(strcmp(child->type, "FunDec")==0){
		FuncDef* newFunc = FunDec(child, varType);
		//TODO: TO DEBUG
		printFuncDef(newFunc);
		printf("ExtDef1\n");
		Type* rtnType = CompSt(child->Sibling);
		//TODO: TO DEBUG
		printf("ExtDef2\n");

		if(findFunc(newFunc->name)!=NULL){
			printf("Error type 4 at line %d: redefined function '%s'. \n", child->line, newFunc->name);
			free(newFunc);
		}
		else{
			newFunc->rtn = varType;
			if(compareType(varType, rtnType)==false){
				printf("Error type 8 at line %d: Type mismatched for return. \n", child->line);
			}
			else{
				insertFunc(newFunc);
			}
		}
		//TODO: TO DEBUG
		printFuncList();
	}
}

void ExtDecList(Node* node, Type* varType){
	Node* child = node->Child;
	
	//defination of global variable
	FieldList* var=VarDec(child, varType, 1);
	if(findVar(var->name)!=NULL){
		printf("Error type 3 at line %d: redefined variable '%s'.\n", child->line, child->value);
	}
	else{
		insertVarTable(var);
	}

	child = child->Sibling;
	if(child!=NULL){
		ExtDecList(child->Sibling, varType);
	}
}

FieldList* VarDec(Node* node, Type* varType, int from){
	//YOU may need to add codes for struct defination.
	Node* child = node->Child;
	assert(child!=NULL);
	FieldList* rtn=NULL;
	
	//TODO: TO DEBUG
	printf("VarDec0\n");
	printf("%s\n", child->type);

	if(strcmp(child->type, "ID")==0){
		//TODO: TO DEBUG
		printf("VarDec1\n");
		if(findVar(child->value)!=NULL){
			if(from==1)
				printf("Error type 3 at line %d: redefined variable '%s'.\n", child->line, child->value);
			else
				printf("Error type 15 at line %d: redefine field '%s' in struct. \n", child->line, child->value);
			//YOU can add code to free memory of varType.
			return NULL;
		}
		
		FieldList* temp = malloc(sizeof(FieldList));
		strcpy(temp->name, child->value);
		temp->type = varType;
		temp->next = NULL;
		rtn = temp;
		return rtn;
	}

	else{
		//TODO: deal with the array. VarDec==>VarDec LB INT RB
		assert(child->Sibling->Sibling!=NULL);
		Node* size = child->Sibling->Sibling;

		Type* subType = malloc(sizeof(Type));
		subType->kind = ARRAY;
		subType->u.array.elem = varType;
		subType->u.array.size = atoi(size->value);
		rtn = VarDec(child, subType, from);
		return rtn;
	}
}

Type* Specifier(Node* node){
	assert(node!=NULL);
	Type* ret;
	if(strcmp(node->Child->type, "TYPE")==0){
		ret = malloc(sizeof(Type));
		ret->kind = BASIC;
		if(strcmp(node->Child->value,"int")==0){
			ret->u.basic = INTTYPE;
		}
		else if(strcmp(node->Child->value, "float")==0){
			ret->u.basic = FLOATTYPE;
		}
		else{
			printf("Unknown type: debug your codes, Yan!\n");
		}
	}

	//deal with struct defination:
	else{
		ret = StructSpecifier(node->Child);
		//TODO: TO DEBUG
		printf("Specifier1\n");
	}

	return ret;
}

Type* StructSpecifier(Node* node){
	Type* rtn=NULL;
	FieldList* FieldNode=NULL;
	Node* child = node->Child;
	//child = child->Sibling;

	while(child!=NULL){
		//declaration:StructSpecifier==>STRUCT Tag, check if tag is in varTable
		if(strcmp(child->type, "Tag")==0){
			child = child->Child;
			FieldList* varType = findVar(child->value);
			if(varType == NULL){
				printf("Error type 17 at line %d: undefined structure '%s'. \n", child->line, child->value);
				return NULL;
			}
			return varType->type;
		}

		else if(strcmp(child->type, "OptTag")==0){
			rtn = malloc(sizeof(Type));
			FieldNode = malloc(sizeof(FieldList));

			if(child->Child!=NULL){
				//check the ID name: if it was defined before.
				if(findVar(child->Child->value)!=NULL){
					printf("Error type 16 at line %d: the new defined structure '%s' shares the same name of another variable. \n", child->Child->line, child->Child->value);
					return NULL;
				}
				
				rtn->kind = STRUCTURE;
			//	rtn->u.structure = malloc(sizeof(Structure));
				strcpy(rtn->u.structure.name, child->Child->value);
				strcpy(FieldNode->name, child->Child->value);
				FieldNode->type = rtn;
			}
			else{
				//struct without name, needn't add to VarTable:
				strcpy(rtn->u.structure.name, "\0");
				free(FieldNode);
			}
		}

		else if(strcmp(child->type, "DefList")==0){
			rtn->u.structure.inList = DefList(child, 2);
			if(rtn->u.structure.inList==NULL){
				//printf("Error 15 at line %d: duplicated defination in structure or try to assign the variable or empty structure. \n", child->Child->line);
				free(rtn); free(FieldNode);
				return NULL;
			}
			if(rtn->u.structure.name[0]=='\0'){
				return rtn;
			}
			else{
				insertVarTable(FieldNode);
				return rtn;
			}
		}

		child = child->Sibling;
	
	}
}

FuncDef* FunDec(Node* node, Type* rtn){
	//Create the new function node, but leave the checkment to ExtDef.
	Node* child = node->Child;
	FuncDef* newFunc = malloc(sizeof(FuncDef));
	strcpy(newFunc->name, child->value);
	newFunc->rtn = rtn;
	
	Node* param = child->Sibling->Sibling;
	if(strcmp(param->type, "VarList")==0){
		newFunc->param = VarList(param);
	}
	else
		newFunc->param = NULL;
	
	return newFunc;
}

FieldList* VarList(Node* node){
	FieldList* param = NULL;
	Node* child = node->Child;
	while(child!=NULL){
		if(strcmp(child->type, "ParamDec")==0){
			param = ParamDec(child);
			
		}
		
		else if(strcmp(child->type, "VarList")==0){
			if(param==NULL)
				param = VarList(child);
			else{
				FieldList* temp = param;
				while(temp->next!=NULL)
					temp = temp->next;
				temp->next = VarList(child);
			}
		}
		
		child = child->Sibling;
	}
	return param;
}

FieldList* ParamDec(Node* node){
	Node* child = node->Child;
	assert(strcmp(child->type, "Specifier")==0);
	
	Type* paramType = Specifier(child);
	if(paramType==NULL)
		return NULL;

	child = child->Sibling;
	assert(strcmp(child->type, "VarDec")==0);
	FieldList* arg_r = VarDec(child, paramType, 1);

	if(arg_r==NULL)
		return NULL;
	else{
		FieldList* arg_i = malloc(sizeof(FieldList));
		memcpy(arg_i, arg_r, sizeof(FieldList));
		insertVarTable(arg_i);
		return arg_r;
	}
}

Type* checkRtnType(Type* rtn1, Type* rtn2){
	Type* rtn = NULL;

	//there is no RETURN statement:
	if(rtn1==NULL && rtn2==NULL)
		return NULL;
	//multi RETURN statement, check if they return the same type:
	else if(rtn1!=NULL && rtn2!=NULL){
		if(compareType(rtn1, rtn2)==false){
		//	free(rtn1); free(rtn2);
			rtn = malloc(sizeof(Type));
			rtn->kind = UNKNOWN;
		}
		else
			rtn = rtn1;
	}
		
	else
		rtn = rtn1==NULL?rtn2:rtn1;
	
	return rtn;
}

Type* CompSt(Node* node){
	//return the return type of function
	assert(strcmp(node->type, "CompSt")==0);

	Node* child = node->Child;
	Type* rtnType = NULL;
	//TODO: TO DEBUG
	printf("CompSt0\n");
	while(child!=NULL){
		if(strcmp(child->type, "DefList")==0){
			DefList(child, 1);
			//TODO: TO DEBUG
			printf("CompSt1\n");
			printVarTable();
			//TODO: TO DEBUG
			printf("CompSt2\n");
		}

		else if(strcmp(child->type, "StmtList")==0){
			//TODO: TO DEBUG
			printf("CompSt3\n");
			rtnType = StmtList(child);
			printf("CompSt4\n");
		}

		child = child->Sibling;
	}
	return rtnType;
}

Type* StmtList(Node* node){
	if(node==NULL)
		return NULL;
	assert(strcmp(node->type, "StmtList")==0);
	
	Type* rtn = NULL;
	Node* child = node->Child;
	if(child!=NULL){
		//TODO: TO DEBUG
		printf("StmtList0\n");
		Type* rtn1 = Stmt(child);
		printf("StmtList1\n");
		child = child->Sibling;
		Type* rtn2 = StmtList(child);	
		
		rtn = checkRtnType(rtn1, rtn2);
	}
	return rtn;
}

Type* Stmt(Node* node){
	Node* child = node->Child;
	Type* rtn = NULL;

	while(child!=NULL){
		if(strcmp(child->type, "Exp")==0){
			//TODO: TO DEBUG
			printf("Stmt0\n");
			Exp(child);	
			printf("Stmt1\n");
		}
		
		else if(strcmp(child->type, "CompSt")==0){
			Type* tempType = CompSt(child);
			rtn = checkRtnType(tempType, rtn);
		}

		else if(strcmp(child->type, "IF")==0 || strcmp(child->type, "WHILE")==0){
			child = child->Sibling->Sibling;
			Type* tempType = Exp(child);
			if(tempType!=NULL){
				if(tempType->kind!=BASIC || tempType->u.basic!=INTTYPE)
					printf("Error ** at line %d: int type is expected in IF and WHILE statement. \n", child->line);
			}
			else
				printf("Error ** at line %d:int type is expected in IF and WHILE statement. \n", child->line);
		}

		else if(strcmp(child->type, "Stmt")==0){
			Type* tempType = Stmt(child);
			rtn = checkRtnType(tempType, rtn);
		}

		else if(strcmp(child->type, "RETURN")==0){
			child = child->Sibling;
			rtn = Exp(child);
		}
		
		child = child->Sibling;
	}	

	return rtn;
}

FieldList* DefList(Node* node, int from){
	assert(strcmp(node->type, "DefList")==0);

	Node* child = node->Child;
	FieldList* rtn = NULL;

	if(NULL==child)
		return NULL;
	
	//for function statements:
	if(1==from){
		//TODO: TO DEBUG
		printf("DefList0\n");
		Def(child, from);
		printf("DefList1\n");
		DefList(child->Sibling, from);	
		return NULL;
	}

	//for structure defination
	else if(2==from){
		rtn = Def(child, from);
		if(rtn!=NULL)
			rtn->next = DefList(child->Sibling, from);
		else{
		//	printf("Error 15 at line %d: duplicated defination in structure or try to assign the variable or empty structure. \n", child->line);
			return NULL;
		}
		return rtn;
	}

	else{
		printf("Wrong DefList, debug your codes, Yan!\n");
		assert(0);
	}
}

FieldList* Def(Node* node, int from){
	assert(strcmp(node->type, "Def")==0);

	Node* child = node->Child;
	Type* varType = Specifier(child);
	//TODO: TO DEBUG
	printf("Def1\n");
	
	if(varType!=NULL){
		child = child->Sibling;
		return DecList(child, varType, from);
	}
	else 
		return NULL;
}

FieldList* DecList(Node* node, Type* varType, int from){
	assert(strcmp(node->type, "DecList")==0);

	Node* child = node->Child;
	FieldList* rtn = NULL;

	//in functions:
	if(from == 1){
		while(child!=NULL){
			if(strcmp(child->type, "Dec")==0){
				//TODO: TO DEBUG
				printf("DecList0\n");
				FieldList* var = Dec(child, varType, 1);
				if(var!=NULL)
					insertVarTable(var);
				//TODO: TO DEBUG
				printf("DecList1\n");
			}
			
			if(strcmp(child->type, "DecList")==0){
				DecList(child, varType, from);
			}

			child = child->Sibling;
		}		
		return NULL;
	}

	//in structure definations:
	else if(from == 2){
		while(child!=NULL){
			if(strcmp(child->type, "Dec")==0){
				FieldList* var_i = Dec(child, varType, from);
				if(var_i==NULL){
					freeFieldList(rtn);
					return NULL;
				}
				
				//as long as the var_i is not NULL,
				//we can assure the field is not redefined.
				FieldList* var_s = malloc(sizeof(FieldList));
				memcpy(var_s, var_i, sizeof(FieldList));

				insertVarTable(var_i);
				var_s->next = NULL;
				FieldList* temp = getFieldListTail(rtn);
				if(temp==NULL)
					rtn = var_s;
				else
					temp->next = var_s;
				/*
				//check if it's redefined, if so, return NULL;
				if(findVar(var_i->name)!=NULL){
					free(var_i);free(var_s);
					freeFieldList(rtn);	
					return NULL;
				}
				//if not, insert it to varTable, and add to the tail of the structure
				else{
					insertVarTable(var_i);

					FieldList* temp = getFieldListTail(rtn);
					if(temp==NULL)
						rtn = var_s;
					else
						temp->next = var_s;	
				}*/
			}

			else if(strcmp(child->type, "DecList")==0){
				//check the statement left, if error occurs, abondon all the list and return NULL. 
				//error msg is given in structspecifier
				FieldList* Tail = DecList(child, varType, from);
				if(Tail==NULL){
					freeFieldList(rtn);
					return NULL;
				}

				FieldList* temp = getFieldListTail(rtn);
				if(temp==NULL)
					rtn = Tail;
				else
					temp->next = Tail;
			}
			
			child = child->Sibling;
		}
		return rtn;
	}

	else
		assert(0);
	
}

FieldList* Dec(Node* node, Type* varType, int from){
	assert(strcmp(node->type, "Dec")==0);

	Node* child = node->Child;
	FieldList* var=NULL;
	//in the function:
	if(from==1){
		//TODO: TO DEBUG
		printf("Dec0\n");
		printf("%s\n", child->type);
		var = VarDec(child, varType, from);
		//TODO: TO DEBUG
		printf("Dec1: varType\n");
		
		child = child->Sibling;
		printf("a\n");
		if(child!=NULL){
			child = child->Sibling;
			assert(child!=NULL && strcmp(child->type, "Exp")==0);
			//TODO: TO DEBUG
			printf("Dec2\n");
			Type* rightType = Exp(child);
			if(compareType(varType, rightType)==false){
				printf("Error5 at line%d: the types on both sides of '=' are not the same. \n", child->line);
				free(var);
				var = NULL;
			}
		}	

		return var;
	}

	//in the structure:
	else if(from==2){
		var = VarDec(child, varType, from);
		
		child = child->Sibling;
		if(child==NULL)
			return var;
		else{
			//try to assign the variable in structure, msg is given in DefList
			//printf("Error 15 at line %d: duplicated defination in structure or try to assign the variable or empty structure. \n", child->line);
			free(var);
			return NULL;
		}
	}
	else
		assert(0);
}

Type* Exp(Node* node){
	Node* child = node->Child;
	if(child==NULL)
		return NULL;

	if(strcmp(child->type, "Exp")==0){
		//case like "Exp OP Exp"
		Type* leftType = Exp(child);
		//TODO: TO DEBUG
		printf("Exp0\n");
		Type* rightType = Exp(child->Sibling->Sibling);
		printf("Exp1\n");
		//Used when Exp==>Exp Dot ID
		Node* structNode = child->Child;
		child = child->Sibling;
	

		//assignment:
		if(strcmp(child->type, "ASSIGNOP")==0){
			if(leftType==NULL || rightType==NULL)
				return NULL;
			else if(leftType->assign == RIGHT){
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable. \n ", child->line);
				return NULL;
			}

			else if(compareType(leftType, rightType)==false){
				printf("Error5 at line %d: the types on both sides of '=' is not the same. \n", child->line);
				return NULL;
			}

			else{
				leftType->assign = BOTH;
				return leftType;
			}		
		}
		
		//array:
		else if(strcmp(child->type, "LB")==0){
			if(rightType->kind != BASIC || rightType->u.basic!=INTTYPE){
				printf("Error type 12 at line %d:Only int can be used as index of an array. \n", child->line);
				return NULL;
			}
			else if(leftType->kind!=ARRAY){
				printf("Error type 10 at line %d: you cannot use '[..]' operator to a non-array variable. \n", child->line);
				return NULL;
			}
			else{
				assert(leftType->u.array.elem!=NULL);
				leftType->u.array.elem->assign = BOTH;
				return leftType->u.array.elem;
			}
		}

		//structure field:
		else if(strcmp(child->type, "DOT")==0){
			//Node* structNode = child->Child;
			assert(strcmp(structNode->type, "ID")==0);
			FieldList* structID = findVar(structNode->value);
			if(structID==NULL){
				printf("Error type 1 at line %d: undefined variable '%s'. \n", structNode->line, structNode->value);
				return NULL;
			}
			else if(structID->type->kind != STRUCTURE){
				printf("Error type 13 at line %d: Illegal use of '.' \n", structNode->line);
				return NULL;
			}
			else{
				Type* rtn = checkStructInlist(&structID->type->u.structure, child->Sibling->value);
				if(rtn==NULL){
					printf("Error type 14 at line %d: undefine field '%s'. \n", child->line, child->Sibling->value);
					return NULL;
				}
				else{
					rtn->assign = BOTH;
					return rtn;
				}
			}
		}

		//bool operators:
		else if(strcmp(child->type, "AND")==0 || strcmp(child->type, "OR")==0){
			if(leftType->kind!=BASIC || rightType->kind!=BASIC || leftType->u.basic!=INTTYPE || rightType->u.basic!=INTTYPE){
				printf("Error ** at line %d: && || ! can only operate int type. \n", child->line);
				return NULL;
			}
			else{
				leftType->assign = RIGHT;
				return leftType;
			}
		}
		
		//RELOP, PLUS, STAR, DIV, MINUS:
		else{
			
			if(leftType==NULL || rightType==NULL){
				printf("Error type 7 at line %d: Type mismatched for operands. \n", child->line);
				return NULL;	
			}

			else if( leftType->kind!=BASIC || rightType->kind!=BASIC || leftType->u.basic!=rightType->u.basic){
				printf("Error type 7 at line %d: Type mismatched for operands \n", child->line);
				return NULL;
			}
		
			else{
				leftType->assign = RIGHT;
				return leftType;
			}
		}
	}


	else if(strcmp(child->type, "MINUS")==0){
		Type* temp = Exp(child->Sibling);
		if(temp->kind!=BASIC){
			printf("Error type 7 at line %d: Type mismatched for operands. \n", child->line);
			return NULL;
		}
		else{
			temp->assign = RIGHT;
			return temp;
		}
	}

	else if(strcmp(child->type, "NOT")==0){
		Type* temp = Exp(child->Sibling);
		if(temp->kind!=BASIC || temp->u.basic == INTTYPE){
			printf("Error ** at line %d: expect int type for operator '!'. \n", child->line);
			return NULL;
		}
		else{
			temp->assign = RIGHT;
			return temp;
		}
	}
	
	//ID can be a variable or a funtion
	else if(strcmp(child->type, "ID")==0){
		if(child->Sibling==NULL){
			FieldList* var = findVar(child->value);
			if(var==NULL){
				printf("Error type 1 at line %d: undefined variable '%s'. \n", child->line, child->value);
				return NULL;
			}
			else{
				Type* rtn = var->type;
				rtn->assign = BOTH;
				return rtn;
			}
		}
		else{
			FuncDef* func = findFunc(child->value);
			if(func==NULL){
				FieldList* temp = findVar(child->value);
				if(temp==NULL)
					printf("Error type 2 at line %d: undefined function '%s'. \n", child->line, child->value);
				else
					printf("Error type 11 at line %d: %s is not a function. \n", child->line, child->value);
				return NULL;
			}
			Node* third = child->Sibling->Sibling;
			assert(third!=NULL);
			
			// function without arguments:
			if(strcmp(third->type, "RP")==0){
				if(func->param!=NULL){
					printf("Error type 9 at line %d: Function '%s' mismatched for arguments. \n", third->line, func->name);
					return NULL;
				}
				else{
					Type* rtn = func->rtn;
					rtn->assign = RIGHT;
					return rtn;
				}
			}
			
			// third is args:
			else{
				if(Args(third, func->param)==false){
					printf("Error type 9 at line %d: Function '%s' mismatched for arguments. \n", third->line, func->name);
					return NULL;
				}
				else{
					Type* rtn = func->rtn;
					rtn->assign = RIGHT;
					return rtn;
				}
			}
		}
	}

	else if(strcmp(child->type, "INT")==0 || strcmp(child->type, "FLOAT")==0){
		Type* rtn = malloc(sizeof(Type));
		rtn->kind = BASIC;
		rtn->assign = RIGHT;
		if(strcmp(child->type, "INT")==0)
			rtn->u.basic = INTTYPE;
		else
			rtn->u.basic = FLOATTYPE;

		return rtn;
	}

	else if(strcmp(child->type, "LP")==0){
		return Exp(child->Sibling);
	}

	else
		assert(0);
}

bool Args(Node* node, FieldList* param){
	assert(node!=NULL);
	if(param==NULL){
		return false;
	}

	Node* child = node->Child;
	Type* param1 = Exp(child);
	if(compareType(param1, param->type)==true){
		if(child->Sibling==NULL)
			return true;
		else
			return Args(child->Sibling->Sibling, param->next);
	}
	else
		return false;
}
