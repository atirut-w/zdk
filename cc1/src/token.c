#include "token.h"

void token_init(Token *token, TokenKind kind, const char *lexeme,
                unsigned int line, unsigned int column) {
  token->kind = kind;
  token->lexeme = lexeme;
  token->line = line;
  token->column = column;
}
