grammar C;

compilationUnit: externalDeclaration* EOF;

externalDeclaration:  lineMarker | variableDeclaration;

lineMarker: '#' INT_LITERAL STRING_LITERAL INT_LITERAL*;

variableDeclaration: primitiveType IDENTIFIER initializer? ';';

primitiveType: INT;

initializer: '=' expression;

expression: IDENTIFIER | INT_LITERAL | STRING_LITERAL;

// Lexer rules
WHITESPACE: [ \t\r\n]+ -> skip;

INT: 'int';

IDENTIFIER: ALPHA (ALPHA | DIGIT)*;
STRING_LITERAL: '"' ('\\' . | ~["\\])* '"';
INT_LITERAL: DIGIT+;

fragment ALPHA: [a-zA-Z_];
fragment DIGIT: [0-9];
