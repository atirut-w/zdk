#include "target.h"
#include "sema.h" /* for TypeKind on expr->type */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Minimal default initialization: no-op for action pointers (targets fill them). */
void codegen_init_defaults(struct Codegen *cg) {
  (void)cg;
}

/* Simple local variable table for codegen (recomputed). */
struct CGLocal {
  char *name;
  int offset; /* negative offset from IX */
  int size;   /* bytes */
  int is_char; /* 1 if char, else treat as int */
  int pointer_level; /* >0 for pointer */
  int base_is_char;  /* base type is char (for pointers) */
  int is_array; /* declarator is array */
  struct CGLocal *next;
};

static void cg_locals_free(struct CGLocal *l) {
  struct CGLocal *n; while (l) { n = l->next; if (l->name) free(l->name); free(l); l = n; }
}

/* ==========================
   String literal collection
   ========================== */

struct CGString {
  int id;
  char *content;
  struct CGString *next;
};

static struct CGString *g_string_list = NULL;
static int g_string_count = 0;

static char *cg_strdup(const char *s) {
  char *r; size_t n;
  if (!s) return NULL;
  n = strlen(s) + 1;
  r = (char *)malloc(n);
  if (r) memcpy(r, s, n);
  return r;
}

static int cg_add_string(struct Codegen *cg, const char *content) {
  struct CGString *node;
  int id;
  (void)cg;
  id = g_string_count++;
  node = (struct CGString *)malloc(sizeof(struct CGString));
  if (!node) return id;
  node->id = id;
  node->content = cg_strdup(content ? content : "");
  node->next = g_string_list;
  g_string_list = node;
  return id;
}

static void cg_free_strings(struct Codegen *cg) {
  struct CGString *s = g_string_list; struct CGString *n;
  (void)cg;
  while (s) { n = s->next; if (s->content) free(s->content); free(s); s = n; }
  g_string_list = NULL; g_string_count = 0;
}

/* Emit the string literal content between quotes as-is for the assembler,
   escaping only embedded double-quotes. This preserves backslash escapes like \r, \n. */
static void cg_emit_string_for_asm(FILE *out, const char *p) {
  const char *s = p;
  size_t len;
  size_t i;
  if (!s) return;
  len = strlen(s);
  if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
    s = s + 1;
    len -= 2;
  }
  for (i = 0; i < len; i++) {
    unsigned char c = (unsigned char)s[i];
    if (c == '"') {
      fprintf(out, "\\\"");
    } else {
      fputc(c, out);
    }
  }
}

static void cg_emit_string_data(struct Codegen *cg) {
  struct CGString *s;
  if (!cg || !cg->output) return;
  if (!g_string_list) return;
  fprintf(cg->output, "\n\t.data\n");
  /* Emit in reverse of collection order doesn't matter for labels */
  for (s = g_string_list; s; s = s->next) {
    fprintf(cg->output, ".LC%d:\n", s->id);
    /* Emit as asciz, preserving single backslash escapes (\r, \n, etc.). */
    fprintf(cg->output, "\t.asciz \"");
    cg_emit_string_for_asm(cg->output, s->content);
    fprintf(cg->output, "\"\n");
  }
}

/* Compute byte length of C string literal content (handles basic escapes). */
static int cg_string_length_bytes(const char *s) {
  int len = 0;
  const char *p;
  if (!s) return 0;
  p = s;
  if (*p == '"') {
    p++;
    while (*p) {
      if (*p == '"') { p++; break; }
      if (*p == '\\') {
        p++;
        if (*p == 'x' || *p == 'X') {
          p++;
          while ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) p++;
          len++;
        } else if (*p >= '0' && *p <= '7') {
          int i = 0; while (i < 3 && *p >= '0' && *p <= '7') { p++; i++; }
          len++;
        } else if (*p) {
          p++; len++;
        }
      } else { p++; len++; }
    }
  }
  return len;
}

