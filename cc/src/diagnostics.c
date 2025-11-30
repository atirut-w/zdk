#include <diagnostics.h>
#include <stdlib.h>
#include <string.h>

static DiagnosticList list = {NULL, 0, 0};
static const char *current_filename = NULL;

const char *diagnostic_level_to_string(DiagnosticLevel level) {
  switch (level) {
  case DIAG_LEVEL_ERROR:
    return "error";
  default:
    return "unknown";
  }
}

void diagnostics_set_file(const char *filename) {
  current_filename = realloc((char *)current_filename, strlen(filename) + 1);
  strcpy((char *)current_filename, filename);
}

DiagnosticList diagnostics_get() { return list; }

void diagnostics_add(const char *message, int line, int column,
                     DiagnosticLevel level) {
  Diagnostic diag;
  diag.filename = malloc(strlen(current_filename) + 1);
  strcpy((char *)diag.filename, current_filename);
  diag.message = malloc(strlen(message) + 1);
  strcpy((char *)diag.message, message);
  diag.line = line;
  diag.column = column;
  diag.level = level;

  if (list.count >= list.capacity) {
    list.capacity = list.capacity == 0 ? 4 : list.capacity * 2;
    list.diagnostics =
        realloc(list.diagnostics, list.capacity * sizeof(Diagnostic));
  }
  list.diagnostics[list.count++] = diag;
}
