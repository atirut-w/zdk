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

/* Local variable entry for tracking stack-allocated variables */
typedef struct LocalVar {
    char *name;
    int offset; /* Offset from IX (negative for local variables) */
    struct LocalVar *next;
} LocalVar;

static LocalVar *local_vars = NULL;
static int stack_offset = 0; /* Current stack offset for local variables */

/* String literal storage */
typedef struct StringLiteral {
    int label_id;
    char *value;
    struct StringLiteral *next;
} StringLiteral;

static StringLiteral *string_literals = NULL;

/* Add or update a symbol in the symbol table */
static void add_symbol(const char *name, int is_defined) {
    SymbolEntry *entry = symbol_table;
    
    /* Check if symbol already exists */
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            /* Update only if this is a definition (not a prototype) */
            /* This ensures that once a function is defined, it stays defined */
            if (is_defined) {
                entry->is_defined = 1;
            }
            /* If is_defined is 0 (prototype), preserve existing value */
            return;
        }
        entry = entry->next;
    }
    
    /* Symbol doesn't exist, create a new entry */
    entry = malloc(sizeof(SymbolEntry));
    if (!entry) {
        fprintf(stderr, "Codegen error: out of memory\n");
        exit(1);
    }
    
    entry->name = malloc(strlen(name) + 1);
    if (!entry->name) {
        fprintf(stderr, "Codegen error: out of memory\n");
        free(entry);
        exit(1);
    }
    
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

/* Add a local variable to the current function's symbol table */
static void add_local_var(const char *name, int offset) {
    LocalVar *var = malloc(sizeof(LocalVar));
    if (!var) {
        fprintf(stderr, "Codegen error: out of memory\n");
        exit(1);
    }
    
    var->name = malloc(strlen(name) + 1);
    if (!var->name) {
        fprintf(stderr, "Codegen error: out of memory\n");
        free(var);
        exit(1);
    }
    
    strcpy(var->name, name);
    var->offset = offset;
    var->next = local_vars;
    local_vars = var;
}

/* Find a local variable by name, return its offset or -1 if not found */
static int find_local_var(const char *name) {
    LocalVar *var = local_vars;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var->offset;
        }
        var = var->next;
    }
    return 0; /* Return 0 to indicate not found (offset 0 would be saved IX) */
}

/* Free the local variable table */
static void free_local_vars(void) {
    LocalVar *var = local_vars;
    while (var) {
        LocalVar *next = var->next;
        free(var->name);
        free(var);
        var = next;
    }
    local_vars = NULL;
    stack_offset = 0;
}

/* Add a string literal for later emission */
static void add_string_literal(int label_id, const char *value) {
    StringLiteral *lit = malloc(sizeof(StringLiteral));
    lit->label_id = label_id;
    lit->value = malloc(strlen(value) + 1);
    strcpy(lit->value, value);
    lit->next = string_literals;
    string_literals = lit;
}

/* Free string literals */
static void free_string_literals(void) {
    StringLiteral *lit = string_literals;
    while (lit) {
        StringLiteral *next = lit->next;
        free(lit->value);
        free(lit);
        lit = next;
    }
    string_literals = NULL;
}

