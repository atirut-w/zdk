#ifndef CC1_TYPE_H
#define CC1_TYPE_H

struct cc1_type;

enum cc1_type_kind {
  TY_VOID,
  TY_CHAR,
  TY_UCHAR,
  TY_SCHAR,
  TY_SHORT,
  TY_USHORT,
  TY_INT,
  TY_UINT,
  TY_PTR,
  TY_FUNC
};

struct cc1_param {
  const char *name; /* may be NULL */
  struct cc1_type *type;
  struct cc1_param *next;
};

struct cc1_type {
  enum cc1_type_kind kind;
  int is_const;

  struct cc1_type *base; /* for ptr */
  struct cc1_type *ret;  /* for func */
  struct cc1_param *params;
  int has_proto;  /* 1 for prototyped, 0 for unspecified params */
  int is_varargs; /* later */
};

struct cc1_type_ctx {
  struct cc1_type *t_void;
  struct cc1_type *t_char;
  struct cc1_type *t_uchar;
  struct cc1_type *t_schar;
  struct cc1_type *t_short;
  struct cc1_type *t_ushort;
  struct cc1_type *t_int;
  struct cc1_type *t_uint;
};

struct cc1_type_ctx *cc1_type_ctx_new(void);
void cc1_type_ctx_free(struct cc1_type_ctx *tc);

struct cc1_type *cc1_type_ptr(struct cc1_type_ctx *tc, struct cc1_type *base);
struct cc1_type *cc1_type_func(struct cc1_type_ctx *tc, struct cc1_type *ret,
                               struct cc1_param *params, int has_proto);

unsigned long cc1_type_sizeof(struct cc1_type *t);
unsigned long cc1_type_alignof(struct cc1_type *t);

int cc1_type_is_integer(struct cc1_type *t);
int cc1_type_is_char8(struct cc1_type *t);

#endif
