grammar MyVMAsm;

options { caseInsensitive = true; }

program
    : (statement? NEWLINE)* statement? EOF
    ;

statement
    : sectionDirective
    | constantDefinition
    | legacyConstantDefinition
    | labelDefinition
    | dataDirective
    | instruction
    ;

sectionDirective
    : HASH_SYMBOL
    | HASH_CODE
    | HASH_END
    | DOT_CODE
    | DOT_DATA
    ;

constantDefinition
    : DOT_CONST IDENT EQUAL expression
    ;

legacyConstantDefinition
    : IDENT COLON expression
    ;

labelDefinition
    : IDENT COLON
    ;

instruction
    : IDENT operandList?
    ;

dataDirective
    : DOT_ASCII STRING
    | (DOT_BYTE | DOT_WORD | DOT_DWORD | DOT_QWORD | DOT_SPACE | DOT_ALIGN) operandList?
    ;

operandList
    : operand (COMMA? operand)*
    ;

operand
    : HASH? expression
    ;

expression
    : bitwiseOr
    ;

bitwiseOr
    : bitwiseXor (PIPE bitwiseXor)*
    ;

bitwiseXor
    : bitwiseAnd (CARET bitwiseAnd)*
    ;

bitwiseAnd
    : shiftExpr (AMP shiftExpr)*
    ;

shiftExpr
    : additiveExpr ((LSHIFT | RSHIFT) additiveExpr)*
    ;

additiveExpr
    : multiplicativeExpr ((PLUS | MINUS) multiplicativeExpr)*
    ;

multiplicativeExpr
    : unaryExpr ((STAR | SLASH | PERCENT) unaryExpr)*
    ;

unaryExpr
    : (PLUS | MINUS | TILDE)* primary
    ;

primary
    : NUMBER
    | IDENT
    | DOLLAR IDENT
    | LPAREN expression RPAREN
    ;

HASH_SYMBOL : '#symbol';
HASH_CODE   : '#code';
HASH_END    : '#end';
DOT_CODE    : '.code';
DOT_DATA    : '.data';
DOT_CONST   : '.const';
DOT_BYTE    : '.byte';
DOT_WORD    : '.word';
DOT_DWORD   : '.dword';
DOT_QWORD   : '.qword';
DOT_ASCII   : '.ascii';
DOT_SPACE   : '.space';
DOT_ALIGN   : '.align';

NUMBER
    : '0x' HEX_DIGIT (HEX_DIGIT | '_')*
    | '0b' [01] ([01] | '_')*
    | '0o' [0-7] ([0-7] | '_')*
    | 'd' DIGIT (DIGIT | '_')*
    | DIGIT (DIGIT | [a-f] | '_')*
    ;

IDENT   : [a-z_] [a-z_0-9]*;
STRING  : '"' (ESCAPE | ~["\\\r\n])* '"';
DOLLAR  : '$';
COLON   : ':';
EQUAL   : '=';
COMMA   : ',';
HASH    : '#';
PLUS    : '+';
MINUS   : '-';
STAR    : '*';
SLASH   : '/';
PERCENT : '%';
TILDE   : '~';
PIPE    : '|';
CARET   : '^';
AMP     : '&';
LSHIFT  : '<<';
RSHIFT  : '>>';
LPAREN  : '(';
RPAREN  : ')';
NEWLINE : '\r'? '\n';
LINE_COMMENT : ('//' | ';') ~[\r\n]* -> skip;
WS : [ \t]+ -> skip;
INVALID : .;

fragment DIGIT : [0-9];
fragment HEX_DIGIT : [0-9a-f];
fragment ESCAPE : '\\' [\\"nrt0];
