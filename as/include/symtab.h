#ifndef SYMTAB_H
#define SYMTAB_H

#include "symbol.h"
#include <stddef.h>

typedef struct {
  Symbol **symbols;
  size_t count;
  size_t capacity;
} SymTab;

extern SymTab symtab;

int symtab_append(Symbol *symbol);
Symbol *symtab_find(const char *name);

#endif /* SYMTAB_H */
