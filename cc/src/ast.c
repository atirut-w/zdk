#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct ASTNode *ast_alloc(void) {
  struct ASTNode *n = (struct ASTNode *)malloc(sizeof(struct ASTNode));
  if (n)
    memset(n, 0, sizeof(*n));
  return n;
}

static char *dupstr(const char *s) {
  size_t len;
  char *d;
  if (!s)
    return 0;
  len = strlen(s);
  d = (char *)malloc(len + 1);
  if (!d)
    return 0;
  strcpy(d, s);
  return d;
}

struct ASTList *ast_list_prepend(struct ASTList *list, struct ASTNode *node) {
  struct ASTList *l = (struct ASTList *)malloc(sizeof(struct ASTList));
  if (!l)
    return list;
  l->node = node;
  l->next = list;
  return l;
}

struct ASTList *ast_list_append(struct ASTList *list, struct ASTNode *node) {
  struct ASTList *l, *p;
  l = (struct ASTList *)malloc(sizeof(struct ASTList));
  if (!l)
    return list;
  l->node = node;
  l->next = 0;
  if (!list)
    return l;
  p = list;
  while (p->next)
    p = p->next;
  p->next = l;
  return list;
}

struct ASTList *ast_list_concat(struct ASTList *a, struct ASTList *b) {
  struct ASTList *p;
  if (!a)
    return b;
  p = a;
  while (p->next)
    p = p->next;
  p->next = b;
  return a;
}

struct ASTNode *ast_new_expr_ident(const char *name, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_IDENT;
  n->u.expr.ident = dupstr(name);
  return n;
}

struct ASTNode *ast_new_expr_const(struct Token *tok) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = tok->line;
  n->column = tok->column;
  n->u.expr.kind = EXPR_CONST;
  n->u.expr.const_tok =
      *tok; /* shallow copy (lexeme owned by token in parse?) */
  if (tok->lexeme) {
    n->u.expr.const_tok.lexeme = dupstr(tok->lexeme);
  }
  return n;
}

struct ASTNode *ast_new_expr_string(const char *s, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_STRING;
  n->u.expr.str = dupstr(s);
  return n;
}

struct ASTNode *ast_new_expr_unary(int op, struct ASTNode *e1, int line,
                                   int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_UNARY;
  n->u.expr.op = op;
  n->u.expr.e1 = e1;
  return n;
}

struct ASTNode *ast_new_expr_binary(int op, struct ASTNode *l,
                                    struct ASTNode *r, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_BINARY;
  n->u.expr.op = op;
  n->u.expr.e1 = l;
  n->u.expr.e2 = r;
  return n;
}

struct ASTNode *ast_new_expr_cond(struct ASTNode *c, struct ASTNode *t,
                                  struct ASTNode *f, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_COND;
  n->u.expr.e1 = c;
  n->u.expr.e2 = t;
  n->u.expr.e3 = f;
  return n;
}

struct ASTNode *ast_new_expr_call(struct ASTNode *fn, struct ASTList *args,
                                  int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_CALL;
  n->u.expr.e1 = fn;
  n->u.expr.args = args;
  return n;
}

struct ASTNode *ast_new_expr_index(struct ASTNode *arr, struct ASTNode *idx,
                                   int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = EXPR_INDEX;
  n->u.expr.e1 = arr;
  n->u.expr.e2 = idx;
  return n;
}

struct ASTNode *ast_new_expr_member(struct ASTNode *obj, const char *name,
                                    int is_arrow, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 0;
  n->line = line;
  n->column = col;
  n->u.expr.kind = is_arrow ? EXPR_ARROW : EXPR_MEMBER;
  n->u.expr.e1 = obj;
  n->u.expr.ident = dupstr(name);
  return n;
}

