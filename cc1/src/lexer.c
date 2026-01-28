#include "cc1/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "cc1/strpool.h"
#include "cc1/sym.h"

static int cc1_peek_raw(struct cc1_lexer *lx) {
  int c = fgetc(lx->in);
  if (c != EOF)
    ungetc(c, lx->in);
  return c;
}

static int cc1_lex_getc(struct cc1_lexer *lx) {
  int c = fgetc(lx->in);
  if (c == '\n') {
    lx->line += 1;
    lx->col = 0;
  } else if (c != EOF) {
    lx->col += 1;
  }
  return c;
}

static void cc1_lex_advance(struct cc1_lexer *lx) { lx->ch = cc1_lex_getc(lx); }

static struct cc1_loc cc1_lex_loc(struct cc1_lexer *lx) {
  struct cc1_loc loc;
  loc.path = lx->path;
  loc.line = lx->line;
  loc.col = lx->col ? lx->col : 1;
  return loc;
}

static void cc1_tokbuf_reset(struct cc1_lexer *lx) {
  lx->toklen = 0;
  lx->tokbuf[0] = 0;
}

static void cc1_tokbuf_push(struct cc1_lexer *lx, int c) {
  if (lx->toklen + 2 >= sizeof(lx->tokbuf))
    return;
  lx->tokbuf[lx->toklen++] = (char)c;
  lx->tokbuf[lx->toklen] = 0;
}

static void cc1_skip_ws_and_comments(struct cc1_lexer *lx) {
  for (;;) {
    while (lx->ch != EOF && isspace((unsigned char)lx->ch)) {
      cc1_lex_advance(lx);
    }
    if (lx->ch == '/') {
      int c1;
      c1 = cc1_peek_raw(lx);
      if (c1 == '*') {
        cc1_lex_advance(lx);
        cc1_lex_advance(lx);
        for (;;) {
          if (lx->ch == EOF)
            return;
          if (lx->ch == '*') {
            cc1_lex_advance(lx);
            if (lx->ch == '/') {
              cc1_lex_advance(lx);
              break;
            }
          } else {
            cc1_lex_advance(lx);
          }
        }
        continue;
      }
      if (c1 == '/') {
        /* non-C90, but useful */
        cc1_lex_advance(lx);
        cc1_lex_advance(lx);
        while (lx->ch != EOF && lx->ch != '\n')
          cc1_lex_advance(lx);
        continue;
      }
    }
    break;
  }
}

static enum cc1_tok_kind cc1_kw_kind(const char *s) {
  if (!strcmp(s, "auto"))
    return TOK_KW_AUTO;
  if (!strcmp(s, "break"))
    return TOK_KW_BREAK;
  if (!strcmp(s, "case"))
    return TOK_KW_CASE;
  if (!strcmp(s, "char"))
    return TOK_KW_CHAR;
  if (!strcmp(s, "const"))
    return TOK_KW_CONST;
  if (!strcmp(s, "continue"))
    return TOK_KW_CONTINUE;
  if (!strcmp(s, "default"))
    return TOK_KW_DEFAULT;
  if (!strcmp(s, "do"))
    return TOK_KW_DO;
  if (!strcmp(s, "double"))
    return TOK_KW_DOUBLE;
  if (!strcmp(s, "else"))
    return TOK_KW_ELSE;
  if (!strcmp(s, "enum"))
    return TOK_KW_ENUM;
  if (!strcmp(s, "extern"))
    return TOK_KW_EXTERN;
  if (!strcmp(s, "float"))
    return TOK_KW_FLOAT;
  if (!strcmp(s, "for"))
    return TOK_KW_FOR;
  if (!strcmp(s, "goto"))
    return TOK_KW_GOTO;
  if (!strcmp(s, "if"))
    return TOK_KW_IF;
  if (!strcmp(s, "int"))
    return TOK_KW_INT;
  if (!strcmp(s, "long"))
    return TOK_KW_LONG;
  if (!strcmp(s, "register"))
    return TOK_KW_REGISTER;
  if (!strcmp(s, "return"))
    return TOK_KW_RETURN;
  if (!strcmp(s, "short"))
    return TOK_KW_SHORT;
  if (!strcmp(s, "signed"))
    return TOK_KW_SIGNED;
  if (!strcmp(s, "sizeof"))
    return TOK_KW_SIZEOF;
  if (!strcmp(s, "static"))
    return TOK_KW_STATIC;
  if (!strcmp(s, "struct"))
    return TOK_KW_STRUCT;
  if (!strcmp(s, "switch"))
    return TOK_KW_SWITCH;
  if (!strcmp(s, "typedef"))
    return TOK_KW_TYPEDEF;
  if (!strcmp(s, "union"))
    return TOK_KW_UNION;
  if (!strcmp(s, "unsigned"))
    return TOK_KW_UNSIGNED;
  if (!strcmp(s, "void"))
    return TOK_KW_VOID;
  if (!strcmp(s, "volatile"))
    return TOK_KW_VOLATILE;
  if (!strcmp(s, "while"))
    return TOK_KW_WHILE;
  return TOK_IDENT;
}

