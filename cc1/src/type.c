#include "cc1/type.h"

#include <stdlib.h>
#include <string.h>

static struct cc1_type *cc1_new_type(enum cc1_type_kind k) {
  struct cc1_type *t = (struct cc1_type *)calloc(1, sizeof(*t));
  t->kind = k;
  t->is_const = 0;
  t->base = 0;
  t->ret = 0;
  t->params = 0;
  t->has_proto = 0;
  t->is_varargs = 0;
  return t;
}

struct cc1_type_ctx *cc1_type_ctx_new(void) {
  struct cc1_type_ctx *tc = (struct cc1_type_ctx *)calloc(1, sizeof(*tc));
  tc->t_void = cc1_new_type(TY_VOID);
  tc->t_char = cc1_new_type(TY_CHAR);
  tc->t_uchar = cc1_new_type(TY_UCHAR);
  tc->t_schar = cc1_new_type(TY_SCHAR);
  tc->t_short = cc1_new_type(TY_SHORT);
  tc->t_ushort = cc1_new_type(TY_USHORT);
  tc->t_int = cc1_new_type(TY_INT);
  tc->t_uint = cc1_new_type(TY_UINT);
  return tc;
}

void cc1_type_ctx_free(struct cc1_type_ctx *tc) {
  /* NOTE: leaks derived types for now; fine for bring-up */
  if (!tc)
    return;
  free(tc->t_void);
  free(tc->t_char);
  free(tc->t_uchar);
  free(tc->t_schar);
  free(tc->t_short);
  free(tc->t_ushort);
  free(tc->t_int);
  free(tc->t_uint);
  free(tc);
}

struct cc1_type *cc1_type_ptr(struct cc1_type_ctx *tc, struct cc1_type *base) {
  struct cc1_type *t = cc1_new_type(TY_PTR);
  (void)tc;
  t->base = base;
  return t;
}

struct cc1_type *cc1_type_func(struct cc1_type_ctx *tc, struct cc1_type *ret,
                               struct cc1_param *params, int has_proto) {
  struct cc1_type *t = cc1_new_type(TY_FUNC);
  (void)tc;
  t->ret = ret;
  t->params = params;
  t->has_proto = has_proto;
  return t;
}

unsigned long cc1_type_sizeof(struct cc1_type *t) {
  if (!t)
    return 0;
  switch (t->kind) {
  case TY_VOID:
    return 0;
  case TY_CHAR:
  case TY_UCHAR:
  case TY_SCHAR:
    return 1;
  case TY_SHORT:
  case TY_USHORT:
  case TY_INT:
  case TY_UINT:
  case TY_PTR:
    return 2;
  case TY_FUNC:
    return 2;
  default:
    return 2;
  }
}

unsigned long cc1_type_alignof(struct cc1_type *t) {
  if (!t)
    return 1;
  switch (t->kind) {
  case TY_CHAR:
  case TY_UCHAR:
  case TY_SCHAR:
    return 1;
  default:
    return 2;
  }
}

int cc1_type_is_integer(struct cc1_type *t) {
  if (!t)
    return 0;
  switch (t->kind) {
  case TY_CHAR:
  case TY_UCHAR:
  case TY_SCHAR:
  case TY_SHORT:
  case TY_USHORT:
  case TY_INT:
  case TY_UINT:
    return 1;
  default:
    return 0;
  }
}

int cc1_type_is_char8(struct cc1_type *t) {
  if (!t)
    return 0;
  return t->kind == TY_CHAR || t->kind == TY_UCHAR || t->kind == TY_SCHAR;
}
