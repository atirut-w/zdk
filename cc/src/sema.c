#include "sema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Externs shared with parser/lexer for unified error reporting */
extern const char *yyfilename; /* current input filename */
extern char current_line[];    /* contents of current lexer line */
extern int current_line_len;   /* length of current line */

#include "errorreport.h"

/* strdup is not C90 standard */
static char *my_strdup(const char *s) {
  char *result;
  size_t len;
  if (!s) return NULL;
  len = strlen(s) + 1;
  result = (char *)malloc(len);
  if (result) {
    memcpy(result, s, len);
  }
  return result;
}

/* Forward declarations */
static struct Type *analyze_expr(struct Sema *sema, struct ASTNode *expr);
static void set_expr_type(struct ASTNode *expr, struct Type *t) {
  if (!expr || expr->kind_tag != 0) return;
  if (expr->type) {
    type_free(expr->type);
    expr->type = NULL;
  }
  if (t) {
    expr->type = type_copy(t);
  }
}

/* Recursive walker to annotate expression nodes with inferred types */
static void annotate_expr_types(struct Sema *sema, struct ASTNode *n) {
  struct ASTList *it;
  struct Type *t;
  if (!n) return;
  switch (n->kind_tag) {
    case 0: /* expr */
      t = analyze_expr(sema, n);
      set_expr_type(n, t);
      type_free(t);
      /* Visit children */
      annotate_expr_types(sema, n->u.expr.e1);
      annotate_expr_types(sema, n->u.expr.e2);
      annotate_expr_types(sema, n->u.expr.e3);
      for (it = n->u.expr.args; it; it = it->next) annotate_expr_types(sema, it->node);
      break;
    case 1: /* stmt */
      if (n->u.stmt.stmts) {
        for (it = n->u.stmt.stmts; it; it = it->next) annotate_expr_types(sema, it->node);
      }
      annotate_expr_types(sema, n->u.stmt.s1);
      annotate_expr_types(sema, n->u.stmt.s2);
      annotate_expr_types(sema, n->u.stmt.s3);
      annotate_expr_types(sema, n->u.stmt.expr);
      break;
    case 2: /* decl */
      if (n->u.decl.init) annotate_expr_types(sema, n->u.decl.init);
      if (n->u.decl.decls) {
        for (it = n->u.decl.decls; it; it = it->next) annotate_expr_types(sema, it->node);
      }
      break;
    case 3: /* ext */
      annotate_expr_types(sema, n->u.ext.decl);
      annotate_expr_types(sema, n->u.ext.body);
      break;
  }
}
static void analyze_stmt(struct Sema *sema, struct ASTNode *stmt);
static void analyze_compound(struct Sema *sema, struct ASTNode *compound);
static void sema_error(struct Sema *sema, int line, int col, const char *msg);

/* ===== Helpers for conversions/promotions (C90-subset) ===== */
static int type_rank(const struct Type *t) {
  if (!t) return -1;
  switch (t->kind) {
    case TYPE_CHAR: return 1;
    case TYPE_SHORT: return 2;
    case TYPE_INT: return 3;
    case TYPE_LONG: return 4;
    case TYPE_FLOAT: return 5;
    case TYPE_DOUBLE: return 6;
    default: return -1;
  }
}

static struct Type *integer_promotion(struct Type *t) {
  if (!t) return NULL;
  if (t->kind == TYPE_CHAR || t->kind == TYPE_SHORT) {
    struct Type *r = type_new_basic(TYPE_INT);
    r->is_signed = t->is_signed;
    return r;
  }
  return type_copy(t);
}

static struct Type *decay_array_to_pointer(struct Type *t) {
  if (!t) return NULL;
  if (t->kind == TYPE_ARRAY && t->base_type) {
    return type_new_pointer(type_copy(t->base_type));
  }
  return type_copy(t);
}

static struct Type *usual_arith_conv(struct Sema *sema, struct Type *a, struct Type *b, int line, int col) {
  struct Type *pa, *pb, *res;
  if (!a || !b) {
    if (a) return a;
    if (b) return b;
    return type_new_basic(TYPE_INT);
  }
  if (a->kind == TYPE_DOUBLE || b->kind == TYPE_DOUBLE) {
    sema_error(sema, line, col, "double is not supported on this target (>32-bit)");
    return type_new_basic(TYPE_DOUBLE);
  }
  if (a->kind == TYPE_FLOAT || b->kind == TYPE_FLOAT) {
    return type_new_basic(TYPE_FLOAT);
  }
  pa = integer_promotion(a);
  pb = integer_promotion(b);
  if (type_rank(pa) >= type_rank(pb)) {
    res = type_new_basic(pa->kind);
    res->is_signed = pa->is_signed;
  } else {
    res = type_new_basic(pb->kind);
    res->is_signed = pb->is_signed;
  }
  type_free(pa);
  type_free(pb);
  return res;
}

/* ========== Type Operations ========== */

struct Type *type_new_basic(enum TypeKind kind) {
  struct Type *t = (struct Type *)malloc(sizeof(struct Type));
  if (!t) return NULL;
  memset(t, 0, sizeof(struct Type));
  t->kind = kind;
  t->is_signed = (kind == TYPE_CHAR || kind == TYPE_SHORT || 
                  kind == TYPE_INT || kind == TYPE_LONG);
  t->array_size = -1;
  return t;
}

struct Type *type_new_pointer(struct Type *base) {
  struct Type *t = (struct Type *)malloc(sizeof(struct Type));
  if (!t) return NULL;
  memset(t, 0, sizeof(struct Type));
  t->kind = TYPE_POINTER;
  t->base_type = base;
  t->array_size = -1;
  return t;
}

