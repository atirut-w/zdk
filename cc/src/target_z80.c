#include "target.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Z80-specific data */
struct Z80Data {
  int stack_offset;
  int temp_count;
  int label_count;
  int hl_uses;
  int de_uses;
};

#define REG_HL 0
#define REG_DE 1

static void z80_mov_hl_to_de(struct Codegen *cg) {
  fprintf(cg->output, "\tld d, h\n");
  fprintf(cg->output, "\tld e, l\n");
}

static void z80_zero_hl(struct Codegen *cg) {
  fprintf(cg->output, "\txor a\n");
  fprintf(cg->output, "\tsbc hl, hl\n");
}

static void z80_enter_dest(struct Codegen *cg, int dest, int *pushed_other) {
  struct Z80Data *d = (struct Z80Data *)cg->target_data;
  if (pushed_other) *pushed_other = 0;
  if (!d || !cg->output) return;
  if (dest == REG_HL) {
    if (d->de_uses > 0) {
      fprintf(cg->output, "\tpush de\n");
      if (pushed_other) *pushed_other = 1;
    }
    d->hl_uses++;
  } else {
    if (d->hl_uses > 0) {
      fprintf(cg->output, "\tpush hl\n");
      if (pushed_other) *pushed_other = 1;
    }
    d->de_uses++;
  }
}

static void z80_leave_dest(struct Codegen *cg, int dest, int pushed_other) {
  struct Z80Data *d = (struct Z80Data *)cg->target_data;
  if (!d || !cg->output) return;
  if (dest == REG_HL) {
    d->hl_uses--;
    if (pushed_other) fprintf(cg->output, "\tpop de\n");
  } else {
    d->de_uses--;
    if (pushed_other) fprintf(cg->output, "\tpop hl\n");
  }
}

static void z80_gen_expression_into(struct Codegen *cg, struct ASTNode *expr, int dest);

/* Z80 codegen implementations */

static void z80_gen_function(struct Codegen *cg, struct ASTNode *func) {
  struct ASTNode *body;
  if (!func || !cg->output) {
    return;
  }

  /* Function prologue */
  fprintf(cg->output, "\t; Function prologue\n");
  fprintf(cg->output, "\tpush ix\n");
  fprintf(cg->output, "\tld ix, 0\n");
  fprintf(cg->output, "\tadd ix, sp\n");

  /* Generate function body */
  body = func->u.ext.body;
  if (body && cg->gen_statement) {
    cg->gen_statement(cg, body);
  }

  /* Function epilogue */
  fprintf(cg->output, "\t; Function epilogue\n");
  fprintf(cg->output, "\tld sp, ix\n");
  fprintf(cg->output, "\tpop ix\n");
  fprintf(cg->output, "\tret\n");
}

static void z80_gen_statement(struct Codegen *cg, struct ASTNode *stmt) {
  struct ASTList *list;
  if (!stmt || !cg->output) {
    return;
  }
  switch (stmt->u.stmt.kind) {
    case STMT_COMPOUND:
      for (list = stmt->u.stmt.stmts; list; list = list->next) {
        if (list->node && cg->gen_statement) {
          cg->gen_statement(cg, list->node);
        }
      }
      break;
    case STMT_RETURN:
      if (stmt->u.stmt.expr && cg->gen_expression) {
        cg->gen_expression(cg, stmt->u.stmt.expr);
      }
      break;
    case STMT_EXPR:
      if (stmt->u.stmt.expr && cg->gen_expression) {
        cg->gen_expression(cg, stmt->u.stmt.expr);
      }
      break;
    default:
      fprintf(cg->output, "\t; TODO: statement kind %d\n", stmt->u.stmt.kind);
      break;
  }
}

