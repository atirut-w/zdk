#ifndef LEXER_H
#define LEXER_H

#include "compilation_ctx.h"
#include "token.h"
#include <stdio.h>

typedef struct {
  CompilationCtx *ctx;
  FILE *input;
} Lexer;

/**
 * Allocates and initializes a new lexer.
 */
Lexer *lexer_new(CompilationCtx *ctx, FILE *input);

/**
 * Frees a lexer.
 */
void lexer_free(Lexer *lexer);

/**
 * Gets the next token from the input stream. Returns 0 on EOF.
 */
int lexer_next_token(Lexer *lexer, Token *token);

#endif /* LEXER_H */
