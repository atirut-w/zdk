#ifndef COMPILATION_CTX_H
#define COMPILATION_CTX_H

#include "queue.h"
#include "string_pool.h"

/**
 * TODO: Document.
 */
typedef struct {
  Queue *diagnostics;
  StringPool *string_pool;
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