static void z80_gen_expression_into(struct Codegen *cg, struct ASTNode *expr, int dest) {
  struct Z80Data *d;
  if (!expr || !cg->output) {
    return;
  }
  d = (struct Z80Data *)cg->target_data;

  if (expr->u.expr.kind == EXPR_CONST) {
    /* Constants can load directly without spilling */
    if (dest == REG_HL) {
      fprintf(cg->output, "\tld hl, %s\n", expr->u.expr.const_tok.lexeme);
    } else {
      fprintf(cg->output, "\tld de, %s\n", expr->u.expr.const_tok.lexeme);
    }
    return;
  }

  if (expr->u.expr.kind == EXPR_UNARY && expr->u.expr.op == '-') {
    /* Evaluate operand to HL, then negate */
    z80_gen_expression_into(cg, expr->u.expr.e1, REG_HL);
    z80_mov_hl_to_de(cg);
    z80_zero_hl(cg);
    fprintf(cg->output, "\tsbc hl, de\n");
    if (dest == REG_DE) {
      z80_mov_hl_to_de(cg);
    }
    return;
  }

  if (expr->u.expr.kind == EXPR_BINARY) {
    int op = expr->u.expr.op;
    int pushed_hl = 0;
    
    /* LHS -> HL */
    z80_gen_expression_into(cg, expr->u.expr.e1, REG_HL);

    /* Only spill HL if RHS is complex (not a constant) */
    if (expr->u.expr.e2 && expr->u.expr.e2->u.expr.kind != EXPR_CONST) {
      fprintf(cg->output, "\tpush hl\n");
      pushed_hl = 1;
    }
    
    /* RHS -> DE */
    z80_gen_expression_into(cg, expr->u.expr.e2, REG_DE);
    
    if (pushed_hl) {
      fprintf(cg->output, "\tpop hl\n");
    }

    if (op == '+') {
      fprintf(cg->output, "\tadd hl, de\n");
    } else if (op == '-') {
      fprintf(cg->output, "\txor a\n");
      fprintf(cg->output, "\tsbc hl, de\n");
    } else if (op == '*') {
      fprintf(cg->output, "\tpush de\n");
      fprintf(cg->output, "\tpush hl\n");
      fprintf(cg->output, "\tcall __mulhi3\n");
      fprintf(cg->output, "\tpop bc\n");
      fprintf(cg->output, "\tpop bc\n");
    } else if (op == '/') {
      fprintf(cg->output, "\tpush de\n");
      fprintf(cg->output, "\tpush hl\n");
      fprintf(cg->output, "\tcall __divhi3\n");
      fprintf(cg->output, "\tpop bc\n");
      fprintf(cg->output, "\tpop bc\n");
    } else if (op == '%') {
      fprintf(cg->output, "\tpush de\n");
      fprintf(cg->output, "\tpush hl\n");
      fprintf(cg->output, "\tcall __modhi3\n");
      fprintf(cg->output, "\tpop bc\n");
      fprintf(cg->output, "\tpop bc\n");
    } else if (op == T_LEFT_OP) {
      fprintf(cg->output, "\tpush de\n");
      fprintf(cg->output, "\tpush hl\n");
      fprintf(cg->output, "\tcall __ashlhi3\n");
      fprintf(cg->output, "\tpop bc\n");
      fprintf(cg->output, "\tpop bc\n");
    } else if (op == T_RIGHT_OP) {
      fprintf(cg->output, "\tpush de\n");
      fprintf(cg->output, "\tpush hl\n");
      fprintf(cg->output, "\tcall __ashrhi3\n");
      fprintf(cg->output, "\tpop bc\n");
      fprintf(cg->output, "\tpop bc\n");
    } else {
      fprintf(cg->output, "\t; TODO: binary op %d\n", op);
    }

    if (dest == REG_DE) {
      z80_mov_hl_to_de(cg);
    }
    return;
  }

  /* Fallback */
  fprintf(cg->output, "\t; TODO: expression kind %d\n", expr->u.expr.kind);
  if (dest == REG_DE) {
    z80_mov_hl_to_de(cg);
  }
}

static void z80_gen_expression(struct Codegen *cg, struct ASTNode *expr) {
  z80_gen_expression_into(cg, expr, REG_HL);
}

/* Z80 target implementations */

static void z80_init(struct Codegen *cg) {
  struct Z80Data *data;
  data = (struct Z80Data *)malloc(sizeof(struct Z80Data));
  if (!data) {
    fprintf(stderr, "Failed to allocate Z80 target data\n");
    return;
  }
  data->stack_offset = 0;
  data->temp_count = 0;
  data->label_count = 0;
  data->hl_uses = 0;
  data->de_uses = 0;
  cg->target_data = data;

  if (cg->output) {
    fprintf(cg->output, "; Z80 assembly output\n");
    fprintf(cg->output, "; Generated by zdk compiler\n");
    fprintf(cg->output, "; Target: Z80 (GNU binutils)\n\n");
    fprintf(cg->output, "\t.text\n\n");
  }
}

static void z80_finalize(struct Codegen *cg) {
  if (cg->target_data) {
    free(cg->target_data);
    cg->target_data = NULL;
  }
}

static int z80_invoke_assembler(struct Codegen *cg, const char *asm_file, const char *obj_file) {
  char cmd[512];
  int ret;
  (void)cg;
  sprintf(cmd, "z80-unknown-none-elf-as -o '%s' '%s'", obj_file, asm_file);
  ret = system(cmd);
  return ret == 0;
}

static int z80_invoke_linker(struct Codegen *cg, const char **obj_files, int num_objs, const char *out_file) {
  char cmd[512];
  int ret;
  (void)cg;
  if (num_objs <= 0) {
    return 0;
  }
  sprintf(cmd, "z80-unknown-none-elf-ld -o '%s' '%s'", out_file, obj_files[0]);
  ret = system(cmd);
  return ret == 0;
}

static void z80_init_codegen(struct Codegen *cg) {
  cg->gen_function = z80_gen_function;
  cg->gen_statement = z80_gen_statement;
  cg->gen_expression = z80_gen_expression;
}

/* Public target descriptor */
struct Target target_z80 = {
  "z80",
  "Zilog Z80 (GNU binutils)",
  "z80-unknown-none-elf-",
  z80_init,
  z80_finalize,
  z80_invoke_assembler,
  z80_invoke_linker,
  z80_init_codegen
};
