#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
  /* Keywords */
  TOKEN_AUTO = 256, /* `auto` */
  TOKEN_BREAK,      /* `break` */
  TOKEN_CASE,       /* `case` */
  TOKEN_CHAR,       /* `char` */
  TOKEN_CONST,      /* `const` */
  TOKEN_CONTINUE,   /* `continue` */
  TOKEN_DEFAULT,    /* `default` */
  TOKEN_DO,         /* `do` */
  TOKEN_DOUBLE,     /* `double` */
  TOKEN_ELSE,       /* `else` */
  TOKEN_ENUM,       /* `enum` */
  TOKEN_EXTERN,     /* `extern` */
  TOKEN_FLOAT,      /* `float` */
  TOKEN_FOR,        /* `for` */
  TOKEN_GOTO,       /* `goto` */
  TOKEN_IF,         /* `if` */
  TOKEN_INT,        /* `int` */
  TOKEN_LONG,       /* `long` */
  TOKEN_REGISTER,   /* `register` */
  TOKEN_RETURN,     /* `return` */
  TOKEN_SHORT,      /* `short` */
  TOKEN_SIGNED,     /* `signed` */
  TOKEN_SIZEOF,     /* `sizeof` */
  TOKEN_STATIC,     /* `static` */
  TOKEN_STRUCT,     /* `struct` */
  TOKEN_SWITCH,     /* `switch` */
  TOKEN_TYPEDEF,    /* `typedef` */
  TOKEN_UNION,      /* `union` */
  TOKEN_UNSIGNED,   /* `unsigned` */
  TOKEN_VOID,       /* `void` */
  TOKEN_VOLATILE,   /* `volatile` */
  TOKEN_WHILE,      /* `while` */

  /* Idents, typedef names, constants, and string literals */
  TOKEN_IDENT,          /* Identifier */
  TOKEN_TYPE_NAME,      /* Typedef name */
  TOKEN_CONSTANT,       /* Integer or floating-point constant */
  TOKEN_STRING_LITERAL, /* String literal */

  /* Operators and punctuators */
  TOKEN_ELLIPSIS,     /* `...` */
  TOKEN_RIGHT_ASSIGN, /* `>>=` */
  TOKEN_LEFT_ASSIGN,  /* `<<=` */
  TOKEN_ADD_ASSIGN,   /* `+=` */
  TOKEN_SUB_ASSIGN,   /* `-=` */
  TOKEN_MUL_ASSIGN,   /* `*=` */
  TOKEN_DIV_ASSIGN,   /* `/=` */
  TOKEN_MOD_ASSIGN,   /* `%=` */
  TOKEN_AND_ASSIGN,   /* `&=` */
  TOKEN_XOR_ASSIGN,   /* `^=` */
  TOKEN_OR_ASSIGN,    /* `|=` */
  TOKEN_RIGHT_OP,     /* `>>` */
  TOKEN_LEFT_OP,      /* `<<` */
  TOKEN_INC_OP,       /* `++` */
  TOKEN_DEC_OP,       /* `--` */
  TOKEN_PTR_OP,       /* `->` */
  TOKEN_AND_OP,       /* `&&` */
  TOKEN_OR_OP,        /* `||` */
  TOKEN_LE_OP,        /* `<=` */
  TOKEN_GE_OP,        /* `>=` */
  TOKEN_EQ_OP,        /* `==` */
  TOKEN_NE_OP         /* `!=` */
} TokenKind;

typedef struct {
  /* Token kind, or the ASCII code of a single-character token */
  TokenKind kind;
  /* Optional interned lexeme */
  const char *lexeme;
  unsigned int line;
  unsigned int column;
} Token;

#endif /* TOKEN_H */
