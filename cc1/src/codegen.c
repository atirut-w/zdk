#include "codegen.h"
#include <stdio.h>

static FILE *output_file;

static void codegen_function(ASTNode *node) {
    ASTNode *body;
    ASTNode *value;
    int return_value;
    
    fprintf(output_file, "\t.global %s\n", node->data.function.name);
    fprintf(output_file, "%s:\n", node->data.function.name);
    
    /* Generate code for function body */
    body = node->data.function.body;
    
    if (body->type == AST_RETURN) {
        value = body->data.return_stmt.value;
        
        if (value->type == AST_NUMBER) {
            /* Load 16-bit return value into HL register pair */
            return_value = value->data.number.value;
            fprintf(output_file, "\tld hl, %d\n", return_value);
        }
    }
    
    /* Return instruction */
    fprintf(output_file, "\tret\n");
}

void codegen_generate(ASTNode *ast, FILE *output) {
    output_file = output;
    
    fprintf(output_file, "\t.section .text\n");
    
    if (ast->type == AST_FUNCTION) {
        codegen_function(ast);
    }
}
