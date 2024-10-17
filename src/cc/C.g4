// C90 grammar, based on https://slebok.github.io/zoo/c/c90/rascal/extracted/index.html and https://slebok.github.io/zoo/c/c90/sdf/extracted/index.html
grammar C;

// === Parser rules ===

translationUnit: externalDeclaration+;

externalDeclaration: functionDefinition;

functionDefinition:
	specifier* declarator declaration* '{' declaration* statement* '}';

specifier: typeSpecifier;

typeSpecifier: 'void' | 'int';

declarator: Identifier | functionDeclarator;

functionDeclarator: Identifier '(' parameters? ')';

parameters: parameter (',' parameter)* (',' '...')? | 'void';

parameter: specifier* declarator;

declaration: declarationWithoutInit;

declarationWithoutInit: specifier+ ';';

statement: returnStatement;

returnStatement: 'return' expression? ';';

// For order of precedence, see https://en.cppreference.com/w/c/language/operator_precedence
expression:
	IntegerConstant			# IntegerConstantExpression
	| '-' expression		# NegationExpression
	| '~' expression		# BitwiseNotExpression
	| '(' expression ')'	# ParenthesizedExpression;

// === Lexer rules ===

Whitespace: [ \t\r\n] -> skip;

// Punctuations
LParen: '(';
RParen: ')';
LBrace: '{';
RBrace: '}';
Comma: ',';
Semicolon: ';';

// Keywords
Return: 'return';
Void: 'void';
Int: 'int';

// Arithmetic operators
MinusMinus: '--';
Minus: '-';

// Bitwise operators
Tilde: '~';

Identifier: [a-zA-Z_][a-zA-Z0-9_]*;
IntegerConstant: '0' | [1-9][0-9]*;
