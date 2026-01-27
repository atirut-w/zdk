#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

typedef enum { DIAG_ERROR } DiagnosticLevel;

const char *diagnostic_level_to_string(DiagnosticLevel level);

typedef struct {
  DiagnosticLevel level;
  /* NOTE: Unowned pointer! */
  const char *message;
  unsigned int line;
  unsigned int column;
} Diagnostic;

/**
 * Allocates and initializes a new diagnostic.
 */
Diagnostic *diagnostic_new(DiagnosticLevel level, const char *message,
                           unsigned int line, unsigned int column);

/**
 * Frees a diagnostic.
 */
void diagnostic_free(Diagnostic *diag);

#endif /* DIAGNOSTIC_H */
