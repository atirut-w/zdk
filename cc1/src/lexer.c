#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static FILE *input_file;
static int current_char;
static int line = 1;
static int column = 0;

static void advance(void) {
    if (current_char == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }
    current_char = fgetc(input_file);
}

static void skip_whitespace(void) {
    while (isspace(current_char)) {
        advance();
    }
}

static char *read_identifier(void) {
    char buffer[256];
    int i = 0;
    char *result;
    
    while ((isalnum(current_char) || current_char == '_') && i < 255) {
        buffer[i++] = current_char;
        advance();
    }
    buffer[i] = '\0';
    
    result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);
    return result;
}

static char *read_number(void) {
    char buffer[256];
    int i = 0;
    char *result;
    
    while (isdigit(current_char) && i < 255) {
        buffer[i++] = current_char;
        advance();
    }
    buffer[i] = '\0';
    
    result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);
    return result;
}

static char *read_char_literal(void) {
    char buffer[16];
    int char_value;
    char *result;
    
    advance(); /* Skip opening ' */
    
    if (current_char == '\\') {
        /* Escape sequence */
        advance();
        switch (current_char) {
            case 'n': char_value = '\n'; break;
            case 't': char_value = '\t'; break;
            case 'r': char_value = '\r'; break;
            case '0': char_value = '\0'; break;
            case '\\': char_value = '\\'; break;
            case '\'': char_value = '\''; break;
            default:
                fprintf(stderr, "Unknown escape sequence: '\\%c' at line %d, column %d\n",
                        current_char, line, column);
                exit(1);
        }
        advance();
    } else {
        char_value = current_char;
        advance();
    }
    
    if (current_char != '\'') {
        fprintf(stderr, "Unterminated character literal at line %d, column %d\n",
                line, column);
        exit(1);
    }
    advance(); /* Skip closing ' */
    
    /* Convert to string representation of the integer value */
    snprintf(buffer, sizeof(buffer), "%d", char_value);
    result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);
    return result;
}

void lexer_init(FILE *input) {
    input_file = input;
    line = 1;
    column = 0;
    advance();
}

Token lexer_next_token(void) {
    Token token;
    char *identifier;
    
    skip_whitespace();
    
    token.line = line;
    token.column = column;
    token.value = NULL;
    
    if (current_char == EOF) {
        token.type = TOK_EOF;
        return token;
    }
    
    if (isalpha(current_char) || current_char == '_') {
        identifier = read_identifier();
        
        if (strcmp(identifier, "int") == 0) {
            token.type = TOK_INT;
            free(identifier);
        } else if (strcmp(identifier, "void") == 0) {
            token.type = TOK_VOID;
            free(identifier);
        } else if (strcmp(identifier, "return") == 0) {
            token.type = TOK_RETURN;
            free(identifier);
        } else {
            token.type = TOK_IDENTIFIER;
            token.value = identifier;
        }
        
        return token;
    }
    
    if (isdigit(current_char)) {
        token.type = TOK_NUMBER;
        token.value = read_number();
        return token;
    }
    
    if (current_char == '\'') {
        token.type = TOK_CHAR;
        token.value = read_char_literal();
        return token;
    }
    
    switch (current_char) {
        case '(':
            token.type = TOK_LPAREN;
            advance();
            break;
        case ')':
            token.type = TOK_RPAREN;
            advance();
            break;
        case '{':
            token.type = TOK_LBRACE;
            advance();
            break;
        case '}':
            token.type = TOK_RBRACE;
            advance();
            break;
        case ';':
            token.type = TOK_SEMICOLON;
            advance();
            break;
        case ',':
            token.type = TOK_COMMA;
            advance();
            break;
        case '=':
            token.type = TOK_ASSIGN;
            advance();
            break;
        default:
            fprintf(stderr, "Unexpected character: '%c' at line %d, column %d\n",
                    current_char, line, column);
            exit(1);
    }
    
    return token;
}

void lexer_free(void) {
    /* Nothing to free currently */
}