struct Type *type_new_array(struct Type *base, int size) {
  struct Type *t = (struct Type *)malloc(sizeof(struct Type));
  if (!t) return NULL;
  memset(t, 0, sizeof(struct Type));
  t->kind = TYPE_ARRAY;
  t->base_type = base;
  t->array_size = size;
  return t;
}

struct Type *type_new_function(struct Type *ret, struct ParamList *params, int has_varargs) {
  struct Type *t = (struct Type *)malloc(sizeof(struct Type));
  struct ParamList *p;
  if (!t) return NULL;
  memset(t, 0, sizeof(struct Type));
  t->kind = TYPE_FUNCTION;
  t->return_type = ret;
  t->params = params;
  t->has_varargs = has_varargs;
  t->param_count = 0;
  for (p = params; p; p = p->next) {
    t->param_count++;
  }
  return t;
}

void type_free(struct Type *type) {
  struct ParamList *p, *next;
  if (!type) return;
  
  if (type->base_type) {
    type_free(type->base_type);
  }
  if (type->return_type) {
    type_free(type->return_type);
  }
  if (type->params) {
    for (p = type->params; p; p = next) {
      next = p->next;
      if (p->type) type_free(p->type);
      if (p->name) free(p->name);
      free(p);
    }
  }
  free(type);
}

struct Type *type_copy(const struct Type *t) {
  struct Type *copy;
  struct ParamList *p, **tail;
  if (!t) return NULL;
  
  copy = (struct Type *)malloc(sizeof(struct Type));
  if (!copy) return NULL;
  memcpy(copy, t, sizeof(struct Type));
  
  copy->base_type = NULL;
  copy->return_type = NULL;
  copy->params = NULL;
  
  if (t->base_type) {
    copy->base_type = type_copy(t->base_type);
  }
  if (t->return_type) {
    copy->return_type = type_copy(t->return_type);
  }
  
  /* Copy parameter list */
  tail = &copy->params;
  for (p = t->params; p; p = p->next) {
    struct ParamList *new_param = (struct ParamList *)malloc(sizeof(struct ParamList));
    if (!new_param) break;
    new_param->type = type_copy(p->type);
    new_param->name = p->name ? my_strdup(p->name) : NULL;
    new_param->next = NULL;
    *tail = new_param;
    tail = &new_param->next;
  }
  
  return copy;
}

int type_equals(const struct Type *a, const struct Type *b) {
  struct ParamList *pa, *pb;
  if (!a && !b) return 1;
  if (!a || !b) return 0;
  if (a->kind != b->kind) return 0;
  if (a->is_const != b->is_const) return 0;
  if (a->is_volatile != b->is_volatile) return 0;
  
  switch (a->kind) {
    case TYPE_VOID:
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
    case TYPE_LONG:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
      return a->is_signed == b->is_signed;
      
    case TYPE_POINTER:
      return type_equals(a->base_type, b->base_type);
      
    case TYPE_ARRAY:
      if (a->array_size != b->array_size) return 0;
      return type_equals(a->base_type, b->base_type);
      
    case TYPE_FUNCTION:
      if (a->has_varargs != b->has_varargs) return 0;
      if (a->param_count != b->param_count) return 0;
      if (!type_equals(a->return_type, b->return_type)) return 0;
      
      for (pa = a->params, pb = b->params; pa && pb; pa = pa->next, pb = pb->next) {
        if (!type_equals(pa->type, pb->type)) return 0;
      }
      return 1;
      
    default:
      return 0;
  }
}

int type_compatible(const struct Type *a, const struct Type *b) {
  /* For now, use strict equality. C90 has complex compatibility rules. */
  return type_equals(a, b);
}

int type_is_arithmetic(const struct Type *t) {
  if (!t) return 0;
  switch (t->kind) {
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
    case TYPE_LONG:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
      return 1;
    default:
      return 0;
  }
}

int type_is_integer(const struct Type *t) {
  if (!t) return 0;
  switch (t->kind) {
    case TYPE_CHAR:
    case TYPE_SHORT:
    case TYPE_INT:
    case TYPE_LONG:
      return 1;
    default:
      return 0;
  }
}

int type_is_pointer(const struct Type *t) {
  return t && t->kind == TYPE_POINTER;
}

/* ========== Scope Operations ========== */

struct Scope *scope_new(struct Scope *parent, int is_function_scope) {
  struct Scope *scope = (struct Scope *)malloc(sizeof(struct Scope));
  if (!scope) return NULL;
  scope->symbols = NULL;
  scope->parent = parent;
  scope->is_function_scope = is_function_scope;
  scope->stack_size = 0;
  return scope;
}

void scope_free(struct Scope *scope) {
  struct Symbol *sym, *next;
  if (!scope) return;
  
  for (sym = scope->symbols; sym; sym = next) {
    next = sym->next;
    if (sym->name) free(sym->name);
    if (sym->type) type_free(sym->type);
    free(sym);
  }
  free(scope);
}

struct Symbol *scope_lookup_current(struct Scope *scope, const char *name) {
  struct Symbol *sym;
  if (!scope || !name) return NULL;
  
  for (sym = scope->symbols; sym; sym = sym->next) {
    if (strcmp(sym->name, name) == 0) {
      return sym;
    }
  }
  return NULL;
}

struct Symbol *scope_lookup(struct Scope *scope, const char *name) {
  struct Symbol *sym;
  if (!scope || !name) return NULL;
  
  /* Search current scope */
  sym = scope_lookup_current(scope, name);
  if (sym) return sym;
  
  /* Search parent scopes */
  if (scope->parent) {
    return scope_lookup(scope->parent, name);
  }
  
  return NULL;
}

