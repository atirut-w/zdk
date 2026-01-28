#ifndef CC1_LEXER_H
#define CC1_LEXER_H

#include <stdio.h>

#include "cc1/diag.h"

struct cc1_strpool;
struct cc1_symtab;

enum cc1_tok_kind {
  TOK_EOF = 0,

  TOK_IDENT,
  TOK_TYPE_NAME,

  TOK_INT_LIT,
  TOK_CHAR_LIT,
  TOK_STR_LIT,

  TOK_KW_AUTO,
  TOK_KW_BREAK,
  TOK_KW_CASE,
  TOK_KW_CHAR,
  TOK_KW_CONST,
  TOK_KW_CONTINUE,
  TOK_KW_DEFAULT,
  TOK_KW_DO,
  TOK_KW_DOUBLE,
  TOK_KW_ELSE,
  TOK_KW_ENUM,
  TOK_KW_EXTERN,
  TOK_KW_FLOAT,
  TOK_KW_FOR,
  TOK_KW_GOTO,
  TOK_KW_IF,
  TOK_KW_INT,
  TOK_KW_LONG,
  TOK_KW_REGISTER,
  TOK_KW_RETURN,
  TOK_KW_SHORT,
  TOK_KW_SIGNED,
  TOK_KW_SIZEOF,
  TOK_KW_STATIC,
  TOK_KW_STRUCT,
  TOK_KW_SWITCH,
  TOK_KW_TYPEDEF,
  TOK_KW_UNION,
  TOK_KW_UNSIGNED,
  TOK_KW_VOID,
  TOK_KW_VOLATILE,
  TOK_KW_WHILE,

  /* punctuators/operators */
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_LBRACE,
  TOK_RBRACE,
  TOK_LBRACKET,
  TOK_RBRACKET,
  TOK_SEMI,
  TOK_COMMA,
  TOK_DOT,
  TOK_ARROW,
  TOK_QUESTION,
  TOK_COLON,

  TOK_PLUS,
  TOK_MINUS,
  TOK_STAR,
  TOK_SLASH,
  TOK_PERCENT,

  TOK_AMP,
  TOK_PIPE,
  TOK_CARET,
  TOK_TILDE,
  TOK_BANG,

  TOK_ASSIGN,
  TOK_EQ,
  TOK_NE,
  TOK_LT,
  TOK_LE,
  TOK_GT,
  TOK_GE,

  TOK_ANDAND,
  TOK_OROR,

  TOK_SHL,
  TOK_SHR
};

struct cc1_tok {
  enum cc1_tok_kind kind;
  struct cc1_loc loc;

  const char *text; /* interned for ident/type_name, raw for string */
  long ival;

  /* For string literals only: points to lexer-owned buffer until next token. */
  const char *str;
  unsigned long strlen;
};

struct cc1_lexer {
  FILE *in;
  const char *path;
  struct cc1_diag *diag;
  struct cc1_strpool *sp;
  struct cc1_symtab *sym;

  int ch;
  unsigned long line;
  unsigned long col;

  char tokbuf[4096];
  unsigned long toklen;
};

void cc1_lex_init(struct cc1_lexer *lx, FILE *in, const char *path,
                  struct cc1_diag *diag, struct cc1_strpool *sp,
                  struct cc1_symtab *sym);

struct cc1_tok cc1_lex_next(struct cc1_lexer *lx);

const char *cc1_tok_kind_name(enum cc1_tok_kind k);

#endif
