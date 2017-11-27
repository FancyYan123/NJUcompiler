parser:
	bison -d syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxTree.c symbolTable.c semantic.c -lfl -ly -o parser

#add yydebug=1 before yyparse() to get file:syntax.output
debug:
	bison -d -t syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxTree.c -lfl -ly -o parser

test:
	gcc test.c symbolTable.c -o test

lexical:
	flex lexical.l

syntax:
	bison -d syntax.y

scanner:
	gcc main.c lex.yy.c -lfl -o scanner

clean:
	rm lex.yy.c syntax.tab.* parser scanner syntax.output

