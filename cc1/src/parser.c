#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Token current_token;

static void advance_token(void) {
    if (current_token.value) {
        free(current_token.value);
    }
    current_token = lexer_next_token();
}

static void expect(TokenType type) {
    if (current_token.type != type) {
        fprintf(stderr, "Parse error at line %d, column %d: unexpected token\n",
                current_token.line, current_token.column);
        exit(1);
    }
    advance_token();
}

static Type parse_type(void) {
    Type type;
    if (current_token.type == TOK_INT) {
        type = TYPE_INT;
        advance_token();
    } else if (current_token.type == TOK_VOID) {
        type = TYPE_VOID;
        advance_token();
    } else {
        fprintf(stderr, "Parse error: expected type at line %d, column %d\n",
                current_token.line, current_token.column);
        exit(1);
    }
    return type;
}

static Parameter *parse_parameter(void) {
    Parameter *param = malloc(sizeof(Parameter));
    param->type = parse_type();
    
    if (current_token.type != TOK_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected parameter name\n");
        exit(1);
    }
    
    param->name = malloc(strlen(current_token.value) + 1);
    strcpy(param->name, current_token.value);
    advance_token();
    
    param->next = NULL;
    return param;
}

static Parameter *parse_parameter_list(void) {
    Parameter *head = NULL;
    Parameter *tail = NULL;
    
    /* Check for void or empty parameter list */
    if (current_token.type == TOK_RPAREN) {
        return NULL;
    }
    
    if (current_token.type == TOK_VOID) {
        advance_token();
        if (current_token.type == TOK_RPAREN) {
            return NULL; /* void parameter */
        }
        fprintf(stderr, "Parse error: unexpected token after void\n");
        exit(1);
    }
    
    /* Parse first parameter */
    head = parse_parameter();
    tail = head;
    
    /* Parse remaining parameters */
    while (current_token.type == TOK_COMMA) {
        advance_token();
        tail->next = parse_parameter();
        tail = tail->next;
    }
    
    return head;
}

static ASTNode *parse_primary_expression(void);
static ASTNode *parse_postfix_expression(void);
static ASTNode *parse_expression(void);

static ASTNode *parse_primary_expression(void) {
    ASTNode *node;
    
    if (current_token.type == TOK_NUMBER || current_token.type == TOK_CHAR) {
        node = malloc(sizeof(ASTNode));
        node->type = AST_NUMBER;
        node->data.number.value = atoi(current_token.value);
        node->next = NULL;
        advance_token();
        return node;
    } else if (current_token.type == TOK_IDENTIFIER) {
        /* Could be a function call or variable (for now, assume function call) */
        return parse_postfix_expression();
    } else if (current_token.type == TOK_LPAREN) {
        advance_token();
        node = parse_primary_expression();
        expect(TOK_RPAREN);
        return node;
    } else {
        fprintf(stderr, "Parse error: unexpected token in expression at line %d, column %d\n",
                current_token.line, current_token.column);
        exit(1);
    }
}

static ASTNode *parse_postfix_expression(void) {
    ASTNode *node;
    char *name;
    int capacity;
    int count;
    ASTNode **args;
    
    if (current_token.type != TOK_IDENTIFIER) {
        return parse_primary_expression();
    }
    
    name = malloc(strlen(current_token.value) + 1);
    strcpy(name, current_token.value);
    advance_token();
    
    if (current_token.type == TOK_LPAREN) {
        /* Function call */
        node = malloc(sizeof(ASTNode));
        node->type = AST_CALL;
        node->data.call.name = name;
        node->next = NULL;
        
        advance_token(); /* consume '(' */
        
        /* Parse arguments */
        capacity = 4;
        count = 0;
        args = malloc(sizeof(ASTNode*) * capacity);
        
        if (current_token.type != TOK_RPAREN) {
            while (1) {
                if (count >= capacity) {
                    capacity *= 2;
                    args = realloc(args, sizeof(ASTNode*) * capacity);
                }
                args[count++] = parse_primary_expression();
                
                if (current_token.type == TOK_COMMA) {
                    advance_token();
                } else {
                    break;
                }
            }
        }
        
        expect(TOK_RPAREN);
        
        node->data.call.args = args;
        node->data.call.arg_count = count;
        
        return node;
    } else if (current_token.type == TOK_ASSIGN) {
        /* Assignment expression */
        advance_token(); /* consume '=' */
        
        node = malloc(sizeof(ASTNode));
        node->type = AST_ASSIGNMENT;
        node->data.assignment.name = name;
        node->data.assignment.value = parse_expression();
        node->next = NULL;
        
        return node;
    } else {
        /* Variable reference */
        node = malloc(sizeof(ASTNode));
        node->type = AST_VARIABLE;
        node->data.variable.name = name;
        node->next = NULL;
        return node;
    }
}

static ASTNode *parse_expression(void) {
    return parse_postfix_expression();
}

static ASTNode *parse_statement(void);

static ASTNode *parse_return_statement(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->next = NULL;
    
    expect(TOK_RETURN);
    
    /* Check for empty return (for void functions) */
    if (current_token.type == TOK_SEMICOLON) {
        node->data.return_stmt.value = NULL;
    } else {
        node->data.return_stmt.value = parse_expression();
    }
    
    expect(TOK_SEMICOLON);
    
    return node;
}

static ASTNode *parse_expression_statement(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_EXPRESSION_STMT;
    node->next = NULL;
    
    node->data.expression_stmt.expr = parse_expression();
    expect(TOK_SEMICOLON);
    
    return node;
}

