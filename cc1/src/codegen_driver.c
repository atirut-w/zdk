#include "codegen_driver.h"
#include <stdlib.h>

CodeGenDriver *codegen_driver_new(CompilationCtx *ctx, Lexer *lexer,
                                  CodeGen *codegen) {
  CodeGenDriver *driver = malloc(sizeof(CodeGenDriver));
  if (!driver) {
    return NULL;
  }

  driver->ctx = ctx;
  driver->lexer = lexer;
  driver->codegen = codegen;

  return driver;
}

void codegen_driver_free(CodeGenDriver *driver) {
  if (driver) {
    free(driver);
  }
}
