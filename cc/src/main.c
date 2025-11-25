#include <lexer.h>
#include <parser.h>

int main(int argc, char *argv[]) {
  yyin = stdin;
  yyout = stdout;
  return yyparse();
}
