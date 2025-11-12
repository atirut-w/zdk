#ifndef TARGET_H
#define TARGET_H

#include "ast.h"
#include <stdio.h>

/* Forward declarations */
struct Codegen;
struct Target;

/* Codegen - holds code generation context and function pointers */
struct Codegen {
  /* Context */
  void *target_data; /* target-specific data */
  FILE *output;      /* output file for generated code */
  
  /* Code generation hooks */
  void (*gen_symbol)(struct Codegen *cg, const char *name, int is_static);
  void (*gen_function)(struct Codegen *cg, struct ASTNode *func);
  void (*gen_statement)(struct Codegen *cg, struct ASTNode *stmt);
  void (*gen_expression)(struct Codegen *cg, struct ASTNode *expr);
};

/* Target interface - holds target-specific information */
struct Target {
  const char *name;
  const char *description;
  const char *toolchain_prefix; /* e.g., "z80-unknown-none-elf-" */
  
  /* Initialization and cleanup */
  void (*init)(struct Codegen *cg);
  void (*finalize)(struct Codegen *cg);
  
  /* Toolchain invocation */
  int (*invoke_assembler)(struct Codegen *cg, const char *asm_file, const char *obj_file);
  int (*invoke_linker)(struct Codegen *cg, const char **obj_files, int num_objs, const char *out_file);
  
  /* Code generation function table initialization */
  void (*init_codegen)(struct Codegen *cg);
};

/* Target registry and selection */
struct Target *target_find(const char *name);
void target_list(void);

/* Codegen initialization */
void codegen_init_defaults(struct Codegen *cg);
void codegen_generate(struct Codegen *cg, struct ASTNode *tree);

/* Default codegen implementations */
void codegen_default_gen_symbol(struct Codegen *cg, const char *name, int is_static);
void codegen_default_gen_function(struct Codegen *cg, struct ASTNode *func);
void codegen_default_gen_statement(struct Codegen *cg, struct ASTNode *stmt);
void codegen_default_gen_expression(struct Codegen *cg, struct ASTNode *expr);

/* Built-in targets */
extern struct Target target_z80;

#endif /* TARGET_H */
