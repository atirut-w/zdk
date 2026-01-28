#ifndef LEXER_H
#define LEXER_H

#include "compilation_ctx.h"
#include "token.h"
#include <stdio.h>

#define LEXER_BUFFER_SIZE 128

typedef struct {
  CompilationCtx *ctx;
  FILE *input;
  char buffer[LEXER_BUFFER_SIZE];
  char *cursor;
  char *limit;
  unsigned int line;
  unsigned int column;
} Lexer;

typedef struct {
  long int offset;
  unsigned int line;
  unsigned int column;
} LexerState;

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

/**
 * Saves a lexer's current state.
 */
LexerState lexer_save_state(Lexer *lexer);

/**
 * Restores a lexer's state. If the buffer state has changed since the last
 * save, the buffer will be reset. This makes the next tokenization slower.
 */
void lexer_restore_state(Lexer *lexer, LexerState state);

#endif /* LEXER_H */
