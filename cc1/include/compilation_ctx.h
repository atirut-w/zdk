#ifndef COMPILATION_CTX_H
#define COMPILATION_CTX_H

/**
 * TODO: Document.
 */
typedef struct {
  char dummy; /* This shuts up errors */
} CompilationCtx;

/**
 * Allocates and initializes a new compilation context.
 */
CompilationCtx *compilation_ctx_new(void);

/**
 * Frees a compilation context.
 */
void compilation_ctx_free(CompilationCtx *ctx);

#endif /* COMPILATION_CTX_H */
