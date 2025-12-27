#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef enum {
    FILE_TYPE_C,      /* .c - C source file */
    FILE_TYPE_ASM,    /* .s - Assembly source file */
    FILE_TYPE_OBJ,    /* .o - Object file */
    FILE_TYPE_UNKNOWN
} file_type_t;

typedef struct {
    char *path;
    file_type_t type;
    char *obj_file;  /* Path to object file (temp or final) */
    char *asm_file;  /* Path to assembly file (for -S) */
} input_file_t;

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [options] file...\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -o <file>  Place the output into <file>\n");
    fprintf(stderr, "  -c         Compile and assemble, but do not link\n");
    fprintf(stderr, "  -S         Compile only; do not assemble or link\n");
    fprintf(stderr, "  -T <file>  Use <file> as the linker script\n");
    fprintf(stderr, "  -h, --help Display this information\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Supported input file types:\n");
    fprintf(stderr, "  .c - C source files (compiled, assembled, and linked)\n");
    fprintf(stderr, "  .s - Assembly source files (assembled and linked)\n");
    fprintf(stderr, "  .o - Object files (linked directly)\n");
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

static file_type_t detect_file_type(const char *filename) {
    const char *ext;
    
    /* Find the last dot in the filename */
    ext = strrchr(filename, '.');
    if (!ext || ext == filename) {
        return FILE_TYPE_UNKNOWN;
    }
    
    if (strcmp(ext, ".c") == 0) {
        return FILE_TYPE_C;
    } else if (strcmp(ext, ".s") == 0) {
        return FILE_TYPE_ASM;
    } else if (strcmp(ext, ".o") == 0) {
        return FILE_TYPE_OBJ;
    }
    
    return FILE_TYPE_UNKNOWN;
}

