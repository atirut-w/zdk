#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *input;

  if (argc < 2) {
    printf("Usage: cc1 <source-file>\n");
    return 1;
  }

  input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "Could not open '%s': %s\n", argv[1], strerror(errno));
    return 1;
  }

  fclose(input);
  return 0;
}