struct ASTNode *ast_new_stmt_compound(struct ASTList *stmts, int line,
                                      int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_COMPOUND;
  n->u.stmt.stmts = stmts;
  return n;
}
struct ASTNode *ast_new_stmt_expr(struct ASTNode *e, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_EXPR;
  n->u.stmt.expr = e;
  return n;
}
struct ASTNode *ast_new_stmt_if(struct ASTNode *cond, struct ASTNode *then_s,
                                struct ASTNode *else_s, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_IF;
  n->u.stmt.s1 = cond;
  n->u.stmt.s2 = then_s;
  n->u.stmt.s3 = else_s;
  return n;
}
struct ASTNode *ast_new_stmt_while(struct ASTNode *cond, struct ASTNode *body,
                                   int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_WHILE;
  n->u.stmt.s1 = cond;
  n->u.stmt.s2 = body;
  return n;
}
struct ASTNode *ast_new_stmt_dowhile(struct ASTNode *body, struct ASTNode *cond,
                                     int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_DOWHILE;
  n->u.stmt.s1 = body;
  n->u.stmt.s2 = cond;
  return n;
}
struct ASTNode *ast_new_stmt_for(struct ASTNode *init, struct ASTNode *cond,
                                 struct ASTNode *step, struct ASTNode *body,
                                 int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_FOR;
  n->u.stmt.s1 = init;
  n->u.stmt.s2 = cond;
  n->u.stmt.s3 = step;
  n->u.stmt.expr = body;
  return n;
}
struct ASTNode *ast_new_stmt_return(struct ASTNode *e, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_RETURN;
  n->u.stmt.expr = e;
  return n;
}
struct ASTNode *ast_new_stmt_break(int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_BREAK;
  return n;
}
struct ASTNode *ast_new_stmt_continue(int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_CONTINUE;
  return n;
}
/* New statements */
static char *dupstr(const char *s);
struct ASTNode *ast_new_stmt_switch(struct ASTNode *cond, struct ASTNode *body,
                                    int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_SWITCH;
  n->u.stmt.expr = cond;
  n->u.stmt.s1 = body;
  return n;
}
struct ASTNode *ast_new_stmt_goto(const char *label, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_GOTO;
  n->u.stmt.label = dupstr(label);
  return n;
}
struct ASTNode *ast_new_stmt_label(const char *label, struct ASTNode *stmt,
                                   int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_LABEL;
  n->u.stmt.label = dupstr(label);
  n->u.stmt.s1 = stmt;
  return n;
}
struct ASTNode *ast_new_stmt_case(struct ASTNode *expr, struct ASTNode *stmt,
                                  int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_CASE;
  n->u.stmt.expr = expr;
  n->u.stmt.s1 = stmt;
  return n;
}
struct ASTNode *ast_new_stmt_default(struct ASTNode *stmt, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 1;
  n->line = line;
  n->column = col;
  n->u.stmt.kind = STMT_DEFAULT;
  n->u.stmt.s1 = stmt;
  return n;
}

struct Declarator *ast_new_declarator(const char *name) {
  struct Declarator *d = (struct Declarator *)malloc(sizeof(struct Declarator));
  if (!d)
    return 0;
  d->name = dupstr(name);
  d->pointer_level = 0;
  d->is_function = 0;
  d->params = 0;
  d->is_array = 0;
  d->array_size = 0;
  return d;
}
void ast_set_declarator_pointer(struct Declarator *d, int level) {
  if (d)
    d->pointer_level = level;
}
void ast_set_declarator_function(struct Declarator *d, struct ASTList *params) {
  if (d) {
    d->is_function = 1;
    d->params = params;
  }
}
void ast_set_declarator_array(struct Declarator *d, struct ASTNode *size) {
  if (d) {
    d->is_array = 1;
    d->array_size = size;
  }
}

struct ASTNode *ast_new_decl(int spec_flags, struct Declarator *decltor,
                             struct ASTNode *init, int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 2;
  n->line = line;
  n->column = col;
  n->u.decl.spec_flags = spec_flags;
  n->u.decl.decltor = decltor;
  n->u.decl.init = init;
  return n;
}
struct ASTNode *ast_new_external(struct ASTNode *decl, struct ASTNode *body,
                                 int line, int col) {
  struct ASTNode *n = ast_alloc();
  if (!n)
    return 0;
  n->kind_tag = 3;
  n->line = line;
  n->column = col;
  n->u.ext.decl = decl;
  n->u.ext.body = body;
  n->u.ext.is_function = body != 0;
  return n;
}

static void ast_free_list(struct ASTList *l) {
  while (l) {
    struct ASTList *n = l->next;
    ast_free(l->node);
    free(l);
    l = n;
  }
}

