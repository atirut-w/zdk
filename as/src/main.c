#include "zobj.h"
#include <stdio.h>
#include <string.h>

int write_object_file(const char *filename) {
  FILE *file = fopen(filename, "wb");
  ZObjExec exec;

  if (!file) {
    fprintf(stderr, "Could not open output file %s for writing\n", filename);
    return 1;
  }

  exec.magic = ZOBJ_OMAGIC;
  exec.text = 0;   /* TODO: Set actual text size */
  exec.data = 0;   /* TODO: Set actual data size */
  exec.bss = 0;    /* TODO: Set actual BSS size */
  exec.syms = 0;   /* TODO: Set actual symbol table size */
  exec.entry = 0;  /* TODO: Set actual entry point */
  exec.trsize = 0; /* TODO: Set actual text relocation size */
  exec.drsize = 0; /* TODO: Set actual data relocation size */

  fwrite(&exec.magic, sizeof(exec.magic), 1, file);
  fwrite(&exec.text, sizeof(exec.text), 1, file);
  fwrite(&exec.data, sizeof(exec.data), 1, file);
  fwrite(&exec.bss, sizeof(exec.bss), 1, file);
  fwrite(&exec.syms, sizeof(exec.syms), 1, file);
  fwrite(&exec.entry, sizeof(exec.entry), 1, file);
  fwrite(&exec.trsize, sizeof(exec.trsize), 1, file);
  fwrite(&exec.drsize, sizeof(exec.drsize), 1, file);

  fclose(file);
  return 0;
}

int main(int argc, char *argv[]) {
  int argi = 1;
  const char *output_filename = "a.out";

  if (argi < argc && strcmp(argv[argi], "-u") == 0) {
    argi++; /* TODO: Set flag to externalize undefs */
  }

  if (argi < argc && strcmp(argv[argi], "-o") == 0) {
    argi++;
    if (argi < argc) {
      output_filename = argv[argi];
      argi++;
    } else {
      fprintf(stderr, "Missing output filename after -o\n");
      return 1;
    }
  }

  while (argi < argc) {
    FILE *file = fopen(argv[argi], "r");
    if (!file) {
      fprintf(stderr, "%s?\n", argv[argi]);
      return 1;
    }

    /* TODO: Assemble the file */

    fclose(file);
    argi++;
  }

  /* TODO: Relocate data to etext, and BSS to edata */
  /* TODO: Patch symbol refs */
  /* TODO: Externalize undefs */

  if (write_object_file(output_filename) != 0) {
    return 1;
  }

  return 0;
}
