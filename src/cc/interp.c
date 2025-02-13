#include "decl.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
static char *ASTop[] = {"+", "-", "*", "/"};

int interpretAST(struct ASTnode *n) {
  int leftval, rightval;

  if (n->op == A_INTLIT) {
    return n->intvalue;
  }

  leftval = interpretAST(n->left);
  rightval = interpretAST(n->right);

  switch (n->op) {
  case A_ADD:
    return leftval + rightval;
  case A_SUBTRACT:
    return leftval - rightval;
  case A_MULTIPLY:
    return leftval * rightval;
  case A_DIVIDE:
    return leftval / rightval;
  case A_INTLIT:
    return n->intvalue;
  default:
    fprintf(stderr, "Unknown AST operator %d\n", n->op);
    exit(1);
  }
}
