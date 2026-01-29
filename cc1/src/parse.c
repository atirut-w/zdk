#include "cc1/parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc1/strpool.h"
#include "cc1/sym.h"
#include "cc1/type.h"
#include "cc1/z80_cg.h"

struct cc1_expr {
  struct cc1_type *type;
  int is_lvalue;
  int is_const;
  long ival;
  const char *label;

  /* Value location: for this bring-up we just track what register holds it */
  int in_a;
  int in_hl;

  /* lvalue location */
  int has_ix_off;
  int ix_off;
  int addr_in_hl;
};

struct cc1_fn_ctx {
  struct cc1_z80_cg cg;
  struct cc1_sym *fn;
  int next_local_off; /* negative */
};

static void cc1_next(struct cc1_compilation *cc) {
  cc->tok = cc1_lex_next(&cc->lex);
}

static int cc1_accept(struct cc1_compilation *cc, enum cc1_tok_kind k) {
  if (cc->tok.kind == k) {
    cc1_next(cc);
    return 1;
  }
  return 0;
}

static void cc1_expect(struct cc1_compilation *cc, enum cc1_tok_kind k,
                       const char *what) {
  if (cc->tok.kind != k) {
    cc1_diag_error_at(&cc->diag, cc->tok.loc, what);
    return;
  }
  cc1_next(cc);
}

static const char *cc1_mangle(struct cc1_compilation *cc, const char *name) {
  char buf[512];
  unsigned long n;
  if (!cc->mangle_underscore)
    return name;
  n = (unsigned long)strlen(name);
  if (n + 2 >= sizeof(buf))
    return name;
  buf[0] = '_';
  memcpy(buf + 1, name, n + 1);
  return cc1_strpool_intern(cc->sp, buf, n + 1);
}

static int cc1_is_decl_start(struct cc1_compilation *cc) {
  switch (cc->tok.kind) {
  case TOK_KW_TYPEDEF:
  case TOK_KW_EXTERN:
  case TOK_KW_STATIC:
  case TOK_KW_CONST:
  case TOK_KW_VOLATILE:
  case TOK_KW_VOID:
  case TOK_KW_CHAR:
  case TOK_KW_SHORT:
  case TOK_KW_INT:
  case TOK_KW_LONG:
  case TOK_KW_SIGNED:
  case TOK_KW_UNSIGNED:
  case TOK_TYPE_NAME:
    return 1;
  default:
    return 0;
  }
}

static struct cc1_type *cc1_parse_declspec(struct cc1_compilation *cc,
                                           int *out_is_typedef,
                                           int *out_is_extern) {
  int saw_unsigned = 0;
  int saw_signed = 0;
  int saw_short = 0;
  int saw_char = 0;
  int saw_void = 0;

  *out_is_typedef = 0;
  *out_is_extern = 0;

  for (;;) {
    if (cc1_accept(cc, TOK_KW_TYPEDEF)) {
      *out_is_typedef = 1;
      continue;
    }
    if (cc1_accept(cc, TOK_KW_EXTERN)) {
      *out_is_extern = 1;
      continue;
    }
    if (cc1_accept(cc, TOK_KW_CONST)) {
      continue;
    }
    if (cc1_accept(cc, TOK_KW_UNSIGNED)) {
      saw_unsigned = 1;
      continue;
    }
    if (cc1_accept(cc, TOK_KW_SIGNED)) {
      saw_signed = 1;
      continue;
    }
    if (cc1_accept(cc, TOK_KW_SHORT)) {
      saw_short = 1;
      continue;
    }
    if (cc1_accept(cc, TOK_KW_INT)) {
      continue;
    }
    if (cc1_accept(cc, TOK_KW_CHAR)) {
      saw_char = 1;
      continue;
    }
    if (cc1_accept(cc, TOK_KW_VOID)) {
      saw_void = 1;
      continue;
    }
    if (cc->tok.kind == TOK_TYPE_NAME) {
      struct cc1_sym *s = cc1_sym_lookup(cc->sym, cc->tok.text);
      struct cc1_type *t = 0;
      if (s && s->kind == SYM_TYPEDEF)
        t = s->type;
      cc1_next(cc);
      return t;
    }
    break;
  }

  if (saw_void) {
    return cc->types->t_void;
  }
  if (saw_char) {
    struct cc1_type *t;
    if (saw_signed)
      t = cc->types->t_schar;
    else
      t = cc->types->t_uchar; /* default unsigned */
    return t;
  }
  if (saw_short) {
    struct cc1_type *t =
        saw_unsigned ? cc->types->t_ushort : cc->types->t_short;
    return t;
  }
  /* default int */
  {
    struct cc1_type *t = saw_unsigned ? cc->types->t_uint : cc->types->t_int;
    return t;
  }
}

struct cc1_decl {
  const char *name;
  struct cc1_type *type;
};

