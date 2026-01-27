#include "lexer.h"
#include <stdlib.h>

Lexer *lexer_new(CompilationCtx *ctx, FILE *input) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  if (!lexer) {
    return NULL;
  }

  lexer->ctx = ctx;
  lexer->input = input;

  return lexer;
}

void lexer_free(Lexer *lexer) {
  if (lexer) {
    free(lexer);
  }
}

int lexer_next_token(Lexer *lexer, Token *token) {
  return 0; /* EOF */
}