void ast_free(struct ASTNode *node) {
  if (!node)
    return;
  switch (node->kind_tag) {
  case 0: /* expr */
    ast_free(node->u.expr.e1);
    ast_free(node->u.expr.e2);
    ast_free(node->u.expr.e3);
    if (node->u.expr.args)
      ast_free_list(node->u.expr.args);
    if (node->u.expr.ident)
      free(node->u.expr.ident);
    if (node->u.expr.str)
      free(node->u.expr.str);
    if (node->u.expr.const_tok.lexeme)
      free(node->u.expr.const_tok.lexeme);
    break;
  case 1: /* stmt */
    if (node->u.stmt.stmts)
      ast_free_list(node->u.stmt.stmts);
    ast_free(node->u.stmt.s1);
    ast_free(node->u.stmt.s2);
    ast_free(node->u.stmt.s3);
    ast_free(node->u.stmt.expr);
    ast_free(node->u.stmt.decl);
    if (node->u.stmt.label)
      free(node->u.stmt.label);
    break;
  case 2: /* decl */
    if (node->u.decl.decltor) {
      if (node->u.decl.decltor->name)
        free(node->u.decl.decltor->name);
      free(node->u.decl.decltor);
    }
    ast_free(node->u.decl.init);
    if (node->u.decl.decls)
      ast_free_list(node->u.decl.decls);
    break;
  case 3: /* external */
    ast_free(node->u.ext.decl);
    ast_free(node->u.ext.body);
    break;
  }
  free(node);
}

static void indent(int n) {
  while (n--)
    putchar(' ');
}

static void print_list(struct ASTList *l, int ind) {
  while (l) {
    ast_print(l->node, ind);
    l = l->next;
  }
}

void ast_print(struct ASTNode *node, int ind) {
  if (!node) {
    indent(ind);
    printf("<nil>\n");
    return;
  }
  switch (node->kind_tag) {
  case 0:
    indent(ind);
    printf("Expr kind=%d op=%d", node->u.expr.kind, node->u.expr.op);
    if (node->u.expr.ident)
      printf(" ident=%s", node->u.expr.ident);
    if (node->u.expr.str)
      printf(" str=\"%s\"", node->u.expr.str);
    if (node->u.expr.kind == EXPR_CONST && node->u.expr.const_tok.lexeme)
      printf(" const=%s", node->u.expr.const_tok.lexeme);
    printf("\n");
    if (node->u.expr.e1)
      ast_print(node->u.expr.e1, ind + 2);
    if (node->u.expr.e2)
      ast_print(node->u.expr.e2, ind + 2);
    if (node->u.expr.e3)
      ast_print(node->u.expr.e3, ind + 2);
    if (node->u.expr.args)
      print_list(node->u.expr.args, ind + 2);
    break;
  case 1:
    indent(ind);
    printf("Stmt kind=%d\n", node->u.stmt.kind);
    if (node->u.stmt.label) {
      indent(ind + 2);
      printf("label=%s\n", node->u.stmt.label);
    }
    if (node->u.stmt.stmts)
      print_list(node->u.stmt.stmts, ind + 2);
    if (node->u.stmt.s1)
      ast_print(node->u.stmt.s1, ind + 2);
    if (node->u.stmt.s2)
      ast_print(node->u.stmt.s2, ind + 2);
    if (node->u.stmt.s3)
      ast_print(node->u.stmt.s3, ind + 2);
    if (node->u.stmt.expr)
      ast_print(node->u.stmt.expr, ind + 2);
    if (node->u.stmt.decl)
      ast_print(node->u.stmt.decl, ind + 2);
    break;
  case 2:
    indent(ind);
    printf("Decl specs=0x%X", node->u.decl.spec_flags);
    if (node->u.decl.decltor && node->u.decl.decltor->name)
      printf(" name=%s", node->u.decl.decltor->name);
    printf("\n");
    if (node->u.decl.init)
      ast_print(node->u.decl.init, ind + 2);
    if (node->u.decl.decls)
      print_list(node->u.decl.decls, ind + 2);
    break;
  case 3:
    indent(ind);
    printf("External is_function=%d\n", node->u.ext.is_function);
    if (node->u.ext.decl)
      ast_print(node->u.ext.decl, ind + 2);
    if (node->u.ext.body)
      ast_print(node->u.ext.body, ind + 2);
    break;
  }
}
