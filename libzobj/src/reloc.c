#include "zobj/reloc.h"

int zobj_write_reloc(FILE *f, const ZObjReloc *reloc) {
  if (!f || !reloc) {
    return -1;
  }

  if (fwrite(&reloc->address, sizeof(reloc->address), 1, f) != 1)
    return -1;
  if (fwrite(&reloc->symbolnum, sizeof(reloc->symbolnum), 1, f) != 1)
    return -1;
  if (fwrite(&reloc->info, sizeof(reloc->info), 1, f) != 1)
    return -1;

  return 0;
}
