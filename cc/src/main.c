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
  char *input_file; /* primary C input for backward compatibility */
  char **inputs;    /* all input files */
  int input_count;
  char *output_file;
  char *target_name;
  int list_targets;
  int no_link;
  int keep_asm;
  char *linker_script; /* -T */
  int verbose; /* -v */
} Args;

int cc_verbose = 0; /* global verbose flag for target tool invocations */

const char *progname(const char *path) {
  const char *slash = strrchr(path, '/');
  return slash ? slash + 1 : path;
}

int parse_args(int argc, char **argv, Args *args) {
  int i;
  memset(args, 0, sizeof(Args));
  args->target_name = "z80";
  args->linker_script = NULL;
  args->verbose = 0;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--target") == 0 || strcmp(argv[i], "-t") == 0) {
      if (i + 1 >= argc) { fprintf(stderr, "Error: %s requires an argument\n", argv[i]); return 0; }
      args->target_name = argv[++i];
    } else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc) { fprintf(stderr, "Error: -o requires an argument\n"); return 0; }
      args->output_file = argv[++i];
    } else if (strcmp(argv[i], "-c") == 0) {
      args->no_link = 1;
    } else if (strcmp(argv[i], "-S") == 0) {
      args->no_link = 1; args->keep_asm = 1;
    } else if (strcmp(argv[i], "--list-targets") == 0) {
      args->list_targets = 1;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      printf("Usage: %s [options] <inputs...>\n", progname(argv[0]));
      printf("Options:\n");
      printf("  -o <file>             Write output to <file> (link mode)\n");
      printf("  -c                    Compile/assemble but do not link\n");
      printf("  -S                    Compile to assembly only (no assemble/link)\n");
      printf("  -t, --target <name>   Select target (default: z80)\n");
      printf("  -T <script>           Use linker script\n");
      printf("  -v                    Verbose (print tool commands)\n");
      printf("  --list-targets        List available targets\n");
      printf("  -h, --help            Show this help message\n");
      return 0;
    } else if (strcmp(argv[i], "-T") == 0) {
      if (i + 1 >= argc) { fprintf(stderr, "Error: -T requires a linker script\n"); return 0; }
      args->linker_script = argv[++i];
    } else if (strcmp(argv[i], "-v") == 0) {
      args->verbose = 1; cc_verbose = 1;
    } else if (argv[i][0] != '-') {
      char **new_inputs;
      new_inputs = (char **)realloc(args->inputs, sizeof(char *) * (args->input_count + 1));
      if (!new_inputs) { fprintf(stderr, "Out of memory allocating inputs\n"); return 0; }
      args->inputs = new_inputs;
      args->inputs[args->input_count++] = argv[i];
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return 0;
    }
  }
  if (args->list_targets) return 1;
  if (args->input_count == 0) {
    fprintf(stderr, "Usage: %s [options] <inputs...>\n", progname(argv[0]));
    fprintf(stderr, "Try '%s --help' for more information.\n", progname(argv[0]));
    return 0;
  }
  /* Select a primary C input for legacy paths */
  for (i = 0; i < args->input_count; i++) {
    const char *p = args->inputs[i];
    const char *dot = strrchr(p, '.');
    if (dot && strcmp(dot, ".c") == 0) { args->input_file = args->inputs[i]; break; }
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

  sprintf(cmd, "cpp '%s' -o '%s'", input_file, temp_template);
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
  FILE *input_file = NULL;
  FILE *asm_file = NULL;
  char *temp_file = NULL;
  char *asm_filename = NULL;
  char *obj_filename = NULL; /* for primary C file */
  struct Target *target;
  struct Codegen codegen;
  int success = 0;
  int parse_result = 0;
  /* Collections for link step */
  const char **obj_files = NULL;
  int *produced_flags = NULL; /* mark which we created to unlink later */
  int obj_count = 0;
  int i;

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

  /* Prepare arrays sized to number of inputs (worst case every input becomes an object) */
  obj_files = (const char **)malloc(sizeof(char *) * args.input_count);
  produced_flags = (int *)malloc(sizeof(int) * args.input_count);
  if (!obj_files || !produced_flags) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }
  for (i = 0; i < args.input_count; i++) produced_flags[i] = 0;

  /* Handle primary C compilation if present */
  if (args.input_file) {
    /* Determine filenames for this C file */
    if (args.keep_asm) {
      if (args.output_file && args.input_count == 1) {
        asm_filename = my_strdup(args.output_file);
      } else {
        asm_filename = change_extension(args.input_file, ".s");
      }
    } else {
      obj_filename = change_extension(args.input_file, ".o");
      asm_filename = change_extension(args.input_file, ".s");
    }
    if (!asm_filename) { fprintf(stderr, "Failed to allocate assembly filename\n"); return 1; }

    input_file = preprocess_file(args.input_file, &temp_file);
    if (!input_file) {
      fprintf(stderr, "Could not preprocess '%s'\n", args.input_file);
      free(asm_filename); if (obj_filename) free(obj_filename); return 1;
    }
    yyin = input_file; yyfilename = args.input_file; parse_result = yyparse();
    if (parse_result != 0) {
      fclose(input_file); if (temp_file) { unlink(temp_file); free(temp_file); }
      free(asm_filename); if (obj_filename) free(obj_filename); return 1;
    }
    {
      struct ASTNode *tree; struct Sema sema; extern struct ASTNode *parser_get_tree(void);
      tree = parser_get_tree();
      if (!tree) {
        fprintf(stderr, "Parse produced no AST\n");
        fclose(input_file); if (temp_file) { unlink(temp_file); free(temp_file); }
        free(asm_filename); if (obj_filename) free(obj_filename); return 1;
      }
      sema_init(&sema);
      if (!sema_analyze(&sema, tree)) {
        fprintf(stderr, "%d error(s) generated\n", sema.error_count);
        sema_destroy(&sema); ast_free(tree); fclose(input_file);
        if (temp_file) { unlink(temp_file); free(temp_file); }
        free(asm_filename); if (obj_filename) free(obj_filename); return 1;
      }
      sema_destroy(&sema);
      asm_file = fopen(asm_filename, "w");
      if (!asm_file) {
        fprintf(stderr, "Failed to open output file '%s': %s\n", asm_filename, strerror(errno));
        ast_free(tree); fclose(input_file); if (temp_file) { unlink(temp_file); free(temp_file); }
        free(asm_filename); if (obj_filename) free(obj_filename); return 1;
      }
      memset(&codegen, 0, sizeof(struct Codegen)); codegen.output = asm_file; codegen_init_defaults(&codegen);
      if (target->init_codegen) target->init_codegen(&codegen);
      if (target->init) target->init(&codegen);
      codegen_generate(&codegen, tree);
      if (target->finalize) target->finalize(&codegen);
      fclose(asm_file); ast_free(tree); success = 1;
    }
    fclose(input_file); if (temp_file) { unlink(temp_file); free(temp_file); temp_file = NULL; }
    if (args.keep_asm) {
      /* Only assembly requested; add nothing to link list */
    } else {
      /* Assemble C output */
      memset(&codegen, 0, sizeof(struct Codegen)); codegen_init_defaults(&codegen);
      if (target->init_codegen) target->init_codegen(&codegen);
      if (!target->invoke_assembler(&codegen, asm_filename, obj_filename)) {
        fprintf(stderr, "Assembly failed for %s\n", args.input_file);
        if (!args.keep_asm) unlink(asm_filename);
        free(asm_filename); if (obj_filename) free(obj_filename); return 1;
      }
      if (!args.keep_asm) { unlink(asm_filename); }
      free(asm_filename); asm_filename = NULL;
  obj_files[obj_count] = obj_filename; produced_flags[obj_count] = 1; obj_count++;
  /* Ownership of obj_filename transferred to obj_files array */
  obj_filename = NULL;
    }
  }

  /* Process other inputs (.s and .o) */
  for (i = 0; i < args.input_count; i++) {
    const char *in = args.inputs[i];
    const char *dot = strrchr(in, '.');
    char *new_obj;
    if (in == args.input_file) continue; /* already handled */
    if (dot && strcmp(dot, ".o") == 0) {
      obj_files[obj_count] = in; produced_flags[obj_count] = 0; obj_count++;
    } else if (dot && strcmp(dot, ".s") == 0) {
      if (args.keep_asm) {
        /* -S: treat as no further action */
        continue;
      }
      new_obj = change_extension(in, ".o");
      if (!new_obj) { fprintf(stderr, "Failed to allocate object name for %s\n", in); return 1; }
      memset(&codegen, 0, sizeof(struct Codegen)); codegen_init_defaults(&codegen);
      if (target->init_codegen) target->init_codegen(&codegen);
      if (!target->invoke_assembler(&codegen, in, new_obj)) {
        fprintf(stderr, "Assembly failed for %s\n", in);
        free(new_obj);
        return 1;
      }
      obj_files[obj_count] = new_obj; produced_flags[obj_count] = 1; obj_count++;
    } else if (!(dot && strcmp(dot, ".c") == 0)) {
      fprintf(stderr, "Warning: unrecognized input type '%s' skipped\n", in);
    }
  }

  if (args.keep_asm && args.no_link) {
    /* Done (assembly outputs for C sources already written) */
    for (i = 0; i < obj_count; i++) if (produced_flags[i]) free((char *)obj_files[i]);
    free(obj_files); free(produced_flags);
    if (obj_filename) free(obj_filename);
    return 0;
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

  if (args.no_link) {
    /* -c: no link; nothing else to do */
    for (i = 0; i < obj_count; i++) {
      if (!produced_flags[i]) continue; /* user-supplied .o, leave it */
      /* Keep produced .o files */
    }
  free(obj_files); free(produced_flags); if (obj_filename) free(obj_filename); return 0;
  }

  if (!args.output_file) args.output_file = "a.out";
  if (!target->invoke_linker(&codegen, obj_files, obj_count, args.output_file, args.linker_script)) {
    fprintf(stderr, "Linking failed\n");
    for (i = 0; i < obj_count; i++) {
      if (produced_flags[i]) {
        unlink(obj_files[i]);
        free((char *)obj_files[i]);
        obj_files[i] = NULL;
      }
    }
    free(obj_files); free(produced_flags); if (obj_filename) free(obj_filename); return 1;
  }
  /* Clean up produced object files on success */
  for (i = 0; i < obj_count; i++) if (produced_flags[i]) { unlink(obj_files[i]); free((char *)obj_files[i]); obj_files[i] = NULL; }
  free(obj_files); free(produced_flags); if (obj_filename) free(obj_filename);
  return 0;
}
