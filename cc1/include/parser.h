#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct ASTNode ASTNode;
typedef struct Parameter Parameter;
typedef struct TranslationUnit TranslationUnit;

typedef enum {
    TYPE_INT,
    TYPE_VOID
} Type;

typedef enum {
    AST_TRANSLATION_UNIT,
    AST_FUNCTION,
    AST_PROTOTYPE,
    AST_COMPOUND,
    AST_RETURN,
    AST_EXPRESSION_STMT,
    AST_DECLARATION,
    AST_VARIABLE,
    AST_ASSIGNMENT,
    AST_CALL,
    AST_NUMBER
} ASTNodeType;

struct Parameter {
    Type type;
    char *name;
    Parameter *next;
};

struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            Type return_type;
            char *name;
            Parameter *params;
            ASTNode *body;
        } function;
        struct {
            Type return_type;
            char *name;
            Parameter *params;
        } prototype;
        struct {
            ASTNode **statements;
            int count;
        } compound;
        struct {
            ASTNode *value;
        } return_stmt;
        struct {
            ASTNode *expr;
        } expression_stmt;
        struct {
            Type type;
            char *name;
            ASTNode *initializer; /* Can be NULL */
        } declaration;
        struct {
            char *name;
        } variable;
        struct {
            char *name;
            ASTNode *value;
        } assignment;
        struct {
            char *name;
            ASTNode **args;
            int arg_count;
        } call;
        struct {
            int value;
        } number;
    } data;
    ASTNode *next; /* For linking declarations in translation unit */
};

struct TranslationUnit {
    ASTNode *declarations;
};

TranslationUnit *parser_parse(void);
void parser_free_translation_unit(TranslationUnit *unit);
void parser_free(ASTNode *node);

#endif
