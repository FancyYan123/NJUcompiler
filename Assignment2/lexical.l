%option yylineno
%{
	#include <stdlib.h>
	#include <math.h>
	#include "Lexlib.h"
	#include "syntax.tab.h"
	#include "syntaxTree.h"
/*	
	//error here: yylloc undeclared? but it's a internal varible! why?
	int yycolumn = 1;
	#define YY_USER_ACTION \
		yylloc->first_line = yylloc->last_line = yylineno; \
		yylloc->first_column = yycolumn; \
		yylloc->last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;
*/
	int LexErrorFlag = 1;	
	int SynErrorFlag = 1;
%}

delim [" "\n\t]
letter [a-zA-Z]
digit [0-9]

%%
[" ""	"\t]+ {/*Do nothing.*/ }

";" { 
	//printf("<SEMI ;> ");
	yylval.node = initNode("SEMI", yytext);
	return SEMI; 
}

"," { 
	//printf("<COMMA ,> "); 
	yylval.node = initNode("COMMA", yytext);
	return COMMA;
}

"=" { 
	//printf("<ASSIGNOP => ");
	yylval.node = initNode("ASSIGNOP", yytext);
	return ASSIGNOP;
}

>|<|>=|<=|==|!= { 
	//printf("<RELOP <|>|>=|<=|==|!= > ");
	yylval.node = initNode("RELOP", yytext);
	return RELOP;
}

"+" { 
	//printf("<PLUS +> "); 
	yylval.node = initNode("PLUS", yytext);
	return PLUS;
}

"-" { 
	//printf("<MINUS -> "); 
	yylval.node = initNode("MINUS", yytext);
	return MINUS;
}

"*" { 
	//printf("<STAR *> "); 
	yylval.node = initNode("STAR", yytext);
	return STAR;
}

"/" { 
	//printf("<DIV /> "); 
	yylval.node = initNode("DIV", yytext);
	return DIV;
}

"&&" { 
	//printf("<AND &&> "); 
	yylval.node = initNode("AND", yytext);
	return AND;
}

"||" { 
	//printf("<OR ||> "); 
	yylval.node = initNode("OR", yytext);
	return OR;
}

"." { 
	//printf("<DOT .> "); 
	yylval.node = initNode("DOT", yytext);
	return DOT;
}

"!" { 
	//printf("<NOT !> "); 
	yylval.node = initNode("NOT", yytext);
	return NOT;
}

(int|float) { 
	//printf("<TYPE int|float>"); 
	yylval.node = initNode("TYPE", yytext);
	return TYPE;
}

"("	{ 
	//printf("<LP (> ");
	yylval.node = initNode("LP", yytext);
	return LP;
}

")" { 
	//printf("<RP )> ");
	yylval.node = initNode("RP", yytext);
	return RP;
}

"[" { 
	//printf("<LB [> ");
	yylval.node = initNode("LB", yytext);
	return LB;
}

"]" { 
	//printf("<RB ]> ");
	yylval.node = initNode("RB", yytext);
	return RB;
}

"{" { 
	//printf("<LC {> ");
	yylval.node = initNode("LC", yytext);
	return LC;
}

"}" { 
	//printf("<RC }> ");
	yylval.node = initNode("RC", yytext);
	return RC;
}

"struct" {
	//printf("<STRUCT > ");
	yylval.node = initNode("STRUCT", yytext);
	return STRUCT;
}

"return" {
	//printf("<RETURN > ");
	yylval.node = initNode("RETURN", yytext);
	return RETURN;
}

"if" { 
	//printf("<IF >"); 
	yylval.node = initNode("IF", yytext);
	return IF;
}

"else" { 
	//printf("<ELSE >"); 
	yylval.node = initNode("ELSE", yytext);
	return ELSE;
}

"while" { 
	//printf("WHILE >"); 
	yylval.node = initNode("WHILE", yytext);
	return WHILE;
}

(([1-9][0-9]*)|0) { 
	//printf("<INT %d> ", atoi(yytext));
	yylval.node = initNode("INT", yytext);
	return INT;
}

((0[1-7][0-7]*)|0) {
	//printf("<INT oct %d> ", oct_to_deci(yytext));
	int val = oct_to_deci(yytext);
	char text[32];
	sprintf(text, "%d", val);
	yylval.node = initNode("INT", text);
	return INT;
}

((0x|0X)([1-9a-fA-F][0-9a-fA-F]*|0)) {
	//printf("INT hex %d> ", hex_to_deci(yytext));
	int val = hex_to_deci(yytext);
	char text[32];
	sprintf(text, "%d", val);
	yylval.node = initNode("INT", text);
	return INT;
}

(([0-9]*\.[0-9]+)|([0-9]+\.)) {
	//printf("<FLOAT %f> ", atof(yytext));
	yylval.node = initNode("FLOAT", yytext);
	return FLOAT;
}

(([0-9]*\.[0-9]+)|([0-9]+\.))e[+-]?(([1-9][0-9]*)|0) { 
	//float type in exponential format
	float realFloat = exp_to_float(yytext);
	char text[64];
	sprintf(text, "%f", realFloat);
	yylval.node = initNode("FLOAT", text);
	
	//TO DEBUG: print debug information:
	//printf("%s\n", text);
	return FLOAT;  
}

(([0-9]*\.[0-9]+)|([0-9]+\.))e {
	//error type of expotional float
	printf("Error type A at line %d: error float/double type in expotional format: \'%s\' \n", yylineno, yytext);
	LexErrorFlag = 0;
}
 
(0x|0X)([0-9a-fA-F]*[g-zG-Z]+[0-9a-fA-F]*) {
	//error type of hexadecimal

	printf("Error type A at line %d: error hexadecimal number \'%s\' \n",yylineno, yytext);
	LexErrorFlag = 0;
}

(0[0-7]*[89]+[0-7]*) {
	//error type of octonary

	printf("Error type A at Line %d: error octonary number \'%s\'\n", yylineno, yytext);	
	LexErrorFlag = 0;
}

[_a-zA-Z][a-zA-Z_0-9]* {
	//printf("<ID %s> ", yytext);
	//TODO:you may need to write id to symbol table and record it.
	yylval.node = initNode("ID", yytext);
	return ID;
}

[0-9][a-zA-Z_0-9]+ {
	/*
	 * error type of id
	 * TODO:we may need to change it further to recognize the suffix of a 
     * constant integer 
	 */

	printf("Error type A at line %d: unrecognized identifier \'%s\'\n", yylineno,yytext);
	LexErrorFlag = 0;
}

"//" {
	char c = input();
	while(c!='\n') 
		c = input();
	//printf("<comments> ");
}

"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/" {
	//TODO:unknown error here
	//the regix expression didn't act well
	//printf("%s\n\n", yytext);
}

\n { 
	//printf("\n"); 
	
	//yycolumn = 1;
}

. {

	printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);
	LexErrorFlag = 0;
}

%%

#include "Lexlib.c"

/*
int main(int argc, char** argv) {
	if(argc>1) {
		if(!(yyin=fopen(argv[1], "r"))) {
			perror(argv[1]);
			return 1;
		}
	}
	
	yylex();
	return 0; 
}
*/