static int type_size_from_spec(int spec_flags) {
  if (spec_flags & SPF_CHAR) return 1;
  if (spec_flags & SPF_LONG) return 4; /* placeholder */
  if (spec_flags & SPF_FLOAT) return 4;
  return 2; /* default int/short */
}

static int is_char_from_spec(int spec_flags) {
  return (spec_flags & SPF_CHAR) != 0;
}

static struct CGLocal *build_locals(struct ASTNode *body) {
  struct CGLocal *head = NULL; int accumulated = 0; struct ASTList *list; struct ASTNode *n;
  if (!body || body->kind_tag != 1 || body->u.stmt.kind != STMT_COMPOUND) return NULL;
  for (list = body->u.stmt.stmts; list; list = list->next) {
    n = list->node; if (!n || n->kind_tag != 2) continue; /* declaration */
    if (!n->u.decl.decltor || !n->u.decl.decltor->name) continue;
    {
      struct CGLocal *loc = (struct CGLocal *)malloc(sizeof(struct CGLocal));
      int ptr = n->u.decl.decltor ? n->u.decl.decltor->pointer_level : 0;
      int base_char = is_char_from_spec(n->u.decl.spec_flags);
      int sz;
      /* Handle arrays: element size * count; incomplete arrays default to 0 unless string init */
      if (n->u.decl.decltor->is_array) {
        int elem_size = (ptr > 0) ? 2 : type_size_from_spec(n->u.decl.spec_flags);
        int count = 0;
        /* Try to get explicit constant size */
        if (n->u.decl.decltor->array_size &&
            n->u.decl.decltor->array_size->kind_tag == 0 &&
            n->u.decl.decltor->array_size->u.expr.kind == EXPR_CONST &&
            n->u.decl.decltor->array_size->u.expr.const_lexeme) {
          count = atoi(n->u.decl.decltor->array_size->u.expr.const_lexeme);
        } else if (base_char && ptr == 0 && n->u.decl.init &&
                   n->u.decl.init->kind_tag == 0 && n->u.decl.init->u.expr.kind == EXPR_STRING) {
          /* Infer from string literal */
          int l = cg_string_length_bytes(n->u.decl.init->u.expr.str);
          count = l + 1; /* include NUL */
        }
        sz = (count > 0) ? elem_size * count : 0;
        /* For arrays, treat is_char for element type only when elem_size==1 */
      } else {
        sz = (ptr > 0) ? 2 : type_size_from_spec(n->u.decl.spec_flags);
      }
      loc->size = sz;
      loc->offset = -(accumulated + sz);
      loc->pointer_level = ptr;
      loc->base_is_char = base_char;
      loc->is_char = (ptr == 0) && base_char && !n->u.decl.decltor->is_array;
      loc->is_array = n->u.decl.decltor->is_array;
      loc->name = (char *)malloc(strlen(n->u.decl.decltor->name) + 1);
      if (loc->name) strcpy(loc->name, n->u.decl.decltor->name);
      loc->next = head; head = loc; accumulated += sz;
    }
  }
  return head;
}

static struct CGLocal *find_local(struct CGLocal *head, const char *name) {
  for (; head; head = head->next) if (head->name && strcmp(head->name, name) == 0) return head; return NULL;
}

/* Compute size in bytes for a semantic type (minimal mapping for this target). */
static int type_size_from_type(struct Type *t) {
  if (!t) return 2;
  switch (t->kind) {
    case TYPE_CHAR: return 1;
    case TYPE_SHORT: return 2;
    case TYPE_INT: return 2;
    case TYPE_LONG: return 4;
    case TYPE_FLOAT: return 4;
    case TYPE_POINTER: return 2;
    case TYPE_ARRAY: return type_size_from_type(t->base_type);
    default: return 2;
  }
}

static int kind_is_integer(enum TypeKind k) {
  return (k == TYPE_CHAR || k == TYPE_SHORT || k == TYPE_INT || k == TYPE_LONG);
}

/* Expression generation producing result in target primary value register. */
static void gen_expr(struct Codegen *cg, struct ASTNode *expr, struct CGLocal *locals);

