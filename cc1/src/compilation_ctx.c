#include "compilation_ctx.h"
#include <stdlib.h>

CompilationCtx *compilation_ctx_new(void) {
  CompilationCtx *ctx = (CompilationCtx *)malloc(sizeof(CompilationCtx));
  return ctx;
}

void compilation_ctx_free(CompilationCtx *ctx) {
  if (ctx) {
    free(ctx);
  }
}
