%{
	//#include <stdio.h>
    #define YYDEBUG 1
	int yylex(void);
	void yyerror(char *);
%}


%%

List    : Expr '.' List {printf("parsed expression\n");}
	    | %empty
        ;

Expr    : Term E
        ;

E       : '+' Term E
        | '-' Term E
        | %empty
        ;

Term    : Factor T
        ;

T       : '*' Factor T
        | '%' Factor T
        | %empty
        ;

Factor  : 'n'
        | '(' Expr ')'
        ;

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	return;
}

int main(void) {
    //yydebug = 1;
	yyparse();
	return 0;
}
