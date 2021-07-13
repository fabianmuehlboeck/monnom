lexer grammar NominalGradualLexer;
options {}
@header
{
	#pragma warning disable 3021
}

fragment SMALLLETTER: 'a'..'z';
fragment CAPSLETTER: 'A'..'Z';
fragment LETTER: (SMALLLETTER | CAPSLETTER);
fragment POSDIGIT: '0'..'9';
fragment DIGIT: ('0' | POSDIGIT);
fragment ALPHANUMERIC: (LETTER | DIGIT);
fragment WSCHAR: (' ' | '\t' | '\r' | '\n');

STRING : '"' ( '\\"' | . )*? '"' ;
FLOAT: ('0' | POSDIGIT DIGIT*)? '.' DIGIT+;
INT : '0' | POSDIGIT DIGIT*;
TRUE : 'true';
FALSE : 'false';

USING: 'using';
ABSTRACT: 'abstract';
PARTIAL: 'partial';
EXPANDO: 'expando';
READONLY: 'readonly';
NAMESPACE: 'namespace';
INTERFACE : 'interface';
CLASS : 'class';
INSTANCE : 'instance';
EXTENDS : 'extends';
IMPLEMENTS : 'implements';
DYN : 'dyn';
STATIC : 'static';
FINAL : 'final';
NUL : 'null';
VIRTUAL : 'virtual';
OVERRIDE : 'override';
MULTI: 'multi';
DEFAULT: 'default';
SHAPE: 'shape';
MATERIAL: 'material';
IN: 'in';
OUT: 'out';
INOUT: 'inout';
CALLTARGET: 'calltarget';

PUBLIC: 'public';
PRIVATE: 'private';
PROTECTED: 'protected';
INTERNAL: 'internal';
INTERNAL_PROT: ('internal' WSCHAR+ 'protected')|('protected' WSCHAR+ 'internal');

IFNULL: 'ifnull';
IFOBJ : 'ifobj';
IF: 'if';
ELSE: 'else';
ELSEIF: 'elseif';
WHILE: 'while';
FOR: 'for';
FOREACH: 'foreach';
RETURN: 'return';
BREAK: 'break';
CONTINUE: 'continue';
FUN: 'fun';
SUPER: 'super';
CONSTRUCT: 'constructor';
NEW: 'new';
LET: 'let';
LETVAR: 'letvar';
DBG: 'DEBUG';
ERR: 'ERROR';
RUNTIMECMD: 'RUNTIMECMD';

SEMICOLON: ';';
COLONCOLON : '::';
COLON : ':';
DOT: '.';
DOTDOT: '..';
COMMA: ',';
LPAREN: '(';
RPAREN: ')';
LBRACKET: '[';
RBRACKET: ']';
LBRACE: '{';
RBRACE: '}';
LANGLE: '<';
RANGLE: '>';
ARROW: '->';
QMARK: '?';

AND: '&&';
OR: '||';
APPEND: '++';
PLUS: '+';
MINUS: '-';
SLASH: '/';
TIMES: '*';
POW: '**';
PERCENT: '%';
EQ: '=';
EQEQ: '==';
NEQ: '!=';
EQEQEQ: '===';
LEQ: '<=';
GEQ: '>=';
BITXOR: '^';
BITAND : '&';
BITOR : '|';
PLUSEQ : '+=';
MINUSEQ : '-=';
TIMESEQ : '*=';
DIVEQ : '/=';
/*BITSHL : '<<';
BITSHR : '>>';*/
BIGARROW : '=>';


BANG: '!';

ID: ('_' | LETTER) ('_' | ALPHANUMERIC)*;

WS: WSCHAR+ -> skip;
COMMENT: '#' .*? '\n' -> skip;