#include "codegen.h"

CodeGen *codegen_new(FILE *output) {
  CodeGen *codegen = malloc(sizeof(CodeGen));
  if (!codegen) {
    return NULL;
  }

  codegen->output = output;

  return codegen;
}

void codegen_free(CodeGen *codegen) {
  if (codegen) {
    free(codegen);
  }
}