static int cc1_is_ident_start(int c) {
  return isalpha((unsigned char)c) || c == '_';
}

static int cc1_is_ident(int c) { return isalnum((unsigned char)c) || c == '_'; }

static long cc1_parse_int(const char *s) {
  long v = 0;
  int base = 10;
  if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
    base = 16;
    s += 2;
  } else if (s[0] == '0') {
    base = 8;
    s += 1;
  }
  while (*s) {
    int d = -1;
    if (*s >= '0' && *s <= '9')
      d = *s - '0';
    else if (*s >= 'a' && *s <= 'f')
      d = 10 + (*s - 'a');
    else if (*s >= 'A' && *s <= 'F')
      d = 10 + (*s - 'A');
    else
      break;
    v = v * base + d;
    s++;
  }
  return v;
}

static int cc1_read_escape(struct cc1_lexer *lx) {
  int c = lx->ch;
  if (c == 'n')
    return '\n';
  if (c == 't')
    return '\t';
  if (c == 'r')
    return '\r';
  if (c == '\\')
    return '\\';
  if (c == '\'')
    return '\'';
  if (c == '"')
    return '"';
  return c;
}

void cc1_lex_init(struct cc1_lexer *lx, FILE *in, const char *path,
                  struct cc1_diag *diag, struct cc1_strpool *sp,
                  struct cc1_symtab *sym) {
  memset(lx, 0, sizeof(*lx));
  lx->in = in;
  lx->path = path;
  lx->diag = diag;
  lx->sp = sp;
  lx->sym = sym;
  lx->line = 1;
  lx->col = 0;
  lx->ch = cc1_lex_getc(lx);
}

struct cc1_tok cc1_lex_next(struct cc1_lexer *lx) {
  struct cc1_tok t;
  memset(&t, 0, sizeof(t));

  cc1_skip_ws_and_comments(lx);
  t.loc = cc1_lex_loc(lx);

  if (lx->ch == EOF) {
    t.kind = TOK_EOF;
    return t;
  }

  cc1_tokbuf_reset(lx);

  /* identifiers/keywords */
  if (cc1_is_ident_start(lx->ch)) {
    while (lx->ch != EOF && cc1_is_ident(lx->ch)) {
      cc1_tokbuf_push(lx, lx->ch);
      cc1_lex_advance(lx);
    }
    t.kind = cc1_kw_kind(lx->tokbuf);
    t.text = cc1_strpool_intern(lx->sp, lx->tokbuf, lx->toklen);
    if (t.kind == TOK_IDENT) {
      if (cc1_sym_is_typedef_visible(lx->sym, t.text)) {
        t.kind = TOK_TYPE_NAME;
      }
    }
    return t;
  }

  /* numbers */
  if (isdigit((unsigned char)lx->ch)) {
    while (lx->ch != EOF &&
           (isalnum((unsigned char)lx->ch) || lx->ch == 'x' || lx->ch == 'X')) {
      cc1_tokbuf_push(lx, lx->ch);
      cc1_lex_advance(lx);
    }
    t.kind = TOK_INT_LIT;
    t.ival = cc1_parse_int(lx->tokbuf);
    return t;
  }

  /* string literal */
  if (lx->ch == '"') {
    cc1_lex_advance(lx);
    while (lx->ch != EOF && lx->ch != '"') {
      int c = lx->ch;
      if (c == '\\') {
        cc1_lex_advance(lx);
        c = cc1_read_escape(lx);
      }
      cc1_tokbuf_push(lx, c);
      cc1_lex_advance(lx);
    }
    if (lx->ch == '"')
      cc1_lex_advance(lx);
    t.kind = TOK_STR_LIT;
    t.str = lx->tokbuf;
    t.strlen = lx->toklen;
    return t;
  }