/* Generate code for an expression, result in HL */
static void codegen_expression(ASTNode *node) {
    int i;
    int arg_offset;
    int var_offset;
    
    if (!node) return;
    
    switch (node->type) {
        case AST_NUMBER:
            /* Load immediate value into HL */
            fprintf(output_file, "\tld hl, %d\n", node->data.number.value);
            break;
            
        case AST_STRING_LITERAL:
            /* Load address of string literal into HL */
            add_string_literal(node->data.string_literal.label_id, node->data.string_literal.value);
            fprintf(output_file, "\tld hl, _str%d\n", node->data.string_literal.label_id);
            break;
            
        case AST_VARIABLE:
            /* Load variable value from stack into HL */
            var_offset = find_local_var(node->data.variable.name);
            if (var_offset == 0) {
                fprintf(stderr, "Codegen error: undefined variable '%s'\n", 
                        node->data.variable.name);
                exit(1);
            }
            /* Load 16-bit value from (IX + offset) into HL */
            fprintf(output_file, "\tld l, (ix%+d)\n", var_offset);
            fprintf(output_file, "\tld h, (ix%+d)\n", var_offset + 1);
            break;
            
        case AST_DEREFERENCE:
            /* Evaluate operand to get address in HL */
            codegen_expression(node->data.dereference.operand);
            /* Load byte from address in HL */
            fprintf(output_file, "\tld a, (hl)\n");
            fprintf(output_file, "\tld l, a\n");
            fprintf(output_file, "\tld h, 0\n"); /* Zero-extend byte to word */
            break;
            
        case AST_POSTFIX_INC:
            /* Load current value */
            var_offset = find_local_var(node->data.postfix_inc.name);
            if (var_offset == 0) {
                fprintf(stderr, "Codegen error: undefined variable '%s'\n", 
                        node->data.postfix_inc.name);
                exit(1);
            }
            /* Load variable into HL (the current value, which is returned) */
            fprintf(output_file, "\tld l, (ix%+d)\n", var_offset);
            fprintf(output_file, "\tld h, (ix%+d)\n", var_offset + 1);
            /* Increment the variable */
            fprintf(output_file, "\tinc (ix%+d)\n", var_offset);
            /* Check for carry to high byte */
            fprintf(output_file, "\tjr nz, .skip%d\n", node->data.postfix_inc.label_id);
            fprintf(output_file, "\tinc (ix%+d)\n", var_offset + 1);
            fprintf(output_file, ".skip%d:\n", node->data.postfix_inc.label_id);
            /* HL still contains the old value */
            break;
            
        case AST_ASSIGNMENT:
            /* Evaluate right-hand side */
            codegen_expression(node->data.assignment.value);
            
            /* Store HL into variable */
            var_offset = find_local_var(node->data.assignment.name);
            if (var_offset == 0) {
                fprintf(stderr, "Codegen error: undefined variable '%s'\n", 
                        node->data.assignment.name);
                exit(1);
            }
            fprintf(output_file, "\tld (ix%+d), l\n", var_offset);
            fprintf(output_file, "\tld (ix%+d), h\n", var_offset + 1);
            /* Result of assignment is the value, which is already in HL */
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
            /* Restore stack pointer and frame pointer, then return */
            fprintf(output_file, "\tld sp, ix\n");
            fprintf(output_file, "\tpop ix\n");
            fprintf(output_file, "\tret\n");
            break;
            
        case AST_DECLARATION:
            /* Variable declaration - already allocated in prologue */
            /* Just generate initialization code if there's an initializer */
            if (node->data.declaration.initializer) {
                int var_offset;
                
                /* Evaluate initializer expression */
                codegen_expression(node->data.declaration.initializer);
                
                /* Find variable's offset */
                var_offset = find_local_var(node->data.declaration.name);
                if (var_offset == 0) {
                    fprintf(stderr, "Codegen error: undefined variable '%s'\n", 
                            node->data.declaration.name);
                    exit(1);
                }
                
                /* Store HL into variable location (IX + offset) */
                fprintf(output_file, "\tld (ix%+d), l\n", var_offset);
                fprintf(output_file, "\tld (ix%+d), h\n", var_offset + 1);
            }
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

/* Count local variables in a compound statement and calculate total stack space needed */
static int count_local_vars(ASTNode *node, int current_offset) {
    int i;
    int offset = current_offset;
    
    if (!node) return offset;
    
    switch (node->type) {
        case AST_COMPOUND:
            for (i = 0; i < node->data.compound.count; i++) {
                offset = count_local_vars(node->data.compound.statements[i], offset);
            }
            break;
            
        case AST_DECLARATION:
            /* All types are 2 bytes (int or pointer) on Z80 */
            offset -= 2;
            add_local_var(node->data.declaration.name, offset);
            break;
            
        default:
            /* Other statements don't declare variables */
            break;
    }
    
    return offset;
}

/* Generate code for a function definition */
static void codegen_function(ASTNode *node) {
    int total_locals_size;
    
    /* Reset local variable tracking for this function */
    free_local_vars();
    
    /* First pass: count local variables and build symbol table */
    stack_offset = 0;
    stack_offset = count_local_vars(node->data.function.body, stack_offset);
    total_locals_size = -stack_offset; /* Convert negative offset to positive size */
    
    fprintf(output_file, "\t.global %s\n", node->data.function.name);
    fprintf(output_file, "%s:\n", node->data.function.name);
    
    /* Function prologue: set up frame pointer */
    fprintf(output_file, "\tpush ix\n");
    fprintf(output_file, "\tld ix, 0\n");
    fprintf(output_file, "\tadd ix, sp\n");
    
    /* Allocate space for local variables */
    if (total_locals_size > 0) {
        fprintf(output_file, "\tld hl, -%d\n", total_locals_size);
        fprintf(output_file, "\tadd hl, sp\n");
        fprintf(output_file, "\tld sp, hl\n");
    }
    
    /* At this point:
     * IX+0: saved IX (old frame pointer)
     * IX+2: return address
     * IX+4: first argument
     * IX+6: second argument
     * etc.
     * IX-2: first local variable
     * IX-4: second local variable
     * etc.
     */
    
    /* Generate code for function body */
    codegen_statement(node->data.function.body);
    
    /* If we reach here (no explicit return), add epilogue */
    /* This handles void functions or functions that fall through */
    fprintf(output_file, "\tld sp, ix\n");
    fprintf(output_file, "\tpop ix\n");
    fprintf(output_file, "\tret\n");
    
    /* Clean up local variables for this function */
    free_local_vars();
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
    
    /* Emit string literals in .rodata section */
    if (string_literals) {
        StringLiteral *lit;
        
        fprintf(output_file, "\t.section .rodata\n");
        
        for (lit = string_literals; lit != NULL; lit = lit->next) {
            int i;
            fprintf(output_file, "_str%d:\n", lit->label_id);
            fprintf(output_file, "\t.byte ");
            for (i = 0; lit->value[i] != '\0'; i++) {
                if (i > 0) fprintf(output_file, ", ");
                fprintf(output_file, "%d", (unsigned char)lit->value[i]);
            }
            fprintf(output_file, ", 0\n"); /* Null terminator */
        }
        
        free_string_literals();
    }
    
    /* Clean up symbol table */
    free_symbol_table();
}
