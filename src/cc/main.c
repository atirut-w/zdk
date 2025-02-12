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

const char *tokstr[] = {"+", "-", "*", "/", "intlit"};

static void scanfile(void) {
  struct token T;

  while (scan(&T)) {
    printf("Token: %s", tokstr[T.token]);
    if (T.token == T_INTLIT) {
      printf(", value: %d", T.intvalue);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv[0]);
  }

  init();

  if ((Infile = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open %s\n", argv[1]);
    exit(1);
  }

  scanfile();
  return 0;
}
