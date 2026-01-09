#include "symtab.h"
#include <stdlib.h>
#include <string.h>

SymTab symtab = {NULL, 0, 0};

int symtab_append(Symbol *symbol) {
  if (symtab_find(symbol->name)) {
    return -1; /* Symbol already exists */
  }

  if (symtab.count >= symtab.capacity) {
    size_t new_capacity = symtab.capacity == 0 ? 4 : symtab.capacity * 2;
    Symbol **new_symbols =
        realloc(symtab.symbols, new_capacity * sizeof(Symbol *));

    if (!new_symbols) {
      return -1; /* Memory allocation failed */
    }
    symtab.symbols = new_symbols;
    symtab.capacity = new_capacity;
  }

  symtab.symbols[symtab.count++] = symbol;
  return 0;
}

Symbol *symtab_find(const char *name) {
  size_t i;

  for (i = 0; i < symtab.count; i++) {
    if (strcmp(symtab.symbols[i]->name, name) == 0) {
      return symtab.symbols[i];
    }
  }

  return NULL;
}
