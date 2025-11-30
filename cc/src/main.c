#include <diagnostics.h>
#include <lexer.h>
#include <parser.h>

int main(int argc, char *argv[]) {
  int result;
  DiagnosticList diags;
  size_t i;

  diagnostics_set_file("<stdin>");
  yyin = stdin;
  yyout = stdout;
  result = yyparse();

  diags = diagnostics_get();
  for (i = 0; i < diags.count; i++) {
    Diagnostic diag = diags.diagnostics[i];
    result |= diag.level == DIAG_LEVEL_ERROR ? 1 : 0;
    fprintf(stderr, "%s:%d:%d: %s: %s\n", diag.filename, diag.line, diag.column,
            diagnostic_level_to_string(diag.level), diag.message);
  }

  return result;
}
