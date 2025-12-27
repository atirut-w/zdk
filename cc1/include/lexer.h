#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    TOK_EOF,
    TOK_INT,
    TOK_RETURN,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

void lexer_init(FILE *input);
Token lexer_next_token(void);
void lexer_free(void);

#endif