static char *replace_extension(const char *filename, const char *new_ext) {
    const char *ext;
    size_t base_len;
    size_t new_ext_len = strlen(new_ext);
    char *result;
    
    /* Find the last dot to determine extension position */
    ext = strrchr(filename, '.');
    if (!ext) {
        /* No extension found, shouldn't happen with validated input */
        return NULL;
    }
    
    base_len = ext - filename;  /* Length of filename before extension */
    result = malloc(base_len + new_ext_len + 1);
    if (!result) {
        return NULL;
    }
    
    /* Copy base filename and append new extension */
    memcpy(result, filename, base_len);
    strcpy(result + base_len, new_ext);
    
    return result;
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
    input_file_t *input_files = NULL;
    int num_inputs = 0;
    int input_capacity = 0;
    char *output_file = NULL;
    char *linker_script = NULL;
    int compile_only = 0;
    int assemble_only = 0;
    int i, j;
    int ret = 0;
    char **ld_argv = NULL;
    int ld_argc;
    
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
        } else if (strcmp(argv[i], "-T") == 0) {
            if (i + 1 < argc) {
                linker_script = argv[++i];
            } else {
                fprintf(stderr, "Error: -T requires an argument\n");
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
            /* Input file */
            if (num_inputs >= input_capacity) {
                input_capacity = input_capacity == 0 ? 4 : input_capacity * 2;
                input_files = realloc(input_files, input_capacity * sizeof(input_file_t));
                if (!input_files) {
                    perror("Failed to allocate memory for input files");
                    return 1;
                }
            }
            input_files[num_inputs].path = argv[i];
            input_files[num_inputs].type = detect_file_type(argv[i]);
            input_files[num_inputs].obj_file = NULL;
            input_files[num_inputs].asm_file = NULL;
            
            if (input_files[num_inputs].type == FILE_TYPE_UNKNOWN) {
                fprintf(stderr, "Error: unknown file type for '%s'\n", argv[i]);
                fprintf(stderr, "Supported extensions: .c, .s, .o\n");
                free(input_files);
                return 1;
            }
            
            num_inputs++;
        }
    }
    
    if (num_inputs == 0) {
        fprintf(stderr, "Error: no input files specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    /* Check for conflicting options */
    if (compile_only && assemble_only) {
        fprintf(stderr, "Error: -c and -S are mutually exclusive\n");
        free(input_files);
        return 1;
    }
    
    /* Check if multiple inputs with -S or -c without output specified */
    if (num_inputs > 1 && (compile_only || assemble_only) && !output_file) {
        if (compile_only) {
            fprintf(stderr, "Error: cannot specify multiple input files with -S without -o\n");
        } else {
            fprintf(stderr, "Error: cannot specify multiple input files with -c without -o\n");
        }
        free(input_files);
        return 1;
    }
    
    /* Process each input file */
    for (i = 0; i < num_inputs; i++) {
        char *asm_file = NULL;
        char *obj_file = NULL;
        int need_free_asm = 0;
        int need_free_obj = 0;
        
        switch (input_files[i].type) {
            case FILE_TYPE_C: {
                char *cc1_argv[5];
                char *as_argv[5];
                
                /* Determine assembly file name */
                if (compile_only) {
                    if (output_file) {
                        asm_file = output_file;
                    } else {
                        asm_file = replace_extension(input_files[i].path, ".s");
                        if (!asm_file) {
                            perror("Failed to allocate memory");
                            ret = 1;
                            goto cleanup;
                        }
                    }
                } else {
                    asm_file = create_temp_file(".s");
                    if (!asm_file) {
                        ret = 1;
                        goto cleanup;
                    }
                    need_free_asm = 1;
                }
                
                /* Step 1: Compile with cc1 */
                cc1_argv[0] = "cc1";
                cc1_argv[1] = input_files[i].path;
                cc1_argv[2] = "-o";
                cc1_argv[3] = asm_file;
                cc1_argv[4] = NULL;
                
                ret = run_command(cc1_argv);
                if (ret != 0) {
                    fprintf(stderr, "Compilation failed for '%s'\n", input_files[i].path);
                    if (need_free_asm) {
                        unlink(asm_file);
                        free(asm_file);
                    }
                    goto cleanup;
                }
                
                if (compile_only) {
                    /* Store asm_file for cleanup if allocated */
                    if (asm_file != output_file) {
                        input_files[i].asm_file = asm_file;
                    }
                    continue;
                }
                
                /* Step 2: Assemble */
                if (assemble_only) {
                    if (output_file) {
                        obj_file = output_file;
                    } else {
                        obj_file = replace_extension(input_files[i].path, ".o");
                        if (!obj_file) {
                            perror("Failed to allocate memory");
                            if (need_free_asm) {
                                unlink(asm_file);
                                free(asm_file);
                            }
                            ret = 1;
                            goto cleanup;
                        }
                    }
                } else {
                    obj_file = create_temp_file(".o");
                    if (!obj_file) {
                        if (need_free_asm) {
                            unlink(asm_file);
                            free(asm_file);
                        }
                        ret = 1;
                        goto cleanup;
                    }
                    need_free_obj = 1;
                }
                
                as_argv[0] = "z80-unknown-none-elf-as";
                as_argv[1] = "-o";
                as_argv[2] = obj_file;
                as_argv[3] = asm_file;
                as_argv[4] = NULL;
                
                ret = run_command(as_argv);
                if (need_free_asm) {
                    unlink(asm_file);
                    free(asm_file);
                }
                
                if (ret != 0) {
                    fprintf(stderr, "Assembly failed for '%s'\n", input_files[i].path);
                    if (need_free_obj) {
                        unlink(obj_file);
                        free(obj_file);
                    }
                    goto cleanup;
                }
                
                input_files[i].obj_file = obj_file;
                
                if (assemble_only) {
                    continue;
                }
                
                break;
            }
            
            case FILE_TYPE_ASM: {
                char *as_argv[5];
                
                /* For -S, assembly files don't need processing */
                if (compile_only) {
                    fprintf(stderr, "Warning: -S specified but '%s' is already an assembly file, skipping\n", input_files[i].path);
                    continue;
                }
                
                /* Determine object file name */
                if (assemble_only) {
                    if (output_file) {
                        obj_file = output_file;
                    } else {
                        obj_file = replace_extension(input_files[i].path, ".o");
                        if (!obj_file) {
                            perror("Failed to allocate memory");
                            ret = 1;
                            goto cleanup;
                        }
                    }
                } else {
                    obj_file = create_temp_file(".o");
                    if (!obj_file) {
                        ret = 1;
                        goto cleanup;
                    }
                    need_free_obj = 1;
                }
                
                as_argv[0] = "z80-unknown-none-elf-as";
                as_argv[1] = "-o";
                as_argv[2] = obj_file;
                as_argv[3] = input_files[i].path;
                as_argv[4] = NULL;
                
                ret = run_command(as_argv);
                if (ret != 0) {
                    fprintf(stderr, "Assembly failed for '%s'\n", input_files[i].path);
                    if (need_free_obj) {
                        unlink(obj_file);
                        free(obj_file);
                    }
                    goto cleanup;
                }
                
                input_files[i].obj_file = obj_file;
                
                if (assemble_only) {
                    continue;
                }
                
                break;
            }
            
            case FILE_TYPE_OBJ:
                /* Object files are used directly */
                if (compile_only) {
                    fprintf(stderr, "Warning: -S specified but '%s' is already an object file\n", input_files[i].path);
                    continue;
                } else if (assemble_only) {
                    fprintf(stderr, "Warning: -c specified but '%s' is already an object file\n", input_files[i].path);
                    continue;
                }
                input_files[i].obj_file = input_files[i].path;
                break;
                
            case FILE_TYPE_UNKNOWN:
                /* Should not reach here */
                break;
        }
    }
    
    /* If -S or -c, we're done */
    if (compile_only || assemble_only) {
        /* Clean up any allocated output filenames */
        for (i = 0; i < num_inputs; i++) {
            if (input_files[i].asm_file && input_files[i].asm_file != output_file) {
                free(input_files[i].asm_file);
            }
            if (input_files[i].obj_file && 
                input_files[i].obj_file != input_files[i].path &&
                input_files[i].obj_file != output_file) {
                free(input_files[i].obj_file);
            }
        }
        free(input_files);
        return 0;
    }
    
    /* Step 3: Link all object files */
    if (!output_file) {
        output_file = "a.out";
    }
    
    /* Count object files to link */
    for (i = 0, j = 0; i < num_inputs; i++) {
        if (input_files[i].obj_file) {
            j++;
        }
    }
    
    if (j == 0) {
        fprintf(stderr, "Error: no object files to link\n");
        ret = 1;
        goto cleanup;
    }
    
    /* Build linker command: ld [-T script] -o <output> <obj1> <obj2> ... NULL */
    ld_argc = 3 + j;  /* "ld", "-o", output_file, obj1, obj2, ..., NULL */
    if (linker_script) {
        ld_argc += 2;  /* Add "-T" and script path */
    }
    ld_argv = malloc((ld_argc + 1) * sizeof(char *));
    if (!ld_argv) {
        perror("Failed to allocate memory for linker arguments");
        ret = 1;
        goto cleanup;
    }
    
    j = 0;
    ld_argv[j++] = "z80-unknown-none-elf-ld";
    
    if (linker_script) {
        ld_argv[j++] = "-T";
        ld_argv[j++] = linker_script;
    }
    
    ld_argv[j++] = "-o";
    ld_argv[j++] = output_file;
    
    for (i = 0; i < num_inputs; i++) {
        if (input_files[i].obj_file) {
            ld_argv[j++] = input_files[i].obj_file;
        }
    }
    ld_argv[j] = NULL;
    
    ret = run_command(ld_argv);
    free(ld_argv);
    
    if (ret != 0) {
        fprintf(stderr, "Linking failed\n");
        goto cleanup;
    }
    
cleanup:
    /* Clean up temporary object files */
    if (input_files) {
        for (i = 0; i < num_inputs; i++) {
            if (input_files[i].obj_file && 
                input_files[i].obj_file != input_files[i].path &&
                input_files[i].obj_file != output_file) {
                unlink(input_files[i].obj_file);
                free(input_files[i].obj_file);
            }
        }
        free(input_files);
    }
    
    return ret;
}
