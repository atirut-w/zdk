#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "ast.h"

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

static FILE *preprocess_file(const char *input_file, char **temp_file) {
  char temp_template[] = "/tmp/zdk_cpp_XXXXXX";
  int temp_fd;
  FILE *preprocessed;
  char cmd[512];
  int ret;

  temp_fd = mkstemp(temp_template);
  if (temp_fd == -1) {
    fprintf(stderr, "Failed to create temporary file: %s\n", strerror(errno));
    return NULL;
  }
  close(temp_fd);

  *temp_file = (char *)malloc(strlen(temp_template) + 1);
  if (!*temp_file) {
    unlink(temp_template);
    return NULL;
  }
  strcpy(*temp_file, temp_template);

  sprintf(cmd, "cpp -P '%s' -o '%s'", input_file, temp_template);
  ret = system(cmd);
  if (ret != 0) {
    fprintf(stderr, "Preprocessing failed with exit code %d\n", ret);
    free(*temp_file);
    *temp_file = NULL;
    unlink(temp_template);
    return NULL;
  }

  preprocessed = fopen(temp_template, "r");
  if (!preprocessed) {
    fprintf(stderr, "Failed to open preprocessed file: %s\n", strerror(errno));
    free(*temp_file);
    *temp_file = NULL;
    unlink(temp_template);
    return NULL;
  }

  return preprocessed;
}

int main(int argc, char **argv) {
  Args args;
  FILE *input_file;
  char *temp_file = NULL;

  if (!parse_args(argc, argv, &args)) {
    return 1;
  }

  input_file = preprocess_file(args.input_file, &temp_file);
  if (!input_file) {
    fprintf(stderr, "Could not preprocess '%s'\n", args.input_file);
    return 1;
  }
  {
    struct Symbols syms;
    struct Lexer lx;
    struct Parser ps;
    struct ASTNode *tree;

    symbols_init(&syms);
    lexer_init(&lx, input_file, args.input_file, &syms);
    parser_init(&ps, &lx, &syms);

    tree = parse_translation_unit(&ps);
    if (tree) {
      ast_print(tree, 0);
      ast_free(tree);
    }

    parser_destroy(&ps);
    lexer_destroy(&lx);
    symbols_free(&syms);
  }

  fclose(input_file);
  if (temp_file) {
    unlink(temp_file);
    free(temp_file);
  }
  return 0;
}
