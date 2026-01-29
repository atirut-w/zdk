#ifndef CC1_DIAG_H
#define CC1_DIAG_H

/* C90-friendly diagnostics */

struct cc1_loc {
  const char *path;
  unsigned long line;
  unsigned long col;
};

struct cc1_diag {
  int had_error;
};

void cc1_diag_init(struct cc1_diag *d);

void cc1_diag_warn_at(struct cc1_diag *d, struct cc1_loc loc, const char *msg);
void cc1_diag_warn_atf(struct cc1_diag *d, struct cc1_loc loc, const char *fmt,
                       const char *a);

void cc1_diag_error_at(struct cc1_diag *d, struct cc1_loc loc, const char *msg);
void cc1_diag_error_atf(struct cc1_diag *d, struct cc1_loc loc, const char *fmt,
                        const char *a);
void cc1_diag_error_atf2(struct cc1_diag *d, struct cc1_loc loc,
                         const char *fmt, const char *a, const char *b);

#endif