static void gen_load_ident(struct Codegen *cg, struct ASTNode *ident_expr, struct CGLocal *locals) {
  struct CGLocal *loc;
  if (!cg) return;
  if (!ident_expr || ident_expr->kind_tag != 0) return;
  loc = find_local(locals, ident_expr->u.expr.ident);
  if (loc) {
    /* For arrays used as rvalues, yield their address (HL), not the contents. */
    if (loc->is_array) {
      if (cg->addr_local) cg->addr_local(cg, loc->offset); /* HL = &array */
      return;
    }
    if (cg->addr_local) cg->addr_local(cg, loc->offset);
    if (cg->addr_from_value) cg->addr_from_value(cg);
    if (loc->is_char) {
      if (cg->load_char) cg->load_char(cg); /* A,L = byte (target ensures L set) */
    } else {
      if (cg->load_int) cg->load_int(cg);
    }
  } else {
    /* Treat as global symbol */
    /* If this identifier refers to an object with addressable storage and is used as rvalue,
       prefer emitting its address for arrays/labels; for other objects, load value. */
    if (cg->addr_symbol) cg->addr_symbol(cg, ident_expr->u.expr.ident);
    if (ident_expr->type && ident_expr->type->kind == TYPE_POINTER) {
      /* Assume decayed array -> want address in HL. Do not dereference/load. */
      return;
    } else {
      if (cg->addr_from_value) cg->addr_from_value(cg);
      if (ident_expr->type && ident_expr->type->kind == TYPE_CHAR) {
        if (cg->load_char) cg->load_char(cg);
      } else {
        if (cg->load_int) cg->load_int(cg);
      }
    }
  }
}

static void gen_store_ident(struct Codegen *cg, const char *name, struct CGLocal *locals, int is_char) {
  struct CGLocal *loc;
  if (!cg) return;
  loc = find_local(locals, name);
  if (loc) {
    if (cg->addr_local) cg->addr_local(cg, loc->offset);
    if (cg->addr_from_value) cg->addr_from_value(cg);
    if (loc->is_char || is_char) {
      if (cg->store_char) cg->store_char(cg);
    } else {
      if (cg->store_int) cg->store_int(cg);
    }
  } else {
    /* Global variable store (assume int) */
    if (cg->addr_symbol) cg->addr_symbol(cg, name);
    if (cg->addr_from_value) cg->addr_from_value(cg);
    if (cg->store_int) cg->store_int(cg);
  }
}