static struct cc1_type *cc1_parse_declarator(struct cc1_compilation *cc,
                                             struct cc1_type *base,
                                             struct cc1_decl *out) {
  struct cc1_type *t = base;
  int ptrs = 0;
  while (cc1_accept(cc, TOK_STAR)) {
    ptrs += 1;
  }

  if (cc->tok.kind != TOK_IDENT) {
    cc1_diag_error_at(&cc->diag, cc->tok.loc, "expected identifier");
    out->name = 0;
  } else {
    out->name = cc->tok.text;
    cc1_next(cc);
  }

  while (ptrs--) {
    t = cc1_type_ptr(cc->types, t);
  }

  if (cc1_accept(cc, TOK_LPAREN)) {
    /* function declarator */
    struct cc1_param *params = 0;
    struct cc1_param **tail = &params;
    int has_proto = 0;
    if (!cc1_accept(cc, TOK_RPAREN)) {
      /* (void) means no parameters */
      if (cc->tok.kind == TOK_KW_VOID) {
        int is_td0 = 0;
        int is_ex0 = 0;
        struct cc1_type *vt = cc1_parse_declspec(cc, &is_td0, &is_ex0);
        (void)is_td0;
        (void)is_ex0;
        if (vt && vt->kind == TY_VOID && cc1_accept(cc, TOK_RPAREN)) {
          has_proto = 1;
          t = cc1_type_func(cc->types, t, 0, has_proto);
          out->type = t;
          return t;
        }
        cc1_diag_error_at(&cc->diag, cc->tok.loc, "unsupported parameter list");
      }

      has_proto = 1;
      for (;;) {
        int is_td2 = 0;
        int is_ex2 = 0;
        struct cc1_type *pt = cc1_parse_declspec(cc, &is_td2, &is_ex2);
        struct cc1_decl pd;
        struct cc1_param *p;
        memset(&pd, 0, sizeof(pd));
        (void)is_td2;
        (void)is_ex2;
        pt = cc1_parse_declarator(cc, pt, &pd);

        p = (struct cc1_param *)calloc(1, sizeof(*p));
        p->name = pd.name;
        p->type = pt;
        p->next = 0;
        *tail = p;
        tail = &p->next;

        if (cc1_accept(cc, TOK_COMMA))
          continue;
        cc1_expect(cc, TOK_RPAREN, "expected ')'");
        break;
      }
    } else {
      has_proto = 0; /* unspecified */
    }
    t = cc1_type_func(cc->types, t, params, has_proto);
  }

  out->type = t;
  return t;
}

static void cc1_emit_cstr(struct cc1_compilation *cc, const char *label,
                          const char *s, unsigned long n) {
  unsigned long i;
  char buf[64];
  cc1_emit_rodata0(&cc->emit, label);
  cc1_emit_rodata0(&cc->emit, ":\n");
  cc1_emit_rodata0(&cc->emit, "\t.byte ");
  for (i = 0; i < n; i++) {
    sprintf(buf, "0x%02lx", (unsigned long)(unsigned char)s[i]);
    cc1_emit_rodata0(&cc->emit, buf);
    if (i + 1 != n)
      cc1_emit_rodata0(&cc->emit, ", ");
  }
  if (n)
    cc1_emit_rodata0(&cc->emit, ", ");
  cc1_emit_rodata0(&cc->emit, "0x00\n");
}

static void cc1_z80_ensure_rvalue_hl(struct cc1_compilation *cc,
                                     struct cc1_fn_ctx *fn, struct cc1_expr *e);
static void cc1_z80_ensure_rvalue_a(struct cc1_compilation *cc,
                                    struct cc1_fn_ctx *fn, struct cc1_expr *e);

static void cc1_z80_load_ident(struct cc1_fn_ctx *fn, struct cc1_expr *out,
                               struct cc1_sym *s) {
  (void)fn;
  memset(out, 0, sizeof(*out));
  out->type = s->type;
  out->is_lvalue = (s->kind == SYM_VAR);
  if (s->kind == SYM_VAR) {
    out->has_ix_off = 1;
    out->ix_off = s->stack_offset;
  } else {
    /* function designator: treat as const address */
    out->is_const = 1;
    out->label = s->asm_name;
  }
}

static int cc1_prec(enum cc1_tok_kind k) {
  switch (k) {
  case TOK_ASSIGN:
    return 1;
  case TOK_OROR:
    return 2;
  case TOK_ANDAND:
    return 3;
  case TOK_EQ:
  case TOK_NE:
    return 4;
  case TOK_LT:
  case TOK_LE:
  case TOK_GT:
  case TOK_GE:
    return 5;
  case TOK_PLUS:
  case TOK_MINUS:
    return 6;
  case TOK_STAR:
  case TOK_SLASH:
  case TOK_PERCENT:
    return 7;
  default:
    return 0;
  }
}

static void cc1_parse_expr(struct cc1_compilation *cc, struct cc1_fn_ctx *fn,
                           int min_prec, struct cc1_expr *out);

