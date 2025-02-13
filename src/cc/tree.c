#include "defs.h"
#include <stdio.h>
#include <stdlib.h>

struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *right,
                          int intvalue) {
  struct ASTnode *n;

  n = malloc(sizeof(struct ASTnode));
  if (n == NULL) {
    fprintf(stderr, "Unable to malloc in mkastnode()\n");
    exit(1);
  }
  n->op = op;
  n->left = left;
  n->right = right;
  n->intvalue = intvalue;
  return n;
}

struct ASTnode *mkastleaf(int op, int intvalue) {
  return mkastnode(op, NULL, NULL, intvalue);
}

struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
  return mkastnode(op, left, NULL, intvalue);
}
