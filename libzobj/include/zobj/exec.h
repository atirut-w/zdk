#ifndef ZOBJ_EXEC_H
#define ZOBJ_EXEC_H

#include <stdio.h>

typedef struct {
  unsigned short magic;
  unsigned short text;
  unsigned short data;
  unsigned short bss;
  unsigned short syms;
  unsigned short entry;
  unsigned short trsize;
  unsigned short drsize;
} ZObjExec;

enum { ZOBJ_OMAGIC = 0x5a01, ZOBJ_NMAGIC = 0x5a02 };

int zobj_write_exec(FILE *f, const ZObjExec *exec);

#endif /* ZOBJ_EXEC_H */
