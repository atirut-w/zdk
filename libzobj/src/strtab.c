#include "zobj/strtab.h"

int zobj_write_strtab(FILE *f, const ZObjStrTab *strtab) {
  if (!f || !strtab) {
    return -1;
  }

  if (fwrite(&strtab->size, sizeof(strtab->size), 1, f) != 1)
    return -1;
  /* size field includes itself, so write remaining bytes */
  if (fwrite(strtab->data, strtab->size - sizeof(strtab->size), 1, f) != 1)
    return -1;

  return 0;
}
