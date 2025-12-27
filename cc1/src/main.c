#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [input_file] [-o output_file]\n", program_name);
    fprintf(stderr, "  If no input_file is specified, reads from stdin\n");
    fprintf(stderr, "  If no output_file is specified, writes to stdout\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Note: Input is assumed to be preprocessed C code\n");
    fprintf(stderr, "TODO: Integrate preprocessor (cpp) for handling directives\n");
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    FILE *output = stdout;
    char *input_filename = NULL;
    char *output_filename = NULL;
    int i;
    ASTNode *ast;
    
    /* Parse command-line arguments */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_filename = argv[++i];
            } else {
                fprintf(stderr, "Error: -o requires an argument\n");
                print_usage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else {
            input_filename = argv[i];
        }
    }
    
    /* Open input file if specified */
    if (input_filename) {
        input = fopen(input_filename, "r");
        if (!input) {
            fprintf(stderr, "Error: cannot open input file '%s'\n", input_filename);
            return 1;
        }
    }
    
    /* Open output file if specified */
    if (output_filename) {
        output = fopen(output_filename, "w");
        if (!output) {
            fprintf(stderr, "Error: cannot open output file '%s'\n", output_filename);
            if (input_filename) fclose(input);
            return 1;
        }
    }
    
    /* Initialize lexer */
    lexer_init(input);
    
    /* Parse the input */
    ast = parser_parse();
    
    /* Generate code */
    codegen_generate(ast, output);
    
    /* Cleanup */
    parser_free(ast);
    lexer_free();
    
    if (input_filename) fclose(input);
    if (output_filename) fclose(output);
    
    return 0;
}
