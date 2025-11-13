#include "target.h"
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
  struct CGLocal *next;
};

static void cg_locals_free(struct CGLocal *l) {
  struct CGLocal *n; while (l) { n = l->next; if (l->name) free(l->name); free(l); l = n; }
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
      int sz = type_size_from_spec(n->u.decl.spec_flags);
      loc->size = sz;
      loc->offset = -(accumulated + sz);
      loc->is_char = is_char_from_spec(n->u.decl.spec_flags);
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

/* Expression generation producing result in value register (HL for Z80 backend). */
static void gen_expr(struct Codegen *cg, struct ASTNode *expr, struct CGLocal *locals);

static void gen_load_ident(struct Codegen *cg, const char *name, struct CGLocal *locals) {
  struct CGLocal *loc;
  if (!cg) return;
  loc = find_local(locals, name);
  if (loc) {
    if (cg->addr_local) cg->addr_local(cg, loc->offset);
    if (loc->is_char) {
      if (cg->load_char) cg->load_char(cg);
    } else {
      if (cg->load_int) cg->load_int(cg);
    }
  } else {
    /* Treat as global symbol */
    if (cg->addr_symbol) cg->addr_symbol(cg, name);
    if (cg->load_int) cg->load_int(cg);
  }
}

static void gen_store_ident(struct Codegen *cg, const char *name, struct CGLocal *locals, int is_char) {
  struct CGLocal *loc;
  if (!cg) return;
  loc = find_local(locals, name);
  if (loc) {
    if (cg->addr_local) cg->addr_local(cg, loc->offset);
    if (loc->is_char || is_char) {
      if (cg->store_char) cg->store_char(cg);
    } else {
      if (cg->store_int) cg->store_int(cg);
    }
  } else {
    /* Global variable store (assume int) */
    if (cg->addr_symbol) cg->addr_symbol(cg, name);
    if (cg->store_int) cg->store_int(cg);
  }
}

static void gen_binary(struct Codegen *cg, struct ASTNode *expr, struct CGLocal *locals) {
  int op;
  if (!cg || !expr) return;
  op = expr->u.expr.op;
  /* Left-associative: evaluate left first into HL. Preserve HL while computing RHS. */
  gen_expr(cg, expr->u.expr.e1, locals); /* HL = left */
  /* Save left on stack (always for simplicity). */
  fprintf(cg->output, "\tpush hl\n");
  /* Evaluate right into HL, move to DE, restore left back to HL. */
  gen_expr(cg, expr->u.expr.e2, locals); /* HL = right */
  if (cg->value_to_rhs) cg->value_to_rhs(cg); /* DE = right */
  fprintf(cg->output, "\tpop hl\n"); /* HL = left */
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

static void gen_expr(struct Codegen *cg, struct ASTNode *expr, struct CGLocal *locals) {
  if (!cg || !expr) return;
  if (expr->kind_tag != 0) return; /* not expression */
  switch (expr->u.expr.kind) {
    case EXPR_CONST:
      if (cg->emit_const_int) cg->emit_const_int(cg, expr->u.expr.const_lexeme ? expr->u.expr.const_lexeme : "0");
      break;
    case EXPR_STRING:
      if (cg->emit_string_literal) cg->emit_string_literal(cg, expr->u.expr.str ? expr->u.expr.str : "");
      break;
    case EXPR_IDENT:
      gen_load_ident(cg, expr->u.expr.ident, locals);
      break;
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
        if (cg->op_neg) cg->op_neg(cg);
      } else {
        gen_expr(cg, expr->u.expr.e1, locals);
        fprintf(cg->output, "\t; TODO unary op %d\n", expr->u.expr.op);
      }
      break;
    case EXPR_BINARY:
      if (expr->u.expr.op == '=') {
        /* Assignment: left ident only for now. Compute address first to avoid clobbering HL value. */
        if (expr->u.expr.e1 && expr->u.expr.e1->u.expr.kind == EXPR_IDENT) {
          const char *name = expr->u.expr.e1->u.expr.ident;
          struct CGLocal *loc = find_local(locals, name);
          int is_char = 0;
          if (loc) {
            if (cg->addr_local) cg->addr_local(cg, loc->offset);
            is_char = loc->is_char;
          } else {
            if (cg->addr_symbol) cg->addr_symbol(cg, name);
          }
          /* Now compute RHS value into HL */
          gen_expr(cg, expr->u.expr.e2, locals);
          /* And store */
          if (is_char) {
            if (cg->store_char) cg->store_char(cg);
          } else {
            if (cg->store_int) cg->store_int(cg);
          }
        } else {
          fprintf(cg->output, "\t; TODO complex lvalue\n");
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
        if (list->node->kind_tag == 2) continue; /* declaration handled already */
        if (list->node->kind_tag == 1) gen_statement(cg, list->node, locals, end_label);
      }
      break;
    case STMT_EXPR:
      gen_expr(cg, stmt->u.stmt.expr, locals);
      break;
    case STMT_RETURN:
      gen_expr(cg, stmt->u.stmt.expr, locals);
      /* Early return: jump to epilogue */
      fprintf(cg->output, "\tjp %s\n", end_label);
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
}
