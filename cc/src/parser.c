#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Minimal recursive-descent parser approximating the provided yacc grammar.
    Focus: expressions, simple declarations, function definitions, compound
   statements. Maintains typedef-name table to distinguish TYPE_NAME tokens.
*/

/* Forward declarations */
static struct ASTNode *parse_expression(struct Parser *p);
static struct ASTNode *parse_statement(struct Parser *p);
static struct ASTNode *parse_compound(struct Parser *p);
static struct ASTNode *parse_declaration(struct Parser *p, int *out_typedef);
static struct ASTNode *parse_initializer(struct Parser *p);
static struct Declarator *parse_declarator_shape(struct Parser *p,
                                                 char **out_name);
static int parse_decl_specifiers(struct Parser *p, int *out_spec_flags,
                                 int *out_is_typedef);
static struct ASTNode *parse_constant_expression(struct Parser *p);

static void parser_error(struct Parser *p, const char *msg) {
  fprintf(stderr, "Parse error at %d:%d: %s\n", p->cur.line, p->cur.column,
          msg);
}

/* Lookahead buffer helpers */
static int la_ensure(struct Parser *p, int n) {
  while (p->la_count < n &&
         p->la_count < (int)(sizeof(p->la) / sizeof(p->la[0]))) {
    struct Token t;
    if (!lexer_next(p->lx, &t))
      break;
    p->la[p->la_count++] = t;
  }
  return p->la_count >= n;
}

static int la_peek_kind(struct Parser *p, int n) {
  if (!la_ensure(p, n))
    return T_EOF;
  return p->la[n - 1].kind;
}

static int next(struct Parser *p) {
  int i;
  if (p->la_count == 0) {
    if (!la_ensure(p, 1))
      return 0;
  }
  p->cur = p->la[0];
  for (i = 1; i < p->la_count; i++) {
    p->la[i - 1] = p->la[i];
  }
  if (p->la_count > 0)
    p->la_count--;
  p->has_cur = 1;
  return 1;
}

static int accept(struct Parser *p, int kind) {
  if (!p->has_cur)
    next(p);
  if (p->cur.kind == kind) {
    next(p);
    return 1;
  }
  return 0;
}

static void sync_until(struct Parser *p, int t1, int t2) {
  int k;
  int steps = 0;
  for (;;) {
    if (!p->has_cur) {
      if (!next(p))
        return;
    }
    k = p->cur.kind;
    if (k == t1 || (t2 && k == t2) || k == '}' || k == T_EOF) {
      next(p);
      return;
    }
    next(p);
    steps++;
    if (steps > 10000)
      return; /* safety */
  }
}

static int expect(struct Parser *p, int kind, const char *what) {
  if (!accept(p, kind)) {
    parser_error(p, what);
    /* crude recovery depending on what was expected */
    if (kind == ';') {
      sync_until(p, ';', 0);
    } else if (kind == ')') {
      sync_until(p, ')', ';');
    } else if (kind == ']') {
      sync_until(p, ']', ';');
    } else if (kind == '}') {
      sync_until(p, '}', 0);
    } else {
      /* consume one token to make progress */
      if (!p->has_cur)
        next(p);
      else
        next(p);
    }
    return 0;
  }
  return 1;
}

static struct ASTNode *parse_expression(struct Parser *p);
static struct ASTNode *parse_statement(struct Parser *p);
static struct ASTNode *parse_compound(struct Parser *p);
static struct ASTNode *parse_declaration(struct Parser *p, int *out_typedef);

static struct ASTNode *make_ident_node(struct Token *t) {
  return ast_new_expr_ident(t->lexeme, t->line, t->column);
}

static int is_type_spec_token(int k) {
  switch (k) {
  case T_VOID:
  case T_CHAR:
  case T_SHORT:
  case T_INT:
  case T_LONG:
  case T_FLOAT:
  case T_DOUBLE:
  case T_SIGNED:
  case T_UNSIGNED:
  case T_CONST:
  case T_VOLATILE:
  case T_TYPE_NAME:
  case T_STRUCT:
  case T_UNION:
  case T_ENUM:
    return 1;
  default:
    return 0;
  }
}

static int is_type_start_token(int k) {
  switch (k) {
  case T_VOID:
  case T_CHAR:
  case T_SHORT:
  case T_INT:
  case T_LONG:
  case T_FLOAT:
  case T_DOUBLE:
  case T_SIGNED:
  case T_UNSIGNED:
  case T_CONST:
  case T_VOLATILE:
  case T_TYPE_NAME:
  case T_STRUCT:
  case T_UNION:
  case T_ENUM:
    return 1;
  default:
    return 0;
  }
}

/* heuristic: after '(' decide if a type-name starts and ends with a ')' */
static int looks_like_type_name(struct Parser *p) {
  /* After consuming '(' current token is first token inside parens. */
  if (!p->has_cur)
    next(p);
  if (is_type_start_token(p->cur.kind) || p->cur.kind == '*')
    return 1;
  /* Simple extra case: if next lookahead is TYPE_NAME treat as type-name */
  if (la_peek_kind(p, 1) == T_TYPE_NAME)
    return 1;
  return 0;
}

