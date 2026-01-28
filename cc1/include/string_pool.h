#ifndef STRING_POOL_H
#define STRING_POOL_H

#include <stddef.h>

/* TODO: Reconsider size */
#define STRING_POOL_MAX_SIZE 128

typedef struct StringPoolNode {
  char *string;
  struct StringPoolNode *next;
} StringPoolNode;

/**
 * A string pool for interning strings.
 */
typedef struct {
  StringPoolNode *slots[STRING_POOL_MAX_SIZE];
} StringPool;

/**
 * Allocates and initializes a new string pool.
 */
StringPool *string_pool_new(void);

/**
 * Frees a string pool and its strings.
 */
void string_pool_free(StringPool *pool);

/**
 * Interns a string of length len into the string pool. Returns a pointer to the
 * interned string.
 */
const char *string_pool_intern_len(StringPool *pool, const char *str,
                                   size_t len);

/**
 * Interns a null-terminated string into the string pool. Returns a pointer to
 * the interned string.
 */
const char *string_pool_intern(StringPool *pool, const char *str);

#endif /* STRING_POOL_H */