static ASTNode *parse_declaration(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_DECLARATION;
    node->next = NULL;
    
    /* Parse type */
    node->data.declaration.type = parse_type();
    
    /* Parse variable name */
    if (current_token.type != TOK_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected variable name in declaration\n");
        exit(1);
    }
    
    node->data.declaration.name = malloc(strlen(current_token.value) + 1);
    strcpy(node->data.declaration.name, current_token.value);
    advance_token();
    
    /* Check for initialization */
    if (current_token.type == TOK_ASSIGN) {
        advance_token();
        node->data.declaration.initializer = parse_expression();
    } else {
        node->data.declaration.initializer = NULL;
    }
    
    expect(TOK_SEMICOLON);
    
    return node;
}

static ASTNode *parse_statement(void) {
    if (current_token.type == TOK_RETURN) {
        return parse_return_statement();
    } else if (current_token.type == TOK_INT || current_token.type == TOK_VOID) {
        /* Variable declaration */
        return parse_declaration();
    } else {
        return parse_expression_statement();
    }
}

static ASTNode *parse_compound_statement(void) {
    ASTNode *node;
    int capacity;
    int count;
    ASTNode **statements;
    
    node = malloc(sizeof(ASTNode));
    node->type = AST_COMPOUND;
    node->next = NULL;
    
    expect(TOK_LBRACE);
    
    capacity = 8;
    count = 0;
    statements = malloc(sizeof(ASTNode*) * capacity);
    
    while (current_token.type != TOK_RBRACE) {
        if (count >= capacity) {
            capacity *= 2;
            statements = realloc(statements, sizeof(ASTNode*) * capacity);
        }
        statements[count++] = parse_statement();
    }
    
    expect(TOK_RBRACE);
    
    node->data.compound.statements = statements;
    node->data.compound.count = count;
    
    return node;
}

static ASTNode *parse_function_or_prototype(void) {
    ASTNode *node;
    Type return_type;
    char *name;
    Parameter *params;
    
    return_type = parse_type();
    
    if (current_token.type != TOK_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected function name\n");
        exit(1);
    }
    
    name = malloc(strlen(current_token.value) + 1);
    strcpy(name, current_token.value);
    advance_token();
    
    expect(TOK_LPAREN);
    params = parse_parameter_list();
    expect(TOK_RPAREN);
    
    if (current_token.type == TOK_SEMICOLON) {
        /* Function prototype */
        node = malloc(sizeof(ASTNode));
        node->type = AST_PROTOTYPE;
        node->data.prototype.return_type = return_type;
        node->data.prototype.name = name;
        node->data.prototype.params = params;
        node->next = NULL;
        advance_token();
    } else if (current_token.type == TOK_LBRACE) {
        /* Function definition */
        node = malloc(sizeof(ASTNode));
        node->type = AST_FUNCTION;
        node->data.function.return_type = return_type;
        node->data.function.name = name;
        node->data.function.params = params;
        node->data.function.body = parse_compound_statement();
        node->next = NULL;
    } else {
        fprintf(stderr, "Parse error: expected ';' or '{' after function declaration\n");
        exit(1);
    }
    
    return node;
}

TranslationUnit *parser_parse(void) {
    TranslationUnit *unit = malloc(sizeof(TranslationUnit));
    ASTNode *head = NULL;
    ASTNode *tail = NULL;
    
    advance_token();
    
    while (current_token.type != TOK_EOF) {
        ASTNode *decl = parse_function_or_prototype();
        
        if (head == NULL) {
            head = decl;
            tail = decl;
        } else {
            tail->next = decl;
            tail = decl;
        }
    }
    
    unit->declarations = head;
    return unit;
}

static void free_parameters(Parameter *param) {
    while (param) {
        Parameter *next = param->next;
        free(param->name);
        free(param);
        param = next;
    }
}

void parser_free(ASTNode *node) {
    int i;
    
    if (!node) return;
    
    switch (node->type) {
        case AST_FUNCTION:
            free(node->data.function.name);
            free_parameters(node->data.function.params);
            parser_free(node->data.function.body);
            break;
        case AST_PROTOTYPE:
            free(node->data.prototype.name);
            free_parameters(node->data.prototype.params);
            break;
        case AST_COMPOUND:
            for (i = 0; i < node->data.compound.count; i++) {
                parser_free(node->data.compound.statements[i]);
            }
            free(node->data.compound.statements);
            break;
        case AST_RETURN:
            parser_free(node->data.return_stmt.value);
            break;
        case AST_EXPRESSION_STMT:
            parser_free(node->data.expression_stmt.expr);
            break;
        case AST_DECLARATION:
            free(node->data.declaration.name);
            parser_free(node->data.declaration.initializer);
            break;
        case AST_VARIABLE:
            free(node->data.variable.name);
            break;
        case AST_ASSIGNMENT:
            free(node->data.assignment.name);
            parser_free(node->data.assignment.value);
            break;
        case AST_CALL:
            free(node->data.call.name);
            for (i = 0; i < node->data.call.arg_count; i++) {
                parser_free(node->data.call.args[i]);
            }
            free(node->data.call.args);
            break;
        case AST_NUMBER:
            break;
        case AST_TRANSLATION_UNIT:
            /* Should not reach here */
            break;
    }
    
    free(node);
}

void parser_free_translation_unit(TranslationUnit *unit) {
    ASTNode *node = unit->declarations;
    while (node) {
        ASTNode *next = node->next;
        parser_free(node);
        node = next;
    }
    free(unit);
}
