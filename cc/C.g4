// C90 grammar, based on https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
grammar C;

// === Parser rules ===

primaryExpression:
	Identifier				# IdPrimary
	| Constant				# ConstPrimary
	| StringLiteral			# StrPrimary
	| '(' expression ')'	# ParenPrimary;

postfixExpression:
	primaryExpression									# PostfixPrimary
	| postfixExpression '[' expression ']'				# PostfixArray
	| postfixExpression '(' argumentExpressionList? ')'	# PostfixFuncCall
	| postfixExpression '.' Identifier					# PostfixMember
	| postfixExpression PtrOp Identifier				# PostfixPtrMember
	| postfixExpression IncOp							# PostfixInc
	| postfixExpression DecOp							# PostfixDec;

argumentExpressionList:
	assignmentExpression (',' assignmentExpression)*;

unaryExpression:
	postfixExpression				# UnaryPostfix
	| IncOp unaryExpression			# UnaryInc
	| DecOp unaryExpression			# UnaryDec
	| unaryOperator castExpression	# UnaryOp
	| 'sizeof' unaryExpression		# UnarySizeofExpr
	/*| 'sizeof' '(' typeName ')'		# UnarySizeofType*/;

unaryOperator: '&' | '*' | '+' | '-' | '~' | '!';

castExpression:
	unaryExpression # CastUnary
	/*| '(' typeName ')' castExpression	# CastType*/;

multiplicativeExpression:
	castExpression									# MulCast
	| multiplicativeExpression '*' castExpression	# MulMul
	| multiplicativeExpression '/' castExpression	# MulDiv
	| multiplicativeExpression '%' castExpression	# MulMod;

additiveExpression:
	multiplicativeExpression							# AddMul
	| additiveExpression '+' multiplicativeExpression	# AddAdd
	| additiveExpression '-' multiplicativeExpression	# AddSub;

shiftExpression:
	additiveExpression								# ShiftAdd
	| shiftExpression LeftOp additiveExpression		# ShiftLeft
	| shiftExpression RightOp additiveExpression	# ShiftRight;

relationalExpression:
	shiftExpression								# RelShift
	| relationalExpression '<' shiftExpression	# RelLT
	| relationalExpression '>' shiftExpression	# RelGT
	| relationalExpression LeOp shiftExpression	# RelLE
	| relationalExpression GeOp shiftExpression	# RelGE;

equalityExpression:
	relationalExpression							# EqRel
	| equalityExpression EqOp relationalExpression	# EqEQ
	| equalityExpression NeOp relationalExpression	# EqNE;

andExpression:
	equalityExpression						# AndEq
	| andExpression '&' equalityExpression	# AndAnd;

exclusiveOrExpression:
	andExpression								# XorAnd
	| exclusiveOrExpression '^' andExpression	# XorXor;

inclusiveOrExpression:
	exclusiveOrExpression								# OrXor
	| inclusiveOrExpression '|' exclusiveOrExpression	# OrOr;

logicalAndExpression:
	inclusiveOrExpression								# LandOr
	| logicalAndExpression AndOp inclusiveOrExpression	# LandAnd;

logicalOrExpression:
	logicalAndExpression							# LorLand
	| logicalOrExpression OrOp logicalAndExpression	# LorOr;

conditionalExpression:
	logicalOrExpression												# CondLor
	| logicalOrExpression '?' expression ':' conditionalExpression	# CondTernary;

assignmentExpression:
	conditionalExpression										# AssignCond
	| unaryExpression assignmentOperator assignmentExpression	# AssignOp;

assignmentOperator:
	'='
	| MulAssign
	| DivAssign
	| ModAssign
	| AddAssign
	| SubAssign
	| LeftAssign
	| RightAssign
	| AndAssign
	| XorAssign
	| OrAssign;

expression:
	assignmentExpression (',' assignmentExpression)*;

// Lexer rules

MulAssign: '*=';
DivAssign: '/=';
ModAssign: '%=';
AddAssign: '+=';
SubAssign: '-=';
LeftAssign: '<<=';
RightAssign: '>>=';
AndAssign: '&=';
XorAssign: '^=';
OrAssign: '|=';

LeftOp: '<<';
RightOp: '>>';
LeOp: '<=';
GeOp: '>=';
EqOp: '==';
NeOp: '!=';
AndOp: '&&';
OrOp: '||';
IncOp: '++';
DecOp: '--';
PtrOp: '->';

Identifier: [a-zA-Z_][a-zA-Z0-9_]*;
Constant: '0' | [1-9][0-9]*;
StringLiteral: '"' (~["\r\n] | '\\"')* '"';