/* Consume a type-name after '(' when looks_like_type_name() returned true. */
static void consume_type_name(struct Parser *p) {
  if (!p->has_cur)
    next(p);
  /* consume specifiers/qualifiers and typedef names from current tokens */
  while (is_type_start_token(p->cur.kind)) {
    if (p->cur.kind == T_STRUCT || p->cur.kind == T_UNION) {
      next(p);
      if (!p->has_cur)
        next(p);
      if (p->cur.kind == T_IDENTIFIER || p->cur.kind == T_TYPE_NAME) {
        next(p);
      }
      /* handle struct/union definition in cast */
      if (accept(p, '{')) {
        /* skip the entire struct body */
        int depth = 1;
        while (depth > 0) {
          if (!p->has_cur)
            next(p);
          if (p->cur.kind == T_EOF)
            break;
          if (p->cur.kind == '{')
            depth++;
          if (p->cur.kind == '}')
            depth--;
          next(p);
        }
      }
    } else if (p->cur.kind == T_ENUM) {
      next(p);
      if (!p->has_cur)
        next(p);
      if (p->cur.kind == T_IDENTIFIER || p->cur.kind == T_TYPE_NAME) {
        next(p);
      }
    } else {
      next(p);
    }
    if (!p->has_cur)
      next(p);
  }
  while (p->cur.kind == '*') {
    next(p);
    if (!p->has_cur)
      next(p);
  }
}

static int spec_flag_for(int k) {
  switch (k) {
  case T_TYPEDEF:
    return SPF_TYPEDEF;
  case T_EXTERN:
    return SPF_EXTERN;
  case T_STATIC:
    return SPF_STATIC;
  case T_AUTO:
    return SPF_AUTO;
  case T_REGISTER:
    return SPF_REGISTER;
  case T_VOID:
    return SPF_VOID;
  case T_CHAR:
    return SPF_CHAR;
  case T_SHORT:
    return SPF_SHORT;
  case T_INT:
    return SPF_INT;
  case T_LONG:
    return SPF_LONG;
  case T_SIGNED:
    return SPF_SIGNED;
  case T_UNSIGNED:
    return SPF_UNSIGNED;
  case T_FLOAT:
    return SPF_FLOAT;
  case T_DOUBLE:
    return SPF_DOUBLE;
  case T_CONST:
    return SPF_CONST;
  case T_VOLATILE:
    return SPF_VOLATILE;
  default:
    return 0;
  }
}

/* helpers for declarators and parameters per C90 grammar */
static void parse_type_qualifier_list(struct Parser *p) {
  while (1) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind == T_CONST || p->cur.kind == T_VOLATILE) {
      next(p);
      continue;
    }
    break;
  }
}

static int parse_pointer(struct Parser *p) {
  int level = 0;
  while (accept(p, '*')) {
    level++;
    parse_type_qualifier_list(p);
  }
  return level;
}

static int starts_declaration(struct Parser *p) {
  int k;
  if (!p->has_cur)
    next(p);
  k = p->cur.kind;
  if (k == T_TYPEDEF || k == T_EXTERN || k == T_STATIC || k == T_AUTO ||
      k == T_REGISTER)
    return 1;
  if (k == T_CONST || k == T_VOLATILE)
    return 1;
  if (is_type_start_token(k))
    return 1;
  return 0;
}

/* struct_or_union_specifier:
   struct_or_union IDENTIFIER '{' struct_declaration_list '}'
 | struct_or_union '{' struct_declaration_list '}'
 | struct_or_union IDENTIFIER
 */
static void parse_struct_or_union_specifier(struct Parser *p) {
  int k;
  /* assumes current token is T_STRUCT or T_UNION */
  k = p->cur.kind;
  if (k != T_STRUCT && k != T_UNION) {
    parser_error(p, "struct or union expected");
    return;
  }
  next(p);

  if (!p->has_cur)
    next(p);
  if (p->cur.kind == T_IDENTIFIER) {
    next(p); /* tag name */
  }

  if (accept(p, '{')) {
    /* struct_declaration_list */
    while (1) {
      int flags = 0;
      int is_td = 0;
      if (!p->has_cur)
        next(p);
      if (p->cur.kind == '}') {
        next(p);
        break;
      }
      if (p->cur.kind == T_EOF) {
        parser_error(p, "unexpected EOF in struct/union");
        break;
      }
      /* struct_declaration: specifier_qualifier_list struct_declarator_list ';'
       */
      if (!parse_decl_specifiers(p, &flags, &is_td)) {
        parser_error(p, "type specifier expected in struct member");
        sync_until(p, ';', '}');
        if (p->cur.kind == '}') {
          next(p);
          break;
        }
        continue;
      }
      /* struct_declarator_list: one or more declarators/bit-fields separated by
       * ',' */
      while (1) {
        if (accept(p, ':')) {
          /* unnamed bit-field */
          (void)parse_constant_expression(p);
        } else {
          /* full declarator (can be pointers, function pointers, arrays, etc.)
           */
          struct Declarator *md = parse_declarator_shape(p, 0);
          if (!md) {
            /* sync to next ',' or ';' to continue */
            sync_until(p, ',', ';');
          } else {
            /* optional bit-field width */
            if (accept(p, ':')) {
              (void)parse_constant_expression(p);
            }
          }
        }
        if (accept(p, ',')) {
          continue;
        }
        break;
      }
      expect(p, ';', "; expected after struct member");
    }
  }
}

/* enum_specifier:
   ENUM '{' enumerator_list '}'
 | ENUM IDENTIFIER '{' enumerator_list '}'
 | ENUM IDENTIFIER
 */
