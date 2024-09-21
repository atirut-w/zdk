grammar C;

compilationUnit: function;

function:
	Int Identifier LParen Void RParen LBrace statement RBrace;

statement: Return expression Semicolon;

expression: Constant;

Whitespace: [ \t\r\n] -> skip;

LParen: '(';
RParen: ')';
LBrace: '{';
RBrace: '}';
Semicolon: ';';

Void: 'void';
Int: 'int';
Return: 'return';

Identifier: [a-zA-Z_] [a-zA-Z_0-9]*;
Constant: '0' | [1-9] [0-9]*;
