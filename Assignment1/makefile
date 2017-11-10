parser:
	bison -d syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxTree.c -lfl -ly -o parser

#add yydebug=1 before yyparse() to get file:syntax.output
debug:
	bison -d -t syntax.y
	flex lexical.l
	gcc main.c syntax.tab.c syntaxTree.c -lfl -ly -o parser

lexical:
	flex lexical.l

syntax:
	bison -d syntax.y

scanner:
	gcc main.c lex.yy.c -lfl -o scanner

clean:
	rm lex.yy.c syntax.tab.* parser scanner syntax.output

