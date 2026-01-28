#include "string_pool.h"
#include <stdlib.h>
#include <string.h>

static unsigned int hash(const char *str, size_t len) {
  unsigned int hash = 5381;
  size_t i;
  for (i = 0; i < len; i++) {
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
    size_t i;
    for (i = 0; i < STRING_POOL_MAX_SIZE; i++) {
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
  char *interned_str;
  StringPoolNode *new_node;

  while (node) {
    if (strlen(node->string) == len && strncmp(node->string, str, len) == 0) {
      return node->string;
    }
    node = node->next;
  }

  interned_str = (char *)malloc(len + 1);
  if (!interned_str) {
    return NULL;
  }
  strncpy(interned_str, str, len);
  interned_str[len] = '\0';

  new_node = (StringPoolNode *)malloc(sizeof(StringPoolNode));
  if (!new_node) {
    free(interned_str);
    return NULL;
  }

  new_node->string = interned_str;
  new_node->next = pool->slots[index];
  pool->slots[index] = new_node;
  return interned_str;
}

const char *string_pool_intern(StringPool *pool, const char *str) {
  return string_pool_intern_len(pool, str, strlen(str));
}
