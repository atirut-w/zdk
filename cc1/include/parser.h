#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct ASTNode ASTNode;

typedef enum {
    AST_FUNCTION,
    AST_RETURN,
    AST_NUMBER
} ASTNodeType;

struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            char *name;
            ASTNode *body;
        } function;
        struct {
            ASTNode *value;
        } return_stmt;
        struct {
            int value;
        } number;
    } data;
};

ASTNode *parser_parse(void);
void parser_free(ASTNode *node);

#endif
