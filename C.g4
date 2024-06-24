grammar C;

translationUnit
    : externalDeclaration*
    ;

externalDeclaration
    : lineMarker
    ;

lineMarker
    : HASH decLiteral stringLiteral decLiteral (',' decLiteral)? '\n'
    ;

stringLiteral
    : DQUOTE ( ~DQUOTE | '\\' DQUOTE )* DQUOTE
    ;

decLiteral
    : DEC_DIGIT+
    ;

WHITESPACE: [ \t\n\r]+ -> skip;
HASH: '#';
DEC_DIGIT: [0-9];
DQUOTE: '"';
SQUOTE: '\'';
