#ifndef STRING_POOL_H
#define STRING_POOL_H

/* TODO: Reconsider size */
#define STRING_POOL_MAX_SIZE 128

typedef struct {
  char *string;
  struct StringPoolNode *next;
} StringPoolNode;

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

#endif /* STRING_POOL_H */
