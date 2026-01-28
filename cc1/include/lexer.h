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
  fpos_t fpos;
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
 * Restores a lexer's state. Do note that restoring state will make the next
 * tokenization slower due to the need to reset buffer.
 */
void lexer_restore_state(Lexer *lexer, LexerState state);

#endif /* LEXER_H */