static void gen_binary(struct Codegen *cg, struct ASTNode *expr, struct CGLocal *locals) {
  int op;
  if (!cg || !expr) return;
  op = expr->u.expr.op;
  {
    struct Type *t1 = (expr->u.expr.e1 && expr->u.expr.e1->type) ? expr->u.expr.e1->type : NULL;
    struct Type *t2 = (expr->u.expr.e2 && expr->u.expr.e2->type) ? expr->u.expr.e2->type : NULL;
    int left_is_ptr = (t1 && t1->kind == TYPE_POINTER);
    int right_is_ptr = (t2 && t2->kind == TYPE_POINTER);
    int left_is_int = (t1 && kind_is_integer(t1->kind));
    int right_is_int = (t2 && kind_is_integer(t2->kind));
    int elem_size;
    /* Default evaluation: left then right */
  gen_expr(cg, expr->u.expr.e1, locals); /* left value */
  if (cg->save_value) cg->save_value(cg);
  gen_expr(cg, expr->u.expr.e2, locals); /* right value */
  if (cg->value_to_rhs) cg->value_to_rhs(cg); /* move right to RHS */
  if (cg->restore_value) cg->restore_value(cg); /* restore left */

    /* Pointer arithmetic */
    if (op == '+') {
      if (left_is_ptr && right_is_int) {
        elem_size = type_size_from_type(t1 ? t1->base_type : NULL);
        if (cg->scale_rhs_by) cg->scale_rhs_by(cg, elem_size);
      } else if (left_is_int && right_is_ptr) {
        /* Swap to get HL=ptr, DE=int */
        if (cg->rhs_to_lhs) cg->rhs_to_lhs(cg);
        elem_size = type_size_from_type(t2 ? t2->base_type : NULL);
        if (cg->scale_rhs_by) cg->scale_rhs_by(cg, elem_size);
      }
    } else if (op == '-') {
      if (left_is_ptr && right_is_int) {
        elem_size = type_size_from_type(t1 ? t1->base_type : NULL);
        if (cg->scale_rhs_by) cg->scale_rhs_by(cg, elem_size);
      } else if (left_is_ptr && right_is_ptr) {
        /* Raw pointer difference, then divide by element size */
  if (cg->op_sub) cg->op_sub(cg);
        elem_size = type_size_from_type(t1 ? t1->base_type : NULL);
        if (elem_size > 1 && cg->divide_value_by) cg->divide_value_by(cg, elem_size);
        return; /* Done */
      }
    }
  }
  {
    switch (op) {
      case '+': if (cg->op_add) cg->op_add(cg); break;
      case '-': if (cg->op_sub) cg->op_sub(cg); break;
      case '*': if (cg->op_mul) cg->op_mul(cg); break;
      case '/': if (cg->op_div) cg->op_div(cg); break;
      case '%': if (cg->op_mod) cg->op_mod(cg); break;
      case OP_SHL: if (cg->op_shl) cg->op_shl(cg); break;
      case OP_SHR: if (cg->op_shr) cg->op_shr(cg); break;
      case '=': /* handled elsewhere */ break;
      default: fprintf(cg->output, "\t; TODO unsupported binary op %d\n", op); break;
    }
  }
}

