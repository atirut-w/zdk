#include "string_pool.h"
#include <stdlib.h>
#include <string.h>

static unsigned int hash(const char *str, size_t len) {
  unsigned int hash = 5381;
  for (size_t i = 0; i < len; i++) {
    hash = ((hash << 5) + hash) + (unsigned char)str[i]; /* hash * 33 + c */
  }
  return hash;
}

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

const char *string_pool_intern_len(StringPool *pool, const char *str,
                                   size_t len) {
  unsigned int index = hash(str, len) % STRING_POOL_MAX_SIZE;
  StringPoolNode *node = pool->slots[index];

  if (node) {
    while (node) {
      if (strlen(node->string) == len && strncmp(node->string, str, len) == 0) {
        return node->string;
      }
      node = node->next;
    }
  } else {
    node = malloc(sizeof(StringPoolNode));
    if (!node) {
      return NULL;
    }

    node->string = malloc(len + 1);
    if (!node->string) {
      free(node);
      return NULL;
    }

    strncpy(node->string, str, len);
    node->string[len] = '\0';
    node->next = NULL;
    pool->slots[index] = node;
    return node->string;
  }
}

const char *string_pool_intern(StringPool *pool, const char *str) {
  return string_pool_intern_len(pool, str, strlen(str));
}
