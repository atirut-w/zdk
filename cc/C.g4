// C90 grammar, based on https://slebok.github.io/zoo/c/c90/rascal/extracted/index.html and https://slebok.github.io/zoo/c/c90/sdf/extracted/index.html
grammar C;

// === Parser rules ===

translationUnit: declaration+;

declaration: variableDeclaration | functionDeclaration;

variableDeclaration:
	specifier+ Identifier ('=' expression)? ';';

functionDeclaration:
	specifier+ Identifier '(' paramList ')' (block | ';');

paramList: 'void' | 'int' Identifier (',' 'int' Identifier)*;

specifier: 'int' | 'static' | 'extern';

block: '{' blockItem* '}';

blockItem: statement | declaration;

forInit: variableDeclaration | expression? ';';

statement:
	'return' expression? ';'										# ReturnStatement
	| expression ';'												# ExpressionStatement
	| 'if' '(' expression ')' statement ('else' statement)?			# IfStatement
	| 'while' '(' expression ')' statement							# WhileStatement
	| 'do' statement 'while' '(' expression ')' ';'					# DoWhileStatement
	| 'for' '(' forInit expression? ';' expression? ')' statement	# ForStatement
	| ';'															# NullStatement;

// For order of precedence, see https://en.cppreference.com/w/c/language/operator_precedence
expression:
	Identifier												# IdentifierExpression
	| IntegerConstant										# IntegerConstantExpression
	| '(' expression ')'									# ParenthesizedExpression
	| expression '(' (expression (',' expression)*)? ')'	# FunctionCallExpression
	| <assoc = right> '-' expression						# NegationExpression
	| <assoc = right> '!' expression						# LogicalNotExpression
	| <assoc = right> '~' expression						# BitwiseNotExpression
	| expression ('*' | '/' | '%') expression				# MultiplicativeExpression
	| expression ('+' | '-') expression						# AdditiveExpression
	| expression ('<' | '<=' | '>' | '>=') expression		# RelationalExpression
	| expression ('==' | '!=') expression					# EqualityExpression
	| expression '&&' expression							# LogicalAndExpression
	| expression '||' expression							# LogicalOrExpression
	| <assoc = right> expression '=' expression				# AssignmentExpression;

// === Lexer rules ===

Whitespace: [ \t\r\n] -> skip;
Comment: '/*' .*? '*/' -> skip;

// Keywords
KeywordIf: 'if';
KeywordElse: 'else';
KeywordReturn: 'return';
KeywordVoid: 'void';
KeywordInt: 'int';
KeywordStatic: 'static';
KeywordExtern: 'extern';

// Punctuations
LeftParen: '(';
RightParen: ')';
LeftBrace: '{';
RightBrace: '}';
Comma: ',';
Semicolon: ';';

// Arithmetic operators
Increment: '++';
Add: '+';
Decrement: '--';
Subtract: '-';
Multiply: '*';
Divide: '/';
Modulo: '%';

// Bitwise operators
BitwiseNot: '~';
BitwiseAnd: '&';
BitwiseOr: '|';
BitwiseXor: '^';

// Logical operators
LogicalNot: '!';
LogicalAnd: '&&';
LogicalOr: '||';
Equal: '==';
NotEqual: '!=';
Less: '<';
LessEqual: '<=';
Greater: '>';
GreaterEqual: '>=';

// Miscellaneous
Assignment: '=';

// Identifiers and constants
Identifier: [a-zA-Z_][a-zA-Z0-9_]*;
IntegerConstant: '0' | [1-9][0-9]*;
