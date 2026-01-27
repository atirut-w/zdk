#include "lexer.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *input;
  CompilationCtx *ctx;
  Lexer *lexer;
  Token token;

  if (argc < 2) {
    printf("Usage: cc1 <source-file>\n");
    return 1;
  }

  input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "Could not open '%s': %s\n", argv[1], strerror(errno));
    return 1;
  }

  ctx = compilation_ctx_new();
  lexer = lexer_new(ctx, input);

  while (lexer_next_token(lexer, &token)) {
    printf("Token: kind=%d, lexeme='%s', line=%u, column=%u\n", token.kind,
           token.lexeme ? token.lexeme : "", token.line, token.column);
  }

  fclose(input);
  return 0;
}
