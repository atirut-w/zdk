grammar C;

// Parser rules

program: function;

function: 'int' Identifier '(' 'void' ')' '{' statement '}';

statement: 'return' expression ';' # ReturnStatement;

expression: Integer # IntegerExpr;

// Lexer rules

Whitespace: [ \t\r\n]+ -> skip;

KeywordVoid: 'void';
KeywordInt: 'int';

KeywordReturn: 'return';

LeftParen: '(';
RightParen: ')';
LeftBrace: '{';
RightBrace: '}';
Semicolon: ';';

Identifier: [a-zA-Z_][a-zA-Z0-9_]*;
Integer: '0' | [1-9][0-9]*;