struct Symbol *scope_add_symbol(struct Scope *scope, const char *name,
                                enum SymbolKind kind, struct Type *type,
                                int line, int column) {
  struct Symbol *sym;
  if (!scope || !name) return NULL;
  
  sym = (struct Symbol *)malloc(sizeof(struct Symbol));
  if (!sym) return NULL;
  
  sym->name = (char *)malloc(strlen(name) + 1);
  if (!sym->name) {
    free(sym);
    return NULL;
  }
  strcpy(sym->name, name);
  
  sym->kind = kind;
  sym->type = type;
  sym->is_defined = 0;
  sym->is_extern = 0;
  sym->is_static = 0;
  sym->stack_offset = 0;
  sym->line = line;
  sym->column = column;
  sym->next = scope->symbols;
  scope->symbols = sym;
  
  return sym;
}

/* ========== Semantic Analysis ========== */

void sema_init(struct Sema *sema) {
  if (!sema) return;
  sema->global_scope = scope_new(NULL, 0);
  sema->current_scope = sema->global_scope;
  sema->error_count = 0;
  sema->in_function = 0;
  sema->current_function_return_type = NULL;
}

void sema_destroy(struct Sema *sema) {
  struct Scope *scope, *next;
  if (!sema) return;
  
  /* Free all scopes */
  scope = sema->current_scope;
  while (scope) {
    next = scope->parent;
    scope_free(scope);
    scope = next;
  }
}

static void sema_error(struct Sema *sema, int err_line, int err_col, const char *msg)
{
  /* Delegate to shared reporter */
  error_report(yyfilename, err_line, err_col, msg, current_line, current_line_len);
  sema->error_count++;
}

static void sema_enter_scope(struct Sema *sema, int is_function_scope) {
  struct Scope *new_scope = scope_new(sema->current_scope, is_function_scope);
  if (new_scope) {
    sema->current_scope = new_scope;
  }
}

static void sema_exit_scope(struct Sema *sema) {
  struct Scope *parent;
  if (!sema->current_scope || sema->current_scope == sema->global_scope) {
    return;
  }
  
  parent = sema->current_scope->parent;
  /* Don't free scope yet - might be needed later */
  sema->current_scope = parent;
}

/* Convert AST declaration specifiers to Type */
struct Type *sema_type_from_decl(struct Sema *sema, struct ASTNode *decl) {
  struct Type *type;
  struct Declarator *decltor;
  int spec_flags;
  enum TypeKind kind;
  
  if (!decl || decl->kind_tag != 2) {
    return type_new_basic(TYPE_INT);
  }
  
  spec_flags = decl->u.decl.spec_flags;
  decltor = decl->u.decl.decltor;
  
  /* Determine base type from specifiers */
  if (spec_flags & SPF_VOID) {
    kind = TYPE_VOID;
  } else if (spec_flags & SPF_CHAR) {
    kind = TYPE_CHAR;
  } else if (spec_flags & SPF_SHORT) {
    kind = TYPE_SHORT;
  } else if (spec_flags & SPF_LONG) {
    kind = TYPE_LONG;
  } else if (spec_flags & SPF_FLOAT) {
    kind = TYPE_FLOAT;
  } else if (spec_flags & SPF_DOUBLE) {
    kind = TYPE_DOUBLE;
  } else {
    kind = TYPE_INT; /* default */
  }
  
  type = type_new_basic(kind);
  if (!type) return NULL;
  
  if (kind == TYPE_DOUBLE) {
    sema_error(sema, decl ? decl->line : 0, decl ? decl->column : 0,
               "double is not supported on this target (>32-bit)");
  }
  
  if (spec_flags & SPF_UNSIGNED) {
    type->is_signed = 0;
  }
  if (spec_flags & SPF_CONST) {
    type->is_const = 1;
  }
  if (spec_flags & SPF_VOLATILE) {
    type->is_volatile = 1;
  }
  
  /* Apply declarator (pointers, arrays, functions) */
  if (decltor) {
    int i;
    struct Type *derived = type;
    
    /* Apply pointer levels */
    for (i = 0; i < decltor->pointer_level; i++) {
      derived = type_new_pointer(derived);
      if (!derived) return type;
    }
    
    /* Apply array */
    if (decltor->is_array) {
      int size = -1;
      if (decltor->array_size) {
        /* TODO: evaluate constant expression */
        size = -1;
      }
      derived = type_new_array(derived, size);
      if (!derived) return type;
    }
    
    /* Apply function */
    if (decltor->is_function) {
      struct ParamList *params = NULL;
      struct ParamList **tail = &params;
      struct ASTList *p;
      int param_count = 0;
      
      /* Convert AST param list to ParamList */
      for (p = decltor->params; p; p = p->next) {
        struct ParamList *new_param;
        struct ASTNode *param_node = p->node;
        
        new_param = (struct ParamList *)malloc(sizeof(struct ParamList));
        if (!new_param) break;
        
        if (param_node && param_node->kind_tag == 2) {
          /* Extract parameter type */
          new_param->type = sema_type_from_decl(sema, param_node);
          new_param->name = (param_node->u.decl.decltor && param_node->u.decl.decltor->name)
                            ? my_strdup(param_node->u.decl.decltor->name)
                            : NULL;
        } else {
          /* Old-style or abstract declarator */
          new_param->type = type_new_basic(TYPE_INT);
          new_param->name = NULL;
        }
        
        new_param->next = NULL;
        *tail = new_param;
        tail = &new_param->next;
        param_count++;
      }
      
      derived = type_new_function(derived, params, 0);
      if (!derived) return type;
    }
    
    type = derived;
  }
  
  return type;
}

