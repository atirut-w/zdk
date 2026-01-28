#include "cc1/sym.h"

#include <stdlib.h>
#include <string.h>

struct cc1_symtab *cc1_symtab_new(struct cc1_strpool *sp) {
  struct cc1_symtab *st = (struct cc1_symtab *)calloc(1, sizeof(*st));
  st->sp = sp;
  st->top = 0;
  cc1_symtab_push(st);
  return st;
}

void cc1_symtab_free(struct cc1_symtab *st) {
  if (!st)
    return;
  while (st->top)
    cc1_symtab_pop(st);
  free(st);
}

void cc1_symtab_push(struct cc1_symtab *st) {
  struct cc1_scope *sc = (struct cc1_scope *)calloc(1, sizeof(*sc));
  sc->syms = 0;
  sc->prev = st->top;
  st->top = sc;
}

void cc1_symtab_pop(struct cc1_symtab *st) {
  struct cc1_scope *sc = st->top;
  struct cc1_sym *s;
  struct cc1_sym *nx;
  if (!sc)
    return;
  s = sc->syms;
  while (s) {
    nx = s->next;
    free(s);
    s = nx;
  }
  st->top = sc->prev;
  free(sc);
}

struct cc1_sym *cc1_sym_lookup_current(struct cc1_symtab *st,
                                       const char *name) {
  struct cc1_sym *s;
  if (!st->top)
    return 0;
  for (s = st->top->syms; s; s = s->next) {
    if (s->name == name || (s->name && name && !strcmp(s->name, name)))
      return s;
  }
  return 0;
}

struct cc1_sym *cc1_sym_lookup(struct cc1_symtab *st, const char *name) {
  struct cc1_scope *sc;
  for (sc = st->top; sc; sc = sc->prev) {
    struct cc1_sym *s;
    for (s = sc->syms; s; s = s->next) {
      if (s->name == name || (s->name && name && !strcmp(s->name, name)))
        return s;
    }
  }
  return 0;
}

int cc1_sym_is_typedef_visible(struct cc1_symtab *st, const char *name) {
  struct cc1_sym *s = cc1_sym_lookup(st, name);
  return s && s->kind == SYM_TYPEDEF;
}

struct cc1_sym *cc1_sym_add(struct cc1_symtab *st, enum cc1_sym_kind kind,
                            const char *name, const char *asm_name,
                            struct cc1_type *type) {
  struct cc1_sym *s;
  if (!st->top)
    return 0;
  s = (struct cc1_sym *)calloc(1, sizeof(*s));
  s->kind = kind;
  s->name = name;
  s->asm_name = asm_name;
  s->type = type;
  s->is_extern = 0;
  s->is_defined = 0;
  s->stack_offset = 0;
  s->next = st->top->syms;
  st->top->syms = s;
  return s;
}
