#include "compilation_ctx.h"
#include <stdlib.h>

CompilationCtx *compilation_ctx_new(void) {
  CompilationCtx *ctx = (CompilationCtx *)malloc(sizeof(CompilationCtx));
  if (!ctx) {
    return NULL;
  }

  ctx->diagnostics = queue_new();
  if (!ctx->diagnostics) {
    free(ctx);
    return NULL;
  }

  return ctx;
}

void compilation_ctx_free(CompilationCtx *ctx) {
  if (ctx) {
    queue_free(ctx->diagnostics);
    free(ctx);
  }
}
