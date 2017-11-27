%{
	#include <stdio.h>
	#include "syntaxTree.h"
	#include "lex.yy.c"

	int yydebug = 1;
//	Node* root;
%}


%union{
	int intVal;
	double floatVal;
	char* stringVal; 
	Node* node;
}


/*declared tokens*/
/*REAL TYPES*/
%token <node> INT
%token <node> FLOAT
%token <node> ID

/*tokens*/
%token <node> PLUS MINUS STAR DIV
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> AND
%token <node> OR
%token <node> DOT
%token <node> TYPE
%token <node> LP
%token <node> NOT
%token <node> RP
%token <node> LB
%token <node> RB
%token <node> LC
%token <node> RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF
%token <node> ELSE
%token <node> WHILE


/*define combination principles*/
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%left NOT 
%left DOT LP RP LB RB

/*handle if-else reduce-shift conflict*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/*non-ternimal*/
%type <node> Program ExtDefList ExtDef ExtDecList Specifier 
%type <node> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <node> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <node> Dec Exp Args

%%
/* High-level Defination */
Program:ExtDefList { $$ = initNode("Program","");insert($$, $1); root=$$;}
  ;

ExtDefList:ExtDef ExtDefList {$$=initNode("ExtDefList", ""); insert($$, $1);insert($$, $2);}
  |/* empty */ { $$=initNode("ExtDefList", ""); }
  ;

ExtDef:Specifier ExtDecList SEMI {$$=initNode("ExtDef",""); insert($$,$1); insert($$, $2); insert($$, $3);}
  | Specifier SEMI {$$=initNode("ExtDef", ""); insert($$, $1); insert($$, $2);}
  | Specifier FunDec CompSt {$$=initNode("ExtDef", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | error SEMI { SynErrorFlag = 0; }
  ;
ExtDecList:VarDec {$$=initNode("ExtDecList", ""); insert($$, $1);}
  | VarDec COMMA ExtDecList {$$=initNode("ExtDecList", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  ;

/* Specifier */
Specifier:TYPE {$$=initNode("Specifier", ""); insert($$, $1);}
  | StructSpecifier {$$=initNode("Specifier",""); insert($$, $1);}
  ;

StructSpecifier:STRUCT OptTag LC DefList RC {$$=initNode("StructSpecifier", ""); insert($$,$1); insert($$, $2); insert($$, $3); insert($$, $4); insert($$, $5);}
  | STRUCT Tag {$$=initNode("StructSpecifier", ""); insert($$, $1); insert($$, $2);}
  ;
OptTag:ID {$$=initNode("OptTag", ""); insert($$, $1);}
  |/* empty */ {$$=initNode("OptTag", "");}
  ;
Tag:ID {$$=initNode("Tag", ""); insert($$, $1);}
   ;

/* Declarators */
VarDec:ID {$$=initNode("VarDec",""); insert($$, $1);}
  | VarDec LB INT RB {$$=initNode("VarDec",""); insert($$, $1); insert($$, $2); insert($$, $3); insert($$, $4);}
  ;
FunDec:ID LP VarList RP {$$=initNode("FunDec", ""); insert($$, $1); insert($$, $2); insert($$, $3); insert($$, $4);}
  | ID LP RP {$$=initNode("FunDec", ""); insert($$,$1); insert($$, $2); insert($$, $3);}
  | error RP { SynErrorFlag = 0; }
  ;
VarList:ParamDec COMMA VarList {$$=initNode("VarList",""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | ParamDec {$$=initNode("VarList", ""); insert($$, $1);}
  ;
ParamDec:Specifier VarDec {$$=initNode("ParamDec", ""); insert($$, $1); insert($$, $2);}
  ;

/* Statement */
CompSt:LC DefList StmtList RC {$$=initNode("CompSt",""); insert($$, $1); insert($$, $2); insert($$,$3); insert($$, $4);}
	  | error RC {SynErrorFlag = 0;}
      ;
StmtList:Stmt StmtList {$$=initNode("StmtList", ""); insert($$, $1); insert($$, $2);}
  | /* empty */ {$$=initNode("StmtList", "");}
  ;
Stmt:Exp SEMI {$$=initNode("Stmt", ""); insert($$, $1); insert($$, $2); }
  | CompSt	{$$=initNode("Stmt", ""); insert($$, $1);}
  | RETURN Exp SEMI {$$=initNode("Stmt", ""); insert($$, $1); insert($$,$2); insert($$, $3);}
  | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=initNode("Stmt", ""); insert($$, $1); insert($$, $2);insert($$, $3); insert($$, $4); insert($$, $5); }
  | IF LP Exp RP Stmt ELSE Stmt {$$=initNode("Stmt", ""); insert($$, $1); insert($$, $2); insert($$,$3); insert($$, $4); insert($$, $5); insert($$, $6); insert($$, $7);}
  | WHILE LP Exp RP Stmt {$$=initNode("Stmt", ""); insert($$, $1); insert($$, $2); insert($$, $3); insert($$, $4); insert($$, $5); }
  | error RP {SynErrorFlag = 0;}
  | error SEMI {SynErrorFlag=0;}
  ;

/*Local Defination*/
DefList:Def DefList {$$=initNode("DefList", ""); insert($$, $1); /*here the bug: segmentation fault*/insert($$, $2); }
  |/*empty*/ {$$=initNode("DefList", "");}
  ;
Def:Specifier DecList SEMI {$$=initNode("Def", ""); insert($$, $1); insert($$, $2); insert($$, $3); }
   ;
DecList:Dec {$$=initNode("DecList", ""); insert($$, $1); }
  | Dec COMMA DecList {$$=initNode("DecList",""); insert($$, $1); insert($$, $2); insert($$, $3); }
  ;
Dec:VarDec {$$=initNode("Dec", ""); insert($$,$1); }
   | VarDec ASSIGNOP Exp {$$=initNode("Dec", ""); insert($$, $1); insert($$, $2); insert($$, $3); }
   ;

/* Expression */
Exp:Exp ASSIGNOP Exp {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp AND Exp {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp OR Exp	{$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp RELOP Exp    {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp PLUS Exp    {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp MINUS Exp    {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp STAR Exp    {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp DIV Exp  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | LP Exp RP  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | MINUS Exp  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2);}
  | NOT Exp  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2);}
  | ID LP Args RP  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3); insert($$, $4);}
  | ID LP RP   {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | Exp LB Exp RB  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3); insert($$, $4); }
  | Exp DOT ID  {$$=initNode("Exp", ""); insert($$, $1); insert($$, $2); insert($$, $3);}
  | ID  {$$=initNode("Exp", ""); insert($$, $1);}
  | INT   {$$=initNode("Exp", ""); insert($$, $1);}
  | FLOAT    {$$=initNode("Exp", ""); insert($$, $1);}
  ;
Args:Exp COMMA Args  {$$=initNode("Args", ""); insert($$, $1); insert($$, $2); insert($$, $3); }
  | Exp	{$$=initNode("Args", ""); insert($$, $1); }
  ;
%%


void yyerror(char* msg){
	if(LexErrorFlag==1)
		fprintf(stderr, "Error type B at line %d: %s\n", yylineno, msg);
}
