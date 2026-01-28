#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

typedef struct {
  FILE *output;
} CodeGen;

/**
 * Allocates and initializes a new code generator.
 */
CodeGen *codegen_new(FILE *output);

/**
 * Frees a code generator.
 */
void codegen_free(CodeGen *codegen);

#endif /* CODEGEN_H */
