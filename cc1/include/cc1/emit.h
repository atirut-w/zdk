#ifndef CC1_EMIT_H
#define CC1_EMIT_H

#include <stdio.h>

struct cc1_emit {
  FILE *text;
  FILE *rodata;
  FILE *data;
  FILE *bss;

  unsigned long next_label_id;
  unsigned long next_cstr_id;
};

int cc1_emit_init(struct cc1_emit *e);
void cc1_emit_dispose(struct cc1_emit *e);

const char *cc1_emit_new_local_label(struct cc1_emit *e, char *buf,
                                     unsigned long buflen);
const char *cc1_emit_new_cstr_label(struct cc1_emit *e, char *buf,
                                    unsigned long buflen);

void cc1_emit_concat_to(FILE *out, struct cc1_emit *e);

void cc1_emit_text(struct cc1_emit *e, const char *fmt, const char *a);
void cc1_emit_text0(struct cc1_emit *e, const char *s);
void cc1_emit_rodata0(struct cc1_emit *e, const char *s);
void cc1_emit_data0(struct cc1_emit *e, const char *s);
void cc1_emit_bss0(struct cc1_emit *e, const char *s);

#endif
