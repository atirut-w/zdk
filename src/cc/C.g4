// C90 grammar, based on https://slebok.github.io/zoo/c/c90/rascal/extracted/index.html and https://slebok.github.io/zoo/c/c90/sdf/extracted/index.html
grammar C;

// === Parser rules ===

translationUnit: externalDeclaration+;

externalDeclaration: functionDefinition;

functionDefinition:
	specifier* declarator declaration* '{' declaration* statement* '}';

specifier: typeSpecifier;

typeSpecifier: Identifier | 'void' | 'int';

declarator:
	Identifier							# IdentifierDeclarator
	| Identifier '(' parameters? ')'	# FunctionDeclarator;

parameters: parameter (',' parameter)* (',' '...')? | 'void';

parameter: specifier* declarator;

declaration:
	specifier+ ';'												# DeclarationWithoutInit
	| specifier+ (initDeclarator (',' initDeclarator)*)? ';'	# DeclarationWithInit;

initDeclarator: declarator ('=' initializer)?;

initializer: nonCommaExpression;

nonCommaExpression: expression;

statement: 'return' expression? ';' # ReturnStatement;

// For order of precedence, see https://en.cppreference.com/w/c/language/operator_precedence
expression:
	Identifier											# IdentifierExpression
	| IntegerConstant									# IntegerConstantExpression
	| '(' expression ')'								# ParenthesizedExpression
	| '-' expression									# NegationExpression
	| '!' expression									# LogicalNotExpression
	| '~' expression									# BitwiseNotExpression
	| expression ('*' | '/' | '%') expression			# MultiplicativeExpression
	| expression ('+' | '-') expression					# AdditiveExpression
	| expression ('<' | '<=' | '>' | '>=') expression	# RelationalExpression
	| expression ('==' | '!=') expression				# EqualityExpression
	| expression '&&' expression						# LogicalAndExpression
	| expression '||' expression						# LogicalOrExpression;

// === Lexer rules ===

Whitespace: [ \t\r\n] -> skip;

// Keywords
KeywordReturn: 'return';
KeywordVoid: 'void';
KeywordInt: 'int';

// Punctuations
LeftParen: '(';
RightParen: ')';
LeftBrace: '{';
RightBrace: '}';
Comma: ',';
Semicolon: ';';

// Arithmetic operators
Increment: '++';
Plus: '+';
Decrement: '--';
Minus: '-';
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

// Identifiers and constants
Identifier: [a-zA-Z_][a-zA-Z0-9_]*;
IntegerConstant: '0' | [1-9][0-9]*;
