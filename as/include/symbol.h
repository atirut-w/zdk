#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>

enum {
  SYMBOL_UNDEFINED,
  SYMBOL_TEXT,
  SYMBOL_DATA,
  SYMBOL_BSS
};

typedef struct {
  const char *name;
  int type;
  uint16_t value;
} Symbol;

Symbol *symbol_new(const char *name, int type, uint16_t value);
void symbol_free(Symbol *symbol);

#endif /* SYMBOL_H */
