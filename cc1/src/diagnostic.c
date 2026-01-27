#include "diagnostic.h"
#include <stdlib.h>

const char *diagnostic_level_to_string(DiagnosticLevel level) {
  switch (level) {
  case DIAG_ERROR:
    return "error";
  default:
    abort(); /* Bug at the call site! */
  }
}

Diagnostic *diagnostic_new(DiagnosticLevel level, const char *message,
                           unsigned int line, unsigned int column) {
  Diagnostic *diag = malloc(sizeof(Diagnostic));
  if (!diag) {
    return NULL;
  }

  diag->level = level;
  diag->message = message;
  diag->line = line;
  diag->column = column;
  
  return diag;
}

void diagnostic_free(Diagnostic *diag) {
  if (diag) {
    free(diag);
  }
}
