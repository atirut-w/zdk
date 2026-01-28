#ifndef CODEGEN_DRIVER_H
#define CODEGEN_DRIVER_H

#include "lexer.h"

typedef struct {
  Lexer *lexer;
} CodeGenDriver;

/**
 * Allocates and initializes a new code generation driver.
 */
CodeGenDriver *codegen_driver_new(Lexer *lexer);

/**
 * Frees a code generation driver.
 */
void codegen_driver_free(CodeGenDriver *driver);

#endif /* CODEGEN_DRIVER_H */
