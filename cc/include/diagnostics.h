#ifndef CC_INCLUDE_DIAGNOSTICS_H
#define CC_INCLUDE_DIAGNOSTICS_H

#include <stddef.h>

typedef enum { DIAG_LEVEL_ERROR } DiagnosticLevel;

typedef struct {
  const char *filename;
  const char *message;
  int line;
  int column;
  DiagnosticLevel level;
} Diagnostic;

typedef struct {
  Diagnostic *diagnostics;
  size_t count;
  size_t capacity;
} DiagnosticList;

const char *diagnostic_level_to_string(DiagnosticLevel level);

void diagnostics_set_file(const char *filename);
DiagnosticList diagnostics_get();
void diagnostics_add(const char *message, int line, int column,
                     DiagnosticLevel level);

#endif