static void cc1_parse_primary(struct cc1_compilation *cc, struct cc1_fn_ctx *fn,
                              struct cc1_expr *out) {
  memset(out, 0, sizeof(*out));
  if (cc->tok.kind == TOK_INT_LIT) {
    out->type = cc->types->t_int;
    out->is_const = 1;
    out->ival = cc->tok.ival;
    cc1_next(cc);
    return;
  }
  if (cc->tok.kind == TOK_CHAR_LIT) {
    /* character literals are int */
    out->type = cc->types->t_int;
    out->is_const = 1;
    out->ival = cc->tok.ival;
    cc1_next(cc);
    return;
  }
  if (cc->tok.kind == TOK_STR_LIT) {
    char lbuf[32];
    const char *lab = cc1_emit_new_cstr_label(&cc->emit, lbuf, sizeof(lbuf));
    cc1_emit_cstr(cc, lab, cc->tok.str, cc->tok.strlen);
    out->type = cc1_type_ptr(cc->types, cc->types->t_uchar);
    out->is_const = 1;
    out->label = cc1_strpool_intern(cc->sp, lab, (unsigned long)strlen(lab));
    cc1_next(cc);
    return;
  }
  if (cc->tok.kind == TOK_IDENT) {
    struct cc1_sym *s = cc1_sym_lookup(cc->sym, cc->tok.text);
    const char *nm = cc->tok.text;
    struct cc1_loc loc = cc->tok.loc;
    cc1_next(cc);
    if (!s) {
      struct cc1_type *ft;
      cc1_diag_warn_atf(&cc->diag, loc, "implicit declaration of function '%s'",
                        nm);
      /* implicit int function */
      ft = cc1_type_func(cc->types, cc->types->t_int, 0, 0);
      s = cc1_sym_add(cc->sym, SYM_FUNC, nm, cc1_mangle(cc, nm), ft);
    }
    cc1_z80_load_ident(fn, out, s);
    return;
  }
  if (cc1_accept(cc, TOK_LPAREN)) {
    cc1_parse_expr(cc, fn, 1, out);
    cc1_expect(cc, TOK_RPAREN, "expected ')'");
    return;
  }
  cc1_diag_error_at(&cc->diag, cc->tok.loc, "expected expression");
}

static void cc1_parse_postfix(struct cc1_compilation *cc, struct cc1_fn_ctx *fn,
                              struct cc1_expr *out) {
  cc1_parse_primary(cc, fn, out);
  if (cc1_accept(cc, TOK_LPAREN)) {
    /* function call: only direct identifier/function designator for now */
    struct cc1_type *ft = 0;
    int has_proto = 0;
    int slots = 0;
    int argc = 0;
    struct cc1_expr args[32];
    struct cc1_type *param_types[32];
    int paramc = 0;
    int i;

    if (out->type && out->type->kind == TY_FUNC) {
      ft = out->type;
      has_proto = ft->has_proto;
    }

    if (ft && has_proto) {
      struct cc1_param *p = ft->params;
      while (p && paramc < 32) {
        param_types[paramc++] = p->type;
        p = p->next;
      }
    }

    if (!cc1_accept(cc, TOK_RPAREN)) {
      for (;;) {
        if (argc >= 32) {
          cc1_diag_error_at(&cc->diag, cc->tok.loc, "too many args");
          break;
        }
        cc1_parse_expr(cc, fn, 1, &args[argc]);
        argc++;
        if (cc1_accept(cc, TOK_COMMA))
          continue;
        cc1_expect(cc, TOK_RPAREN, "expected ')'");
        break;
      }
    }

    /* push right-to-left */
    for (i = argc - 1; i >= 0; i--) {
      struct cc1_type *pt = 0;
      if (has_proto && i < paramc)
        pt = param_types[i];
      if (pt && cc1_type_is_char8(pt)) {
        cc1_z80_ensure_rvalue_a(cc, fn, &args[i]);
        cc1_z80_emit_push_char_from_a(&fn->cg);
      } else {
        /* default promotions => int */
        cc1_z80_ensure_rvalue_hl(cc, fn, &args[i]);
        cc1_z80_emit_push_hl(&fn->cg);
      }
      slots += 1;
    }

    /* call */
    if (!out->label) {
      cc1_diag_error_at(&cc->diag, cc->tok.loc, "call target not supported");
    } else {
      cc1_z80_emit_call(&fn->cg, out->label);
    }
    cc1_z80_emit_call_cleanup(&fn->cg, slots);

    /* result */
    memset(out, 0, sizeof(*out));
    if (ft) {
      out->type = ft->ret;
    } else {
      out->type = cc->types->t_int;
    }
    if (out->type && cc1_type_is_char8(out->type)) {
      out->in_a = 1;
    } else if (out->type && out->type->kind != TY_VOID) {
      out->in_hl = 1;
    }
  }
}

