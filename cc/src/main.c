#include "ast.h"
#include "sema.h"
#include "symbols.h"
#include "target.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Prototypes from the generated parser/lexer (Flex/Bison classic interface) */
extern int yyparse(void);
extern FILE *yyin;
extern const char *yyfilename;

/* strdup is not part of C90 standard */
static char *my_strdup(const char *s) {
  char *result;
  size_t len = strlen(s) + 1;
  result = (char *)malloc(len);
  if (result) {
    memcpy(result, s, len);
  }
  return result;
}

typedef struct {
  char *input_file;
  char *output_file;
  char *target_name;
  int list_targets;
  int no_link;
  int keep_asm;
} Args;

const char *progname(const char *path) {
  const char *slash = strrchr(path, '/');
  return slash ? slash + 1 : path;
}

int parse_args(int argc, char **argv, Args *args) {
  int i;
  memset(args, 0, sizeof(Args));
  
  /* Set defaults */
  args->target_name = "z80";
  args->list_targets = 0;
  args->output_file = NULL; /* Will be determined based on mode */
  args->no_link = 0;
  args->keep_asm = 0;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--target") == 0 || strcmp(argv[i], "-t") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Error: %s requires an argument\n", argv[i]);
        return 0;
      }
      args->target_name = argv[++i];
    } else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Error: -o requires an argument\n");
        return 0;
      }
      args->output_file = argv[++i];
    } else if (strcmp(argv[i], "-c") == 0) {
      args->no_link = 1;
    } else if (strcmp(argv[i], "-S") == 0) {
      args->no_link = 1;
      args->keep_asm = 1;
    } else if (strcmp(argv[i], "--list-targets") == 0) {
      args->list_targets = 1;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      printf("Usage: %s [options] <input_file>\n", progname(argv[0]));
      printf("Options:\n");
      printf("  -o <file>             Write output to <file>\n");
      printf("  -c                    Compile and assemble, but do not link\n");
      printf("  -S                    Compile only; do not assemble or link\n");
      printf("  -t, --target <name>   Select target architecture (default: z80)\n");
      printf("  --list-targets        List available targets\n");
      printf("  -h, --help            Show this help message\n");
      return 0;
    } else if (argv[i][0] != '-') {
      args->input_file = argv[i];
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return 0;
    }
  }

  if (args->list_targets) {
    return 1;
  }

  if (!args->input_file) {
    fprintf(stderr, "Usage: %s [options] <input_file>\n", progname(argv[0]));
    fprintf(stderr, "Try '%s --help' for more information.\n", progname(argv[0]));
    return 0;
  }

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

static char *change_extension(const char *filename, const char *new_ext) {
  const char *dot;
  size_t base_len;
  char *result;
  
  dot = strrchr(filename, '.');
  if (dot) {
    base_len = dot - filename;
  } else {
    base_len = strlen(filename);
  }
  
  result = (char *)malloc(base_len + strlen(new_ext) + 1);
  if (!result) {
    return NULL;
  }
  
  memcpy(result, filename, base_len);
  strcpy(result + base_len, new_ext);
  
  return result;
}

