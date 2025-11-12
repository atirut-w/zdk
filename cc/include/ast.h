#ifndef AST_H
#define AST_H

#include "token.h"

/* Simple AST for C90 subset. Keep structures minimal and C90-friendly. */

struct ASTNode;
struct ASTList {
  struct ASTNode *node;
  struct ASTList *next;
};

/* Expression kinds */
enum ASTExprKind {
  EXPR_IDENT,
  EXPR_CONST,
  EXPR_STRING,
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_COND,
  EXPR_CALL,
  EXPR_INDEX,
  EXPR_MEMBER,
  EXPR_ARROW
};

/* Statement kinds */
enum ASTStmtKind {
  STMT_COMPOUND,
  STMT_EXPR,
  STMT_IF,
  STMT_WHILE,
  STMT_DOWHILE,
  STMT_FOR,
  STMT_RETURN,
  STMT_BREAK,
  STMT_CONTINUE,
  STMT_SWITCH,
  STMT_GOTO,
  STMT_LABEL,
  STMT_CASE,
  STMT_DEFAULT
};

/* Declaration spec flags */
enum SpecFlags {
  SPF_NONE = 0,
  SPF_TYPEDEF = 1 << 0,
  SPF_EXTERN = 1 << 1,
  SPF_STATIC = 1 << 2,
  SPF_AUTO = 1 << 3,
  SPF_REGISTER = 1 << 4,

  SPF_VOID = 1 << 5,
  SPF_CHAR = 1 << 6,
  SPF_SHORT = 1 << 7,
  SPF_INT = 1 << 8,
  SPF_LONG = 1 << 9,
  SPF_SIGNED = 1 << 10,
  SPF_UNSIGNED = 1 << 11,
  SPF_FLOAT = 1 << 12,
  SPF_DOUBLE = 1 << 13,

  SPF_CONST = 1 << 14,
  SPF_VOLATILE = 1 << 15
};

/* Declarator shape (simplified) */
struct Declarator {
  char *name;             /* identifier */
  int pointer_level;      /* number of * */
  int is_function;        /* 1 if function declarator */
  struct ASTList *params; /* list of declarations or identifiers (simplified) */
  int is_array;           /* 1 if array declarator */
  struct ASTNode *array_size; /* optional constant expression */
};

/* AST node */
struct ASTNode {
  int kind_tag; /* 0 expr, 1 stmt, 2 decl, 3 ext */
  int line, column;
  union {
    /* expr */
    struct {
      int kind;             /* enum ASTExprKind */
      int op;               /* for unary/binary: operator token or char */
      struct ASTNode *e1;   /* or function in call */
      struct ASTNode *e2;   /* rhs or index or member */
      struct ASTNode *e3;   /* conditional third */
      struct ASTList *args; /* for calls */
      char *ident;
      char *str;
      struct Token const_tok; /* for constants */
    } expr;
    /* stmt */
    struct {
      int kind;              /* enum ASTStmtKind */
      struct ASTList *stmts; /* compound */
      struct ASTNode *s1;    /* condition or body */
      struct ASTNode *s2;    /* else or post */
      struct ASTNode *s3;    /* for-increment */
      struct ASTNode *expr;  /* expr or return value or switch cond */
      char *label;           /* for goto/label */
    } stmt;
    /* decl */
    struct {
      int spec_flags;             /* SpecFlags */
      struct ASTList *decls;      /* list of single declarators */
      struct Declarator *decltor; /* for single */
      struct ASTNode *init;       /* initializer expr */
    } decl;
    /* external */
    struct {
      int is_function;      /* 1 for function def */
      struct ASTNode *decl; /* declaration specifiers + declarator */
      struct ASTNode *body; /* compound statement */
    } ext;
  } u;
};

/* Constructors and utilities */
struct ASTNode *ast_new_expr_ident(const char *name, int line, int col);
struct ASTNode *ast_new_expr_const(struct Token *tok);
struct ASTNode *ast_new_expr_string(const char *s, int line, int col);
struct ASTNode *ast_new_expr_unary(int op, struct ASTNode *e1, int line,
                                   int col);
struct ASTNode *ast_new_expr_binary(int op, struct ASTNode *l,
                                    struct ASTNode *r, int line, int col);
struct ASTNode *ast_new_expr_cond(struct ASTNode *c, struct ASTNode *t,
                                  struct ASTNode *f, int line, int col);
struct ASTNode *ast_new_expr_call(struct ASTNode *fn, struct ASTList *args,
                                  int line, int col);
struct ASTNode *ast_new_expr_index(struct ASTNode *arr, struct ASTNode *idx,
                                   int line, int col);
struct ASTNode *ast_new_expr_member(struct ASTNode *obj, const char *name,
                                    int is_arrow, int line, int col);

struct ASTNode *ast_new_stmt_compound(struct ASTList *stmts, int line, int col);
struct ASTNode *ast_new_stmt_expr(struct ASTNode *e, int line, int col);
struct ASTNode *ast_new_stmt_if(struct ASTNode *cond, struct ASTNode *then_s,
                                struct ASTNode *else_s, int line, int col);
struct ASTNode *ast_new_stmt_while(struct ASTNode *cond, struct ASTNode *body,
                                   int line, int col);
struct ASTNode *ast_new_stmt_dowhile(struct ASTNode *body, struct ASTNode *cond,
                                     int line, int col);
struct ASTNode *ast_new_stmt_for(struct ASTNode *init, struct ASTNode *cond,
                                 struct ASTNode *step, struct ASTNode *body,
                                 int line, int col);
struct ASTNode *ast_new_stmt_return(struct ASTNode *e, int line, int col);
struct ASTNode *ast_new_stmt_break(int line, int col);
struct ASTNode *ast_new_stmt_continue(int line, int col);
struct ASTNode *ast_new_stmt_switch(struct ASTNode *cond, struct ASTNode *body,
                                    int line, int col);
struct ASTNode *ast_new_stmt_goto(const char *label, int line, int col);
struct ASTNode *ast_new_stmt_label(const char *label, struct ASTNode *stmt,
                                   int line, int col);
struct ASTNode *ast_new_stmt_case(struct ASTNode *expr, struct ASTNode *stmt,
                                  int line, int col);
struct ASTNode *ast_new_stmt_default(struct ASTNode *stmt, int line, int col);

struct Declarator *ast_new_declarator(const char *name);
void ast_set_declarator_pointer(struct Declarator *d, int level);
void ast_set_declarator_function(struct Declarator *d, struct ASTList *params);
void ast_set_declarator_array(struct Declarator *d, struct ASTNode *size);

struct ASTNode *ast_new_decl(int spec_flags, struct Declarator *decltor,
                             struct ASTNode *init, int line, int col);
struct ASTNode *ast_new_external(struct ASTNode *decl, struct ASTNode *body,
                                 int line, int col);

struct ASTList *ast_list_prepend(struct ASTList *list, struct ASTNode *node);
struct ASTList *ast_list_append(struct ASTList *list, struct ASTNode *node);
struct ASTList *ast_list_concat(struct ASTList *a, struct ASTList *b);

void ast_print(struct ASTNode *node, int indent);
void ast_free(struct ASTNode *node);

#endif /* AST_H */