static void parse_enum_specifier(struct Parser *p) {
  /* assumes current token is T_ENUM */
  expect(p, T_ENUM, "enum expected");
  if (p->cur.kind == T_IDENTIFIER) {
    next(p); /* tag name */
  }
  if (accept(p, '{')) {
    /* enumerator_list */
    for (;;) {
      if (!p->has_cur)
        next(p);
      if (p->cur.kind == '}') {
        next(p);
        break;
      }
      if (p->cur.kind != T_IDENTIFIER) {
        parser_error(p, "enumerator identifier expected");
        sync_until(p, ',', '}');
        if (p->cur.kind == '}') {
          next(p);
          break;
        }
      } else {
        next(p);
        if (accept(p, '=')) {
          /* constant_expression */
          (void)parse_constant_expression(p);
        }
        if (accept(p, ',')) {
          if (accept(p, '}'))
            break; /* allow trailing comma */
          continue;
        }
      }
    }
  }
}

/* Forward decls for expression precedence */
static struct ASTNode *parse_assignment(struct Parser *p);
static struct ASTNode *parse_conditional(struct Parser *p);
static struct ASTNode *parse_logical_or(struct Parser *p);
static struct ASTNode *parse_logical_and(struct Parser *p);
static struct ASTNode *parse_inclusive_or(struct Parser *p);
static struct ASTNode *parse_exclusive_or(struct Parser *p);
static struct ASTNode *parse_and(struct Parser *p);
static struct ASTNode *parse_equality(struct Parser *p);
static struct ASTNode *parse_relational(struct Parser *p);
static struct ASTNode *parse_shift(struct Parser *p);
static struct ASTNode *parse_additive(struct Parser *p);
static struct ASTNode *parse_multiplicative(struct Parser *p);
static struct ASTNode *parse_cast(struct Parser *p);
static struct ASTNode *parse_unary(struct Parser *p);
static struct ASTNode *parse_postfix(struct Parser *p);
static struct ASTNode *parse_primary(struct Parser *p);
/* constant_expression per grammar: conditional_expression (no comma
 * expressions) */
static struct ASTNode *parse_constant_expression(struct Parser *p) {
  return parse_conditional(p);
}

static int token_is_assignment(int k) {
  switch (k) {
  case '=':
  case T_MUL_ASSIGN:
  case T_DIV_ASSIGN:
  case T_MOD_ASSIGN:
  case T_ADD_ASSIGN:
  case T_SUB_ASSIGN:
  case T_LEFT_ASSIGN:
  case T_RIGHT_ASSIGN:
  case T_AND_ASSIGN:
  case T_XOR_ASSIGN:
  case T_OR_ASSIGN:
    return 1;
  default:
    return 0;
  }
}

static struct ASTNode *parse_primary(struct Parser *p) {
  struct Token t;
  if (!p->has_cur)
    next(p);
  t = p->cur;
  if (t.kind == T_IDENTIFIER) {
    next(p);
    return make_ident_node(&t);
  }
  if (t.kind == T_CONSTANT) {
    next(p);
    return ast_new_expr_const(&t);
  }
  if (t.kind == T_STRING_LITERAL) {
    next(p);
    return ast_new_expr_string(t.lexeme, t.line, t.column);
  }
  if (t.kind == '(') {
    next(p);
    {
      struct ASTNode *e = parse_expression(p);
      expect(p, ')', ") expected");
      return e;
    }
  }
  parser_error(p, "primary expression expected");
  return 0;
}

/* argument list consumed in parse_postfix directly */

static struct ASTNode *parse_postfix(struct Parser *p) {
  struct ASTNode *e = parse_primary(p);
  while (e) {
    if (accept(p, '[')) {
      struct ASTNode *idx = parse_constant_expression(p);
      expect(p, ']', "] expected");
      e = ast_new_expr_index(e, idx, p->cur.line, p->cur.column);
      continue;
    }
    if (accept(p, '(')) {
      if (accept(p, ')')) {
        e = ast_new_expr_call(e, 0, p->cur.line, p->cur.column);
      } else {
        struct ASTList *args = 0;
        struct ASTNode *a;
        a = parse_assignment(p);
        args = ast_list_append(args, a);
        while (accept(p, ',')) {
          a = parse_assignment(p);
          args = ast_list_append(args, a);
        }
        expect(p, ')', ") expected");
        e = ast_new_expr_call(e, args, p->cur.line, p->cur.column);
      }
      continue;
    }
    if (accept(p, '.')) {
      struct Token id;
      if (!p->has_cur)
        next(p);
      id = p->cur;
      expect(p, T_IDENTIFIER, "identifier after .");
      e = ast_new_expr_member(e, id.lexeme, 0, id.line, id.column);
      continue;
    }
    if (accept(p, T_PTR_OP)) {
      struct Token id;
      if (!p->has_cur)
        next(p);
      id = p->cur;
      expect(p, T_IDENTIFIER, "identifier after ->");
      e = ast_new_expr_member(e, id.lexeme, 1, id.line, id.column);
      continue;
    }
    if (accept(p, T_INC_OP)) {
      e = ast_new_expr_unary(T_INC_OP, e, p->cur.line, p->cur.column);
      continue;
    }
    if (accept(p, T_DEC_OP)) {
      e = ast_new_expr_unary(T_DEC_OP, e, p->cur.line, p->cur.column);
      continue;
    }
    break;
  }
  return e;
}

