#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [options] file...\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -o <file>  Place the output into <file>\n");
    fprintf(stderr, "  -c         Compile and assemble, but do not link\n");
    fprintf(stderr, "  -S         Compile only; do not assemble or link\n");
    fprintf(stderr, "  -h, --help Display this information\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "TODO: Add preprocessor (cpp) integration for handling #include, #define, etc.\n");
}

static char *create_temp_file(const char *suffix) {
    char *template;
    int fd;
    
    template = malloc(256);
    snprintf(template, 256, "/tmp/cc-XXXXXX%s", suffix);
    
    fd = mkstemps(template, strlen(suffix));
    if (fd == -1) {
        perror("Failed to create temporary file");
        free(template);
        return NULL;
    }
    
    close(fd);
    return template;
}

static int run_command(char **argv) {
    pid_t pid;
    int status;
    
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        /* Child process */
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    }
    
    /* Parent process */
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return -1;
    }
    
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    
    return -1;
}

int main(int argc, char **argv) {
    char *input_file = NULL;
    char *output_file = NULL;
    char *asm_file = NULL;
    char *obj_file = NULL;
    int compile_only = 0;
    int assemble_only = 0;
    int i;
    int ret = 0;
    char *cc1_argv[5];
    char *as_argv[5];
    char *ld_argv[5];
    
    /* Parse command-line arguments */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "Error: -o requires an argument\n");
                print_usage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-c") == 0) {
            assemble_only = 1;
        } else if (strcmp(argv[i], "-S") == 0) {
            compile_only = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else {
            input_file = argv[i];
        }
    }
    
    if (!input_file) {
        fprintf(stderr, "Error: no input file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    /* Determine output file names */
    if (compile_only) {
        if (!output_file) {
            output_file = malloc(strlen(input_file) + 3);
            strcpy(output_file, input_file);
            /* Replace .c with .s */
            if (strlen(output_file) > 2 && strcmp(output_file + strlen(output_file) - 2, ".c") == 0) {
                strcpy(output_file + strlen(output_file) - 2, ".s");
            } else {
                strcat(output_file, ".s");
            }
        }
        asm_file = output_file;
    } else {
        asm_file = create_temp_file(".s");
        if (!asm_file) return 1;
    }
    
    /* Step 1: Compile with cc1 */
    cc1_argv[0] = "cc1";
    cc1_argv[1] = input_file;
    cc1_argv[2] = "-o";
    cc1_argv[3] = asm_file;
    cc1_argv[4] = NULL;
    
    ret = run_command(cc1_argv);
    if (ret != 0) {
        fprintf(stderr, "Compilation failed\n");
        if (!compile_only) unlink(asm_file);
        if (!compile_only && !output_file) free(asm_file);
        return ret;
    }
    
    if (compile_only) {
        return 0;
    }
    
    /* Step 2: Assemble with z80-unknown-none-elf-as */
    if (assemble_only) {
        if (!output_file) {
            output_file = malloc(strlen(input_file) + 3);
            strcpy(output_file, input_file);
            /* Replace .c with .o */
            if (strlen(output_file) > 2 && strcmp(output_file + strlen(output_file) - 2, ".c") == 0) {
                strcpy(output_file + strlen(output_file) - 2, ".o");
            } else {
                strcat(output_file, ".o");
            }
        }
        obj_file = output_file;
    } else {
        obj_file = create_temp_file(".o");
        if (!obj_file) {
            unlink(asm_file);
            free(asm_file);
            return 1;
        }
    }
    
    as_argv[0] = "z80-unknown-none-elf-as";
    as_argv[1] = "-o";
    as_argv[2] = obj_file;
    as_argv[3] = asm_file;
    as_argv[4] = NULL;
    
    ret = run_command(as_argv);
    unlink(asm_file);
    free(asm_file);
    
    if (ret != 0) {
        fprintf(stderr, "Assembly failed\n");
        if (!assemble_only) {
            unlink(obj_file);
            free(obj_file);
        }
        return ret;
    }
    
    if (assemble_only) {
        return 0;
    }
    
    /* Step 3: Link with z80-unknown-none-elf-ld */
    if (!output_file) {
        output_file = "a.out";
    }
    
    ld_argv[0] = "z80-unknown-none-elf-ld";
    ld_argv[1] = "-o";
    ld_argv[2] = output_file;
    ld_argv[3] = obj_file;
    ld_argv[4] = NULL;
    
    ret = run_command(ld_argv);
    unlink(obj_file);
    free(obj_file);
    
    if (ret != 0) {
        fprintf(stderr, "Linking failed\n");
        return ret;
    }
    
    return 0;
}