static void gen_expr(struct Codegen *cg, struct ASTNode *expr, struct CGLocal *locals) {
  if (!cg || !expr) return;
  if (expr->kind_tag != 0) return; /* not expression */
  switch (expr->u.expr.kind) {
    case EXPR_CONST:
      if (cg->emit_const_int) cg->emit_const_int(cg, expr->u.expr.const_lexeme ? expr->u.expr.const_lexeme : "0");
      break;
    case EXPR_INDEX: {
      /* a[b] -> *(a + b) with scaling by element size */
      int elem_size = 1; int is_char_elem = 0;
      if (expr->type) {
        is_char_elem = (expr->type->kind == TYPE_CHAR);
        elem_size = type_size_from_type(expr->type);
        if (elem_size <= 0) elem_size = 1;
      }
      /* Evaluate base address into HL (arrays decay to pointer in gen_load_ident) */
      gen_expr(cg, expr->u.expr.e1, locals); /* HL = base address */
      if (cg->save_value) cg->save_value(cg); /* save base */
      /* Evaluate index into HL */
      gen_expr(cg, expr->u.expr.e2, locals);
      if (cg->value_to_rhs) cg->value_to_rhs(cg); /* DE = index */
      if (cg->restore_value) cg->restore_value(cg); /* HL = base */
      /* Scale index by element size if needed: HL=base, DE=index */
      if (elem_size > 1 && cg->scale_rhs_by) cg->scale_rhs_by(cg, elem_size);
      /* HL += DE -> effective address */
      if (cg->op_add) cg->op_add(cg);
      /* Dereference: move address HL -> IY and load */
      if (cg->addr_from_value) cg->addr_from_value(cg);
      if (is_char_elem) {
        if (cg->load_char) cg->load_char(cg);
      } else {
        if (cg->load_int) cg->load_int(cg);
      }
      break;
    }
    case EXPR_STRING:
      {
        int sid; char label[32];
        sid = cg_add_string(cg, expr->u.expr.str ? expr->u.expr.str : "");
        sprintf(label, ".LC%d", sid);
        if (cg->emit_const_int) cg->emit_const_int(cg, label);
      }
      break;
    case EXPR_IDENT:
      gen_load_ident(cg, expr, locals);
      break;
    case EXPR_CAST: {
      /* Evaluate operand then apply simple width conversion for char/int/pointer.
         Only integral and pointer casts implemented; floats/long not yet handled. */
      struct Type *target = expr->type;
      gen_expr(cg, expr->u.expr.e1, locals);
      if (target) {
        if (target->kind == TYPE_CHAR) {
          /* Truncate to 8 bits and zero-extend */
          if (cg->truncate_to_char) cg->truncate_to_char(cg);
        } else {
          /* For other supported target kinds (int, pointer) no action needed. */
        }
      }
      break;
    }
    case EXPR_CALL: {
      /* Function call: push args right-to-left, direct call only (identifier). */
      struct ASTList *arg; int count = 0, i; struct ASTNode **arr; int total_bytes = 0;
      /* Helper to estimate argument size with limited info */
      /* char identifiers ->1, else 2 (int/pointer). Strings passed as pointer (2). */
      if (expr->u.expr.args) {
        for (arg = expr->u.expr.args; arg; arg = arg->next) count++;
        arr = (struct ASTNode **)malloc(sizeof(struct ASTNode *) * count);
        if (arr) {
          i = 0; for (arg = expr->u.expr.args; arg; arg = arg->next) arr[i++] = arg->node;
          for (i = count - 1; i >= 0; i--) {
            struct ASTNode *a = arr[i]; int sz = 2; struct CGLocal *loc;
            if (a && a->kind_tag == 0) {
              if (a->u.expr.kind == EXPR_IDENT) {
                loc = find_local(locals, a->u.expr.ident);
                if (loc && loc->is_char) sz = 1;
              } else if (a->u.expr.kind == EXPR_STRING) {
                sz = 2; /* pointer */
              } else if (a->u.expr.kind == EXPR_CONST) {
                /* Assume int constant -> 2 */
                sz = 2;
              }
            }
            gen_expr(cg, a, locals); /* HL holds argument value */
            if (cg->push_arg) cg->push_arg(cg); /* pushes 2 bytes always for now */
            /* If sz ==1 we still pushed 2 bytes as ABI says 8-bit promoted/padded. */
            total_bytes += (sz <= 1 ? 2 : 2); /* all pushes are 2 currently */
          }
          free(arr);
        }
      }
      if (expr->u.expr.e1 && expr->u.expr.e1->u.expr.kind == EXPR_IDENT) {
        if (cg->call_direct) cg->call_direct(cg, expr->u.expr.e1->u.expr.ident);
      } else {
        fprintf(cg->output, "\t; TODO indirect call\n");
      }
      if (total_bytes > 0 && cg->cleanup_args) cg->cleanup_args(cg, total_bytes);
      /* Result presumed in HL */
      break;
    }
    case EXPR_UNARY:
      if (expr->u.expr.op == '-') {
        gen_expr(cg, expr->u.expr.e1, locals);
        /* Operand already evaluated and (if char) promoted via zero_extend.
           Use generic neg. */
        if (cg->op_neg) cg->op_neg(cg);
      } else if (expr->u.expr.op == OP_POST_INC) {
        /* Postfix increment for identifiers: pointer (scaled), int/short, char. */
        struct ASTNode *operand = expr->u.expr.e1;
        if (operand && operand->kind_tag == 0 && operand->u.expr.kind == EXPR_IDENT) {
          const char *name = operand->u.expr.ident;
          struct CGLocal *loc = find_local(locals, name);
          enum TypeKind tk = operand->type ? operand->type->kind : TYPE_INT;
          int is_ptr = (tk == TYPE_POINTER);
          int is_char = (tk == TYPE_CHAR);
          int inc_amount = 1;
          if (is_ptr) {
            if (operand->type && operand->type->base_type) {
              inc_amount = type_size_from_type(operand->type->base_type);
              if (inc_amount <= 0) inc_amount = 1;
            }
          }
          /* Compute address of variable into HL */
          if (loc) {
            if (cg->addr_local) cg->addr_local(cg, loc->offset);
          } else {
            if (cg->addr_symbol) cg->addr_symbol(cg, name);
          }
          if (cg->addr_from_value) cg->addr_from_value(cg); /* IY = address */
          /* Load current value */
          if (is_char) {
            if (cg->load_char) cg->load_char(cg); /* A,L = old byte */
          } else {
            if (cg->load_int) cg->load_int(cg); /* HL = old value */
          }
          if (cg->save_value) cg->save_value(cg); /* save original (result of expression) */
          /* Increment */
          if (cg->add_const_to_value) {
            cg->add_const_to_value(cg, inc_amount);
          } else {
            /* Target must supply add_const_to_value; generic codegen stays target-agnostic. */
            fprintf(cg->output, "\t; TODO add_const_to_value not implemented for target\n");
          }
          /* For char, sync A from L for store */
          if (is_char) {
            if (cg->truncate_to_char) cg->truncate_to_char(cg);
            if (cg->store_char) cg->store_char(cg);
          } else {
            if (cg->store_int) cg->store_int(cg);
          }
          /* Restore original value */
          if (cg->restore_value) cg->restore_value(cg);
        } else {
          gen_expr(cg, operand, locals);
          if (cg && cg->output) fprintf(cg->output, "\t; TODO postfix inc (non-ident)\n");
        }
      } else if (expr->u.expr.op == '*') {
        /* rvalue dereference: compute address into IY, then load */
        struct ASTNode *addr = expr->u.expr.e1;
        int is_char_deref = (expr->type && expr->type->kind == TYPE_CHAR);
        /* Compute address value into HL */
        gen_expr(cg, addr, locals);
        /* Move HL -> IY */
        if (cg->addr_from_value) cg->addr_from_value(cg);
        /* Load */
        if (is_char_deref) { if (cg->load_char) cg->load_char(cg); }
        else { if (cg->load_int) cg->load_int(cg); }
      } else {
        gen_expr(cg, expr->u.expr.e1, locals);
        fprintf(cg->output, "\t; TODO unary op %d\n", expr->u.expr.op);
      }
      break;
    case EXPR_BINARY:
  if (expr->u.expr.op == '=') {
        /* Assignment: left ident only for now. Compute LHS address first (may clobber HL), then evaluate RHS into HL, then store. */
        if (expr->u.expr.e1) {
          int is_char = 0;
          if (expr->u.expr.e1->kind_tag == 0 && expr->u.expr.e1->u.expr.kind == EXPR_IDENT) {
            const char *name = expr->u.expr.e1->u.expr.ident;
            struct CGLocal *loc = find_local(locals, name);
            if (loc) {
              is_char = loc->is_char;
              if (cg->addr_local) cg->addr_local(cg, loc->offset);
            } else {
              if (cg->addr_symbol) cg->addr_symbol(cg, name);
              /* Use semantic type of LHS identifier for globals */
              if (expr->u.expr.e1->type && expr->u.expr.e1->type->kind == TYPE_CHAR) is_char = 1;
            }
            if (cg->addr_from_value) cg->addr_from_value(cg);
          } else if (expr->u.expr.e1->kind_tag == 0 && expr->u.expr.e1->u.expr.kind == EXPR_UNARY && expr->u.expr.e1->u.expr.op == '*') {
            /* Complex lvalue: *(...): compute address into IY */
            struct ASTNode *addr = expr->u.expr.e1->u.expr.e1;
            /* Decide store width from the lvalue's type */
            if (expr->u.expr.e1->type && expr->u.expr.e1->type->kind == TYPE_CHAR) is_char = 1;
            gen_expr(cg, addr, locals); /* HL = computed address value */
            if (cg->addr_from_value) cg->addr_from_value(cg); /* IY = HL */
          } else {
            fprintf(cg->output, "\t; TODO complex lvalue\n");
          }
          /* RHS */
          if (cg->save_addr) cg->save_addr(cg);
          if (is_char) {
            struct ASTNode *rhs = expr->u.expr.e2;
            gen_expr(cg, rhs, locals);
            if (cg->truncate_to_char) cg->truncate_to_char(cg);
            if (cg->restore_addr) cg->restore_addr(cg);
            if (cg->store_char) cg->store_char(cg);
          } else {
            gen_expr(cg, expr->u.expr.e2, locals);
            if (cg->restore_addr) cg->restore_addr(cg);
            if (cg->store_int) cg->store_int(cg);
          }
        }
      } else {
        gen_binary(cg, expr, locals);
      }
      break;
    default:
      fprintf(cg->output, "\t; TODO expr kind %d\n", expr->u.expr.kind);
      break;
  }
}