  /* char literal */
  if (lx->ch == '\'') {
    int c;
    cc1_lex_advance(lx);
    c = lx->ch;
    if (c == '\\') {
      cc1_lex_advance(lx);
      c = cc1_read_escape(lx);
    }
    cc1_lex_advance(lx);
    if (lx->ch == '\'')
      cc1_lex_advance(lx);
    t.kind = TOK_CHAR_LIT;
    t.ival = (long)(unsigned char)c;
    return t;
  }

  /* operators/punctuators with lookahead */
  {
    int c = lx->ch;
    int n;
    cc1_lex_advance(lx);
    switch (c) {
    case '(':
      t.kind = TOK_LPAREN;
      return t;
    case ')':
      t.kind = TOK_RPAREN;
      return t;
    case '{':
      t.kind = TOK_LBRACE;
      return t;
    case '}':
      t.kind = TOK_RBRACE;
      return t;
    case '[':
      t.kind = TOK_LBRACKET;
      return t;
    case ']':
      t.kind = TOK_RBRACKET;
      return t;
    case ';':
      t.kind = TOK_SEMI;
      return t;
    case ',':
      t.kind = TOK_COMMA;
      return t;
    case '.':
      t.kind = TOK_DOT;
      return t;
    case '?':
      t.kind = TOK_QUESTION;
      return t;
    case ':':
      t.kind = TOK_COLON;
      return t;
    case '+':
      t.kind = TOK_PLUS;
      return t;
    case '*':
      t.kind = TOK_STAR;
      return t;
    case '/':
      t.kind = TOK_SLASH;
      return t;
    case '%':
      t.kind = TOK_PERCENT;
      return t;
    case '^':
      t.kind = TOK_CARET;
      return t;
    case '~':
      t.kind = TOK_TILDE;
      return t;
    case '!':
      if (lx->ch == '=') {
        cc1_lex_advance(lx);
        t.kind = TOK_NE;
      } else {
        t.kind = TOK_BANG;
      }
      return t;
    case '=':
      if (lx->ch == '=') {
        cc1_lex_advance(lx);
        t.kind = TOK_EQ;
      } else {
        t.kind = TOK_ASSIGN;
      }
      return t;
    case '<':
      if (lx->ch == '=') {
        cc1_lex_advance(lx);
        t.kind = TOK_LE;
        return t;
      }
      if (lx->ch == '<') {
        cc1_lex_advance(lx);
        t.kind = TOK_SHL;
        return t;
      }
      t.kind = TOK_LT;
      return t;
    case '>':
      if (lx->ch == '=') {
        cc1_lex_advance(lx);
        t.kind = TOK_GE;
        return t;
      }
      if (lx->ch == '>') {
        cc1_lex_advance(lx);
        t.kind = TOK_SHR;
        return t;
      }
      t.kind = TOK_GT;
      return t;
    case '&':
      if (lx->ch == '&') {
        cc1_lex_advance(lx);
        t.kind = TOK_ANDAND;
      } else {
        t.kind = TOK_AMP;
      }
      return t;
    case '|':
      if (lx->ch == '|') {
        cc1_lex_advance(lx);
        t.kind = TOK_OROR;
      } else {
        t.kind = TOK_PIPE;
      }
      return t;
    case '-':
      n = lx->ch;
      if (n == '>') {
        cc1_lex_advance(lx);
        t.kind = TOK_ARROW;
      } else {
        t.kind = TOK_MINUS;
      }
      return t;
    default:
      t.kind = TOK_EOF;
      return t;
    }
  }
}

const char *cc1_tok_kind_name(enum cc1_tok_kind k) {
  switch (k) {
  case TOK_EOF:
    return "eof";
  case TOK_IDENT:
    return "ident";
  case TOK_TYPE_NAME:
    return "type_name";
  case TOK_INT_LIT:
    return "int_lit";
  case TOK_CHAR_LIT:
    return "char_lit";
  case TOK_STR_LIT:
    return "str_lit";
  default:
    return "tok";
  }
}