static void cc1_parse_unary(struct cc1_compilation *cc, struct cc1_fn_ctx *fn,
                            struct cc1_expr *out) {
  /* TODO: support unary '!' (needed for idiomatic `while (*p)` and conditions)
   */
  if (cc->tok.kind == TOK_STAR) {
    struct cc1_expr p;
    struct cc1_loc loc = cc->tok.loc;
    cc1_next(cc);
    cc1_parse_unary(cc, fn, &p);
    cc1_z80_ensure_rvalue_hl(cc, fn, &p);
    if (!p.type || p.type->kind != TY_PTR) {
      cc1_diag_error_at(&cc->diag, loc, "cannot dereference non-pointer");
    }
    memset(out, 0, sizeof(*out));
    out->type =
        p.type && p.type->kind == TY_PTR ? p.type->base : cc->types->t_uchar;
    out->is_lvalue = 1;
    out->addr_in_hl = 1;
    return;
  }
  cc1_parse_postfix(cc, fn, out);
}

static void cc1_z80_ensure_rvalue_a(struct cc1_compilation *cc,
                                    struct cc1_fn_ctx *fn, struct cc1_expr *e) {
  (void)cc;
  if (e->in_a)
    return;

  if (e->type && cc1_type_is_char8(e->type)) {
    if (e->has_ix_off) {
      int off = e->ix_off;
      if (off >= -128 && off <= 127) {
        char buf[64];
        sprintf(buf, "\tld a, (ix%+d)\n", off);
        cc1_emit_text0(&cc->emit, buf);
        e->in_a = 1;
        e->has_ix_off = 0;
        return;
      }
      cc1_z80_emit_load_local_addr_hl(&fn->cg, off);
      cc1_z80_emit_load_u8_from_addr_a(&fn->cg);
      e->in_a = 1;
      e->has_ix_off = 0;
      return;
    }
    if (e->addr_in_hl) {
      cc1_z80_emit_load_u8_from_addr_a(&fn->cg);
      e->in_a = 1;
      e->addr_in_hl = 0;
      return;
    }
  }
  if (e->is_const) {
    cc1_emit_text0(&cc->emit, "\tld a, ");
    {
      char buf[64];
      sprintf(buf, "0x%02lx\n", (unsigned long)(e->ival & 0xff));
      cc1_emit_text0(&cc->emit, buf);
    }
    e->in_a = 1;
    return;
  }
  if (e->in_hl) {
    cc1_emit_text0(&cc->emit, "\tld a, l\n");
    e->in_a = 1;
    return;
  }
}

static void cc1_z80_ensure_rvalue_hl(struct cc1_compilation *cc,
                                     struct cc1_fn_ctx *fn,
                                     struct cc1_expr *e) {
  if (e->in_hl)
    return;

  if (e->has_ix_off) {
    int off = e->ix_off;
    if (e->type && cc1_type_is_char8(e->type)) {
      if (off >= -128 && off <= 127) {
        char buf[64];
        sprintf(buf, "\tld l, (ix%+d)\n", off);
        cc1_emit_text0(&cc->emit, buf);
        cc1_emit_text0(&cc->emit, "\tld h, 0\n");
        e->in_hl = 1;
        e->has_ix_off = 0;
        return;
      }
      cc1_z80_emit_load_local_addr_hl(&fn->cg, off);
      cc1_z80_emit_load_u8_from_addr_a(&fn->cg);
      cc1_emit_text0(&cc->emit, "\tld l, a\n");
      cc1_emit_text0(&cc->emit, "\tld h, 0\n");
      e->in_hl = 1;
      e->has_ix_off = 0;
      return;
    }
    if (off >= -128 && (off + 1) <= 127) {
      char buf[64];
      sprintf(buf, "\tld l, (ix%+d)\n", off);
      cc1_emit_text0(&cc->emit, buf);
      sprintf(buf, "\tld h, (ix%+d)\n", off + 1);
      cc1_emit_text0(&cc->emit, buf);
      e->in_hl = 1;
      e->has_ix_off = 0;
      return;
    }
    cc1_z80_emit_load_local_addr_hl(&fn->cg, off);
    cc1_z80_emit_load_u16_from_addr_hl(&fn->cg);
    e->in_hl = 1;
    e->has_ix_off = 0;
    return;
  }

  if (e->addr_in_hl) {
    if (e->type && cc1_type_is_char8(e->type)) {
      cc1_z80_emit_load_u8_from_addr_a(&fn->cg);
      cc1_emit_text0(&cc->emit, "\tld l, a\n");
      /* H becomes defined as 0 for unsigned promotion */
      cc1_emit_text0(&cc->emit, "\tld h, 0\n");
      e->in_hl = 1;
      e->addr_in_hl = 0;
      return;
    }
    cc1_z80_emit_load_u16_from_addr_hl(&fn->cg);
    e->in_hl = 1;
    e->addr_in_hl = 0;
    return;
  }
  if (e->is_const) {
    if (e->label) {
      cc1_emit_text0(&cc->emit, "\tld hl, ");
      cc1_emit_text0(&cc->emit, e->label);
      cc1_emit_text0(&cc->emit, "\n");
    } else {
      cc1_z80_emit_load_imm16_hl(&fn->cg, (unsigned long)e->ival);
    }
    e->in_hl = 1;
    return;
  }
  if (e->in_a) {
    cc1_emit_text0(&cc->emit, "\tld l, a\n");
    cc1_emit_text0(&cc->emit, "\tld h, 0\n");
    e->in_hl = 1;
    e->in_a = 0;
    return;
  }
}