static void analyze_decl(struct Sema *sema, struct ASTNode *decl) {
  struct Type *type;
  struct Symbol *existing, *sym;
  const char *name;
  int is_extern, is_static;
  int size;
  
  if (!decl || decl->kind_tag != 2) return;
  
  if (!decl->u.decl.decltor || !decl->u.decl.decltor->name) {
    return; /* anonymous declaration */
  }
  
  name = decl->u.decl.decltor->name;
  type = sema_type_from_decl(sema, decl);
  
  is_extern = (decl->u.decl.spec_flags & SPF_EXTERN) != 0;
  is_static = (decl->u.decl.spec_flags & SPF_STATIC) != 0;
  
  /* Check for redeclaration in current scope */
  existing = scope_lookup_current(sema->current_scope, name);
  if (existing) {
    /* In C90, we can redeclare with compatible types */
    if (!type_compatible(existing->type, type)) {
      char msg[256];
      sprintf(msg, "conflicting types for '%s'", name);
      sema_error(sema, decl->line, decl->column, msg);
    }
    type_free(type);
    return;
  }
  
  /* Add symbol to current scope */
  sym = scope_add_symbol(sema->current_scope, name, SYM_VAR, type,
                         decl->line, decl->column);
  if (sym) {
    sym->is_extern = is_extern;
    sym->is_static = is_static;
    
    /* Calculate stack offset for local variables using target sizes */
    if (sema->in_function && !is_extern && !is_static) {
      /* Size computation (Z80 16-bit pointers/ints):
         char=1, short=2, int=2, long=4, float=4, pointer=2.
         Arrays: element_size * array_size. Incomplete arrays: 0. */
      size = 0;
      if (type) {
        switch (type->kind) {
          case TYPE_CHAR:  size = 1; break;
          case TYPE_SHORT: size = 2; break;
          case TYPE_INT:   size = 2; break;
          case TYPE_LONG:  size = 4; break;
          case TYPE_FLOAT: size = 4; break;
          case TYPE_DOUBLE: size = 8; break; /* not really supported, but reserve */
          case TYPE_POINTER: size = 2; break;
          case TYPE_ARRAY:
            if (type->base_type && type->array_size >= 0) {
              switch (type->base_type->kind) {
                case TYPE_CHAR:  size = 1 * type->array_size; break;
                case TYPE_SHORT: size = 2 * type->array_size; break;
                case TYPE_INT:   size = 2 * type->array_size; break;
                case TYPE_LONG:  size = 4 * type->array_size; break;
                case TYPE_FLOAT: size = 4 * type->array_size; break;
                case TYPE_DOUBLE: size = 8 * type->array_size; break;
                case TYPE_POINTER: size = 2 * type->array_size; break;
                default: size = 0; break;
              }
            } else {
              size = 0; /* incomplete */
            }
            break;
          default:
            size = 0; /* structs/unions/enums not yet handled */
            break;
        }
      }
      sym->stack_offset = -(sema->current_scope->stack_size + size);
      sema->current_scope->stack_size += size;
    }
  }
}

static void analyze_function_decl(struct Sema *sema, struct ASTNode *ext) {
  struct ASTNode *decl;
  struct Type *type;
  struct Symbol *existing, *sym;
  const char *name;
  int is_static;
  
  if (!ext || ext->kind_tag != 3) return;
  
  decl = ext->u.ext.decl;
  if (!decl || decl->kind_tag != 2) return;
  if (!decl->u.decl.decltor || !decl->u.decl.decltor->name) return;
  
  name = decl->u.decl.decltor->name;
  type = sema_type_from_decl(sema, decl);
  
  is_static = (decl->u.decl.spec_flags & SPF_STATIC) != 0;
  
  /* Look up in global scope */
  existing = scope_lookup_current(sema->global_scope, name);
  
  if (existing) {
    /* Check type compatibility */
    if (!type_compatible(existing->type, type)) {
      char msg[256];
      sprintf(msg, "conflicting types for function '%s'", name);
      sema_error(sema, decl->line, decl->column, msg);
    }
    
    /* Mark as defined if this is a definition */
    if (ext->u.ext.is_function && ext->u.ext.body) {
      if (existing->is_defined) {
        char msg[256];
        sprintf(msg, "redefinition of function '%s'", name);
        sema_error(sema, decl->line, decl->column, msg);
      }
      existing->is_defined = 1;
    }
    
    type_free(type);
    return;
  }
  
  /* Add function symbol to global scope */
  sym = scope_add_symbol(sema->global_scope, name, SYM_FUNC, type,
                         decl->line, decl->column);
  if (sym) {
    sym->is_static = is_static;
    if (ext->u.ext.is_function && ext->u.ext.body) {
      sym->is_defined = 1;
    }
  }
}

static void analyze_function_def(struct Sema *sema, struct ASTNode *ext) {
  struct ASTNode *decl, *body;
  struct Type *func_type;
  
  if (!ext || ext->kind_tag != 3 || !ext->u.ext.is_function) return;
  
  decl = ext->u.ext.decl;
  body = ext->u.ext.body;
  
  if (!decl || !body) return;
  
  /* Get function type */
  func_type = sema_type_from_decl(sema, decl);
  
  /* Enter function scope */
  sema->in_function = 1;
  sema->current_function_return_type = (func_type && func_type->kind == TYPE_FUNCTION)
                                       ? func_type->return_type : NULL;
  sema_enter_scope(sema, 1);
  
  /* TODO: Add parameters to scope */
  
  /* Analyze function body */
  analyze_compound(sema, body);

  /* While the function scope is active, annotate expression nodes with inferred types */
  annotate_expr_types(sema, body);
  
  /* Store stack size in the external node for code generation */
  ext->u.ext.stack_size = sema->current_scope->stack_size;
  
  sema_exit_scope(sema);
  sema->in_function = 0;
  sema->current_function_return_type = NULL;
  
  type_free(func_type);
}

static struct Type *analyze_expr(struct Sema *sema, struct ASTNode *expr) {
  struct Type *t1, *t2, *t3;
  struct Symbol *sym;
  struct ASTList *arg;
  int arg_count;
  char msg[256];
  
