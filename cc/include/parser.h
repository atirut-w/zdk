#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "symbols.h"

struct Parser {
  struct Lexer *lx;
  struct Symbols *symbols;
  struct Token cur;
  int has_cur;
  /* simple lookahead buffer */
  int la_count;       /* number of tokens currently buffered */
  struct Token la[8]; /* LA(1..8) */
};

void parser_init(struct Parser *p, struct Lexer *lx, struct Symbols *symbols);
void parser_destroy(struct Parser *p);
struct ASTNode *parse_translation_unit(struct Parser *p);

#endif /* PARSER_H */
