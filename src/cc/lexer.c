#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int chrpos(const char *s, int c) {
  char *p = strchr(s, c);
  return p ? p - s : -1;
}

int scanint(Lexer *lexer, int c) {
  int k, value = 0;

  while ((k = chrpos("0123456789", c)) >= 0) {
    value = value * 10 + k;
    c = lexer_next(lexer);
  }

  lexer_putback(lexer, c);
  return value;
}

void lexer_init(Lexer *lexer, FILE *input) {
  lexer->input = input;
  lexer->putback = 0;
  lexer->line = 1;
}

void lexer_destroy(Lexer *lexer) {
  fclose(lexer->input);
}

int lexer_next(Lexer *lexer) {
  int c;

  if (lexer->putback) {
    c = lexer->putback;
    lexer->putback = 0;
    return c;
  }

  c = fgetc(lexer->input);
  if (c == '\n') {
    lexer->line++;
  }
  return c;
}

void lexer_putback(Lexer *lexer, int c) {
  lexer->putback = c;
}

int lexer_skip(Lexer *lexer) {
  int c;

  c = lexer_next(lexer);
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') {
    c = lexer_next(lexer);
  }

  return c;
}

int lexer_scan(Lexer *lexer, Token *token) {
  int c;

  c = lexer_skip(lexer);
  switch(c) {
  case EOF:
    return 0;
  case '+':
    token->type = T_PLUS;
    break;
  case '-':
    token->type = T_MINUS;
    break;
  case '*':
    token->type = T_STAR;
    break;
  case '/':
    token->type = T_SLASH;
    break;
  default:
    if (isdigit(c)) {
      token->type = T_INTLIT;
      token->int_value = scanint(lexer, c);
      break;
    }

    fprintf(stderr, "Unknown character %c on line %d\n", c, lexer->line);
    exit(1);
  }

  return 1;
}