static struct ASTNode *parse_unary(struct Parser *p) {
  if (accept(p, T_INC_OP)) {
    struct ASTNode *e = parse_unary(p);
    return ast_new_expr_unary(T_INC_OP, e, p->cur.line, p->cur.column);
  }
  if (accept(p, T_DEC_OP)) {
    struct ASTNode *e = parse_unary(p);
    return ast_new_expr_unary(T_DEC_OP, e, p->cur.line, p->cur.column);
  }
  if (accept(p, '&')) {
    struct ASTNode *e = parse_cast(p);
    return ast_new_expr_unary('&', e, p->cur.line, p->cur.column);
  }
  if (accept(p, '*')) {
    struct ASTNode *e = parse_cast(p);
    return ast_new_expr_unary('*', e, p->cur.line, p->cur.column);
  }
  if (accept(p, '+')) {
    struct ASTNode *e = parse_cast(p);
    return ast_new_expr_unary('+', e, p->cur.line, p->cur.column);
  }
  if (accept(p, '-')) {
    struct ASTNode *e = parse_cast(p);
    return ast_new_expr_unary('-', e, p->cur.line, p->cur.column);
  }
  if (accept(p, '~')) {
    struct ASTNode *e = parse_cast(p);
    return ast_new_expr_unary('~', e, p->cur.line, p->cur.column);
  }
  if (accept(p, '!')) {
    struct ASTNode *e = parse_cast(p);
    return ast_new_expr_unary('!', e, p->cur.line, p->cur.column);
  }
  if (accept(p, T_SIZEOF)) {
    if (accept(p, '(')) {
      if (looks_like_type_name(p)) {
        /* consume type-name tokens from current stream */
        consume_type_name(p);
        expect(p, ')', ") expected");
        return ast_new_expr_unary(T_SIZEOF, 0, p->cur.line, p->cur.column);
      } else {
        struct ASTNode *e = parse_expression(p);
        expect(p, ')', ") expected");
        return ast_new_expr_unary(T_SIZEOF, e, p->cur.line, p->cur.column);
      }
    } else {
      struct ASTNode *e = parse_unary(p);
      return ast_new_expr_unary(T_SIZEOF, e, p->cur.line, p->cur.column);
    }
  }
  return parse_postfix(p);
}

static struct ASTNode *bin_chain(struct Parser *p,
                                 struct ASTNode *(*sub)(struct Parser *),
                                 int ops1, int ops2) {
  struct ASTNode *lhs = sub(p);
  while (lhs) {
    int k;
    if (!p->has_cur)
      next(p);
    k = p->cur.kind;
    if (!(k == ops1 || k == ops2))
      break;
    next(p);
    {
      struct ASTNode *rhs = sub(p);
      lhs = ast_new_expr_binary(k, lhs, rhs, p->cur.line, p->cur.column);
    }
  }
  return lhs;
}