static void cc1_parse_expr(struct cc1_compilation *cc, struct cc1_fn_ctx *fn,
                           int min_prec, struct cc1_expr *out) {
  struct cc1_expr lhs;
  cc1_parse_unary(cc, fn, &lhs);

  for (;;) {
    int p = cc1_prec(cc->tok.kind);
    enum cc1_tok_kind op = cc->tok.kind;
    struct cc1_expr rhs;
    if (p < min_prec)
      break;

    cc1_next(cc);
    if (op == TOK_ASSIGN) {
      /* only simple lvalue = rvalue; lvalue must be local var */
      cc1_parse_expr(cc, fn, p, &rhs);
      if (!lhs.is_lvalue || (!lhs.has_ix_off && !lhs.addr_in_hl)) {
        cc1_diag_error_at(&cc->diag, cc->tok.loc, "lvalue required");
      }
      if (lhs.type && cc1_type_is_char8(lhs.type)) {
        cc1_z80_ensure_rvalue_a(cc, fn, &rhs);
        if (lhs.has_ix_off && lhs.ix_off >= -128 && lhs.ix_off <= 127) {
          char buf[64];
          sprintf(buf, "\tld (ix%+d), a\n", lhs.ix_off);
          cc1_emit_text0(&cc->emit, buf);
        } else {
          if (lhs.has_ix_off) {
            cc1_z80_emit_load_local_addr_hl(&fn->cg, lhs.ix_off);
            lhs.has_ix_off = 0;
            lhs.addr_in_hl = 1;
          }
          cc1_z80_emit_store_u8_to_addr_from_a(&fn->cg);
        }
        lhs.in_a = 1;
        lhs.has_ix_off = 0;
        lhs.addr_in_hl = 0;
      } else {
        cc1_z80_ensure_rvalue_hl(cc, fn, &rhs);
        if (lhs.has_ix_off && lhs.ix_off >= -128 && (lhs.ix_off + 1) <= 127) {
          char buf[64];
          sprintf(buf, "\tld (ix%+d), l\n", lhs.ix_off);
          cc1_emit_text0(&cc->emit, buf);
          sprintf(buf, "\tld (ix%+d), h\n", lhs.ix_off + 1);
          cc1_emit_text0(&cc->emit, buf);
        } else {
          if (lhs.has_ix_off) {
            cc1_z80_emit_load_local_addr_hl(&fn->cg, lhs.ix_off);
            lhs.has_ix_off = 0;
            lhs.addr_in_hl = 1;
          }
          cc1_z80_emit_store_u16_to_addr_from_hl(&fn->cg);
        }
        lhs.in_hl = 1;
        lhs.has_ix_off = 0;
        lhs.addr_in_hl = 0;
      }
      lhs.is_lvalue = 0;
      continue;
    }

    cc1_parse_expr(cc, fn, p + 1, &rhs);

    /* For bring-up: only handle + for ints */
    if (op == TOK_PLUS) {
      /* TODO: pointer arithmetic scaling (add/sub by sizeof(pointee) for ptr
       * +/- int) */
      cc1_z80_ensure_rvalue_hl(cc, fn, &lhs);
      cc1_z80_emit_push_hl(&fn->cg);
      cc1_z80_ensure_rvalue_hl(cc, fn, &rhs);
      cc1_emit_text0(&cc->emit, "\tpop de\n");
      cc1_emit_text0(&cc->emit, "\tadd hl, de\n");
      lhs.in_hl = 1;
      lhs.type = cc->types->t_int;
      continue;
    }

    if (op == TOK_EQ) {
      char lbuf[32];
      const char *lend;
      cc1_z80_ensure_rvalue_hl(cc, fn, &lhs);
      cc1_z80_emit_push_hl(&fn->cg);
      cc1_z80_ensure_rvalue_hl(cc, fn, &rhs);
      cc1_emit_text0(&cc->emit, "\tpop de\n");
      cc1_emit_text0(&cc->emit, "\txor a\n");
      cc1_emit_text0(&cc->emit, "\tsbc hl, de\n");
      cc1_emit_text0(&cc->emit, "\tld hl, 0\n");
      cc1_emit_new_local_label(&cc->emit, lbuf, sizeof(lbuf));
      lend = cc1_strpool_intern(cc->sp, lbuf, (unsigned long)strlen(lbuf));
      cc1_emit_text0(&cc->emit, "\tjp nz, ");
      cc1_emit_text0(&cc->emit, lend);
      cc1_emit_text0(&cc->emit, "\n");
      cc1_emit_text0(&cc->emit, "\tinc l\n");
      cc1_emit_text0(&cc->emit, lend);
      cc1_emit_text0(&cc->emit, ":\n");
      lhs.in_hl = 1;
      lhs.in_a = 0;
      lhs.addr_in_hl = 0;
      lhs.has_ix_off = 0;
      lhs.is_lvalue = 0;
      lhs.type = cc->types->t_int;
      continue;
    }

    if (op == TOK_NE) {
      char lbuf[32];
      const char *lend;
      cc1_z80_ensure_rvalue_hl(cc, fn, &lhs);
      cc1_z80_emit_push_hl(&fn->cg);
      cc1_z80_ensure_rvalue_hl(cc, fn, &rhs);
      cc1_emit_text0(&cc->emit, "\tpop de\n");
      cc1_emit_text0(&cc->emit, "\txor a\n");
      cc1_emit_text0(&cc->emit, "\tsbc hl, de\n");
      cc1_emit_text0(&cc->emit, "\tld hl, 0\n");
      cc1_emit_new_local_label(&cc->emit, lbuf, sizeof(lbuf));
      lend = cc1_strpool_intern(cc->sp, lbuf, (unsigned long)strlen(lbuf));
      cc1_emit_text0(&cc->emit, "\tjp z, ");
      cc1_emit_text0(&cc->emit, lend);
      cc1_emit_text0(&cc->emit, "\n");
      cc1_emit_text0(&cc->emit, "\tinc l\n");
      cc1_emit_text0(&cc->emit, lend);
      cc1_emit_text0(&cc->emit, ":\n");
      lhs.in_hl = 1;
      lhs.in_a = 0;
      lhs.addr_in_hl = 0;
      lhs.has_ix_off = 0;
      lhs.is_lvalue = 0;
      lhs.type = cc->types->t_int;
      continue;
    }

    /* otherwise: just keep rhs for now */
    lhs = rhs;
  }

  *out = lhs;
}

