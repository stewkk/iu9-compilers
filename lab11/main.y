%{
#include <stdio.h>

void yyerror(const char *s) {
    fprintf(stderr, "Ошибка: %s\n", s);
}

int yylex(void) {
    return 0;
}
%}

%start program

%%

program: /* Пустое правило */
;

%%

int main(void) {
    yyparse();
    printf("hello world");
    return 0;
}
