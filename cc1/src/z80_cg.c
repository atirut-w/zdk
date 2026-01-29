#include "cc1/z80_cg.h"

#include <stdio.h>

void cc1_z80_cg_init(struct cc1_z80_cg *cg, struct cc1_emit *e,
                     const char *exit_label) {
  cg->e = e;
  cg->exit_label = exit_label;
  cg->cur_fn_exit_label = 0;
  cg->stack_local_bytes = 0;
}

void cc1_z80_emit_fn_prologue(struct cc1_z80_cg *cg, const char *asm_name) {
  (void)asm_name;
  cc1_emit_text0(cg->e, "\tpush ix\n");
  cc1_emit_text0(cg->e, "\tld ix, 0\n");
  cc1_emit_text0(cg->e, "\tadd ix, sp\n");
}

void cc1_z80_emit_fn_epilogue(struct cc1_z80_cg *cg) {
  if (cg->exit_label) {
    cc1_emit_text0(cg->e, cg->exit_label);
    cc1_emit_text0(cg->e, ":\n");
  }
  /* ensure sp restored */
  cc1_emit_text0(cg->e, "\tld sp, ix\n");
  cc1_emit_text0(cg->e, "\tpop ix\n");
  cc1_emit_text0(cg->e, "\tret\n");
}

void cc1_z80_emit_call_cleanup(struct cc1_z80_cg *cg, int slots) {
  int i;
  for (i = 0; i < slots; i++) {
    cc1_emit_text0(cg->e, "\tpop bc\n");
  }
}

void cc1_z80_emit_load_imm16_hl(struct cc1_z80_cg *cg, unsigned long v) {
  char buf[64];
  sprintf(buf, "\tld hl, %lu\n", v & 0xffffu);
  cc1_emit_text0(cg->e, buf);
}

void cc1_z80_emit_push_hl(struct cc1_z80_cg *cg) {
  cc1_emit_text0(cg->e, "\tpush hl\n");
}

void cc1_z80_emit_push_char_from_a(struct cc1_z80_cg *cg) {
  cc1_emit_text0(cg->e, "\tld l, a\n");
  cc1_emit_text0(cg->e, "\tpush hl\n");
}

void cc1_z80_emit_call(struct cc1_z80_cg *cg, const char *asm_name) {
  cc1_emit_text0(cg->e, "\tcall ");
  cc1_emit_text0(cg->e, asm_name);
  cc1_emit_text0(cg->e, "\n");
}

void cc1_z80_emit_alloc_stack(struct cc1_z80_cg *cg, unsigned long bytes) {
  char buf[64];
  if (bytes == 0)
    return;
  sprintf(buf, "\tld hl, -%lu\n", bytes & 0xffffu);
  cc1_emit_text0(cg->e, buf);
  cc1_emit_text0(cg->e, "\tadd hl, sp\n");
  cc1_emit_text0(cg->e, "\tld sp, hl\n");
  cg->stack_local_bytes += (int)bytes;
}

void cc1_z80_emit_return_void(struct cc1_z80_cg *cg) {
  if (cg->exit_label) {
    cc1_emit_text0(cg->e, "\tjp ");
    cc1_emit_text0(cg->e, cg->exit_label);
    cc1_emit_text0(cg->e, "\n");
  }
}

void cc1_z80_emit_return_hl(struct cc1_z80_cg *cg) {
  cc1_z80_emit_return_void(cg);
}

void cc1_z80_emit_return_a(struct cc1_z80_cg *cg) {
  cc1_z80_emit_return_void(cg);
}

void cc1_z80_emit_load_local_addr_hl(struct cc1_z80_cg *cg, int ix_off) {
  char buf[64];
  cc1_emit_text0(cg->e, "\tpush ix\n");
  cc1_emit_text0(cg->e, "\tpop hl\n");
  sprintf(buf, "\tld de, %d\n", ix_off);
  cc1_emit_text0(cg->e, buf);
  cc1_emit_text0(cg->e, "\tadd hl, de\n");
}

void cc1_z80_emit_load_u8_from_addr_a(struct cc1_z80_cg *cg) {
  cc1_emit_text0(cg->e, "\tld a, (hl)\n");
}

void cc1_z80_emit_load_u16_from_addr_hl(struct cc1_z80_cg *cg) {
  cc1_emit_text0(cg->e, "\tld a, (hl)\n");
  cc1_emit_text0(cg->e, "\tinc hl\n");
  cc1_emit_text0(cg->e, "\tld h, (hl)\n");
  cc1_emit_text0(cg->e, "\tld l, a\n");
}

void cc1_z80_emit_store_u8_to_addr_from_a(struct cc1_z80_cg *cg) {
  cc1_emit_text0(cg->e, "\tld (hl), a\n");
}

void cc1_z80_emit_store_u16_to_addr_from_hl(struct cc1_z80_cg *cg) {
  cc1_emit_text0(cg->e, "\tld (hl), l\n");
  cc1_emit_text0(cg->e, "\tinc hl\n");
  cc1_emit_text0(cg->e, "\tld (hl), h\n");
}
