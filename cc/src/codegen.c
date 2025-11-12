#include "target.h"
#include <stdio.h>

/* Default codegen implementations using GNU binutils syntax */

void codegen_default_gen_symbol(struct Codegen *cg, const char *name, int is_static) {
  if (!cg->output) {
    return;
  }
  
  /* If public, emit .globl directive */
  if (!is_static) {
    fprintf(cg->output, "\t.globl %s\n", name);
  }
  
  /* Emit the label */
  fprintf(cg->output, "%s:\n", name);
}

void codegen_default_gen_function(struct Codegen *cg, struct ASTNode *func) {
  /* Default: placeholder */
  if (cg->output) {
    fprintf(cg->output, "\t# TODO: gen_function\n");
  }
}

void codegen_default_gen_statement(struct Codegen *cg, struct ASTNode *stmt) {
  /* Default: placeholder */
  if (cg->output) {
    fprintf(cg->output, "\t# TODO: gen_statement\n");
  }
}

void codegen_default_gen_expression(struct Codegen *cg, struct ASTNode *expr) {
  /* Default: placeholder */
  if (cg->output) {
    fprintf(cg->output, "\t# TODO: gen_expression\n");
  }
}

void codegen_init_defaults(struct Codegen *cg) {
  cg->gen_symbol = codegen_default_gen_symbol;
  cg->gen_function = codegen_default_gen_function;
  cg->gen_statement = codegen_default_gen_statement;
  cg->gen_expression = codegen_default_gen_expression;
}

void codegen_generate(struct Codegen *cg, struct ASTNode *tree) {
  struct ASTNode *node;
  struct ASTList *list;
  const char *func_name;
  int is_static;
  
  if (!tree) {
    return;
  }
  
  /* Translation unit is represented as a compound statement with external declarations */
  if (tree->kind_tag == 1 && tree->u.stmt.kind == STMT_COMPOUND) {
    /* Walk through all external declarations */
    for (list = tree->u.stmt.stmts; list; list = list->next) {
      node = list->node;
      if (!node || node->kind_tag != 3) {
        continue;
      }
      
      /* Handle function definition */
      if (node->u.ext.is_function) {
        struct ASTNode *decl = node->u.ext.decl;
        if (decl && decl->kind_tag == 2) {
          func_name = decl->u.decl.decltor ? decl->u.decl.decltor->name : NULL;
          if (func_name) {
            /* Check if it's static */
            is_static = (decl->u.decl.spec_flags & SPF_STATIC) != 0;
            
            /* Generate symbol (handles visibility and label) */
            if (cg->gen_symbol) {
              cg->gen_symbol(cg, func_name, is_static);
            }
            
            /* Generate the function body */
            if (cg->gen_function) {
              cg->gen_function(cg, node);
            }
          }
        }
      }
    }
  }
}
