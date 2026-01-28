#include "codegen_driver.h"
#include "diagnostic.h"
#include "lexer.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *input, *output;
  CompilationCtx *ctx;
  Lexer *lexer;
  CodeGen *codegen;
  CodeGenDriver *driver;
  Diagnostic *diag;

  if (argc < 3) {
    printf("Usage: cc1 <source-file> <output-file>\n");
    return 1;
  }

  input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "Could not open '%s': %s\n", argv[1], strerror(errno));
    return 1;
  }

  ctx = compilation_ctx_new();
  lexer = lexer_new(ctx, input);
  output = fopen(argv[2], "w");
  if (!output) {
    fprintf(stderr, "Could not open '%s': %s\n", argv[2], strerror(errno));
    return 1;
  }
  codegen = codegen_new(output);
  driver = codegen_driver_new(ctx, lexer, codegen);

  codegen_driver_compile(driver);

  while ((diag = (Diagnostic *)queue_dequeue(ctx->diagnostics)) != NULL) {
    printf("%s:%u:%u: %s: %s\n", argv[1], diag->line, diag->column,
           diagnostic_level_to_string(diag->level), diag->message);
    diagnostic_free(diag);
  }

  codegen_driver_free(driver);
  lexer_free(lexer);
  compilation_ctx_free(ctx);
  codegen_free(codegen);
  fclose(output);
  fclose(input);
  return 0;
}
