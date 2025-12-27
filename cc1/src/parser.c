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

static ASTNode *parse_number(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->data.number.value = atoi(current_token.value);
    advance_token();
    return node;
}

static ASTNode *parse_return_statement(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    
    expect(TOK_RETURN);
    node->data.return_stmt.value = parse_number();
    expect(TOK_SEMICOLON);
    
    return node;
}

static ASTNode *parse_function(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    char *name;
    
    node->type = AST_FUNCTION;
    
    expect(TOK_INT);
    
    if (current_token.type != TOK_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected function name\n");
        exit(1);
    }
    
    name = malloc(strlen(current_token.value) + 1);
    strcpy(name, current_token.value);
    node->data.function.name = name;
    advance_token();
    
    expect(TOK_LPAREN);
    expect(TOK_RPAREN);
    expect(TOK_LBRACE);
    
    node->data.function.body = parse_return_statement();
    
    expect(TOK_RBRACE);
    
    return node;
}

ASTNode *parser_parse(void) {
    advance_token();
    return parse_function();
}

void parser_free(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_FUNCTION:
            free(node->data.function.name);
            parser_free(node->data.function.body);
            break;
        case AST_RETURN:
            parser_free(node->data.return_stmt.value);
            break;
        case AST_NUMBER:
            break;
    }
    
    free(node);
}
