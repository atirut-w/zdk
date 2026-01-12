#include "zobj/exec.h"

int zobj_write_exec(FILE *f, const ZObjExec *exec) {
  if (!f || !exec) {
    return -1;
  }

  if (fwrite(&exec->magic, sizeof(exec->magic), 1, f) != 1)
    return -1;
  if (fwrite(&exec->text, sizeof(exec->text), 1, f) != 1)
    return -1;
  if (fwrite(&exec->data, sizeof(exec->data), 1, f) != 1)
    return -1;
  if (fwrite(&exec->bss, sizeof(exec->bss), 1, f) != 1)
    return -1;
  if (fwrite(&exec->syms, sizeof(exec->syms), 1, f) != 1)
    return -1;
  if (fwrite(&exec->entry, sizeof(exec->entry), 1, f) != 1)
    return -1;
  if (fwrite(&exec->trsize, sizeof(exec->trsize), 1, f) != 1)
    return -1;
  if (fwrite(&exec->drsize, sizeof(exec->drsize), 1, f) != 1)
    return -1;

  return 0;
}
