#include "cc1/emit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void cc1_copy_file(FILE *out, FILE *in) {
  int c;
  rewind(in);
  while ((c = fgetc(in)) != EOF) {
    fputc(c, out);
  }
}

int cc1_emit_init(struct cc1_emit *e) {
  memset(e, 0, sizeof(*e));
  e->text = tmpfile();
  e->rodata = tmpfile();
  e->data = tmpfile();
  e->bss = tmpfile();
  if (!e->text || !e->rodata || !e->data || !e->bss) {
    return 0;
  }
  e->next_label_id = 1;
  e->next_cstr_id = 1;
  return 1;
}

void cc1_emit_dispose(struct cc1_emit *e) {
  if (e->text)
    fclose(e->text);
  if (e->rodata)
    fclose(e->rodata);
  if (e->data)
    fclose(e->data);
  if (e->bss)
    fclose(e->bss);
  memset(e, 0, sizeof(*e));
}

const char *cc1_emit_new_local_label(struct cc1_emit *e, char *buf,
                                     unsigned long buflen) {
  unsigned long id = e->next_label_id++;
  if (buflen < 32)
    return 0;
  sprintf(buf, ".L%lu", id);
  return buf;
}

const char *cc1_emit_new_cstr_label(struct cc1_emit *e, char *buf,
                                    unsigned long buflen) {
  unsigned long id = e->next_cstr_id++;
  if (buflen < 32)
    return 0;
  sprintf(buf, ".LC%lu", id);
  return buf;
}

void cc1_emit_concat_to(FILE *out, struct cc1_emit *e) {
  fputs("\t.section .text,\"ax\",@progbits\n", out);
  cc1_copy_file(out, e->text);

  fputs("\n\t.section .rodata,\"a\",@progbits\n", out);
  cc1_copy_file(out, e->rodata);

  fputs("\n\t.section .data,\"aw\",@progbits\n", out);
  cc1_copy_file(out, e->data);

  fputs("\n\t.section .bss,\"aw\",@nobits\n", out);
  cc1_copy_file(out, e->bss);
}

void cc1_emit_text(struct cc1_emit *e, const char *fmt, const char *a) {
  fprintf(e->text, fmt, a);
}

void cc1_emit_text0(struct cc1_emit *e, const char *s) { fputs(s, e->text); }
void cc1_emit_rodata0(struct cc1_emit *e, const char *s) {
  fputs(s, e->rodata);
}
void cc1_emit_data0(struct cc1_emit *e, const char *s) { fputs(s, e->data); }
void cc1_emit_bss0(struct cc1_emit *e, const char *s) { fputs(s, e->bss); }
