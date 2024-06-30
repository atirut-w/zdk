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
    : specifier* declarator declaration* '{' declaration* statement* '}'
    ;

statement
    : 'return' ';'
    ;

declaration
    : specifier+ ';'
    ;

declarator
    : IDENTIFIER
    | declarator '(' parameters? ')'
    ;

parameters
    : (parameter ',')+ moreParameters?
    ;

moreParameters
    : ','
    | '...'
    ;

parameter
    : specifier* declarator
    ;

specifier
    : typeSpecifier
    ;

typeSpecifier
    : 'void'
    | 'char'
    | 'short'
    | 'int'
    ;

lineMarker
    : HASH DEC_LITERAL STRING_LITERAL DEC_LITERAL*
    ;

// LEXER RULES START HERE
WHITESPACE: [ \t\n\r]+ -> skip;

HASH: '#';
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*;
DEC_LITERAL: [0-9]+;
STRING_LITERAL: '"' ('\\' . | .)*? '"';
