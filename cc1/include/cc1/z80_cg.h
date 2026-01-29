#ifndef CC1_Z80_CG_H
#define CC1_Z80_CG_H

#include "cc1/emit.h"

struct cc1_type;
struct cc1_sym;

struct cc1_z80_cg {
  struct cc1_emit *e;

  const char *exit_label;

  unsigned long cur_fn_exit_label;
  int stack_local_bytes;
};

void cc1_z80_cg_init(struct cc1_z80_cg *cg, struct cc1_emit *e,
                     const char *exit_label);

void cc1_z80_emit_fn_prologue(struct cc1_z80_cg *cg, const char *asm_name);
void cc1_z80_emit_fn_epilogue(struct cc1_z80_cg *cg);

void cc1_z80_emit_call_cleanup(struct cc1_z80_cg *cg, int slots);

void cc1_z80_emit_load_imm16_hl(struct cc1_z80_cg *cg, unsigned long v);
void cc1_z80_emit_push_hl(struct cc1_z80_cg *cg);
void cc1_z80_emit_push_char_from_a(struct cc1_z80_cg *cg);

void cc1_z80_emit_call(struct cc1_z80_cg *cg, const char *asm_name);

void cc1_z80_emit_alloc_stack(struct cc1_z80_cg *cg, unsigned long bytes);

void cc1_z80_emit_return_void(struct cc1_z80_cg *cg);
void cc1_z80_emit_return_hl(struct cc1_z80_cg *cg);
void cc1_z80_emit_return_a(struct cc1_z80_cg *cg);

void cc1_z80_emit_load_local_addr_hl(struct cc1_z80_cg *cg, int ix_off);
void cc1_z80_emit_load_u8_from_addr_a(struct cc1_z80_cg *cg);
void cc1_z80_emit_load_u16_from_addr_hl(struct cc1_z80_cg *cg);
void cc1_z80_emit_store_u8_to_addr_from_a(struct cc1_z80_cg *cg);
void cc1_z80_emit_store_u16_to_addr_from_hl(struct cc1_z80_cg *cg);

#endif
