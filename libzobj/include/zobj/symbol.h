#ifndef ZOBJ_SYMBOL_H
#define ZOBJ_SYMBOL_H

#include <stdio.h>

typedef struct {
  union {
    const char *name;
    unsigned short strx;
  } un;
  unsigned char type;
  unsigned char other;
  unsigned short desc;
  unsigned short value;
} ZObjSymbol;

enum {
  ZOBJ_SYM_UNDF = (1 << 0),
  ZOBJ_SYM_ABS = (1 << 1),
  ZOBJ_SYM_TEXT = (1 << 2),
  ZOBJ_SYM_DATA = (1 << 3),
  ZOBJ_SYM_BSS = (1 << 4),
  ZOBJ_SYM_FN = (1 << 5),
  ZOBJ_SYM_EXT = (1 << 6)
};

int zobj_write_symbol(FILE *f, const ZObjSymbol *sym);

#endif /* ZOBJ_SYMBOL_H */
