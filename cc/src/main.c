#include <stdio.h>

extern int yyparse(void);
extern FILE *yyin;

int main(int argc, char *argv[]) {
  if (argc > 1) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      fprintf(stderr, "Error: cannot open file '%s'\n", argv[1]);
      return 1;
    }
  } else {
    yyin = stdin;
  }

  yyparse();

  if (yyin != stdin) {
    fclose(yyin);
  }

  return 0;
}