static void cc1_parse_statement(struct cc1_compilation *cc,
                                struct cc1_fn_ctx *fn);

static void cc1_emit_test_hl_jz(struct cc1_compilation *cc, const char *label) {
  cc1_emit_text0(&cc->emit, "\tld a, h\n");
  cc1_emit_text0(&cc->emit, "\tor l\n");
  cc1_emit_text0(&cc->emit, "\tjp z, ");
  cc1_emit_text0(&cc->emit, label);
  cc1_emit_text0(&cc->emit, "\n");
}

static void cc1_parse_return(struct cc1_compilation *cc,
                             struct cc1_fn_ctx *fn) {
  if (cc1_accept(cc, TOK_SEMI)) {
    cc1_z80_emit_return_void(&fn->cg);
    return;
  }
  {
    struct cc1_expr e;
    cc1_parse_expr(cc, fn, 1, &e);
    cc1_expect(cc, TOK_SEMI, "expected ';'");
    if (fn->fn->type && fn->fn->type->ret &&
        cc1_type_is_char8(fn->fn->type->ret)) {
      cc1_z80_ensure_rvalue_a(cc, fn, &e);
      cc1_z80_emit_return_a(&fn->cg);
    } else if (fn->fn->type && fn->fn->type->ret &&
               fn->fn->type->ret->kind == TY_VOID) {
      cc1_z80_emit_return_void(&fn->cg);
    } else {
      cc1_z80_ensure_rvalue_hl(cc, fn, &e);
      cc1_z80_emit_return_hl(&fn->cg);
    }
  }
}

