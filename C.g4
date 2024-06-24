grammar C;

translationUnit
    : externalDeclaration*
    ;

externalDeclaration
    : lineMarker
    ;

lineMarker
    : HASH DEC_LITERAL STRING_LITERAL DEC_LITERAL*
    ;

WHITESPACE: [ \t\n\r]+ -> skip;
HASH: '#';

DEC_LITERAL: [0-9]+;
STRING_LITERAL: '"' ('\\' . | .)*? '"';
