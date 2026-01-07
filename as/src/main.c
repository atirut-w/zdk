#include <stdio.h>
#include <string.h>

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
  /* TODO: Write out object file */

  return 0;
}
