#ifndef LEXER_H
#define LEXER_H

#include "symbols.h"
#include "token.h"
#include <stdio.h>

struct Lexer {
  FILE *fp;
  const char *filename;
  int cur;
  int line;
  int col;
  int peeked;
  struct Token peek_tok;
  struct Symbols *symbols;
};

void lexer_init(struct Lexer *lx, FILE *fp, const char *filename,
                struct Symbols *symbols);
void lexer_destroy(struct Lexer *lx);
int lexer_next(struct Lexer *lx, struct Token *out);
int lexer_peek(struct Lexer *lx, struct Token *out);
void token_free(struct Token *t);

#endif /* LEXER_H */
