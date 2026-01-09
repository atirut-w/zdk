#ifndef SYMREF_H
#define SYMREF_H

#include "section.h"
#include "symbol.h"
#include <stdint.h>

typedef struct SymRef {
  /* The section this reference originates from */
  Section *section;
  /* The offset within the section */
  uint16_t offset;
  /* The symbol being referenced */
  Symbol *symbol;
} SymRef;

#endif /* SYMREF_H */