static void cc1_parse_compound(struct cc1_compilation *cc,
                               struct cc1_fn_ctx *fn) {
  cc1_expect(cc, TOK_LBRACE, "expected '{'");
  cc1_symtab_push(cc->sym);
  while (cc->tok.kind != TOK_RBRACE && cc->tok.kind != TOK_EOF) {
    if (cc1_is_decl_start(cc)) {
      /* local declaration (no init for now) */
      int is_td = 0;
      int is_ex = 0;
      struct cc1_type *bt = cc1_parse_declspec(cc, &is_td, &is_ex);
      struct cc1_decl d;
      struct cc1_sym *s;
      memset(&d, 0, sizeof(d));
      bt = cc1_parse_declarator(cc, bt, &d);
      if (is_td) {
        if (cc->tok.kind != TOK_SEMI) {
          cc1_diag_error_at(&cc->diag, cc->tok.loc, "expected ';'");
        } else {
          /* must update typedef set before lexing next token */
          cc1_sym_add(cc->sym, SYM_TYPEDEF, d.name, d.name, bt);
          cc1_next(cc);
        }
      } else {
        unsigned long sz;
        unsigned long slot;
        sz = cc1_type_sizeof(bt);
        /* keep stack 2-aligned; store 8-bit locals in 2-byte slots */
        slot = (sz <= 2) ? 2 : sz;
        fn->next_local_off -= (int)slot;
        cc1_z80_emit_alloc_stack(&fn->cg, slot);
        s = cc1_sym_add(cc->sym, SYM_VAR, d.name, d.name, bt);
        s->stack_offset = fn->next_local_off;

        if (cc1_accept(cc, TOK_ASSIGN)) {
          struct cc1_expr rhs;
          cc1_parse_expr(cc, fn, 1, &rhs);
          cc1_expect(cc, TOK_SEMI, "expected ';'");

          if (bt && cc1_type_is_char8(bt)) {
            cc1_z80_ensure_rvalue_a(cc, fn, &rhs);
            if (s->stack_offset >= -128 && s->stack_offset <= 127) {
              char buf[64];
              sprintf(buf, "\tld (ix%+d), a\n", s->stack_offset);
              cc1_emit_text0(&cc->emit, buf);
            } else {
              cc1_z80_emit_load_local_addr_hl(&fn->cg, s->stack_offset);
              cc1_z80_emit_store_u8_to_addr_from_a(&fn->cg);
            }
          } else {
            cc1_z80_ensure_rvalue_hl(cc, fn, &rhs);
            if (s->stack_offset >= -128 && (s->stack_offset + 1) <= 127) {
              char buf[64];
              sprintf(buf, "\tld (ix%+d), l\n", s->stack_offset);
              cc1_emit_text0(&cc->emit, buf);
              sprintf(buf, "\tld (ix%+d), h\n", s->stack_offset + 1);
              cc1_emit_text0(&cc->emit, buf);
            } else {
              cc1_z80_emit_load_local_addr_hl(&fn->cg, s->stack_offset);
              cc1_z80_emit_store_u16_to_addr_from_hl(&fn->cg);
            }
          }
        } else {
          cc1_expect(cc, TOK_SEMI, "expected ';'");
        }
        (void)is_ex;
      }
      continue;
    }
    cc1_parse_statement(cc, fn);
  }
  cc1_expect(cc, TOK_RBRACE, "expected '}'");
  cc1_symtab_pop(cc->sym);
}

static void cc1_parse_expr_stmt(struct cc1_compilation *cc,
                                struct cc1_fn_ctx *fn) {
  struct cc1_expr e;
  if (cc1_accept(cc, TOK_SEMI))
    return;
  cc1_parse_expr(cc, fn, 1, &e);
  cc1_expect(cc, TOK_SEMI, "expected ';'");
  (void)e;
}

static void cc1_parse_statement(struct cc1_compilation *cc,
                                struct cc1_fn_ctx *fn) {
  if (cc->tok.kind == TOK_LBRACE) {
    cc1_parse_compound(cc, fn);
    return;
  }
  if (cc1_accept(cc, TOK_KW_WHILE)) {
    /* TODO: implement break/continue with a loop context stack */
    struct cc1_expr cond;
    char lbuf1[32];
    char lbuf2[32];
    const char *lcond;
    const char *lend;

    cc1_expect(cc, TOK_LPAREN, "expected '('");
    cc1_emit_new_local_label(&cc->emit, lbuf1, sizeof(lbuf1));
    cc1_emit_new_local_label(&cc->emit, lbuf2, sizeof(lbuf2));
    lcond = cc1_strpool_intern(cc->sp, lbuf1, (unsigned long)strlen(lbuf1));
    lend = cc1_strpool_intern(cc->sp, lbuf2, (unsigned long)strlen(lbuf2));

    cc1_emit_text0(&cc->emit, lcond);
    cc1_emit_text0(&cc->emit, ":\n");
    cc1_parse_expr(cc, fn, 1, &cond);
    cc1_expect(cc, TOK_RPAREN, "expected ')'");
    cc1_z80_ensure_rvalue_hl(cc, fn, &cond);
    cc1_emit_test_hl_jz(cc, lend);

    cc1_parse_statement(cc, fn);
    cc1_emit_text0(&cc->emit, "\tjp ");
    cc1_emit_text0(&cc->emit, lcond);
    cc1_emit_text0(&cc->emit, "\n");
    cc1_emit_text0(&cc->emit, lend);
    cc1_emit_text0(&cc->emit, ":\n");
    return;
  }
  if (cc1_accept(cc, TOK_KW_IF)) {
    struct cc1_expr cond;
    char lbuf1[32];
    char lbuf2[32];
    const char *lelse;
    const char *lend;

    cc1_expect(cc, TOK_LPAREN, "expected '('");
    cc1_parse_expr(cc, fn, 1, &cond);
    cc1_expect(cc, TOK_RPAREN, "expected ')'");

    cc1_z80_ensure_rvalue_hl(cc, fn, &cond);
    cc1_emit_new_local_label(&cc->emit, lbuf1, sizeof(lbuf1));
    cc1_emit_new_local_label(&cc->emit, lbuf2, sizeof(lbuf2));
    lelse = cc1_strpool_intern(cc->sp, lbuf1, (unsigned long)strlen(lbuf1));
    lend = cc1_strpool_intern(cc->sp, lbuf2, (unsigned long)strlen(lbuf2));
    cc1_emit_test_hl_jz(cc, lelse);

    cc1_parse_statement(cc, fn);
    cc1_emit_text0(&cc->emit, "\tjp ");
    cc1_emit_text0(&cc->emit, lend);
    cc1_emit_text0(&cc->emit, "\n");
    cc1_emit_text0(&cc->emit, lelse);
    cc1_emit_text0(&cc->emit, ":\n");
    if (cc1_accept(cc, TOK_KW_ELSE)) {
      cc1_parse_statement(cc, fn);
    }
    cc1_emit_text0(&cc->emit, lend);
    cc1_emit_text0(&cc->emit, ":\n");
    return;
  }
  if (cc1_accept(cc, TOK_KW_RETURN)) {
    cc1_parse_return(cc, fn);
    return;
  }
  cc1_parse_expr_stmt(cc, fn);
}

