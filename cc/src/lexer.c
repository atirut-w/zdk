#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Simple character buffer for better position tracking */
static int lx_next(struct Lexer *lx) {
  int c = fgetc(lx->fp);
  if (c == '\n') {
    lx->line++;
    lx->col = 1;
  } else if (c != EOF) {
    lx->col++;
  }
  lx->cur = c;
  return c;
}

static int lx_peek(struct Lexer *lx) {
  int c = fgetc(lx->fp);
  if (c != EOF)
    ungetc(c, lx->fp);
  return c;
}

static void mark_position(struct Lexer *lx, int *line, int *col) {
  *line = lx->line;
  *col = lx->col;
}

static char *str_dup_range(const char *buf, size_t n) {
  char *s = (char *)malloc(n + 1);
  if (!s)
    return 0;
  memcpy(s, buf, n);
  s[n] = '\0';
  return s;
}

static int is_ident_start(int c) { return (c == '_') || isalpha(c); }

static int is_ident_part(int c) {
  return (c == '_') || isalpha(c) || isdigit(c);
}

static void token_set_pos(struct Token *t, struct Lexer *lx) {
  t->line = lx->line;
  t->column = lx->col;
}

static void token_clear(struct Token *t) {
  t->kind = 0;
  t->lexeme = 0;
  t->line = 0;
  t->column = 0;
  t->const_kind = C_NONE;
}

static int match_keyword(const char *s) {
  /* Return token kind for keyword or 0 */
  struct KW {
    const char *kw;
    int tk;
  };
  static const struct KW kws[] = {
      {"auto", T_AUTO},         {"break", T_BREAK},
      {"case", T_CASE},         {"char", T_CHAR},
      {"const", T_CONST},       {"continue", T_CONTINUE},
      {"default", T_DEFAULT},   {"do", T_DO},
      {"double", T_DOUBLE},     {"else", T_ELSE},
      {"enum", T_ENUM},         {"extern", T_EXTERN},
      {"float", T_FLOAT},       {"for", T_FOR},
      {"goto", T_GOTO},         {"if", T_IF},
      {"int", T_INT},           {"long", T_LONG},
      {"register", T_REGISTER}, {"return", T_RETURN},
      {"short", T_SHORT},       {"signed", T_SIGNED},
      {"sizeof", T_SIZEOF},     {"static", T_STATIC},
      {"struct", T_STRUCT},     {"switch", T_SWITCH},
      {"typedef", T_TYPEDEF},   {"union", T_UNION},
      {"unsigned", T_UNSIGNED}, {"void", T_VOID},
      {"volatile", T_VOLATILE}, {"while", T_WHILE}};
  size_t i;
  for (i = 0; i < sizeof(kws) / sizeof(kws[0]); i++) {
    if (strcmp(s, kws[i].kw) == 0)
      return kws[i].tk;
  }
  return 0;
}

static int read_number(struct Lexer *lx, int first, struct Token *out) {
  char buf[256];
  size_t n = 0;
  int c = first;
  int is_hex = 0;
  int has_dot = 0;
  int has_exp = 0;
  int has_f_suffix = 0;
  int has_u_suffix = 0;

  buf[n++] = (char)c;
  if (c == '0') {
    c = lx_next(lx);
    if (c == 'x' || c == 'X') {
      is_hex = 1;
      buf[n++] = (char)c;
      c = lx_next(lx);
    }
  } else {
    c = lx_next(lx);
  }

  while (1) {
    if (is_hex) {
      if (isxdigit(c)) {
        if (n < sizeof(buf) - 1)
          buf[n++] = (char)c;
        c = lx_next(lx);
      } else
        break;
    } else {
      if (isdigit(c)) {
        if (n < sizeof(buf) - 1)
          buf[n++] = (char)c;
        c = lx_next(lx);
      } else if (!has_dot && c == '.') {
        has_dot = 1;
        if (n < sizeof(buf) - 1)
          buf[n++] = (char)c;
        c = lx_next(lx);
      } else if (!has_exp && (c == 'e' || c == 'E')) {
        has_exp = 1;
        if (n < sizeof(buf) - 1)
          buf[n++] = (char)c;
        c = lx_next(lx);
        if (c == '+' || c == '-') {
          if (n < sizeof(buf) - 1)
            buf[n++] = (char)c;
          c = lx_next(lx);
        }
      } else
        break;
    }
  }

  /* integer/float suffixes (simplified) */
  while (isalpha(c)) {
    if (c == 'f' || c == 'F')
      has_f_suffix = 1;
    if (c == 'u' || c == 'U')
      has_u_suffix = 1;
    if (n < sizeof(buf) - 1)
      buf[n++] = (char)c;
    c = lx_next(lx);
  }

  ungetc(c, lx->fp);
  buf[n] = '\0';

  out->kind = T_CONSTANT;
  out->lexeme = str_dup_range(buf, n);
  if (has_dot || has_exp) {
    if (has_f_suffix)
      out->const_kind = C_FLOAT;
    else
      out->const_kind = C_DOUBLE;
  } else {
    out->const_kind = has_u_suffix ? C_UINT : C_INT;
  }
  return 1;
}

