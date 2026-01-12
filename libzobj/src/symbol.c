#include "zobj/symbol.h"

int zobj_write_symbol(FILE *f, const ZObjSymbol *sym) {
  if (!f || !sym) {
    return -1;
  }

  if (fwrite(&sym->un.strx, sizeof(sym->un.strx), 1, f) != 1)
    return -1;
  if (fwrite(&sym->type, sizeof(sym->type), 1, f) != 1)
    return -1;
  if (fwrite(&sym->other, sizeof(sym->other), 1, f) != 1)
    return -1;
  if (fwrite(&sym->desc, sizeof(sym->desc), 1, f) != 1)
    return -1;
  if (fwrite(&sym->value, sizeof(sym->value), 1, f) != 1)
    return -1;

  return 0;
}