static void cc1_define_function(struct cc1_compilation *cc,
                                struct cc1_sym *fn_sym) {
  struct cc1_fn_ctx fn;
  struct cc1_param *p;
  int off;
  char lbuf[32];
  const char *exit_lab;

  memset(&fn, 0, sizeof(fn));
  fn.fn = fn_sym;
  fn.next_local_off = 0;

  cc1_symtab_push(cc->sym);

  /* assign parameter offsets */
  off = 4;
  p = fn_sym->type ? fn_sym->type->params : 0;
  while (p) {
    if (p->name) {
      struct cc1_sym *ps =
          cc1_sym_add(cc->sym, SYM_VAR, p->name, p->name, p->type);
      ps->stack_offset = off;
    }
    off += 2;
    p = p->next;
  }

  cc1_emit_new_local_label(&cc->emit, lbuf, sizeof(lbuf));
  exit_lab = cc1_strpool_intern(cc->sp, lbuf, (unsigned long)strlen(lbuf));
  cc1_z80_cg_init(&fn.cg, &cc->emit, exit_lab);
  cc1_z80_emit_fn_prologue(&fn.cg, fn_sym->asm_name);
  cc1_parse_compound(cc, &fn);
  cc1_z80_emit_fn_epilogue(&fn.cg);

  cc1_symtab_pop(cc->sym);
}

static void cc1_parse_external_decl(struct cc1_compilation *cc) {
  int is_td = 0;
  int is_ex = 0;
  struct cc1_type *bt = cc1_parse_declspec(cc, &is_td, &is_ex);
  struct cc1_decl d;
  struct cc1_sym *s;

  memset(&d, 0, sizeof(d));
  bt = cc1_parse_declarator(cc, bt, &d);
  if (!d.name)
    return;

  if (is_td) {
    if (cc->tok.kind != TOK_SEMI) {
      cc1_diag_error_at(&cc->diag, cc->tok.loc, "expected ';'");
    } else {
      /* must update typedef set before lexing next token */
      cc1_sym_add(cc->sym, SYM_TYPEDEF, d.name, d.name, bt);
      cc1_next(cc);
    }
    return;
  }

  if (bt && bt->kind == TY_FUNC && cc->tok.kind == TOK_LBRACE) {
    s = cc1_sym_lookup_current(cc->sym, d.name);
    if (!s) {
      s = cc1_sym_add(cc->sym, SYM_FUNC, d.name, cc1_mangle(cc, d.name), bt);
    }
    s->is_defined = 1;
    /* emit global */
    cc1_emit_text0(&cc->emit, "\t.globl ");
    cc1_emit_text0(&cc->emit, s->asm_name);
    cc1_emit_text0(&cc->emit, "\n");
    cc1_emit_text0(&cc->emit, "\t.type ");
    cc1_emit_text0(&cc->emit, s->asm_name);
    cc1_emit_text0(&cc->emit, ", @function\n");
    cc1_emit_text0(&cc->emit, s->asm_name);
    cc1_emit_text0(&cc->emit, ":\n");
    cc1_define_function(cc, s);
    cc1_emit_text0(&cc->emit, "\t.size ");
    cc1_emit_text0(&cc->emit, s->asm_name);
    cc1_emit_text0(&cc->emit, ", .- ");
    cc1_emit_text0(&cc->emit, s->asm_name);
    cc1_emit_text0(&cc->emit, "\n");
    return;
  }

  cc1_expect(cc, TOK_SEMI, "expected ';'");
  s = cc1_sym_lookup_current(cc->sym, d.name);
  if (!s) {
    enum cc1_sym_kind k = (bt && bt->kind == TY_FUNC) ? SYM_FUNC : SYM_VAR;
    s = cc1_sym_add(cc->sym, k, d.name, cc1_mangle(cc, d.name), bt);
  }
  s->is_extern = is_ex;
}

int cc1_parse_translation_unit(struct cc1_compilation *cc) {
  cc1_next(cc);
  while (cc->tok.kind != TOK_EOF && !cc->diag.had_error) {
    if (!cc1_is_decl_start(cc)) {
      cc1_diag_error_at(&cc->diag, cc->tok.loc, "expected declaration");
      break;
    }
    cc1_parse_external_decl(cc);
  }
  return cc->diag.had_error ? 0 : 1;
}