static int read_char_or_string(struct Lexer *lx, int quote, struct Token *out) {
  char buf[512];
  size_t n = 0;
  int c;
  int is_str = (quote == '"');
  while ((c = lx_next(lx)) != EOF) {
    if (c == '\\') {
      if (n < sizeof(buf) - 1)
        buf[n++] = (char)c;
      c = lx_next(lx);
      if (c == EOF)
        break;
      if (n < sizeof(buf) - 1)
        buf[n++] = (char)c;
    } else if (c == quote) {
      break;
    } else if (!is_str && c == '\n') {
      break; /* unterminated char literal */
    } else {
      if (n < sizeof(buf) - 1)
        buf[n++] = (char)c;
    }
  }
  buf[n] = '\0';
  if (is_str) {
    out->kind = T_STRING_LITERAL;
    out->const_kind = C_STRING;
  } else {
    out->kind = T_CONSTANT;
    out->const_kind = C_CHAR;
  }
  out->lexeme = str_dup_range(buf, n);
  return 1;
}

static void skip_whitespace_and_comments(struct Lexer *lx) {
  int c, d, prev;
  
  while (1) {
    c = lx_peek(lx);
    
    /* Skip whitespace */
    if (isspace(c)) {
      lx_next(lx);
      continue;
    }
    
    /* Check for comments */
    if (c == '/') {
      lx_next(lx);
      d = lx_peek(lx);
      
      if (d == '*') {
        /* C-style comment */
        lx_next(lx);
        prev = 0;
        while ((c = lx_next(lx)) != EOF) {
          if (prev == '*' && c == '/')
            break;
          prev = c;
        }
        continue;
      } else if (d == '/') {
        /* C++-style comment (not C90 but commonly supported) */
        lx_next(lx);
        while ((c = lx_next(lx)) != EOF && c != '\n')
          ;
        continue;
      } else {
        /* Just a '/', put it back by seeking back one char */
        ungetc(c, lx->fp);
        if (c == '\n') {
          lx->line--;
          lx->col = 1; /* approximate */
        } else {
          lx->col--;
        }
        return;
      }
    }
    
    /* Not whitespace or comment */
    return;
  }
}

