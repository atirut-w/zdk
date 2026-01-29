#ifndef CC1_PARSE_H
#define CC1_PARSE_H

#include "cc1/diag.h"
#include "cc1/emit.h"
#include "cc1/lexer.h"

struct cc1_strpool;
struct cc1_symtab;
struct cc1_type_ctx;

struct cc1_compilation {
  struct cc1_diag diag;
  struct cc1_strpool *sp;
  struct cc1_symtab *sym;
  struct cc1_type_ctx *types;

  struct cc1_emit emit;
  struct cc1_lexer lex;

  struct cc1_tok tok;

  int mangle_underscore;
};

int cc1_parse_translation_unit(struct cc1_compilation *cc);

#endif
