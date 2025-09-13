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
	| 'sizeof' '(' typeName ')'		# UnarySizeofType;

unaryOperator: '&' | '*' | '+' | '-' | '~' | '!';

castExpression:
	unaryExpression						# CastUnary
	| '(' typeName ')' castExpression	# CastType;

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

expression: assignmentExpression (',' assignmentExpression)*;

constantExpression: conditionalExpression;

declaration: declarationSpecifiers initDeclaratorList? ';';

declarationSpecifiers: (
		storageClassSpecifier
		| typeSpecifier
		| typeQualifier
	)+;

initDeclaratorList: initDeclarator (',' initDeclarator)*;

initDeclarator: declarator ('=' initializer)?;

storageClassSpecifier:
	Typedef
	| Extern
	| Static
	| Auto
	| Register;

typeSpecifier:
	Void
	| Char
	| Short
	| Int
	| Long
	| Float
	| Double
	| Signed
	| Unsigned
	| structOrUnionSpecifier
	| enumSpecifier
	| Identifier;

structOrUnionSpecifier:
	(structOrUnion Identifier? '{' structDeclarationList '}')	# StructDef
	| (structOrUnion Identifier)								# StructRef;

structOrUnion: Struct | Union;

structDeclarationList: structDeclaration+;

structDeclaration:
	specifierQualifierList structDeclaratorList ';';

specifierQualifierList: (typeSpecifier | typeQualifier)+;

structDeclaratorList: structDeclarator (',' structDeclarator)*;

structDeclarator:
	declarator (':' constantExpression)?	# StructDec
	| ':' constantExpression				# StructBitField;

enumSpecifier:
	Enum Identifier? '{' enumeratorList '}'	# EnumDef
	| Enum Identifier						# EnumRef;

enumeratorList: enumerator (',' enumerator)*;

enumerator: Identifier ('=' constantExpression)?;

typeQualifier: Const | Volatile;

declarator: pointer? directDeclarator;

directDeclarator:
	Identifier										# DirId
	| '(' declarator ')'							# DirParen
	| directDeclarator '[' constantExpression? ']'	# DirArray
	| directDeclarator '(' parameterTypeList? ')'	# DirFunc
	| directDeclarator '(' identifierList? ')'		# DirOldFunc;

pointer: '*' typeQualifier* pointer?;

parameterTypeList: parameterList (',' Elipsis)?;

parameterList: parameterDeclaration (',' parameterDeclaration)*;

parameterDeclaration:
	declarationSpecifiers declarator
	| declarationSpecifiers abstractDeclarator
	| declarationSpecifiers;

identifierList: Identifier (',' Identifier)*;

typeName:
	specifierQualifierList
	| specifierQualifierList abstractDeclarator;

abstractDeclarator:
	pointer directAbstractDeclarator?	# AbsPointer
	| directAbstractDeclarator?			# AbsDirect;

directAbstractDeclarator:
	'(' abstractDeclarator ')'								# AbsParen
	| '[' constantExpression? ']'							# AbsArray
	| directAbstractDeclarator '[' constantExpression? ']'	# AbsArrayRec
	| '(' parameterTypeList? ')'							# AbsFunc
	| directAbstractDeclarator '(' parameterTypeList? ')'	# AbsFuncRec;

initializer:
	assignmentExpression				# InitAssign
	| '{' initializerList (',')? '}'	# InitList
	| '{' initializerList ',' '}'		# InitListComma;

initializerList: initializer (',' initializer)*;

statement:
	labeledStatement
	| compoundStatement
	| expressionStatement
	| selectionStatement
	| iterationStatement
	| jumpStatement;

labeledStatement:
	Identifier ':' statement				# LabelId
	| Case constantExpression ':' statement	# LabelCase
	| Default ':' statement					# LabelDefault;

compoundStatement: '{' declaration* statement* '}';

expressionStatement: expression? ';';

selectionStatement:
	'if' '(' expression ')' then = statement (
		'else' else = statement
	)?										# SelIf
	| 'switch' '(' expression ')' statement	# SelSwitch;

iterationStatement:
	'while' '(' expression ')' statement			# IterWhile
	| 'do' statement 'while' '(' expression ')' ';'	# IterDo
	| 'for' '(' init = expressionStatement cond = expressionStatement update = expression? ')'
		statement # IterFor;

jumpStatement:
	Goto Identifier ';'			# JumpGoto
	| Continue ';'				# JumpContinue
	| Break ';'					# JumpBreak
	| Return expression? ';'	# JumpReturn;

translationUnit: externalDeclaration+;

externalDeclaration: functionDefinition | declaration;

functionDefinition:
	declarationSpecifiers? declarator declaration* compoundStatement;

// === Lexer rules ===

fragment D: [0-9];
fragment L: [a-zA-Z_];
fragment H: [a-fA-F0-9];
fragment E: [Ee] [+-]? D+;
fragment FS: [fFlL];
fragment IS: [uUlL];

Comment: '/*' .*? '*/' -> skip;

Auto: 'auto';
Break: 'break';
Case: 'case';
Char: 'char';
Const: 'const';
Continue: 'continue';
Default: 'default';
Do: 'do';
Double: 'double';
Else: 'else';
Enum: 'enum';
Extern: 'extern';
Float: 'float';
For: 'for';
Goto: 'goto';
If: 'if';
Int: 'int';
Long: 'long';
Register: 'register';
Return: 'return';
Short: 'short';
Signed: 'signed';
Sizeof: 'sizeof';
Static: 'static';
Struct: 'struct';
Switch: 'switch';
Typedef: 'typedef';
Union: 'union';
Unsigned: 'unsigned';
Void: 'void';
Volatile: 'volatile';
While: 'while';

Identifier: L (L | D)*;
Constant:
	'0' [xX] H+ IS?
	| '0' D+ IS?
	| D+ IS?
	| L? '\'' ('\\' . | ~['\\]) '\''
	| D+ E FS?
	| D* '.' D* E? FS?
	| D+ '.' D* E? FS?;

StringLiteral: L? '"' ('\\' . | ~["\\])* '"';

Elipsis: '...';
RightAssign: '>>=';
LeftAssign: '<<=';
AddAssign: '+=';
SubAssign: '-=';
MulAssign: '*=';
DivAssign: '/=';
ModAssign: '%=';
AndAssign: '&=';
XorAssign: '^=';
OrAssign: '|=';
RightOp: '>>';
LeftOp: '<<';
IncOp: '++';
DecOp: '--';
PtrOp: '->';
AndOp: '&&';
OrOp: '||';
LeOp: '<=';
GeOp: '>=';
EqOp: '==';
NeOp: '!=';

Whitespace: [ \t\r\n]+ -> channel(HIDDEN);
