#ifndef CC1_SYM_H
#define CC1_SYM_H

struct cc1_type;
struct cc1_strpool;

enum cc1_sym_kind { SYM_VAR, SYM_FUNC, SYM_TYPEDEF };

struct cc1_sym {
  enum cc1_sym_kind kind;
  const char *name;     /* source name (interned) */
  const char *asm_name; /* mangled, interned */
  struct cc1_type *type;

  int is_extern;
  int is_defined;

  int stack_offset; /* for locals/params: ix+off (positive for params, negative
                       for locals) */

  struct cc1_sym *next;
};

struct cc1_scope {
  struct cc1_sym *syms;
  struct cc1_scope *prev;
};

struct cc1_symtab {
  struct cc1_strpool *sp;
  struct cc1_scope *top;
};

struct cc1_symtab *cc1_symtab_new(struct cc1_strpool *sp);
void cc1_symtab_free(struct cc1_symtab *st);

void cc1_symtab_push(struct cc1_symtab *st);
void cc1_symtab_pop(struct cc1_symtab *st);

struct cc1_sym *cc1_sym_lookup(struct cc1_symtab *st, const char *name);
struct cc1_sym *cc1_sym_lookup_current(struct cc1_symtab *st, const char *name);

int cc1_sym_is_typedef_visible(struct cc1_symtab *st, const char *name);

struct cc1_sym *cc1_sym_add(struct cc1_symtab *st, enum cc1_sym_kind kind,
                            const char *name, const char *asm_name,
                            struct cc1_type *type);

#endif
