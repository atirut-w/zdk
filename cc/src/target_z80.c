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
}

static void z80_finalize(struct Codegen *cg) {
  if (cg && cg->target_data) {
    free(cg->target_data);
    cg->target_data = NULL;
  }
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

static void z80_act_fn_prologue(struct Codegen *cg, int stack_size) {
  fprintf(cg->output, "\t; prologue\n");
  fprintf(cg->output, "\tpush ix\n");
  fprintf(cg->output, "\tld ix, 0\n");
  fprintf(cg->output, "\tadd ix, sp\n");
  if (stack_size > 0) {
    fprintf(cg->output, "\tld hl, -%d\n", stack_size);
    fprintf(cg->output, "\tadd hl, sp\n");
    fprintf(cg->output, "\tld sp, hl\n");
  }
  /* Record frame size (currently unused) */
  {
    struct Z80Data *d = (struct Z80Data *)cg->target_data;
    if (d) d->stack_offset = stack_size;
  }
}

static void z80_act_fn_epilogue(struct Codegen *cg) {
  fprintf(cg->output, "\t; epilogue\n");
  fprintf(cg->output, "\tld sp, ix\n");
  fprintf(cg->output, "\tpop ix\n");
  fprintf(cg->output, "\tret\n");
}

static void z80_act_alloc_stack(struct Codegen *cg, int bytes) {
  if (bytes > 0) {
    fprintf(cg->output, "\t; allocate %d bytes temp\n", bytes);
    fprintf(cg->output, "\tld hl, -%d\n", bytes);
    fprintf(cg->output, "\tadd hl, sp\n");
    fprintf(cg->output, "\tld sp, hl\n");
  }
}

static void z80_act_addr_symbol(struct Codegen *cg, const char *name) {
  /* Address of global symbol -> HL */
  fprintf(cg->output, "\tld hl, %s\n", name);
}

static void z80_act_addr_local(struct Codegen *cg, int offset) {
  /* locals at negative offsets from IX; compute address in IY */
  fprintf(cg->output, "\tpush ix\n");
  fprintf(cg->output, "\tpop hl\n");
  fprintf(cg->output, "\tld de, %d\n", offset);
  fprintf(cg->output, "\tadd hl, de\n");
}

static void z80_act_addr_param(struct Codegen *cg, int offset) {
  /* params at positive offsets from IX; compute address in IY */
  fprintf(cg->output, "\tpush ix\n");
  fprintf(cg->output, "\tpop hl\n");
  fprintf(cg->output, "\tld de, %d\n", offset);
  fprintf(cg->output, "\tadd hl, de\n");
}

static void z80_act_addr_from_value(struct Codegen *cg) {
  /* Move address in HL into IY */
  fprintf(cg->output, "\tpush hl\n");
  fprintf(cg->output, "\tpop iy\n");
}

static void z80_act_load_int(struct Codegen *cg) {
  /* Load 16-bit little-endian from [IY] into HL */
  fprintf(cg->output, "\tld l, (iy+0)\n");
  fprintf(cg->output, "\tld h, (iy+1)\n");
}

static void z80_act_load_char(struct Codegen *cg) {
  /* Load 8-bit from [IY]; A and L receive the byte; H left undefined */
  fprintf(cg->output, "\tld a, (iy+0)\n");
  fprintf(cg->output, "\tld l, a\n");
}

static void z80_act_store_int(struct Codegen *cg) {
  fprintf(cg->output, "\tld (iy+0), l\n");
  fprintf(cg->output, "\tld (iy+1), h\n");
}

static void z80_act_store_char(struct Codegen *cg) {
  fprintf(cg->output, "\tld (iy+0), a\n");
}

static void z80_act_emit_const_int(struct Codegen *cg, const char *lexeme) {
  fprintf(cg->output, "\tld hl, %s\n", lexeme);
}

static void z80_act_emit_const_char(struct Codegen *cg, const char *lexeme) {
  fprintf(cg->output, "\tld a, %s\n", lexeme);
}

static void z80_act_value_to_rhs(struct Codegen *cg) { /* HL -> DE */
  /* Swap is cheaper here; caller restores HL soon after */
  fprintf(cg->output, "\tex de, hl\n");
}

static void z80_act_rhs_to_lhs(struct Codegen *cg) { /* DE -> HL */
  fprintf(cg->output, "\tex de, hl\n");
}

static void z80_act_value_to_char(struct Codegen *cg) {
  /* Move low byte of HL into A */
  fprintf(cg->output, "\tld a, l\n");
}

/* Generic extension helpers
   from_bits / to_bits allow future widening beyond 8->16.
   Current implementation handles only 8->16; other sizes emit a comment. */
static void z80_zero_extend(struct Codegen *cg, int from_bits, int to_bits) {
  if (!cg || !cg->output) return;
  if (from_bits == 8 && to_bits == 16) {
    fprintf(cg->output, "\tld l, a\n");
    fprintf(cg->output, "\txor a\n");
    fprintf(cg->output, "\tld h, a\n");
  } else if (from_bits == to_bits) {
    /* No-op; ensure value resides in expected register set if needed */
    fprintf(cg->output, "\t; zero_extend %d->%d no-op\n", from_bits, to_bits);
  } else {
    fprintf(cg->output, "\t; TODO zero_extend %d->%d unsupported\n", from_bits, to_bits);
  }
}

static void z80_sign_extend(struct Codegen *cg, int from_bits, int to_bits) {
  if (!cg || !cg->output) return;
  if (from_bits == 8 && to_bits == 16) {
    /* Preserve original A in L first */
    fprintf(cg->output, "\tld l, a\n");
    /* Sign bit to all bits: add a,a; sbc a,a -> 0x00 or 0xFF */
    fprintf(cg->output, "\tadd a, a\n");
    fprintf(cg->output, "\tsbc a, a\n");
    fprintf(cg->output, "\tld h, a\n");
  } else if (from_bits == to_bits) {
    fprintf(cg->output, "\t; sign_extend %d->%d no-op\n", from_bits, to_bits);
  } else {
    fprintf(cg->output, "\t; TODO sign_extend %d->%d unsupported\n", from_bits, to_bits);
  }
}

static void z80_act_op_add(struct Codegen *cg) { fprintf(cg->output, "\tadd hl, de\n"); }
static void z80_act_op_sub(struct Codegen *cg) { fprintf(cg->output, "\txor a\n\tsbc hl, de\n"); }
static void z80_act_op_neg(struct Codegen *cg) { fprintf(cg->output, "\tld d, h\n\tld e, l\n\txor a\n\tsbc hl, de\n"); }
static void z80_act_op_mul(struct Codegen *cg) { fprintf(cg->output, "\tpush de\n\tpush hl\n\tcall __mulhi3\n\tpop bc\n\tpop bc\n"); }
static void z80_act_op_div(struct Codegen *cg) { fprintf(cg->output, "\tpush de\n\tpush hl\n\tcall __divhi3\n\tpop bc\n\tpop bc\n"); }
static void z80_act_op_mod(struct Codegen *cg) { fprintf(cg->output, "\tpush de\n\tpush hl\n\tcall __modhi3\n\tpop bc\n\tpop bc\n"); }
static void z80_act_op_shl(struct Codegen *cg) { fprintf(cg->output, "\tpush de\n\tpush hl\n\tcall __ashlhi3\n\tpop bc\n\tpop bc\n"); }
static void z80_act_op_shr(struct Codegen *cg) { fprintf(cg->output, "\tpush de\n\tpush hl\n\tcall __ashrhi3\n\tpop bc\n\tpop bc\n"); }

static void z80_act_scale_rhs_by(struct Codegen *cg, int factor) {
  /* Reuse op_mul: starting HL=left (ptr), DE=right (int)
     We want DE = right*factor, HL = left preserved. */
  fprintf(cg->output, "\tex de, hl\n"); /* HL = right, DE = left */
  fprintf(cg->output, "\tpush hl\n"); /* save right */
  fprintf(cg->output, "\tld hl, %d\n", factor); /* HL = factor */
  fprintf(cg->output, "\tex de, hl\n"); /* DE = factor, HL = left */
  fprintf(cg->output, "\tpop hl\n"); /* HL = right */
  /* HL=right, DE=factor -> HL = right*factor */
  z80_act_op_mul(cg);
  fprintf(cg->output, "\tex de, hl\n"); /* DE = scaled, HL = left */
}

static void z80_act_divide_value_by(struct Codegen *cg, int divisor) {
  /* Reuse op_div: expect HL = value, DE = divisor */
  fprintf(cg->output, "\tpush hl\n"); /* save value */
  fprintf(cg->output, "\tld hl, %d\n", divisor); /* HL = divisor */
  fprintf(cg->output, "\tex de, hl\n"); /* DE = divisor */
  fprintf(cg->output, "\tpop hl\n"); /* HL = value */
  z80_act_op_div(cg); /* HL = value / divisor */
}

/* Generic truncate helper -> A (low byte) */
static void z80_truncate_to_char(struct Codegen *cg) { fprintf(cg->output, "\tld a, l\n"); }

static void z80_act_push_arg(struct Codegen *cg) { fprintf(cg->output, "\tpush hl\n"); }
static void z80_act_call_direct(struct Codegen *cg, const char *name) { fprintf(cg->output, "\tcall %s\n", name); }
static void z80_act_cleanup_args(struct Codegen *cg, int num_bytes) {
  if (num_bytes == 2) { fprintf(cg->output, "\tpop bc\n"); }
  else if (num_bytes == 4) { fprintf(cg->output, "\tpop bc\n\tpop bc\n"); }
  else if (num_bytes > 0) {
    fprintf(cg->output, "\tld iy, %d\n", num_bytes);
    fprintf(cg->output, "\tadd iy, sp\n");
    fprintf(cg->output, "\tld sp, iy\n");

  }
}

/* Generic helpers now exposed via Codegen for non-target code */
static void z80_save_value(struct Codegen *cg) { fprintf(cg->output, "\tpush hl\n"); }
static void z80_restore_value(struct Codegen *cg) { fprintf(cg->output, "\tpop hl\n"); }
static void z80_jump_label(struct Codegen *cg, const char *label) { fprintf(cg->output, "\tjp %s\n", label); }
static void z80_save_addr(struct Codegen *cg) { fprintf(cg->output, "\tpush iy\n"); }
static void z80_restore_addr(struct Codegen *cg) { fprintf(cg->output, "\tpop iy\n"); }

static void z80_init_local_from_symbol(struct Codegen *cg, int local_offset, const char *symbol, int size) {
  /* HL = IX + local_offset (dest), DE = symbol (src), BC = size; LDIR */
  fprintf(cg->output, "\tpush ix\n");
  fprintf(cg->output, "\tpop hl\n");
  fprintf(cg->output, "\tld de, %d\n", local_offset);
  fprintf(cg->output, "\tadd hl, de\n");
  fprintf(cg->output, "\tld de, %s\n", symbol);
  fprintf(cg->output, "\tld bc, %d\n", size);
  fprintf(cg->output, "\tldir\n");
}

static void z80_init_codegen(struct Codegen *cg) {
  cg->fn_prologue = z80_act_fn_prologue;
  cg->fn_epilogue = z80_act_fn_epilogue;
  cg->alloc_stack = z80_act_alloc_stack;
  cg->addr_symbol = z80_act_addr_symbol;
  cg->addr_local = z80_act_addr_local;
  cg->addr_param = z80_act_addr_param;
  cg->addr_from_value = z80_act_addr_from_value;
  cg->load_int = z80_act_load_int;
  cg->load_char = z80_act_load_char;
  cg->store_int = z80_act_store_int;
  cg->store_char = z80_act_store_char;
  cg->emit_const_int = z80_act_emit_const_int;
  cg->emit_const_char = z80_act_emit_const_char;
  cg->value_to_rhs = z80_act_value_to_rhs;
  cg->rhs_to_lhs = z80_act_rhs_to_lhs;
  cg->truncate_to_char = z80_truncate_to_char;
  cg->zero_extend = z80_zero_extend;
  cg->sign_extend = z80_sign_extend;
  /* Generic arithmetic (operands already promoted) */
  cg->op_add = z80_act_op_add;
  cg->op_sub = z80_act_op_sub;
  cg->op_neg = z80_act_op_neg;
  cg->op_mul = z80_act_op_mul;
  cg->op_div = z80_act_op_div;
  cg->op_mod = z80_act_op_mod;
  cg->op_shl = z80_act_op_shl;
  cg->op_shr = z80_act_op_shr;
  cg->scale_rhs_by = z80_act_scale_rhs_by;
  cg->divide_value_by = z80_act_divide_value_by;
  cg->push_arg = z80_act_push_arg;
  cg->call_direct = z80_act_call_direct;
  cg->cleanup_args = z80_act_cleanup_args;
  cg->save_value = z80_save_value;
  cg->restore_value = z80_restore_value;
  cg->jump_label = z80_jump_label;
  cg->save_addr = z80_save_addr;
  cg->restore_addr = z80_restore_addr;
  cg->init_local_from_symbol = z80_init_local_from_symbol;
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