static int read_operator_or_punct(struct Lexer *lx, int c, struct Token *out) {
  int d;
  switch (c) {
  case '.':
    d = lx_next(lx);
    if (d == '.') {
      int e = lx_next(lx);
      if (e == '.') {
        out->kind = T_ELLIPSIS;
        return 1;
      }
      ungetc(e, lx->fp);
    }
    ungetc(d, lx->fp);
    out->kind = '.';
    return 1;
  case '+':
    d = lx_next(lx);
    if (d == '+') {
      out->kind = T_INC_OP;
      return 1;
    }
    if (d == '=') {
      out->kind = T_ADD_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '+';
    return 1;
  case '-':
    d = lx_next(lx);
    if (d == '-') {
      out->kind = T_DEC_OP;
      return 1;
    }
    if (d == '>') {
      out->kind = T_PTR_OP;
      return 1;
    }
    if (d == '=') {
      out->kind = T_SUB_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '-';
    return 1;
  case '*':
    d = lx_next(lx);
    if (d == '=') {
      out->kind = T_MUL_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '*';
    return 1;
  case '/':
    d = lx_next(lx);
    if (d == '=') {
      out->kind = T_DIV_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '/';
    return 1;
  case '%':
    d = lx_next(lx);
    if (d == '=') {
      out->kind = T_MOD_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '%';
    return 1;
  case '<':
    d = lx_next(lx);
    if (d == '<') {
      int e = lx_next(lx);
      if (e == '=') {
        out->kind = T_LEFT_ASSIGN;
        return 1;
      }
      ungetc(e, lx->fp);
      out->kind = T_LEFT_OP;
      return 1;
    }
    if (d == '=') {
      out->kind = T_LE_OP;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '<';
    return 1;
  case '>':
    d = lx_next(lx);
    if (d == '>') {
      int e = lx_next(lx);
      if (e == '=') {
        out->kind = T_RIGHT_ASSIGN;
        return 1;
      }
      ungetc(e, lx->fp);
      out->kind = T_RIGHT_OP;
      return 1;
    }
    if (d == '=') {
      out->kind = T_GE_OP;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '>';
    return 1;
  case '=':
    d = lx_next(lx);
    if (d == '=') {
      out->kind = T_EQ_OP;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '=';
    return 1;
  case '!':
    d = lx_next(lx);
    if (d == '=') {
      out->kind = T_NE_OP;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '!';
    return 1;
  case '&':
    d = lx_next(lx);
    if (d == '&') {
      out->kind = T_AND_OP;
      return 1;
    }
    if (d == '=') {
      out->kind = T_AND_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '&';
    return 1;
  case '|':
    d = lx_next(lx);
    if (d == '|') {
      out->kind = T_OR_OP;
      return 1;
    }
    if (d == '=') {
      out->kind = T_OR_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '|';
    return 1;
  case '^':
    d = lx_next(lx);
    if (d == '=') {
      out->kind = T_XOR_ASSIGN;
      return 1;
    }
    ungetc(d, lx->fp);
    out->kind = '^';
    return 1;
  case '~':
    out->kind = '~';
    return 1;
  case '(':
  case ')':
  case '[':
  case ']':
  case '{':
  case '}':
  case ',':
  case ';':
  case ':':
  case '?':
    out->kind = c;
    return 1;
  default:
    return 0;
  }
}

void lexer_init(struct Lexer *lx, FILE *fp, const char *filename,
                struct Symbols *symbols) {
  lx->fp = fp;
  lx->filename = filename;
  lx->cur = 0;
  lx->line = 1;
  lx->col = 1;
  lx->peeked = 0;
  lx->symbols = symbols;
}

void lexer_destroy(struct Lexer *lx) { (void)lx; }

void token_free(struct Token *t) {
  if (t->lexeme)
    free(t->lexeme);
  token_clear(t);
}

static int next_token_inner(struct Lexer *lx, struct Token *out) {
  int c, start_line, start_col;
  token_clear(out);

  /* Skip whitespace and comments */
  skip_whitespace_and_comments(lx);
  
  /* Mark current position as token start */
  mark_position(lx, &start_line, &start_col);
  out->line = start_line;
  out->column = start_col;
  
  /* Read first character */
  c = lx_next(lx);
  if (c == EOF) {
    out->kind = T_EOF;
    return 1;
  }

  if (is_ident_start(c)) {
    char buf[256];
    size_t n = 0;
    buf[n++] = (char)c;
    while (1) {
      c = lx_next(lx);
      if (!is_ident_part(c)) {
        ungetc(c, lx->fp);
        break;
      }
      if (n < sizeof(buf) - 1)
        buf[n++] = (char)c;
    }
    buf[n] = '\0';
    out->lexeme = str_dup_range(buf, n);
    {
      int kw = match_keyword(out->lexeme);
      if (kw) {
        out->kind = kw;
      } else if (lx->symbols && symbols_is_typedef(lx->symbols, out->lexeme)) {
        out->kind = T_TYPE_NAME;
      } else {
        out->kind = T_IDENTIFIER;
      }
    }
    return 1;
  }

  if (isdigit(c)) {
    return read_number(lx, c, out);
  }

  if (c == '\'' || c == '"') {
    return read_char_or_string(lx, c, out);
  }

  if (read_operator_or_punct(lx, c, out))
    return 1;

  /* unknown char: return as single char token to avoid getting stuck */
  out->kind = c;
  return 1;
}

int lexer_next(struct Lexer *lx, struct Token *out) {
  if (lx->peeked) {
    *out = lx->peek_tok;
    lx->peeked = 0;
    return 1;
  }
  return next_token_inner(lx, out);
}

int lexer_peek(struct Lexer *lx, struct Token *out) {
  if (!lx->peeked) {
    if (!next_token_inner(lx, &lx->peek_tok))
      return 0;
    lx->peeked = 1;
  }
  *out = lx->peek_tok;
  return 1;
}
