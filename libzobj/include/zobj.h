#ifndef ZOBJ_H
#define ZOBJ_H

#include <stdint.h>

typedef struct {
  uint16_t magic;
  uint16_t text;
  uint16_t data;
  uint16_t bss;
  uint16_t syms;
  uint16_t entry;
  uint16_t trsize;
  uint16_t drsize;
} ZObjExec;

enum { ZOBJ_OMAGIC = 0x5a01, ZOBJ_NMAGIC = 0x5a02 };

typedef struct {
  uint16_t address;
  uint8_t symbolnum;
  uint8_t info;
} ZObjReloc;

enum {
  ZOBJ_RELOC_PCREL = (1 << 0),

  ZOBJ_RELOC_LENGTH_MASK = (3 << 1),
  ZOBJ_RELOC_LENGTH_BYTE = (0 << 1),
  ZOBJ_RELOC_LENGTH_WORD = (1 << 1),
  ZOBJ_RELOC_LENGTH_LONG = (2 << 1),

  ZOBJ_RELOC_EXTERN = (1 << 3),
  ZOBJ_RELOC_BASEREL = (1 << 4),
  ZOBJ_RELOC_JMPTABLE = (1 << 5),
  ZOBJ_RELOC_RELATIVE = (1 << 6),
  ZOBJ_RELOC_COPY = (1 << 7)
};

typedef struct {
  uint16_t strx;
  uint8_t type;
  uint8_t other;
  uint16_t desc;
  uint16_t value;
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

typedef struct {
  uint16_t size;
  char *data;
} ZObjStrTab;

#endif /* ZOBJ_H */