static struct ASTNode *parse_cast(struct Parser *p) {
  if (accept(p, '(')) {
    if (looks_like_type_name(p)) {
      consume_type_name(p);
      expect(p, ')', ") expected");
      return parse_cast(p);
    } else {
      struct ASTNode *e = parse_expression(p);
      expect(p, ')', ") expected");
      return e;
    }
  }
  return parse_unary(p);
}
static struct ASTNode *parse_multiplicative(struct Parser *p) {
  struct ASTNode *lhs = parse_cast(p);
  int k;
  while (lhs) {
    if (!p->has_cur)
      next(p);
    k = p->cur.kind;
    if (k != '*' && k != '/' && k != '%')
      break;
    next(p);
    lhs =
        ast_new_expr_binary(k, lhs, parse_cast(p), p->cur.line, p->cur.column);
  }
  return lhs;
}
static struct ASTNode *parse_additive(struct Parser *p) {
  struct ASTNode *lhs = parse_multiplicative(p);
  int k;
  while (lhs) {
    if (!p->has_cur)
      next(p);
    k = p->cur.kind;
    if (k != '+' && k != '-')
      break;
    next(p);
    lhs = ast_new_expr_binary(k, lhs, parse_multiplicative(p), p->cur.line,
                              p->cur.column);
  }
  return lhs;
}
static struct ASTNode *parse_shift(struct Parser *p) {
  struct ASTNode *lhs = parse_additive(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != T_LEFT_OP && p->cur.kind != T_RIGHT_OP)
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_additive(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_relational(struct Parser *p) {
  struct ASTNode *lhs = parse_shift(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != '<' && p->cur.kind != '>' && p->cur.kind != T_LE_OP &&
        p->cur.kind != T_GE_OP)
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_shift(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_equality(struct Parser *p) {
  struct ASTNode *lhs = parse_relational(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != T_EQ_OP && p->cur.kind != T_NE_OP)
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_relational(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_and(struct Parser *p) {
  struct ASTNode *lhs = parse_equality(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != '&')
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_equality(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_exclusive_or(struct Parser *p) {
  struct ASTNode *lhs = parse_and(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != '^')
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_and(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_inclusive_or(struct Parser *p) {
  struct ASTNode *lhs = parse_exclusive_or(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != '|')
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_exclusive_or(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_logical_and(struct Parser *p) {
  struct ASTNode *lhs = parse_inclusive_or(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != T_AND_OP)
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_inclusive_or(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}
static struct ASTNode *parse_logical_or(struct Parser *p) {
  struct ASTNode *lhs = parse_logical_and(p);
  while (lhs) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != T_OR_OP)
      break;
    {
      int op = p->cur.kind;
      next(p);
      lhs = ast_new_expr_binary(op, lhs, parse_logical_and(p), p->cur.line,
                                p->cur.column);
    }
  }
  return lhs;
}

static struct ASTNode *parse_conditional(struct Parser *p) {
  struct ASTNode *c = parse_logical_or(p);
  if (accept(p, '?')) {
    struct ASTNode *t = parse_expression(p);
    expect(p, ':', ": expected");
    {
      struct ASTNode *f = parse_conditional(p);
      return ast_new_expr_cond(c, t, f, p->cur.line, p->cur.column);
    }
  }
  return c;
}

static struct ASTNode *parse_assignment(struct Parser *p) {
  struct ASTNode *l = parse_conditional(p);
  if (!p->has_cur)
    next(p);
  if (token_is_assignment(p->cur.kind)) {
    int op = p->cur.kind;
    next(p);
    {
      struct ASTNode *r = parse_assignment(p);
      return ast_new_expr_binary(op, l, r, p->cur.line, p->cur.column);
    }
  }
  return l;
}

static struct ASTNode *parse_expression(struct Parser *p) {
  struct ASTNode *e = parse_assignment(p);
  while (accept(p, ',')) {
    e = ast_new_expr_binary(',', e, parse_assignment(p), p->cur.line,
                            p->cur.column);
  }
  return e;
}

/* Declarations (very simplified): declaration_specifiers declarator ['='
 * initializer] ';' */

static int parse_decl_specifiers(struct Parser *p, int *out_spec_flags,
                                 int *out_is_typedef) {
  int flags = 0;
  int any = 0;
  int td = 0;
  int k;
  for (;;) {
    if (!p->has_cur)
      next(p);
    k = p->cur.kind;
    if (k == T_TYPEDEF) {
      flags |= spec_flag_for(k);
      td = 1;
      any = 1;
      next(p);
      continue;
    }
    if (k == T_ENUM) {
      any = 1;
      parse_enum_specifier(p);
      continue;
    }
    if (k == T_STRUCT || k == T_UNION) {
      any = 1;
      parse_struct_or_union_specifier(p);
      continue;
    }
    if (k == T_EXTERN || k == T_STATIC || k == T_AUTO || k == T_REGISTER ||
        k == T_CONST || k == T_VOLATILE || k == T_VOID || k == T_CHAR ||
        k == T_SHORT || k == T_INT || k == T_LONG || k == T_FLOAT ||
        k == T_DOUBLE || k == T_SIGNED || k == T_UNSIGNED || k == T_TYPE_NAME) {
      flags |= spec_flag_for(k);
      any = 1;
      next(p);
      continue;
    }
    break;
  }
  if (out_spec_flags)
    *out_spec_flags = flags;
  if (out_is_typedef)
    *out_is_typedef = td;
  return any;
}

static int parse_param_list(struct Parser *p) {
  /* Very simplified parameter parser: ( decl-specifiers [*]* [identifier] (','
   * ...)? ) ')' */
  if (accept(p, ')'))
    return 1;
  for (;;) {
    int flags = 0, is_td = 0;
    if (!parse_decl_specifiers(p, &flags, &is_td)) {
      parser_error(p, "parameter declaration expected");
      /* try to sync to next ',' or ')' */
      sync_until(p, ',', ')');
      if (p->cur.kind == ')') {
        next(p);
        return 1;
      }
    }
    while (accept(p, '*')) { /* pointer levels */
    }
    if (!p->has_cur)
      next(p);
    /* Handle function pointer: (*name) or (*name)(...) */
    if (accept(p, '(')) {
      if (accept(p, '*')) {
        if (!p->has_cur)
          next(p);
        if (p->cur.kind == T_IDENTIFIER) {
          next(p); /* parameter name */
        }
        expect(p, ')', ") expected in function pointer");
        /* Handle function pointer parameters (...) */
        if (accept(p, '(')) {
          /* Skip parameter list */
          int depth = 1;
          while (depth > 0 && p->has_cur) {
            if (!p->has_cur)
              next(p);
            if (p->cur.kind == T_EOF)
              break;
            if (p->cur.kind == '(')
              depth++;
            if (p->cur.kind == ')')
              depth--;
            next(p);
          }
        }
      } else {
        /* Not a function pointer, backtrack by skipping to ',' or ')' */
        sync_until(p, ',', ')');
        if (p->cur.kind == ')')
          return 1;
        if (p->cur.kind == ',') {
          next(p);
          continue;
        }
      }
    } else if (p->cur.kind == T_IDENTIFIER || p->cur.kind == T_TYPE_NAME) {
      next(p);
    }
    /* Skip array suffixes like [] or [const] */
    while (accept(p, '[')) {
      if (!accept(p, ']')) {
        (void)parse_expression(p);
        expect(p, ']', "] expected");
      }
    }
    if (!accept(p, ','))
      break;
    if (accept(p, ')'))
      return 1; /* allow trailing comma error recovery */
  }
  expect(p, ')', ") expected");
  return 1;
}

/* Forward for declarator (prototype moved to top) */

static int parse_identifier_list(struct Parser *p) {
  int one = 0;
  if (!p->has_cur)
    next(p);
  if (p->cur.kind != T_IDENTIFIER)
    return 0;
  next(p);
  one = 1;
  while (accept(p, ',')) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind != T_IDENTIFIER) {
      parser_error(p, "identifier expected in identifier-list");
      break;
    }
    next(p);
  }
  return one;
}

static int parse_parameter_type_list(struct Parser *p) {
  /* parameter_list [ , ... ] */
  int has_any = 0;
  if (accept(p, ')'))
    return 1; /* empty */
  for (;;) {
    int flags = 0, is_td = 0;
    if (!parse_decl_specifiers(p, &flags, &is_td)) {
      /* Maybe it's an identifier-list style, back out by syncing */
      return 0;
    }
    /* optional declarator or abstract_declarator (very simplified) */
    while (accept(p, '*')) {
      parse_type_qualifier_list(p);
    }
    if (!p->has_cur)
      next(p);
    if (p->cur.kind == T_IDENTIFIER) {
      next(p);
    } else if (accept(p, '(')) {
      /* abstract or named: skip balanced parens content simplistically */
      int depth = 1;
      while (depth > 0) {
        if (!p->has_cur)
          next(p);
        if (p->cur.kind == T_EOF)
          break;
        if (p->cur.kind == '(')
          depth++;
        else if (p->cur.kind == ')')
          depth--;
        next(p);
      }
    }
    /* array suffixes allowed */
    while (accept(p, '[')) {
      if (!accept(p, ']')) {
        (void)parse_expression(p);
        expect(p, ']', "] expected");
      }
    }
    has_any = 1;
    if (accept(p, ',')) {
      if (accept(p, T_ELLIPSIS)) {
        expect(p, ')', ") expected");
        return 1;
      }
      continue;
    }
    break;
  }
  expect(p, ')', ") expected");
  return has_any;
}

static struct Declarator *parse_direct_declarator(struct Parser *p,
                                                  char **out_name) {
  struct Declarator *d = 0;
  struct Token id;
  if (!p->has_cur)
    next(p);
  if (p->cur.kind == T_IDENTIFIER) {
    id = p->cur;
    next(p);
    if (out_name)
      *out_name = id.lexeme;
    d = ast_new_declarator(id.lexeme);
  } else if (accept(p, '(')) {
    /* '(' declarator ')' */
    d = parse_declarator_shape(p, out_name);
    expect(p, ')', ") expected");
  } else {
    parser_error(p, "identifier or '(' expected in declarator");
    return 0;
  }
  if (!d)
    return 0;
  /* handle suffixes */
  for (;;) {
    if (accept(p, '[')) {
      if (!accept(p, ']')) {
        struct ASTNode *sz = parse_constant_expression(p);
        expect(p, ']', "] expected");
        ast_set_declarator_array(d, sz);
      } else {
        ast_set_declarator_array(d, 0);
      }
      continue;
    }
    if (accept(p, '(')) {
      /* parameter_type_list | identifier_list | nothing */
      if (parse_parameter_type_list(p)) {
        d->is_function = 1;
      } else if (parse_identifier_list(p)) {
        expect(p, ')', ") expected");
        d->is_function = 1;
      } else if (accept(p, ')')) {
        d->is_function = 1;
      } else {
        /* sync until ')' */
        sync_until(p, ')', 0);
        d->is_function = 1;
      }
      continue;
    }
    break;
  }
  return d;
}

static struct Declarator *parse_declarator_shape(struct Parser *p,
                                                 char **out_name) {
  int ptr = 0;
  struct Declarator *d;
  ptr = parse_pointer(p);
  d = parse_direct_declarator(p, out_name);
  if (!d)
    return 0;
  ast_set_declarator_pointer(d, ptr);
  return d;
}

static struct ASTNode *parse_initializer_list(struct Parser *p) {
  /* initializer (',' initializer)* [','] */
  struct ASTList *items = 0;
  struct ASTNode *it = parse_initializer(p);
  items = ast_list_append(items, it);
  while (accept(p, ',')) {
    if (accept(p, '}')) { /* trailing comma handled by caller */
      /* put back '}' into stream by setting current token to '}' already
       * consumed - not supported; just break */
      break;
    }
    it = parse_initializer(p);
    items = ast_list_append(items, it);
  }
  /* We don't have a dedicated AST node for initializer lists; return last item
   * for simplicity */
  /* Parser will consume surrounding braces */
  if (items && items->node)
    return items->node;
  return 0;
}

static struct ASTNode *parse_initializer(struct Parser *p) {
  if (accept(p, '{')) {
    struct ASTNode *n;
    n = parse_initializer_list(p);
    if (!accept(p, '}')) {
      /* handle optional trailing comma */
      expect(p, '}', "} expected in initializer");
    }
    return n;
  }
  return parse_assignment(p);
}

static struct ASTNode *parse_declaration(struct Parser *p, int *out_typedef) {
  int flags = 0;
  int is_td = 0;
  struct Declarator *d = 0;
  struct ASTNode *init = 0;
  struct ASTNode *decl_node = 0;
  struct ASTList *more = 0;
  if (!parse_decl_specifiers(p, &flags, &is_td))
    return 0;
  /* Allow declarations with only specifiers */
  if (accept(p, ';')) {
    decl_node = ast_new_decl(flags, 0, 0, p->cur.line, p->cur.column);
    if (out_typedef)
      *out_typedef = is_td;
    return decl_node;
  }
  /* init_declarator_list */
  d = parse_declarator_shape(p, 0);
  if (!d)
    return 0;
  if (accept(p, '=')) {
    init = parse_initializer(p);
  }
  decl_node = ast_new_decl(flags, d, init, p->cur.line, p->cur.column);
  if (is_td && d && d->name)
    symbols_add_typedef(p->symbols, d->name);
  while (accept(p, ',')) {
    struct Declarator *d2 = 0;
    struct ASTNode *init2 = 0;
    struct ASTNode *one;
    d2 = parse_declarator_shape(p, 0);
    if (!d2)
      break;
    if (accept(p, '=')) {
      init2 = parse_initializer(p);
    }
    one = ast_new_decl(flags, d2, init2, p->cur.line, p->cur.column);
    more = ast_list_append(more, one);
    if (is_td && d2 && d2->name)
      symbols_add_typedef(p->symbols, d2->name);
  }
  expect(p, ';', "; expected");
  /* attach additional decls */
  decl_node->u.decl.decls = more;
  if (out_typedef)
    *out_typedef = is_td;
  return decl_node;
}

static struct ASTNode *parse_statement(struct Parser *p) {
  if (accept(p, '{')) { /* compound */
    struct ASTList *stmts = 0;
    while (!accept(p, '}')) {
      struct ASTNode *s = parse_statement(p);
      if (!s)
        break;
      stmts = ast_list_append(stmts, s);
    }
    return ast_new_stmt_compound(stmts, p->cur.line, p->cur.column);
  }
  if (accept(p, ';')) {
    return ast_new_stmt_expr(0, p->cur.line, p->cur.column);
  }
  if (accept(p, T_SWITCH)) {
    expect(p, '(', "( expected");
    {
      struct ASTNode *cond = parse_expression(p);
      expect(p, ')', ") expected");
      {
        struct ASTNode *body = parse_statement(p);
        return ast_new_stmt_switch(cond, body, p->cur.line, p->cur.column);
      }
    }
  }
  if (accept(p, T_GOTO)) {
    struct Token id;
    if (!p->has_cur)
      next(p);
    id = p->cur;
    expect(p, T_IDENTIFIER, "identifier expected after goto");
    expect(p, ';', "; expected");
    return ast_new_stmt_goto(id.lexeme, id.line, id.column);
  }
  if (accept(p, T_CASE)) {
    struct ASTNode *ce = parse_expression(p);
    expect(p, ':', ": expected");
    {
      struct ASTNode *st = parse_statement(p);
      return ast_new_stmt_case(ce, st, p->cur.line, p->cur.column);
    }
  }
  if (accept(p, T_DEFAULT)) {
    expect(p, ':', ": expected");
    {
      struct ASTNode *st = parse_statement(p);
      return ast_new_stmt_default(st, p->cur.line, p->cur.column);
    }
  }
  /* labeled statement: identifier ':' statement */
  if (!p->has_cur)
    next(p);
  if (p->cur.kind == T_IDENTIFIER) {
    /* lookahead for ':' */
    if (la_ensure(p, 1) && p->la[0].kind == ':') {
      struct Token id = p->cur;
      next(p);
      expect(p, ':', ": expected");
      {
        struct ASTNode *st = parse_statement(p);
        return ast_new_stmt_label(id.lexeme, st, id.line, id.column);
      }
    }
  }
  if (accept(p, T_RETURN)) {
    struct ASTNode *e = 0;
    if (!accept(p, ';')) {
      e = parse_expression(p);
      expect(p, ';', "; expected");
    }
    return ast_new_stmt_return(e, p->cur.line, p->cur.column);
  }
  if (accept(p, T_BREAK)) {
    expect(p, ';', "; expected");
    return ast_new_stmt_break(p->cur.line, p->cur.column);
  }
  if (accept(p, T_CONTINUE)) {
    expect(p, ';', "; expected");
    return ast_new_stmt_continue(p->cur.line, p->cur.column);
  }
  if (accept(p, T_IF)) {
    expect(p, '(', "( expected");
    {
      struct ASTNode *cond = parse_expression(p);
      expect(p, ')', ") expected");
      {
        struct ASTNode *then_s = parse_statement(p);
        struct ASTNode *else_s = 0;
        if (accept(p, T_ELSE))
          else_s = parse_statement(p);
        return ast_new_stmt_if(cond, then_s, else_s, p->cur.line,
                               p->cur.column);
      }
    }
  }
  if (accept(p, T_WHILE)) {
    expect(p, '(', "( expected");
    {
      struct ASTNode *cond = parse_expression(p);
      expect(p, ')', ") expected");
      {
        struct ASTNode *body = parse_statement(p);
        return ast_new_stmt_while(cond, body, p->cur.line, p->cur.column);
      }
    }
  }
  if (accept(p, T_DO)) {
    struct ASTNode *body = parse_statement(p);
    expect(p, T_WHILE, "while expected");
    expect(p, '(', "( expected");
    {
      struct ASTNode *cond = parse_expression(p);
      expect(p, ')', ") expected");
      expect(p, ';', "; expected");
      return ast_new_stmt_dowhile(body, cond, p->cur.line, p->cur.column);
    }
  }
  if (accept(p, T_FOR)) {
    struct ASTNode *init = 0, *cond = 0, *step = 0, *body = 0;
    expect(p, '(', "( expected");
    if (!accept(p, ';')) {
      init = parse_expression(p);
      expect(p, ';', "; expected");
    }
    if (!accept(p, ';')) {
      cond = parse_expression(p);
      expect(p, ';', "; expected");
    }
    if (!accept(p, ')')) {
      step = parse_expression(p);
      expect(p, ')', ") expected");
    }
    body = parse_statement(p);
    return ast_new_stmt_for(init, cond, step, body, p->cur.line, p->cur.column);
  }
  /* Declaration vs expression: if starts with specifier, parse a declaration
   * and wrap as statement */
  if (is_type_spec_token(p->cur.kind) || p->cur.kind == T_TYPEDEF ||
      p->cur.kind == T_EXTERN || p->cur.kind == T_STATIC ||
      p->cur.kind == T_AUTO || p->cur.kind == T_REGISTER) {
    int is_td = 0;
    struct ASTNode *d = parse_declaration(p, &is_td);
    {
      struct ASTNode *s = ast_new_stmt_expr(0, d->line, d->column);
      s->u.stmt.decl = d;
      s->u.stmt.kind = STMT_DECL;
      return s;
    }
  }
  {
    struct ASTNode *e = parse_expression(p);
    expect(p, ';', "; expected");
    return ast_new_stmt_expr(e, p->cur.line, p->cur.column);
  }
}

static struct ASTNode *parse_function_definition(struct Parser *p, int flags,
                                                 struct Declarator *d) {
  /* We have seen specifiers and a declarator with function shape, now parse a
   * compound body */
  struct ASTNode *declnode =
      ast_new_decl(flags, d, 0, p->cur.line, p->cur.column);
  struct ASTNode *body = parse_compound(p);
  return ast_new_external(declnode, body, p->cur.line, p->cur.column);
}

static struct ASTNode *parse_compound(struct Parser *p) {
  expect(p, '{', "{ expected");
  {
    struct ASTList *stmts = 0;
    while (1) {
      if (!p->has_cur)
        next(p);
      if (p->cur.kind == '}') {
        next(p);
        break;
      }
      if (p->cur.kind == T_EOF) {
        parser_error(p, "unexpected EOF in compound");
        break;
      }
      stmts = ast_list_append(stmts, parse_statement(p));
      /* if parse_statement didn't consume and didn't make progress, sync to
       * next ';' to avoid infinite loop */
      if (!p->has_cur)
        next(p);
    }
    return ast_new_stmt_compound(stmts, p->cur.line, p->cur.column);
  }
}

static struct ASTNode *parse_external_declaration(struct Parser *p) {
  int flags = 0, is_td = 0;
  struct Declarator *d;
  struct ASTNode *decl;
  if (!parse_decl_specifiers(p, &flags, &is_td))
    return 0;
  /* Allow specifiers only declaration, e.g., enum {...}; */
  if (accept(p, ';')) {
    decl = ast_new_decl(flags, 0, 0, p->cur.line, p->cur.column);
    return ast_new_external(decl, 0, p->cur.line, p->cur.column);
  }
  d = parse_declarator_shape(p, 0);
  if (!d)
    return 0;
  if (d->is_function) {
    /* function declaration or definition
       - definition if '{' next, or if declaration list appears then '{'
       - prototype if ';' */
    if (!p->has_cur)
      next(p);
    if (p->cur.kind == '{') {
      return parse_function_definition(p, flags, d);
    }
    /* Old-style parameter declarations before body */
    while (starts_declaration(p)) {
      int tmp = 0;
      (void)parse_declaration(p, &tmp);
      if (!p->has_cur)
        next(p);
    }
    if (p->cur.kind == '{') {
      return parse_function_definition(p, flags, d);
    }
    /* otherwise it's a prototype */
    decl = ast_new_decl(flags, d, 0, p->cur.line, p->cur.column);
    expect(p, ';', "; expected");
    return ast_new_external(decl, 0, p->cur.line, p->cur.column);
  } else {
    /* normal declaration */
    /* support init_declarator_list here too */
    struct ASTNode *first = 0;
    struct ASTList *more = 0;
    struct ASTNode *one;
    if (accept(p, '=')) {
      struct ASTNode *init = parse_initializer(p);
      first = ast_new_decl(flags, d, init, p->cur.line, p->cur.column);
    } else {
      first = ast_new_decl(flags, d, 0, p->cur.line, p->cur.column);
    }
    if (is_td && d && d->name)
      symbols_add_typedef(p->symbols, d->name);
    while (accept(p, ',')) {
      struct Declarator *d2 = parse_declarator_shape(p, 0);
      struct ASTNode *init2 = 0;
      if (accept(p, '='))
        init2 = parse_initializer(p);
      one = ast_new_decl(flags, d2, init2, p->cur.line, p->cur.column);
      more = ast_list_append(more, one);
      if (is_td && d2 && d2->name)
        symbols_add_typedef(p->symbols, d2->name);
    }
    expect(p, ';', "; expected");
    first->u.decl.decls = more;
    return ast_new_external(first, 0, p->cur.line, p->cur.column);
  }
}

void parser_init(struct Parser *p, struct Lexer *lx, struct Symbols *symbols) {
  p->lx = lx;
  p->symbols = symbols;
  p->has_cur = 0;
  p->la_count = 0;
}
void parser_destroy(struct Parser *p) { (void)p; }

struct ASTNode *parse_translation_unit(struct Parser *p) {
  struct ASTList *exts = 0;
  struct ASTNode *e;
  struct ASTNode *root;
  while (1) {
    if (!p->has_cur)
      next(p);
    if (p->cur.kind == T_EOF)
      break;
    e = parse_external_declaration(p);
    if (!e)
      break;
    exts = ast_list_append(exts, e);
  }
  root = ast_new_stmt_compound(exts, 0, 0); /* reuse compound to hold list */
  return root;
}
