#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc1/parse.h"
#include "cc1/strpool.h"
#include "cc1/sym.h"
#include "cc1/type.h"

static void cc1_usage(const char *prog) {
  fprintf(stderr, "usage: %s <input.c> [-o <out.s>]\n", prog);
}

int main(int argc, char *argv[]) {
  const char *in_path = 0;
  const char *out_path = 0;
  FILE *in = 0;
  FILE *out = 0;
  int i;

  struct cc1_compilation *cc;

  cc = (struct cc1_compilation *)calloc(1, sizeof(*cc));
  if (!cc) {
    fprintf(stderr, "cc1: out of memory\n");
    return 1;
  }
  cc1_diag_init(&cc->diag);
  cc->mangle_underscore = 1;

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-o")) {
      if (i + 1 >= argc) {
        cc1_usage(argv[0]);
        return 2;
      }
      out_path = argv[++i];
      continue;
    }
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      cc1_usage(argv[0]);
      return 0;
    }
    if (!in_path) {
      in_path = argv[i];
      continue;
    }
  }

  if (!in_path) {
    cc1_usage(argv[0]);
    return 2;
  }

  in = fopen(in_path, "rb");
  if (!in) {
    fprintf(stderr, "cc1: failed to open input: %s\n", in_path);
    return 1;
  }

  if (out_path) {
    out = fopen(out_path, "wb");
    if (!out) {
      fprintf(stderr, "cc1: failed to open output: %s\n", out_path);
      fclose(in);
      return 1;
    }
  } else {
    out = stdout;
  }

  cc->sp = cc1_strpool_new();
  cc->sym = cc1_symtab_new(cc->sp);
  cc->types = cc1_type_ctx_new();
  if (!cc1_emit_init(&cc->emit)) {
    fprintf(stderr, "cc1: failed to create temp outputs\n");
    fclose(in);
    if (out != stdout)
      fclose(out);
    free(cc);
    return 1;
  }

  cc1_lex_init(&cc->lex, in, in_path, &cc->diag, cc->sp, cc->sym);

  if (!cc1_parse_translation_unit(cc)) {
    /* error already reported */
    cc1_emit_dispose(&cc->emit);
    cc1_type_ctx_free(cc->types);
    cc1_symtab_free(cc->sym);
    cc1_strpool_free(cc->sp);
    fclose(in);
    if (out != stdout)
      fclose(out);
    free(cc);
    return 1;
  }

  cc1_emit_concat_to(out, &cc->emit);

  cc1_emit_dispose(&cc->emit);
  cc1_type_ctx_free(cc->types);
  cc1_symtab_free(cc->sym);
  cc1_strpool_free(cc->sp);
  fclose(in);
  if (out != stdout)
    fclose(out);
  free(cc);
  return 0;
}
