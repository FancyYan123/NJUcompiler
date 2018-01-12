// #include <stdlib.h>
// #include <stdio.h>
// #include "syntaxTree.h"
// #include "syntax.tab.h"
// #include "symbolTable.h"
// #include "semantic.h"
// #include "IR.h"
// #include "IRtranslate.h"

#include "common.h"

extern int LexErrorFlag;
extern int SynErrorFlag;
extern void yyrestart(FILE *input_file);
extern int yydebug;

int main(int argc, char** argv){
	if(argc <= 1)
		return 1;
	FILE* f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yydebug = 1;
	yyparse();

	if(LexErrorFlag==1 && SynErrorFlag==1){
//		traversal(root, 0);
		Program(root);
		IRhead =  translate_Program(root);
//#ifdef DEBUG
    printf("at main\n");
	outIR(IRhead);
//#endif

		printIR("intercode.ir");
	}

//	else
//		traversal(root, 0);
	return 0;
}
