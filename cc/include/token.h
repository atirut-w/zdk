#ifndef TOKEN_H
#define TOKEN_H

/* C90-compliant token definitions for C parser */

enum TokenKind {
  T_EOF = 0,

  /* Literals and identifiers */
  T_IDENTIFIER = 256,
  T_TYPE_NAME,
  T_CONSTANT,
  T_STRING_LITERAL,

  /* Keywords */
  T_SIZEOF,
  T_TYPEDEF,
  T_EXTERN,
  T_STATIC,
  T_AUTO,
  T_REGISTER,
  T_CHAR,
  T_SHORT,
  T_INT,
  T_LONG,
  T_SIGNED,
  T_UNSIGNED,
  T_FLOAT,
  T_DOUBLE,
  T_CONST,
  T_VOLATILE,
  T_VOID,
  T_STRUCT,
  T_UNION,
  T_ENUM,
  T_ELLIPSIS,
  T_CASE,
  T_DEFAULT,
  T_IF,
  T_ELSE,
  T_SWITCH,
  T_WHILE,
  T_DO,
  T_FOR,
  T_GOTO,
  T_CONTINUE,
  T_BREAK,
  T_RETURN,

  /* Multi-char operators */
  T_PTR_OP,   /* -> */
  T_INC_OP,   /* ++ */
  T_DEC_OP,   /* -- */
  T_LEFT_OP,  /* << */
  T_RIGHT_OP, /* >> */
  T_LE_OP,    /* <= */
  T_GE_OP,    /* >= */
  T_EQ_OP,    /* == */
  T_NE_OP,    /* != */

  /* assignment ops */
  T_MUL_ASSIGN,   /* *= */
  T_DIV_ASSIGN,   /* /= */
  T_MOD_ASSIGN,   /* %= */
  T_ADD_ASSIGN,   /* += */
  T_SUB_ASSIGN,   /* -= */
  T_LEFT_ASSIGN,  /* <<= */
  T_RIGHT_ASSIGN, /* >>= */
  T_AND_ASSIGN,   /* &= */
  T_XOR_ASSIGN,   /* ^= */
  T_OR_ASSIGN,    /* |= */

  /* Logical ops */
  T_AND_OP, /* && */
  T_OR_OP   /* || */
};

/* Token value kinds for constants */
enum ConstKind { C_NONE = 0, C_INT, C_UINT, C_CHAR, C_DOUBLE, C_STRING };

struct Token {
  int kind;     /* enum TokenKind or single-char ASCII for punctuators */
  char *lexeme; /* owned string for identifiers and literals, else NULL */
  int line;
  int column;
  int const_kind; /* enum ConstKind when kind==T_CONSTANT or T_STRING_LITERAL */
};

#endif /* TOKEN_H */