static void gen_statement(struct Codegen *cg, struct ASTNode *stmt, struct CGLocal *locals, const char *end_label) {
  struct ASTList *list;
  if (!cg || !stmt) return;
  if (stmt->kind_tag != 1) return;
  switch (stmt->u.stmt.kind) {
    case STMT_COMPOUND:
      for (list = stmt->u.stmt.stmts; list; list = list->next) {
        if (!list->node) continue;
        if (list->node->kind_tag == 2) {
          /* Local declaration with optional initializer */
          struct ASTNode *dn = list->node;
          if (dn->u.decl.decltor && dn->u.decl.decltor->name && dn->u.decl.init) {
            struct CGLocal *loc = find_local(locals, dn->u.decl.decltor->name);
            int is_array = dn->u.decl.decltor->is_array;
            int is_char_base = (dn->u.decl.spec_flags & SPF_CHAR) != 0 && dn->u.decl.decltor->pointer_level == 0;
            if (is_array && is_char_base && dn->u.decl.init->kind_tag == 0 && dn->u.decl.init->u.expr.kind == EXPR_STRING) {
              /* Initialize char array from string literal using target bulk copy if available. */
              int sid; char base_label[32]; int nbytes;
              if (!loc) { continue; }
              sid = cg_add_string(cg, dn->u.decl.init->u.expr.str ? dn->u.decl.init->u.expr.str : "");
              sprintf(base_label, ".LC%d", sid);
              nbytes = cg_string_length_bytes(dn->u.decl.init->u.expr.str) + 1; /* include NUL */
              if (cg->init_local_from_symbol) {
                cg->init_local_from_symbol(cg, loc->offset, base_label, nbytes);
              } else {
                /* Fallback to byte-wise copy if target lacks bulk helper */
                int i;
                for (i = 0; i < nbytes; i++) {
                  char src_label[48];
                  sprintf(src_label, "%s+%d", base_label, i);
                  if (cg->emit_const_int) cg->emit_const_int(cg, src_label);
                  if (cg->addr_from_value) cg->addr_from_value(cg);
                  if (cg->load_char) cg->load_char(cg);
                  if (cg->addr_local) cg->addr_local(cg, loc->offset + i);
                  if (cg->addr_from_value) cg->addr_from_value(cg);
                  if (cg->store_char) cg->store_char(cg);
                }
              }
            } else {
              int is_char_init = 0;
              if (dn->u.decl.decltor->pointer_level > 0) {
                is_char_init = 0; /* pointers are 16-bit */
              } else {
                is_char_init = (dn->u.decl.spec_flags & SPF_CHAR) != 0;
              }
              /* Compute LHS address first (may clobber HL) */
              if (loc) {
                if (cg->addr_local) cg->addr_local(cg, loc->offset);
              } else {
                if (cg->addr_symbol) cg->addr_symbol(cg, dn->u.decl.decltor->name);
              }
              if (cg->addr_from_value) cg->addr_from_value(cg);
              /* Evaluate RHS */
              if (cg->save_addr) cg->save_addr(cg);
              gen_expr(cg, dn->u.decl.init, locals);
              /* Store (apply conversion as if by assignment) */
              if (is_char_init) { if (cg->truncate_to_char) cg->truncate_to_char(cg); if (cg->restore_addr) cg->restore_addr(cg); if (cg->store_char) cg->store_char(cg); }
              else { if (cg->restore_addr) cg->restore_addr(cg); if (cg->store_int) cg->store_int(cg); }
            }
          }
          continue;
        }
        if (list->node->kind_tag == 1) gen_statement(cg, list->node, locals, end_label);
      }
      break;
    case STMT_EXPR:
      gen_expr(cg, stmt->u.stmt.expr, locals);
      break;
    case STMT_WHILE: {
      static int wl_counter = 0; char lbl_start[32]; char lbl_end[32];
      sprintf(lbl_start, ".LwS%d", wl_counter); sprintf(lbl_end, ".LwE%d", wl_counter); wl_counter++;
      fprintf(cg->output, "%s:\n", lbl_start);
      /* Evaluate condition */
      gen_expr(cg, stmt->u.stmt.s1, locals);
      /* Jump if zero -> end */
      if (cg->jump_if_zero) cg->jump_if_zero(cg, lbl_end);
      /* Body */
      if (stmt->u.stmt.s2) gen_statement(cg, stmt->u.stmt.s2, locals, end_label);
      /* Loop back */
      if (cg->jump_label) cg->jump_label(cg, lbl_start);
      fprintf(cg->output, "%s:\n", lbl_end);
      break;
    }
    case STMT_RETURN:
      gen_expr(cg, stmt->u.stmt.expr, locals);
  /* Early return: use target jump helper */
  if (cg->jump_label) cg->jump_label(cg, end_label);
      break;
    default:
      fprintf(cg->output, "\t; TODO stmt kind %d\n", stmt->u.stmt.kind);
      break;
  }
}

