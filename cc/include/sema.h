#ifndef SEMA_H
#define SEMA_H

#include "ast.h"

/* Type kinds */
enum TypeKind {
  TYPE_VOID,
  TYPE_CHAR,
  TYPE_SHORT,
  TYPE_INT,
  TYPE_LONG,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_POINTER,
  TYPE_ARRAY,
  TYPE_FUNCTION,
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_ENUM
};

/* Type representation */
struct Type {
  enum TypeKind kind;
  int is_signed;     /* for integer types */
  int is_const;
  int is_volatile;
  
  /* For pointers and arrays */
  struct Type *base_type;
  int array_size;    /* -1 for unknown/incomplete */
  
  /* For functions */
  struct Type *return_type;
  struct ParamList *params;
  int has_varargs;
  int param_count;
};

/* Function parameter list */
struct ParamList {
  struct Type *type;
  char *name;        /* can be NULL for abstract declarators */
  struct ParamList *next;
};

/* Symbol kinds */
enum SymbolKind {
  SYM_VAR,      /* variable */
  SYM_FUNC,     /* function */
  SYM_TYPEDEF,  /* typedef name */
  SYM_PARAM     /* function parameter */
};

/* Symbol entry */
struct Symbol {
  char *name;
  enum SymbolKind kind;
  struct Type *type;
  int is_defined;    /* for functions: has body been seen? */
  int is_extern;
  int is_static;
  int stack_offset;  /* for local variables */
  int line, column;  /* where declared */
  struct Symbol *next;
};

/* Scope - linked list of symbols */
struct Scope {
  struct Symbol *symbols;
  struct Scope *parent;
  int is_function_scope;  /* function scope vs block scope */
  int stack_size;         /* total stack space needed for locals */
};

/* Semantic analyzer context */
struct Sema {
  struct Scope *current_scope;
  struct Scope *global_scope;
  int error_count;
  int in_function;       /* currently analyzing a function? */
  struct Type *current_function_return_type;
};

/* Semantic analysis API */
void sema_init(struct Sema *sema);
void sema_destroy(struct Sema *sema);
int sema_analyze(struct Sema *sema, struct ASTNode *tree);

/* Type operations */
struct Type *type_new_basic(enum TypeKind kind);
struct Type *type_new_pointer(struct Type *base);
struct Type *type_new_array(struct Type *base, int size);
struct Type *type_new_function(struct Type *ret, struct ParamList *params, int has_varargs);
void type_free(struct Type *type);
int type_equals(const struct Type *a, const struct Type *b);
int type_compatible(const struct Type *a, const struct Type *b);
int type_is_arithmetic(const struct Type *t);
int type_is_integer(const struct Type *t);
int type_is_pointer(const struct Type *t);
struct Type *type_copy(const struct Type *t);

/* Scope operations */
struct Scope *scope_new(struct Scope *parent, int is_function_scope);
void scope_free(struct Scope *scope);
struct Symbol *scope_lookup(struct Scope *scope, const char *name);
struct Symbol *scope_lookup_current(struct Scope *scope, const char *name);
struct Symbol *scope_add_symbol(struct Scope *scope, const char *name, 
                                enum SymbolKind kind, struct Type *type,
                                int line, int column);

/* Type extraction from AST declaration specifiers and declarators */
struct Type *sema_type_from_decl(struct Sema *sema, struct ASTNode *decl);

#endif /* SEMA_H */
