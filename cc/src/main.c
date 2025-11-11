#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  char *input_file;
} Args;

const char *progname(const char *path) {
  const char *slash = strrchr(path, '/');
  return slash ? slash + 1 : path;
}

int parse_args(int argc, char **argv, Args *args) {
  memset(args, 0, sizeof(Args));

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", progname(argv[0]));
    return 0;
  }

  args->input_file = argv[1];

  return 1;
}

int main(int argc, char **argv) {
  Args args;
  FILE *input_file;

  if (!parse_args(argc, argv, &args)) {
    return 1;
  }

  input_file = fopen(args.input_file, "r");
  if (!input_file) {
    printf("Could not open '%s': %s\n", args.input_file, strerror(errno));
    return 1;
  }

  return 0;
}
