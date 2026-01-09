#include "symbol.h"
#include <stdlib.h>
#include <string.h>

Symbol *symbol_new(const char *name, int type, uint16_t value) {
  Symbol *symbol = malloc(sizeof(Symbol));

  symbol->name = malloc(strlen(name) + 1);
  strcpy((char *)symbol->name, name);
  symbol->type = type;
  symbol->value = value;

  return symbol;
}

void symbol_free(Symbol *symbol) {
  if (symbol) {
    free((char *)symbol->name);
    free(symbol);
  }
}
