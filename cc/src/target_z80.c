#include "target.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int cc_verbose; /* from main.c */

/* Z80-specific data */
struct Z80Data {
  int stack_offset;
  int temp_count;
  int label_count;
  int hl_uses;
  int de_uses;
  int string_count; /* for generating .LC labels */
  struct ASTList *strings; /* collected string literals for .data section */
};

/* String literal wrapper for ASTList */
struct StringEntry {
  int id;
  const char *content;
};

static struct ASTNode *wrap_string_entry(int id, const char *content) {
  struct StringEntry *se = (struct StringEntry *)malloc(sizeof(struct StringEntry));
  struct ASTNode *node = (struct ASTNode *)malloc(sizeof(struct ASTNode));
  if (!se || !node) return NULL;
  se->id = id;
  se->content = content;
  node->kind_tag = 99; /* special marker for string entry */
  node->u.expr.str = (char *)se;
  return node;
}

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
  int stack_size;
  
  if (!func || !cg->output) {
    return;
  }

  /* Get stack size for local variables */
  stack_size = func->u.ext.stack_size;

  /* Function prologue */
  fprintf(cg->output, "\t; Function prologue\n");
  fprintf(cg->output, "\tpush ix\n");
  fprintf(cg->output, "\tld ix, 0\n");
  fprintf(cg->output, "\tadd ix, sp\n");
  
  /* Allocate stack space for local variables */
  if (stack_size > 0) {
    fprintf(cg->output, "\t; Allocate %d bytes for local variables\n", stack_size);
    fprintf(cg->output, "\tld hl, -%d\n", stack_size);
    fprintf(cg->output, "\tadd hl, sp\n");
    fprintf(cg->output, "\tld sp, hl\n");
  }

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
        if (!list->node) {
          continue;
        }
        /* Declarations appear directly in compound lists now; skip them */
        if (list->node->kind_tag == 2) {
          /* TODO: allocate locals, emit storage as needed */
          continue;
        }
        /* Only recurse on statements */
        if (list->node->kind_tag == 1 && cg->gen_statement) {
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
      fprintf(cg->output, "\tld hl, %s\n", expr->u.expr.const_lexeme);
    } else {
      fprintf(cg->output, "\tld de, %s\n", expr->u.expr.const_lexeme);
    }
    return;
  }

  if (expr->u.expr.kind == EXPR_STRING) {
    /* String literal: add to list and load address */
    int str_id = d->string_count++;
    struct ASTNode *str_node = wrap_string_entry(str_id, expr->u.expr.str);
    if (str_node) {
      d->strings = ast_list_append(d->strings, str_node);
    }
    if (dest == REG_HL) {
      fprintf(cg->output, "\tld hl, .LC%d\n", str_id);
    } else {
      fprintf(cg->output, "\tld de, .LC%d\n", str_id);
    }
    return;
  }

  if (expr->u.expr.kind == EXPR_CALL) {
    /* Function call: push args right-to-left, call, clean stack */
    struct ASTList *arg;
    int arg_count = 0;
    int i;
    
    /* Count args */
    for (arg = expr->u.expr.args; arg; arg = arg->next) {
      arg_count++;
    }
    
    /* Push arguments in reverse order (right-to-left for cdecl) */
    if (arg_count > 0) {
      struct ASTNode **arg_array = (struct ASTNode **)malloc(sizeof(struct ASTNode *) * arg_count);
      i = 0;
      for (arg = expr->u.expr.args; arg; arg = arg->next) {
        arg_array[i++] = arg->node;
      }
      for (i = arg_count - 1; i >= 0; i--) {
        z80_gen_expression_into(cg, arg_array[i], REG_HL);
        fprintf(cg->output, "\tpush hl\n");
      }
      free(arg_array);
    }
    
    /* Call function */
    if (expr->u.expr.e1 && expr->u.expr.e1->u.expr.kind == EXPR_IDENT) {
      fprintf(cg->output, "\tcall %s\n", expr->u.expr.e1->u.expr.ident);
    } else {
      /* indirect call not yet supported */
      fprintf(cg->output, "\t; TODO: indirect call\n");
    }
    
    /* Clean stack - pop args into BC repeatedly or adjust SP */
    if (arg_count > 0) {
      if (arg_count == 1) {
        fprintf(cg->output, "\tpop bc\n");
      } else if (arg_count == 2) {
        fprintf(cg->output, "\tpop bc\n");
        fprintf(cg->output, "\tpop bc\n");
      } else {
        /* For many args, adjust SP directly */
        fprintf(cg->output, "\tld bc, %d\n", arg_count * 2);
        fprintf(cg->output, "\tadd ix, bc\n");
        fprintf(cg->output, "\tld sp, ix\n");
      }
    }
    
    /* Result already in HL; move to DE if needed */
    if (dest == REG_DE) {
      z80_mov_hl_to_de(cg);
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
    } else if (op == OP_SHL) {
      fprintf(cg->output, "\tpush de\n");
      fprintf(cg->output, "\tpush hl\n");
      fprintf(cg->output, "\tcall __ashlhi3\n");
      fprintf(cg->output, "\tpop bc\n");
      fprintf(cg->output, "\tpop bc\n");
    } else if (op == OP_SHR) {
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
  data->string_count = 0;
  data->strings = NULL;
  cg->target_data = data;

  if (cg->output) {
    fprintf(cg->output, "; Z80 assembly output\n");
    fprintf(cg->output, "; Generated by zdk compiler\n");
    fprintf(cg->output, "; Target: Z80 (GNU binutils)\n\n");
    fprintf(cg->output, "\t.text\n\n");
  }
}

static void z80_finalize(struct Codegen *cg) {
  struct Z80Data *d = (struct Z80Data *)cg->target_data;
  struct ASTList *str;
  struct StringEntry *se;
  const char *p;
  
  if (!d) return;
  
  /* Emit .data section with string literals */
  if (d->strings && cg->output) {
    fprintf(cg->output, "\n\t.data\n");
    for (str = d->strings; str; str = str->next) {
      if (str->node && str->node->kind_tag == 99) {
        se = (struct StringEntry *)str->node->u.expr.str;
        fprintf(cg->output, ".LC%d:\n", se->id);
        fprintf(cg->output, "\t.ascii \"");
        /* Emit string with escapes */
        for (p = se->content; *p; p++) {
          if (*p == '\n') fprintf(cg->output, "\\n");
          else if (*p == '\t') fprintf(cg->output, "\\t");
          else if (*p == '\r') fprintf(cg->output, "\\r");
          else if (*p == '\\') fprintf(cg->output, "\\\\");
          else if (*p == '"') fprintf(cg->output, "\\\"");
          else if (*p >= 32 && *p < 127) fprintf(cg->output, "%c", *p);
          else fprintf(cg->output, "\\%03o", (unsigned char)*p);
        }
        fprintf(cg->output, "\"\n");
      }
    }
  }
  
  /* Free string list */
  while (d->strings) {
    str = d->strings;
    d->strings = str->next;
    if (str->node && str->node->kind_tag == 99) {
      free((struct StringEntry *)str->node->u.expr.str);
      free(str->node);
    }
    free(str);
  }
  
  free(cg->target_data);
  cg->target_data = NULL;
}

static int z80_invoke_assembler(struct Codegen *cg, const char *asm_file, const char *obj_file) {
  char cmd[512];
  int ret;
  (void)cg;
  sprintf(cmd, "z80-unknown-none-elf-as -o '%s' '%s'", obj_file, asm_file);
  if (cc_verbose) {
    fprintf(stderr, "[cmd] %s\n", cmd);
  }
  ret = system(cmd);
  return ret == 0;
}

static int z80_invoke_linker(struct Codegen *cg, const char **obj_files, int num_objs, const char *out_file, const char *linker_script) {
  char cmd[1024];
  int ret;
  int i;
  int n;
  (void)cg;
  if (num_objs <= 0) {
    return 0;
  }
  /* Start command */
  if (linker_script) {
    n = sprintf(cmd, "z80-unknown-none-elf-ld -o '%s' -T '%s'", out_file, linker_script);
  } else {
    n = sprintf(cmd, "z80-unknown-none-elf-ld -o '%s'", out_file);
  }
  for (i = 0; i < num_objs; i++) {
    if (n > (int)(sizeof(cmd) - 64)) {
      /* Avoid overflow; simplistic check */
      break;
    }
    n += sprintf(cmd + n, " '%s'", obj_files[i]);
  }
  if (cc_verbose) {
    fprintf(stderr, "[cmd] %s\n", cmd);
  }
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
