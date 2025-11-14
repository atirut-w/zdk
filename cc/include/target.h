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
  /* Low-level actions (flattened) */
  void (*fn_prologue)(struct Codegen *cg, int stack_size);
  void (*fn_epilogue)(struct Codegen *cg);
  void (*alloc_stack)(struct Codegen *cg, int bytes);
  void (*addr_symbol)(struct Codegen *cg, const char *name);
  void (*addr_local)(struct Codegen *cg, int offset);
  void (*addr_param)(struct Codegen *cg, int offset);
  /* Move computed address in value register (HL) into address register context (IY) */
  void (*addr_from_value)(struct Codegen *cg);
  void (*load_int)(struct Codegen *cg);
  void (*load_char)(struct Codegen *cg);
  void (*store_int)(struct Codegen *cg);
  void (*store_char)(struct Codegen *cg);
  void (*emit_const_int)(struct Codegen *cg, const char *lexeme);
  void (*emit_const_char)(struct Codegen *cg, const char *lexeme);
  void (*value_to_rhs)(struct Codegen *cg);
  void (*rhs_to_lhs)(struct Codegen *cg);
  void (*value_to_char)(struct Codegen *cg);
  void (*char_to_value)(struct Codegen *cg); /* zero-extend A into HL */
  /* untyped variants removed; use typed below */
  /* Typed variants for 8-bit vs 16-bit arithmetic */
  void (*op_add_char)(struct Codegen *cg);
  void (*op_sub_char)(struct Codegen *cg);
  void (*op_mul_char)(struct Codegen *cg);
  void (*op_div_char)(struct Codegen *cg);
  void (*op_mod_char)(struct Codegen *cg);
  void (*op_neg_char)(struct Codegen *cg);
  void (*op_shl_char)(struct Codegen *cg);
  void (*op_shr_char)(struct Codegen *cg);
  void (*op_add_int)(struct Codegen *cg);
  void (*op_sub_int)(struct Codegen *cg);
  void (*op_mul_int)(struct Codegen *cg);
  void (*op_div_int)(struct Codegen *cg);
  void (*op_mod_int)(struct Codegen *cg);
  void (*op_neg_int)(struct Codegen *cg);
  void (*op_shl_int)(struct Codegen *cg);
  void (*op_shr_int)(struct Codegen *cg);
  /* Helpers for typed arithmetic */
  void (*scale_rhs_by)(struct Codegen *cg, int factor);   /* DE = DE * factor */
  void (*divide_value_by)(struct Codegen *cg, int divisor); /* HL = HL / divisor */
  void (*push_arg)(struct Codegen *cg);
  void (*call_direct)(struct Codegen *cg, const char *name);
  void (*cleanup_args)(struct Codegen *cg, int num_bytes);
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
  int (*invoke_linker)(struct Codegen *cg, const char **obj_files, int num_objs, const char *out_file, const char *linker_script);
  
  /* Code generation function table initialization */
  void (*init_codegen)(struct Codegen *cg);
};

/* Target registry and selection */
struct Target *target_find(const char *name);
void target_list(void);

/* Codegen driver */
void codegen_init_defaults(struct Codegen *cg);
void codegen_generate(struct Codegen *cg, struct ASTNode *tree);

/* Built-in targets */
extern struct Target target_z80;

#endif /* TARGET_H */
