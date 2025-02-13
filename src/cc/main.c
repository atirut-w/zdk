#include "decl.h"
#include "defs.h"
#include <stdlib.h>
#define extern_
#include "data.h"
#undef extern_

static void init(void) {
  Line = 1;
  Putback = '\n';
}

static void usage(const char *prog) {
  fprintf(stderr, "Usage: %s file\n", prog);
  exit(1);
}

int main(int argc, char *argv[]) {
  struct ASTnode *n;

  if (argc != 2) {
    usage(argv[0]);
  }

  init();

  if ((Infile = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open %s\n", argv[1]);
    exit(1);
  }

  scan(&Token);
  n = binexpr();
  printf("%d\n", interpretAST(n));
  return 0;
}
