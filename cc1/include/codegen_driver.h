#ifndef CODEGEN_DRIVER_H
#define CODEGEN_DRIVER_H

#include "codegen.h"
#include "compilation_ctx.h"
#include "lexer.h"

typedef struct {
  CompilationCtx *ctx;
  Lexer *lexer;
  CodeGen *codegen;
} CodeGenDriver;

/**
 * Allocates and initializes a new code generation driver.
 */
CodeGenDriver *codegen_driver_new(CompilationCtx *ctx, Lexer *lexer,
                                  CodeGen *codegen);

/**
 * Frees a code generation driver.
 */
void codegen_driver_free(CodeGenDriver *driver);

#endif /* CODEGEN_DRIVER_H */
