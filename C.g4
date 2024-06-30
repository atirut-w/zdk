// ANSI C/C90 grammar for ANTLR4
// See: https://slebok.github.io/zoo/c/c90/rascal/extracted/index.html
grammar C;

translationUnit
    : externalDeclaration*
    ;

externalDeclaration
    : functionDefinition
    | lineMarker
    ;

functionDefinition
    : specifier* declarator declaration* LBRACE declaration* statement* RBRACE
    ;

statement
    : RETURN SEMICOLON
    ;

declaration
    : specifier+ SEMICOLON
    ;

declarator
    : IDENTIFIER
    | declarator LPAREN parameters? RPAREN
    ;

parameters
    : (parameter COMMA)+ moreParameters?
    | VOID
    ;

moreParameters
    : COMMA
    | VARARGS
    ;

parameter
    : specifier* declarator
    ;

specifier
    : typeSpecifier
    ;

typeSpecifier
    : VOID
    | CHAR
    | SHORT
    | INT
    ;

lineMarker
    : HASH DEC_LITERAL STRING_LITERAL DEC_LITERAL*
    ;

// LEXER RULES START HERE
WHITESPACE: [ \t\n\r]+ -> skip;

RETURN: 'return';

LPAREN: '(';
RPAREN: ')';
LBRACE: '{';
RBRACE: '}';
SEMICOLON: ';';
COMMA: ',';
VARARGS: '...';

VOID: 'void';
CHAR: 'char';
SHORT: 'short';
INT: 'int';

HASH: '#';
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*;
DEC_LITERAL: [0-9]+;
STRING_LITERAL: '"' ('\\' . | .)*? '"';
