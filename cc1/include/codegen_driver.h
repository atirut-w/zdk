#ifndef CODEGEN_DRIVER_H
#define CODEGEN_DRIVER_H

#include "compilation_ctx.h"
#include "lexer.h"

typedef struct {
  CompilationCtx *ctx;
  Lexer *lexer;
} CodeGenDriver;

/**
 * Allocates and initializes a new code generation driver.
 */
CodeGenDriver *codegen_driver_new(CompilationCtx *ctx, Lexer *lexer);

/**
 * Frees a code generation driver.
 */
void codegen_driver_free(CodeGenDriver *driver);

#endif /* CODEGEN_DRIVER_H */
