#ifndef ZOBJ_STRTAB_H
#define ZOBJ_STRTAB_H

#include <stdio.h>

typedef struct {
  unsigned short size;
  char *data;
} ZObjStrTab;

int zobj_write_strtab(FILE *f, const ZObjStrTab *strtab);

#endif /* ZOBJ_STRTAB_H */