  if (!expr || expr->kind_tag != 0) {
    {
      struct Type *rtmp = type_new_basic(TYPE_INT);
      if (expr && expr->kind_tag == 0) set_expr_type(expr, type_copy(rtmp));
      return rtmp;
    }
  }
  
  switch (expr->u.expr.kind) {
    case EXPR_CALL:
      /* Handle function calls first, so we can support implicit declarations in C90 */
      
      /* Check if calling an identifier that may need implicit declaration */
    if (expr->u.expr.e1 && expr->u.expr.e1->kind_tag == 0 &&
          expr->u.expr.e1->u.expr.kind == EXPR_IDENT) {
        sym = scope_lookup(sema->current_scope, expr->u.expr.e1->u.expr.ident);
        
        if (!sym) {
          /* C90 allows implicit function declarations */
          sprintf(msg, "implicit declaration of function '%s'", 
                  expr->u.expr.e1->u.expr.ident);
          fprintf(stderr, "Warning at %d:%d: %s\n", expr->line, expr->column, msg);
          
          /* Add implicit int() function declaration */
          t1 = type_new_function(type_new_basic(TYPE_INT), NULL, 0);
          sym = scope_add_symbol(sema->global_scope, expr->u.expr.e1->u.expr.ident, 
                                SYM_FUNC, t1, expr->line, expr->column);
          
          /* Analyze arguments */
          for (arg = expr->u.expr.args; arg; arg = arg->next) {
            t2 = analyze_expr(sema, arg->node);
            type_free(t2);
          }
          
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
        } else {
          /* Symbol exists, analyze normally */
          t1 = type_copy(sym->type);
        }
      } else {
        /* Not a simple identifier, analyze the expression */
        t1 = analyze_expr(sema, expr->u.expr.e1);
      }
      
      /* Check if it's a function type */
      if (t1 && t1->kind == TYPE_FUNCTION) {
        struct ParamList *param;
        struct ASTList *arg_node;
        struct Type *arg_type;
        int param_idx;
        
        /* Analyze and check arguments against parameters */
        arg_count = 0;
        param = t1->params;
        arg_node = expr->u.expr.args;
        param_idx = 0;
        
        while (arg_node) {
          arg_type = analyze_expr(sema, arg_node->node);
          
          /* Apply array-to-pointer decay */
          if (arg_type && arg_type->kind == TYPE_ARRAY) {
            struct Type *tmp = decay_array_to_pointer(arg_type);
            type_free(arg_type);
            arg_type = tmp;
          }
          
          /* Check against parameter type if we have a prototype */
          if (param && param->type) {
            if (!type_compatible(param->type, arg_type)) {
              if (type_is_arithmetic(param->type) && type_is_arithmetic(arg_type)) {
                /* Allow implicit arithmetic conversions (int<->float, etc.) */
              } else if (type_is_pointer(param->type) && type_is_pointer(arg_type)) {
                /* Pointers should match or be compatible */
                if (!type_equals(param->type, arg_type)) {
                  sprintf(msg, "passing argument %d with incompatible pointer type", param_idx + 1);
                  sema_error(sema, arg_node->node->line, arg_node->node->column, msg);
                }
              } else if (type_is_pointer(param->type) && type_is_integer(arg_type)) {
                /* Allow int-to-pointer for NULL (integer constant 0) only */
                /* For now, we allow any integer since we can't easily check if it's 0 */
                /* This is lenient but safer than allowing pointer-to-int */
              } else {
                sprintf(msg, "passing argument %d of incompatible type", param_idx + 1);
                sema_error(sema, arg_node->node->line, arg_node->node->column, msg);
              }
            }
            param = param->next;
          }
          
          type_free(arg_type);
          arg_node = arg_node->next;
          arg_count++;
          param_idx++;
        }
        
        /* Check argument count (only for non-varargs functions with known params) */
        if (!t1->has_varargs && t1->params && arg_count != t1->param_count) {
          sprintf(msg, "too %s arguments to function call, expected %d, have %d",
                  arg_count < t1->param_count ? "few" : "many",
                  t1->param_count, arg_count);
          sema_error(sema, expr->line, expr->column, msg);
        }
        
        /* Return function's return type */
        if (t1->return_type) {
          struct Type *ret = type_copy(t1->return_type);
          set_expr_type(expr, type_copy(ret));
          type_free(t1);
          return ret;
        }
      } else if (t1 && t1->kind == TYPE_POINTER && t1->base_type &&
                 t1->base_type->kind == TYPE_FUNCTION) {
        /* Function pointer call */
        struct Type *func_type = t1->base_type;
        
        /* Count arguments */
        arg_count = 0;
        for (arg = expr->u.expr.args; arg; arg = arg->next) {
          t2 = analyze_expr(sema, arg->node);
          type_free(t2);
          arg_count++;
        }
        
        if (func_type->return_type) {
          struct Type *ret = type_copy(func_type->return_type);
          set_expr_type(expr, type_copy(ret));
          type_free(t1);
          return ret;
        }
      } else {
        /* Not a function type */
        sema_error(sema, expr->line, expr->column, 
                  "called object is not a function or function pointer");
        
        /* Analyze arguments anyway */
        for (arg = expr->u.expr.args; arg; arg = arg->next) {
          t2 = analyze_expr(sema, arg->node);
          type_free(t2);
        }
      }
      
      type_free(t1);
      {
        struct Type *ret = type_new_basic(TYPE_INT);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
    
    case EXPR_IDENT:
      /* Look up identifier */
      sym = scope_lookup(sema->current_scope, expr->u.expr.ident);
      if (!sym) {
        sprintf(msg, "use of undeclared identifier '%s'", expr->u.expr.ident);
        sema_error(sema, expr->line, expr->column, msg);
        {
          struct Type *ret = type_new_basic(TYPE_INT);
          set_expr_type(expr, type_copy(ret));
          return ret;
        }
      }
      {
        struct Type *ret = type_copy(sym->type);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
      
    case EXPR_CONST:
      /* Determine type from constant kind */
      switch (expr->u.expr.const_kind) {
        case AST_C_INT:
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
        case AST_C_UINT: {
          struct Type *ti = type_new_basic(TYPE_INT);
          ti->is_signed = 0;
          set_expr_type(expr, type_copy(ti));
          return ti;
        }
        case AST_C_CHAR:
          /* In C90, character constants have type int (not char). */
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
        case AST_C_FLOAT:
          {
            struct Type *ret = type_new_basic(TYPE_FLOAT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
        case AST_C_DOUBLE:
          sema_error(sema, expr->line, expr->column,
                     "double constant is not supported on this target (>32-bit)");
          {
            struct Type *ret = type_new_basic(TYPE_DOUBLE);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
        default:
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
      }
      
    case EXPR_STRING: {
      int n = 0;
      if (expr->u.expr.str) {
        n = (int)strlen(expr->u.expr.str) + 1;
      }
      {
        struct Type *ret = type_new_array(type_new_basic(TYPE_CHAR), n);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
    }
      
    case EXPR_UNARY:
      t1 = analyze_expr(sema, expr->u.expr.e1);
      
      /* Check unary operator types */
      switch (expr->u.expr.op) {
        case '&': /* address-of */
          /* Result is pointer to operand type */
          {
            struct Type *ret = type_new_pointer(t1);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
          
        case '*': /* dereference */
          /* Operand must be pointer */
          if (t1 && type_is_pointer(t1) && t1->base_type) {
            struct Type *base = type_copy(t1->base_type);
            type_free(t1);
            set_expr_type(expr, type_copy(base));
            return base;
          }
          sema_error(sema, expr->line, expr->column,
                     "indirection requires pointer operand");
          type_free(t1);
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
          
        case '+':
        case '-':
        case '~':
          /* Arithmetic/bitwise unary - require arithmetic type */
          if (t1 && !type_is_arithmetic(t1)) {
            sema_error(sema, expr->line, expr->column,
                       "unary operator requires arithmetic operand");
          }
          /* Apply integer promotion */
          if (t1 && type_is_integer(t1)) {
            struct Type *promoted = integer_promotion(t1);
            type_free(t1);
            set_expr_type(expr, type_copy(promoted));
            return promoted;
          }
          set_expr_type(expr, type_copy(t1));
          return t1;
          
        case '!':
          /* Logical NOT - any scalar type */
          if (t1 && !type_is_arithmetic(t1) && !type_is_pointer(t1)) {
            sema_error(sema, expr->line, expr->column,
                       "logical not requires scalar operand");
          }
          type_free(t1);
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
          
        case OP_INC:
        case OP_DEC:
          /* Increment/decrement - require arithmetic or pointer */
          if (t1 && !type_is_arithmetic(t1) && !type_is_pointer(t1)) {
            sema_error(sema, expr->line, expr->column,
                       "increment/decrement requires arithmetic or pointer operand");
          }
          set_expr_type(expr, type_copy(t1));
          return t1;
          
        case OP_SIZEOF:
          /* sizeof always returns size_t (we use int for simplicity) */
          type_free(t1);
          {
            struct Type *ret = type_new_basic(TYPE_INT);
            set_expr_type(expr, type_copy(ret));
            return ret;
          }
          
        default:
          /* Unknown unary operator */
          set_expr_type(expr, type_copy(t1));
          return t1;
      }
      
    case EXPR_BINARY:
      t1 = analyze_expr(sema, expr->u.expr.e1);
      t2 = analyze_expr(sema, expr->u.expr.e2);
      
      /* Apply array-to-pointer decay for value operands */
      if (t1 && t1->kind == TYPE_ARRAY) {
        struct Type *tmp = decay_array_to_pointer(t1);
        type_free(t1);
        t1 = tmp;
      }
      if (t2 && t2->kind == TYPE_ARRAY) {
        struct Type *tmp = decay_array_to_pointer(t2);
        type_free(t2);
        t2 = tmp;
      }
      
      /* Assignment operators */
      if (expr->u.expr.op == '=') {
        if (!type_compatible(t1, t2)) {
          if (type_is_arithmetic(t1) && type_is_arithmetic(t2)) {
            /* arithmetic types are compatible */
          } else if (type_is_pointer(t1) && type_is_pointer(t2)) {
            if (!type_equals(t1, t2)) {
              sema_error(sema, expr->line, expr->column,
                         "incompatible pointer types in assignment");
            }
          } else if (type_is_pointer(t1) && type_is_integer(t2)) {
            sema_error(sema, expr->line, expr->column,
                       "cannot assign integer to pointer without cast");
          } else {
            sema_error(sema, expr->line, expr->column,
                       "incompatible types in assignment");
          }
        }
        type_free(t2);
        set_expr_type(expr, type_copy(t1));
        return t1;
      }
      
      /* Compound assignment operators */
      if (expr->u.expr.op == OP_ADD_ASSIGN || expr->u.expr.op == OP_SUB_ASSIGN ||
          expr->u.expr.op == OP_MUL_ASSIGN || expr->u.expr.op == OP_DIV_ASSIGN ||
          expr->u.expr.op == OP_MOD_ASSIGN || expr->u.expr.op == OP_AND_ASSIGN ||
          expr->u.expr.op == OP_OR_ASSIGN || expr->u.expr.op == OP_XOR_ASSIGN ||
          expr->u.expr.op == OP_SHL_ASSIGN || expr->u.expr.op == OP_SHR_ASSIGN) {
        /* Similar rules to simple assignment but with arithmetic operation */
        if (!type_is_arithmetic(t1) || !type_is_arithmetic(t2)) {
          sema_error(sema, expr->line, expr->column,
                     "compound assignment requires arithmetic operands");
        }
        type_free(t2);
        set_expr_type(expr, type_copy(t1));
        return t1;
      }
      
      /* Pointer arithmetic */
      if (expr->u.expr.op == '+' || expr->u.expr.op == '-') {
        if (type_is_pointer(t1) && type_is_integer(t2)) {
          type_free(t2);
          set_expr_type(expr, type_copy(t1));
          return t1;
        }
        if (type_is_pointer(t2) && type_is_integer(t1) && expr->u.expr.op == '+') {
          type_free(t1);
          set_expr_type(expr, type_copy(t2));
          return t2;
        }
      }
      
      /* Arithmetic operators */
      if (expr->u.expr.op == '*' || expr->u.expr.op == '/' || expr->u.expr.op == '%') {
        if (!type_is_arithmetic(t1) || !type_is_arithmetic(t2)) {
          sema_error(sema, expr->line, expr->column,
                     "arithmetic operator requires arithmetic operands");
          type_free(t2);
          return t1;
        }
        if (expr->u.expr.op == '%') {
          if (!type_is_integer(t1) || !type_is_integer(t2)) {
            sema_error(sema, expr->line, expr->column,
                       "modulo operator requires integer operands");
          }
        }
      }
      
      /* Comparison operators */
      if (expr->u.expr.op == '<' || expr->u.expr.op == '>' ||
          expr->u.expr.op == OP_LE || expr->u.expr.op == OP_GE ||
          expr->u.expr.op == OP_EQ || expr->u.expr.op == OP_NE) {
        /* Can compare arithmetic or pointer types */
        if (!type_is_arithmetic(t1) && !type_is_pointer(t1)) {
          sema_error(sema, expr->line, expr->column,
                     "comparison requires scalar operands");
        }
        type_free(t1);
        type_free(t2);
        {
          struct Type *ret = type_new_basic(TYPE_INT); /* comparison result is int */
          set_expr_type(expr, type_copy(ret));
          return ret;
        }
      }
      
      /* Bitwise operators */
      if (expr->u.expr.op == '&' || expr->u.expr.op == '|' || expr->u.expr.op == '^' ||
          expr->u.expr.op == OP_SHL || expr->u.expr.op == OP_SHR) {
        if (!type_is_integer(t1) || !type_is_integer(t2)) {
          sema_error(sema, expr->line, expr->column,
                     "bitwise operator requires integer operands");
        }
      }
      
      /* Logical operators */
      if (expr->u.expr.op == OP_LAND || expr->u.expr.op == OP_LOR) {
        /* Accept any scalar type */
        type_free(t1);
        type_free(t2);
        {
          struct Type *ret = type_new_basic(TYPE_INT);
          set_expr_type(expr, type_copy(ret));
          return ret;
        }
      }
      
      if (type_is_arithmetic(t1) && type_is_arithmetic(t2)) {
        struct Type *rt = usual_arith_conv(sema, t1, t2, expr->line, expr->column);
        set_expr_type(expr, type_copy(rt));
        type_free(t1);
        type_free(t2);
        return rt;
      }
      
      /* Otherwise invalid */
      sema_error(sema, expr->line, expr->column,
                 "invalid operands to binary operator");
      type_free(t2);
      set_expr_type(expr, type_copy(t1));
      return t1;
      
    case EXPR_COND:
      t1 = analyze_expr(sema, expr->u.expr.e1);
      t2 = analyze_expr(sema, expr->u.expr.e2);
      t3 = analyze_expr(sema, expr->u.expr.e3);
      type_free(t1);
      type_free(t3);
      set_expr_type(expr, type_copy(t2));
      return t2;
      
    case EXPR_INDEX:
      t1 = analyze_expr(sema, expr->u.expr.e1);
      t2 = analyze_expr(sema, expr->u.expr.e2);
      type_free(t2);
      
      /* Array subscript returns base type */
      if (t1 && t1->kind == TYPE_ARRAY && t1->base_type) {
        struct Type *base = type_copy(t1->base_type);
        type_free(t1);
        set_expr_type(expr, type_copy(base));
        return base;
      }
      if (t1 && t1->kind == TYPE_POINTER && t1->base_type) {
        struct Type *base = type_copy(t1->base_type);
        type_free(t1);
        set_expr_type(expr, type_copy(base));
        return base;
      }
      
      type_free(t1);
      {
        struct Type *ret = type_new_basic(TYPE_INT);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
      
    case EXPR_MEMBER:
    case EXPR_ARROW:
      /* TODO: struct/union member access */
      analyze_expr(sema, expr->u.expr.e1);
      {
        struct Type *ret = type_new_basic(TYPE_INT);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
      
    case EXPR_CAST: {
      int spec_flags = expr->u.expr.op;
      int pointer_level = (int)(long)expr->u.expr.e2;
      struct Type *operand_type;
      struct Type *target_type;
      
      /* Analyze the operand */
      operand_type = analyze_expr(sema, expr->u.expr.e1);
      if (!operand_type) {
        struct Type *ret = type_new_basic(TYPE_INT);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
      
      /* Build the target type from spec_flags */
      if (spec_flags & SPF_DOUBLE) {
        sema_error(sema, expr->line, expr->column,
                  "double is not supported on this target (>32-bit)");
        {
          struct Type *ret = type_new_basic(TYPE_INT);
          set_expr_type(expr, type_copy(ret));
          return ret;
        }
      }
      
      /* Determine base type */
      if (spec_flags & SPF_VOID)
        target_type = type_new_basic(TYPE_VOID);
      else if (spec_flags & SPF_CHAR)
        target_type = type_new_basic(TYPE_CHAR);
      else if (spec_flags & SPF_SHORT)
        target_type = type_new_basic(TYPE_SHORT);
      else if (spec_flags & SPF_LONG)
        target_type = type_new_basic(TYPE_LONG);
      else if (spec_flags & SPF_FLOAT)
        target_type = type_new_basic(TYPE_FLOAT);
      else
        target_type = type_new_basic(TYPE_INT);
      
      /* Apply pointer levels */
      while (pointer_level > 0) {
        target_type = type_new_pointer(target_type);
        pointer_level--;
      }
      
      /* Explicit casts are always allowed in C90, but warn about dangerous ones */
  set_expr_type(expr, type_copy(target_type));
  return target_type;
    }
      
    default:
      {
        struct Type *ret = type_new_basic(TYPE_INT);
        set_expr_type(expr, type_copy(ret));
        return ret;
      }
  }
}

static void analyze_stmt(struct Sema *sema, struct ASTNode *stmt) {
  struct Type *t;
  
  if (!stmt || stmt->kind_tag != 1) return;
  
  switch (stmt->u.stmt.kind) {
    case STMT_COMPOUND:
      analyze_compound(sema, stmt);
      break;
      
    case STMT_EXPR:
      if (stmt->u.stmt.expr) {
        t = analyze_expr(sema, stmt->u.stmt.expr);
        type_free(t);
      }
      break;
      
    case STMT_IF:
      if (stmt->u.stmt.s1) {
        t = analyze_expr(sema, stmt->u.stmt.s1);
        type_free(t);
      }
      if (stmt->u.stmt.s2) {
        analyze_stmt(sema, stmt->u.stmt.s2);
      }
      if (stmt->u.stmt.s3) {
        analyze_stmt(sema, stmt->u.stmt.s3);
      }
      break;
      
    case STMT_WHILE:
    case STMT_DOWHILE:
      if (stmt->u.stmt.s1) {
        t = analyze_expr(sema, stmt->u.stmt.s1);
        type_free(t);
      }
      if (stmt->u.stmt.s2) {
        analyze_stmt(sema, stmt->u.stmt.s2);
      }
      break;
      
    case STMT_FOR:
      if (stmt->u.stmt.s1) {
        t = analyze_expr(sema, stmt->u.stmt.s1);
        type_free(t);
      }
      if (stmt->u.stmt.s2) {
        t = analyze_expr(sema, stmt->u.stmt.s2);
        type_free(t);
      }
      if (stmt->u.stmt.s3) {
        t = analyze_expr(sema, stmt->u.stmt.s3);
        type_free(t);
      }
      if (stmt->u.stmt.expr) {
        analyze_stmt(sema, stmt->u.stmt.expr);
      }
      break;
      
    case STMT_RETURN:
      if (stmt->u.stmt.expr) {
        t = analyze_expr(sema, stmt->u.stmt.expr);
        /* TODO: check against function return type */
        type_free(t);
      }
      break;
      
    case STMT_SWITCH:
      if (stmt->u.stmt.s1) {
        t = analyze_expr(sema, stmt->u.stmt.s1);
        type_free(t);
      }
      if (stmt->u.stmt.s2) {
        analyze_stmt(sema, stmt->u.stmt.s2);
      }
      break;
      
    case STMT_LABEL:
      if (stmt->u.stmt.s1) {
        analyze_stmt(sema, stmt->u.stmt.s1);
      }
      break;
      
    case STMT_CASE:
    case STMT_DEFAULT:
      if (stmt->u.stmt.expr) {
        t = analyze_expr(sema, stmt->u.stmt.expr);
        type_free(t);
      }
      if (stmt->u.stmt.s1) {
        analyze_stmt(sema, stmt->u.stmt.s1);
      }
      break;
      
    case STMT_BREAK:
    case STMT_CONTINUE:
    case STMT_GOTO:
      /* Nothing to analyze */
      break;
      
    default:
      break;
  }
}

static void analyze_compound(struct Sema *sema, struct ASTNode *compound) {
  struct ASTList *item;
  struct ASTNode *node;
  
  if (!compound || compound->kind_tag != 1 || 
      compound->u.stmt.kind != STMT_COMPOUND) {
    return;
  }
  
  /* Enter new scope for compound statement (except function body scope already entered) */
  if (!sema->current_scope->is_function_scope) {
    sema_enter_scope(sema, 0);
  }
  
  /* Process declarations and statements */
  for (item = compound->u.stmt.stmts; item; item = item->next) {
    node = item->node;
    if (!node) continue;
    
    if (node->kind_tag == 2) {
      /* Declaration */
      analyze_decl(sema, node);
    } else if (node->kind_tag == 1) {
      /* Statement */
      analyze_stmt(sema, node);
    }
  }
  
  if (!sema->current_scope->is_function_scope) {
    sema_exit_scope(sema);
  }
}

int sema_analyze(struct Sema *sema, struct ASTNode *tree) {
  struct ASTList *item;
  struct ASTNode *node;
  
  if (!sema || !tree) return 0;
  
  /* Translation unit is a compound statement */
  if (tree->kind_tag != 1 || tree->u.stmt.kind != STMT_COMPOUND) {
    return 0;
  }
  
  /* Process all external declarations */
  for (item = tree->u.stmt.stmts; item; item = item->next) {
    node = item->node;
    if (!node || node->kind_tag != 3) continue;
    
    /* Declare function/variable in global scope */
    analyze_function_decl(sema, node);
    
    /* If it's a function definition, analyze body */
    if (node->u.ext.is_function && node->u.ext.body) {
      analyze_function_def(sema, node);
    }
  }
  return sema->error_count == 0;
}
