#pragma once
#include "token.h"
#include <stdio.h>

typedef struct Lexer {
  FILE *input;
  int putback;
  int line;
} Lexer;

/* Initialize the lexer */
void lexer_init(Lexer *lexer, FILE *input);
/* Destroy the lexer */
void lexer_destroy(Lexer *lexer);
/* Get the next character from the input */
int lexer_next(Lexer *lexer);
/* Put back a character to the input */
void lexer_putback(Lexer *lexer, int c);
/* Skip whitespace and return the first non-whitespace character */
int lexer_skip(Lexer *lexer);
/* Scan the next token from the input */
int lexer_scan(Lexer *lexer, Token *token);
