#include "codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FILE *output_file;

/* Symbol table entry for tracking function declarations */
typedef struct SymbolEntry {
    char *name;
    int is_defined; /* 1 if function has a definition, 0 if only prototype */
    struct SymbolEntry *next;
} SymbolEntry;

static SymbolEntry *symbol_table = NULL;

/* Add or update a symbol in the symbol table */
static void add_symbol(const char *name, int is_defined) {
    SymbolEntry *entry = symbol_table;
    
    /* Check if symbol already exists */
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            /* If this is a definition, update the entry */
            if (is_defined) {
                entry->is_defined = 1;
            }
            return;
        }
        entry = entry->next;
    }
    
    /* Symbol doesn't exist, create a new entry */
    entry = malloc(sizeof(SymbolEntry));
    entry->name = malloc(strlen(name) + 1);
    strcpy(entry->name, name);
    entry->is_defined = is_defined;
    entry->next = symbol_table;
    symbol_table = entry;
}

/* Free the symbol table */
static void free_symbol_table(void) {
    SymbolEntry *entry = symbol_table;
    while (entry) {
        SymbolEntry *next = entry->next;
        free(entry->name);
        free(entry);
        entry = next;
    }
    symbol_table = NULL;
}

/* Generate code for an expression, result in HL */
static void codegen_expression(ASTNode *node) {
    int i;
    int arg_offset;
    
    if (!node) return;
    
    switch (node->type) {
        case AST_NUMBER:
            /* Load immediate value into HL */
            fprintf(output_file, "\tld hl, %d\n", node->data.number.value);
            break;
            
        case AST_CALL:
            /* Generate code for function call */
            /* Push arguments right-to-left (reverse order) */
            for (i = node->data.call.arg_count - 1; i >= 0; i--) {
                codegen_expression(node->data.call.args[i]);
                fprintf(output_file, "\tpush hl\n");
            }
            
            /* Call the function */
            fprintf(output_file, "\tcall %s\n", node->data.call.name);
            
            /* Clean up arguments from stack */
            arg_offset = node->data.call.arg_count * 2; /* 2 bytes per argument */
            if (arg_offset > 0) {
                fprintf(output_file, "\tld hl, %d\n", arg_offset);
                fprintf(output_file, "\tadd hl, sp\n");
                fprintf(output_file, "\tld sp, hl\n");
            }
            break;
            
        default:
            fprintf(stderr, "Codegen error: unexpected expression type\n");
            break;
    }
}

/* Generate code for a statement */
static void codegen_statement(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_RETURN:
            if (node->data.return_stmt.value) {
                codegen_expression(node->data.return_stmt.value);
            }
            /* Restore frame pointer and return */
            fprintf(output_file, "\tpop ix\n");
            fprintf(output_file, "\tret\n");
            break;
            
        case AST_EXPRESSION_STMT:
            codegen_expression(node->data.expression_stmt.expr);
            break;
            
        case AST_COMPOUND:
            {
                int i;
                for (i = 0; i < node->data.compound.count; i++) {
                    codegen_statement(node->data.compound.statements[i]);
                }
            }
            break;
            
        default:
            fprintf(stderr, "Codegen error: unexpected statement type\n");
            break;
    }
}

/* Generate code for a function definition */
static void codegen_function(ASTNode *node) {
    fprintf(output_file, "\t.global %s\n", node->data.function.name);
    fprintf(output_file, "%s:\n", node->data.function.name);
    
    /* Function prologue: set up frame pointer */
    fprintf(output_file, "\tpush ix\n");
    fprintf(output_file, "\tld ix, 0\n");
    fprintf(output_file, "\tadd ix, sp\n");
    
    /* At this point:
     * IX+0: saved IX (old frame pointer)
     * IX+2: return address
     * IX+4: first argument
     * IX+6: second argument
     * etc.
     */
    
    /* Generate code for function body */
    codegen_statement(node->data.function.body);
    
    /* If we reach here (no explicit return), add epilogue */
    /* This handles void functions or functions that fall through */
    fprintf(output_file, "\tpop ix\n");
    fprintf(output_file, "\tret\n");
}

void codegen_generate(TranslationUnit *unit, FILE *output) {
    ASTNode *node;
    SymbolEntry *entry;
    
    output_file = output;
    
    /* First pass: build symbol table */
    node = unit->declarations;
    while (node) {
        switch (node->type) {
            case AST_FUNCTION:
                add_symbol(node->data.function.name, 1);
                break;
            case AST_PROTOTYPE:
                add_symbol(node->data.prototype.name, 0);
                break;
            default:
                break;
        }
        node = node->next;
    }
    
    fprintf(output_file, "\t.section .text\n");
    
    /* Emit .extern directives for prototypes without definitions */
    entry = symbol_table;
    while (entry) {
        if (!entry->is_defined) {
            fprintf(output_file, "\t.extern %s\n", entry->name);
        }
        entry = entry->next;
    }
    
    /* Second pass: generate code for function definitions */
    node = unit->declarations;
    while (node) {
        switch (node->type) {
            case AST_FUNCTION:
                codegen_function(node);
                break;
            case AST_PROTOTYPE:
                /* Prototypes don't generate code, only .extern directives */
                break;
            default:
                fprintf(stderr, "Codegen error: unexpected top-level declaration\n");
                break;
        }
        node = node->next;
    }
    
    /* Clean up symbol table */
    free_symbol_table();
}
