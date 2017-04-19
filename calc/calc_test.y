%{
	#include <stdio.h>
    #define YYDEBUG 1
	int yylex(void);
	void yyerror(char *);
%}


%%

List    : List Expr '.' {printf("parsed expresion\n");}
	    | Expr '.' {printf("parsed expresion\n");}
        ;

Expr    : Expr Factor
        | Factor
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
    yydebug = 1;
	yyparse();
	return 0;
}
