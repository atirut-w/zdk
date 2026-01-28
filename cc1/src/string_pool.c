#include "string_pool.h"
#include <string.h>
#include <stdlib.h>

StringPool *string_pool_new(void) {
  StringPool *pool = (StringPool *)malloc(sizeof(StringPool));
  if (!pool) {
    return NULL;
  }

  memset(pool->slots, 0, sizeof(pool->slots));

  return pool;
}

void string_pool_free(StringPool *pool) {
  if (pool) {
    for (size_t i = 0; i < STRING_POOL_MAX_SIZE; i++) {
      StringPoolNode *node = pool->slots[i];
      while (node) {
        StringPoolNode *next = node->next;
        free(node->string);
        free(node);
        node = next;
      }
    }
    free(pool);
  }
}