static void emit_function(struct Codegen *cg, struct ASTNode *extfn) {
  struct ASTNode *decl; const char *name; int is_static; int stack_size; struct CGLocal *locals; static int lbl_counter = 0; char end_label[32];
  if (!cg || !extfn || !cg->output) return;
  decl = extfn->u.ext.decl;
  if (!decl || decl->kind_tag != 2 || !decl->u.decl.decltor) return;
  name = decl->u.decl.decltor->name;
  if (!name) return;
  is_static = (decl->u.decl.spec_flags & SPF_STATIC) != 0;
  if (!is_static) fprintf(cg->output, "\t.globl %s\n", name);
  fprintf(cg->output, "%s:\n", name);
  stack_size = extfn->u.ext.stack_size;
  if (cg->fn_prologue) cg->fn_prologue(cg, stack_size);
  /* Build locals */
  locals = build_locals(extfn->u.ext.body);
  sprintf(end_label, ".Lend%d", lbl_counter++);
  gen_statement(cg, extfn->u.ext.body, locals, end_label);
  /* Normal fall-through epilogue */
  fprintf(cg->output, "%s:\n", end_label);
  if (cg->fn_epilogue) cg->fn_epilogue(cg);
  cg_locals_free(locals);
}

void codegen_generate(struct Codegen *cg, struct ASTNode *tree) {
  struct ASTList *list; struct ASTNode *node;
  if (!cg || !tree) return;
  if (tree->kind_tag != 1 || tree->u.stmt.kind != STMT_COMPOUND) return;
  /* Reset string collection per translation unit */
  g_string_list = NULL; g_string_count = 0;
  if (cg->output) {
    /* Generic section prologue */
    fprintf(cg->output, "\t.text\n\n");
  }
  for (list = tree->u.stmt.stmts; list; list = list->next) {
    node = list->node; if (!node || node->kind_tag != 3) continue;
    if (node->u.ext.is_function && node->u.ext.body) {
      emit_function(cg, node);
    } else {
      /* Non-function external: for now just emit .extern if declared extern */
      struct ASTNode *decl = node->u.ext.decl; const char *name; int is_extern;
      if (decl && decl->kind_tag == 2 && decl->u.decl.decltor) {
        name = decl->u.decl.decltor->name;
        is_extern = (decl->u.decl.spec_flags & SPF_EXTERN) != 0;
        if (is_extern && cg->output) fprintf(cg->output, "\t.extern %s\n", name);
      }
    }
  }
  /* Emit collected global data (e.g., string literals) */
  cg_emit_string_data(cg);
  cg_free_strings(cg);
}
