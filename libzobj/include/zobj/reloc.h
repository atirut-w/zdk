#ifndef ZOBJ_RELOC_H
#define ZOBJ_RELOC_H

#include <stdio.h>

typedef struct {
  unsigned short address;
  unsigned char symbolnum;
  unsigned char info;
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

int zobj_write_reloc(FILE *f, const ZObjReloc *reloc);

#endif /* ZOBJ_RELOC_H */
