#include "cc1/diag.h"

#include <stdio.h>

void cc1_diag_init(struct cc1_diag *d) { d->had_error = 0; }

void cc1_diag_warn_at(struct cc1_diag *d, struct cc1_loc loc, const char *msg) {
  (void)d;
  fprintf(stderr, "%s:%lu:%lu: warning: %s\n", loc.path ? loc.path : "<input>",
          loc.line, loc.col, msg);
}

void cc1_diag_warn_atf(struct cc1_diag *d, struct cc1_loc loc, const char *fmt,
                       const char *a) {
  (void)d;
  fprintf(stderr, "%s:%lu:%lu: warning: ", loc.path ? loc.path : "<input>",
          loc.line, loc.col);
  fprintf(stderr, fmt, a);
  fprintf(stderr, "\n");
}

void cc1_diag_error_at(struct cc1_diag *d, struct cc1_loc loc,
                       const char *msg) {
  d->had_error = 1;
  fprintf(stderr, "%s:%lu:%lu: error: %s\n", loc.path ? loc.path : "<input>",
          loc.line, loc.col, msg);
}

void cc1_diag_error_atf(struct cc1_diag *d, struct cc1_loc loc, const char *fmt,
                        const char *a) {
  d->had_error = 1;
  fprintf(stderr, "%s:%lu:%lu: error: ", loc.path ? loc.path : "<input>",
          loc.line, loc.col);
  fprintf(stderr, fmt, a);
  fprintf(stderr, "\n");
}

void cc1_diag_error_atf2(struct cc1_diag *d, struct cc1_loc loc,
                         const char *fmt, const char *a, const char *b) {
  d->had_error = 1;
  fprintf(stderr, "%s:%lu:%lu: error: ", loc.path ? loc.path : "<input>",
          loc.line, loc.col);
  fprintf(stderr, fmt, a, b);
  fprintf(stderr, "\n");
}
