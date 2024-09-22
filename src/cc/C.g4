grammar C;

compilationUnit: function;

function:
	Int Identifier LParen Void RParen LBrace statement RBrace;

statement: Return expression Semicolon;

expression: additiveExpression;

additiveExpression: unaryExpression ((Plus | Minus) unaryExpression)*;

unaryExpression: unaryOperator? primaryExpression;

unaryOperator: Tilde | Minus;

primaryExpression: LParen expression RParen | Constant;

Whitespace: [ \t\r\n] -> skip;

PlusPlus: '++';
Plus: '+';
MinusMinus: '--';
Minus: '-';
Star: '*';
Slash: '/';
Percent: '%';
Tilde: '~';


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