int main(int argc, char **argv) {
  Args args;
  FILE *input_file;
  FILE *asm_file;
  char *temp_file = NULL;
  char *asm_filename = NULL;
  char *obj_filename = NULL;
  struct Target *target;
  struct Codegen codegen;
  int success = 0;
  int parse_result = 0;

  if (!parse_args(argc, argv, &args)) {
    return 1;
  }

  if (args.list_targets) {
    target_list();
    return 0;
  }

  /* Find and validate target */
  target = target_find(args.target_name);
  if (!target) {
    fprintf(stderr, "Error: Unknown target '%s'\n", args.target_name);
    fprintf(stderr, "Use --list-targets to see available targets.\n");
    return 1;
  }

  /* printf("Compiling for target: %s (%s)\n", target->name, target->description); */

  /* Determine output filenames */
  if (args.keep_asm) {
    /* -S flag: output is assembly file */
    if (args.output_file) {
      asm_filename = my_strdup(args.output_file);
    } else {
      asm_filename = change_extension(args.input_file, ".s");
    }
  } else if (args.no_link) {
    /* -c flag: output is object file */
    if (args.output_file) {
      obj_filename = my_strdup(args.output_file);
    } else {
      obj_filename = change_extension(args.input_file, ".o");
    }
    asm_filename = change_extension(args.input_file, ".s");
  } else {
    /* Default: link to executable */
    obj_filename = change_extension(args.input_file, ".o");
    asm_filename = change_extension(args.input_file, ".s");
    if (!args.output_file) {
      args.output_file = "a.out";
    }
  }

  if (!asm_filename) {
    fprintf(stderr, "Failed to allocate assembly filename\n");
    return 1;
  }

  input_file = preprocess_file(args.input_file, &temp_file);
  if (!input_file) {
    fprintf(stderr, "Could not preprocess '%s'\n", args.input_file);
    free(asm_filename);
    if (obj_filename) free(obj_filename);
    return 1;
  }

  /* Temporary: invoke the parser directly on the preprocessed input. */
  yyin = input_file;
  /* Provide original source filename for error messages */
  yyfilename = args.input_file;
  parse_result = yyparse();
  if (parse_result != 0) {
    fclose(input_file);
    if (temp_file) {
      unlink(temp_file);
      free(temp_file);
    }
    free(asm_filename);
    if (obj_filename) free(obj_filename);
    return 1;
  }

  {
    /* Retrieve AST from parser */
    struct ASTNode *tree;
    struct Sema sema;
    extern struct ASTNode *parser_get_tree(void);
    tree = parser_get_tree();

    if (!tree) {
      fprintf(stderr, "Parse produced no AST\n");
      fclose(input_file);
      if (temp_file) {
        unlink(temp_file);
        free(temp_file);
      }
      free(asm_filename);
      if (obj_filename) free(obj_filename);
      return 1;
    }

    sema_init(&sema);
    if (!sema_analyze(&sema, tree)) {
      fprintf(stderr, "Semantic analysis failed with %d error(s)\n", sema.error_count);
      sema_destroy(&sema);
      ast_free(tree);
      fclose(input_file);
      if (temp_file) {
        unlink(temp_file);
        free(temp_file);
      }
      free(asm_filename);
      if (obj_filename) free(obj_filename);
      return 1;
    }
    sema_destroy(&sema);

    asm_file = fopen(asm_filename, "w");
    if (!asm_file) {
      fprintf(stderr, "Failed to open output file '%s': %s\n", 
              asm_filename, strerror(errno));
      ast_free(tree);
      fclose(input_file);
      if (temp_file) {
        unlink(temp_file);
        free(temp_file);
      }
      free(asm_filename);
      if (obj_filename) free(obj_filename);
      return 1;
    }

    memset(&codegen, 0, sizeof(struct Codegen));
    codegen.output = asm_file;
    codegen_init_defaults(&codegen);
    if (target->init_codegen) {
      target->init_codegen(&codegen);
    }
    if (target->init) {
      target->init(&codegen);
    }
    codegen_generate(&codegen, tree);
    if (target->finalize) {
      target->finalize(&codegen);
    }
    fclose(asm_file);

    ast_free(tree);
    success = 1;
  }

  /* TODO: Reimplement
  {
    struct Symbols syms;
    struct Lexer lx;
    struct Parser ps;
    struct Sema sema;
    struct ASTNode *tree;

    symbols_init(&syms);
    lexer_init(&lx, input_file, args.input_file, &syms);
    parser_init(&ps, &lx, &syms);

    tree = parse_translation_unit(&ps);
    if (tree) {
      sema_init(&sema);
      if (!sema_analyze(&sema, tree)) {
        fprintf(stderr, "Semantic analysis failed with %d error(s)\n", sema.error_count);
        sema_destroy(&sema);
        ast_free(tree);
        parser_destroy(&ps);
        lexer_destroy(&lx);
        symbols_free(&syms);
        fclose(input_file);
        if (temp_file) {
          unlink(temp_file);
          free(temp_file);
        }
        free(asm_filename);
        if (obj_filename) free(obj_filename);
        return 1;
      }
      sema_destroy(&sema);
      
      asm_file = fopen(asm_filename, "w");
      if (!asm_file) {
        fprintf(stderr, "Failed to open output file '%s': %s\n", 
                asm_filename, strerror(errno));
        ast_free(tree);
        parser_destroy(&ps);
        lexer_destroy(&lx);
        symbols_free(&syms);
        fclose(input_file);
        if (temp_file) {
          unlink(temp_file);
          free(temp_file);
        }
        free(asm_filename);
        if (obj_filename) free(obj_filename);
        return 1;
      }
      
      memset(&codegen, 0, sizeof(struct Codegen));
      codegen.output = asm_file;
      
      codegen_init_defaults(&codegen);
      
      if (target->init_codegen) {
        target->init_codegen(&codegen);
      }
      
      if (target->init) {
        target->init(&codegen);
      }
      
      codegen_generate(&codegen, tree);
      
      if (target->finalize) {
        target->finalize(&codegen);
      }
      
      fclose(asm_file);
      
      ast_free(tree);
      success = 1;
    }

    parser_destroy(&ps);
    lexer_destroy(&lx);
    symbols_free(&syms);
  }
  */

  fclose(input_file);
  if (temp_file) {
    unlink(temp_file);
    free(temp_file);
  }

  if (!success) {
    free(asm_filename);
    if (obj_filename) free(obj_filename);
    return 1;
  }

  /* If -S flag, we're done */
  if (args.keep_asm && args.no_link) {
    free(asm_filename);
    if (obj_filename) free(obj_filename);
    return 0;
  }

  /* Assemble */
  if (!obj_filename) {
    obj_filename = change_extension(args.input_file, ".o");
  }
  
  memset(&codegen, 0, sizeof(struct Codegen));
  codegen_init_defaults(&codegen);
  if (target->init_codegen) {
    target->init_codegen(&codegen);
  }
  
  if (!target->invoke_assembler(&codegen, asm_filename, obj_filename)) {
    fprintf(stderr, "Assembly failed\n");
    if (!args.keep_asm) {
      unlink(asm_filename);
    }
    free(asm_filename);
    free(obj_filename);
    return 1;
  }

  /* Clean up assembly file unless -S was used */
  if (!args.keep_asm) {
    unlink(asm_filename);
  }
  free(asm_filename);

  /* If -c flag, we're done */
  if (args.no_link) {
    free(obj_filename);
    return 0;
  }

  /* Link */
  {
    const char *obj_files[1];
    obj_files[0] = obj_filename;
    
    if (!target->invoke_linker(&codegen, obj_files, 1, args.output_file)) {
      fprintf(stderr, "Linking failed\n");
      unlink(obj_filename);
      free(obj_filename);
      return 1;
    }
  }

  /* Clean up object file */
  unlink(obj_filename);
  free(obj_filename);

  return 0;
}
