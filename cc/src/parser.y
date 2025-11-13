%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

/* externs from lexer for location */
extern int line;
extern int column;

/* Build result */
static struct ASTNode *parse_tree = 0;

%}

%union {
	char *str;
	struct ASTNode *node;
	struct ASTList *list;
	struct Declarator *decltor;
	int ival; /* flags, pointer level, etc. */
}

%token <str> IDENTIFIER CONSTANT STRING_LITERAL TYPE_NAME
%token SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node> primary_expression postfix_expression unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression conditional_expression assignment_expression expression constant_expression
%type <node> statement labeled_statement compound_statement selection_statement iteration_statement jump_statement initializer expression_statement
%type <list> argument_expression_list declaration_list statement_list initializer_list parameter_list parameter_type_list
%type <ival> declaration_specifiers storage_class_specifier type_specifier type_qualifier type_qualifier_list pointer type_name abstract_declarator direct_abstract_declarator unary_operator assignment_operator specifier_qualifier_list
%type <node> parameter_declaration
%type <decltor> declarator direct_declarator
%type <node> declaration external_declaration function_definition translation_unit init_declarator init_declarator_list identifier_list

%start translation_unit
%%

primary_expression
	: IDENTIFIER              { $$ = ast_new_expr_ident($1, line, column); free($1); }
	| CONSTANT                { $$ = ast_new_expr_const($1, AST_C_INT, line, column); free($1); }
	| STRING_LITERAL          { $$ = ast_new_expr_string($1, line, column); free($1); }
	| '(' expression ')'      { $$ = $2; }
	;

postfix_expression
	: primary_expression                             { $$ = $1; }
	| postfix_expression '[' expression ']'          { $$ = ast_new_expr_index($1, $3, line, column); }
	| postfix_expression '(' ')'                     { $$ = ast_new_expr_call($1, 0, line, column); }
	| postfix_expression '(' argument_expression_list ')' { $$ = ast_new_expr_call($1, $3, line, column); }
	| postfix_expression '.' IDENTIFIER              { $$ = ast_new_expr_member($1, $3, 0, line, column); free($3); }
	| postfix_expression PTR_OP IDENTIFIER           { $$ = ast_new_expr_member($1, $3, 1, line, column); free($3); }
	| postfix_expression INC_OP                      { $$ = ast_new_expr_unary(OP_POST_INC, $1, line, column); }
	| postfix_expression DEC_OP                      { $$ = ast_new_expr_unary(OP_POST_DEC, $1, line, column); }
	;

argument_expression_list
	: assignment_expression                          { $$ = ast_list_append(0, $1); }
	| argument_expression_list ',' assignment_expression { $$ = ast_list_append($1, $3); }
	;

unary_expression
	: postfix_expression                             { $$ = $1; }
	| INC_OP unary_expression                        { $$ = ast_new_expr_unary(OP_INC, $2, line, column); }
	| DEC_OP unary_expression                        { $$ = ast_new_expr_unary(OP_DEC, $2, line, column); }
	| unary_operator cast_expression                 { $$ = ast_new_expr_unary($1, $2, line, column); }
	| SIZEOF unary_expression                        { $$ = ast_new_expr_unary(OP_SIZEOF, $2, line, column); }
	| SIZEOF '(' type_name ')'                       { $$ = ast_new_expr_unary(OP_SIZEOF, 0, line, column); }
	;

unary_operator
	: '&'    { $$ = '&'; }
	| '*'    { $$ = '*'; }
	| '+'    { $$ = '+'; }
	| '-'    { $$ = '-'; }
	| '~'    { $$ = '~'; }
	| '!'    { $$ = '!'; }
	;

cast_expression
	: unary_expression                                { $$ = $1; }
	| '(' type_name ')' cast_expression               { $$ = ast_new_expr_cast($2, 0, $4, line, column); }
	;

multiplicative_expression
	: cast_expression                                 { $$ = $1; }
	| multiplicative_expression '*' cast_expression   { $$ = ast_new_expr_binary('*', $1, $3, line, column); }
	| multiplicative_expression '/' cast_expression   { $$ = ast_new_expr_binary('/', $1, $3, line, column); }
	| multiplicative_expression '%' cast_expression   { $$ = ast_new_expr_binary('%', $1, $3, line, column); }
	;

additive_expression
	: multiplicative_expression                       { $$ = $1; }
	| additive_expression '+' multiplicative_expression { $$ = ast_new_expr_binary('+', $1, $3, line, column); }
	| additive_expression '-' multiplicative_expression { $$ = ast_new_expr_binary('-', $1, $3, line, column); }
	;

shift_expression
	: additive_expression                             { $$ = $1; }
	| shift_expression LEFT_OP additive_expression    { $$ = ast_new_expr_binary(OP_SHL, $1, $3, line, column); }
	| shift_expression RIGHT_OP additive_expression   { $$ = ast_new_expr_binary(OP_SHR, $1, $3, line, column); }
	;

relational_expression
	: shift_expression                                { $$ = $1; }
	| relational_expression '<' shift_expression      { $$ = ast_new_expr_binary('<', $1, $3, line, column); }
	| relational_expression '>' shift_expression      { $$ = ast_new_expr_binary('>', $1, $3, line, column); }
	| relational_expression LE_OP shift_expression    { $$ = ast_new_expr_binary(OP_LE, $1, $3, line, column); }
	| relational_expression GE_OP shift_expression    { $$ = ast_new_expr_binary(OP_GE, $1, $3, line, column); }
	;

equality_expression
	: relational_expression                           { $$ = $1; }
	| equality_expression EQ_OP relational_expression { $$ = ast_new_expr_binary(OP_EQ, $1, $3, line, column); }
	| equality_expression NE_OP relational_expression { $$ = ast_new_expr_binary(OP_NE, $1, $3, line, column); }
	;

and_expression
	: equality_expression                             { $$ = $1; }
	| and_expression '&' equality_expression          { $$ = ast_new_expr_binary('&', $1, $3, line, column); }
	;

exclusive_or_expression
	: and_expression                                  { $$ = $1; }
	| exclusive_or_expression '^' and_expression      { $$ = ast_new_expr_binary('^', $1, $3, line, column); }
	;

inclusive_or_expression
	: exclusive_or_expression                         { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = ast_new_expr_binary('|', $1, $3, line, column); }
	;

logical_and_expression
	: inclusive_or_expression                         { $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression { $$ = ast_new_expr_binary(OP_LAND, $1, $3, line, column); }
	;

logical_or_expression
	: logical_and_expression                          { $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression { $$ = ast_new_expr_binary(OP_LOR, $1, $3, line, column); }
	;

conditional_expression
	: logical_or_expression                           { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression { $$ = ast_new_expr_cond($1, $3, $5, line, column); }
	;

assignment_expression
	: conditional_expression                          { $$ = $1; }
	| unary_expression assignment_operator assignment_expression { $$ = ast_new_expr_binary($2, $1, $3, line, column); }
	;

assignment_operator
	: '='          { $$ = '='; }
	| MUL_ASSIGN   { $$ = OP_MUL_ASSIGN; }
	| DIV_ASSIGN   { $$ = OP_DIV_ASSIGN; }
	| MOD_ASSIGN   { $$ = OP_MOD_ASSIGN; }
	| ADD_ASSIGN   { $$ = OP_ADD_ASSIGN; }
	| SUB_ASSIGN   { $$ = OP_SUB_ASSIGN; }
	| LEFT_ASSIGN  { $$ = OP_SHL_ASSIGN; }
	| RIGHT_ASSIGN { $$ = OP_SHR_ASSIGN; }
	| AND_ASSIGN   { $$ = OP_AND_ASSIGN; }
	| XOR_ASSIGN   { $$ = OP_XOR_ASSIGN; }
	| OR_ASSIGN    { $$ = OP_OR_ASSIGN; }
	;

expression
	: assignment_expression                           { $$ = $1; }
	| expression ',' assignment_expression           { $$ = $3; }
	;

constant_expression
	: conditional_expression                          { $$ = $1; }
	;

declaration
	: declaration_specifiers ';'                      { $$ = ast_new_decl($1, 0, 0, line, column); }
	| declaration_specifiers init_declarator_list ';' { $$ = $2; if ($$) { $$->u.decl.spec_flags |= $1; } }
	;

declaration_specifiers
	: storage_class_specifier                         { $$ = $1; }
	| storage_class_specifier declaration_specifiers  { $$ = $1 | $2; }
	| type_specifier                                  { $$ = $1; }
	| type_specifier declaration_specifiers           { $$ = $1 | $2; }
	| type_qualifier                                  { $$ = $1; }
	| type_qualifier declaration_specifiers           { $$ = $1 | $2; }
	;

init_declarator_list
	: init_declarator                                 { $$ = $1; }
	| init_declarator_list ',' init_declarator        { $$ = $1; }
	;

init_declarator
	: declarator                                      { $$ = ast_new_decl(0, $1, 0, line, column); }
	| declarator '=' initializer                      { $$ = ast_new_decl(0, $1, $3, line, column); }
	;

storage_class_specifier
	: TYPEDEF   { $$ = SPF_TYPEDEF; }
	| EXTERN    { $$ = SPF_EXTERN; }
	| STATIC    { $$ = SPF_STATIC; }
	| AUTO      { $$ = SPF_AUTO; }
	| REGISTER  { $$ = SPF_REGISTER; }
	;

type_specifier
	: VOID      { $$ = SPF_VOID; }
	| CHAR      { $$ = SPF_CHAR; }
	| SHORT     { $$ = SPF_SHORT; }
	| INT       { $$ = SPF_INT; }
	| LONG      { $$ = SPF_LONG; }
	| FLOAT     { $$ = SPF_FLOAT; }
	| DOUBLE    { $$ = SPF_DOUBLE; }
	| SIGNED    { $$ = SPF_SIGNED; }
	| UNSIGNED  { $$ = SPF_UNSIGNED; }
	| struct_or_union_specifier { $$ = 0; }
	| enum_specifier            { $$ = 0; }
	| TYPE_NAME                 { $$ = 0; }
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list        { $$ = $1 | $2; }
	| type_specifier                                 { $$ = $1; }
	| type_qualifier specifier_qualifier_list        { $$ = $1 | $2; }
	| type_qualifier                                 { $$ = $1; }
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: CONST     { $$ = SPF_CONST; }
	| VOLATILE  { $$ = SPF_VOLATILE; }
	;

declarator
	: pointer direct_declarator                       { ast_set_declarator_pointer($2, $1); $$ = $2; }
	| direct_declarator                               { $$ = $1; }
	;

direct_declarator
	: IDENTIFIER                                      { $$ = ast_new_declarator($1); free($1); }
	| '(' declarator ')'                              { $$ = $2; }
	| direct_declarator '[' constant_expression ']'   { ast_set_declarator_array($1, $3); $$ = $1; }
	| direct_declarator '[' ']'                       { ast_set_declarator_array($1, 0); $$ = $1; }
	| direct_declarator '(' parameter_type_list ')'   { ast_set_declarator_function($1, $3); $$ = $1; }
	| direct_declarator '(' identifier_list ')'       { ast_set_declarator_function($1, 0); $$ = $1; }
	| direct_declarator '(' ')'                       { ast_set_declarator_function($1, 0); $$ = $1; }
	;

pointer
	: '*'                                { $$ = 1; }
	| '*' type_qualifier_list            { $$ = 1; }
	| '*' pointer                        { $$ = 1 + $2; }
	| '*' type_qualifier_list pointer    { $$ = 1 + $3; }
	;

type_qualifier_list
	: type_qualifier                     { $$ = $1; }
	| type_qualifier_list type_qualifier { $$ = $1 | $2; }
	;


parameter_type_list
	: parameter_list                     { $$ = $1; }
	| parameter_list ',' ELLIPSIS        { $$ = $1; }
	;

parameter_list
	: parameter_declaration              { $$ = ast_list_append(0, $1); }
	| parameter_list ',' parameter_declaration { $$ = ast_list_append($1, $3); }
	;

parameter_declaration
	: declaration_specifiers declarator             { $$ = ast_new_decl($1, $2, 0, line, column); }
	| declaration_specifiers abstract_declarator    { $$ = ast_new_decl($1, 0, 0, line, column); }
	| declaration_specifiers                        { $$ = ast_new_decl($1, 0, 0, line, column); }
	;

identifier_list
	: IDENTIFIER                          { free($1); $$ = 0; }
	| identifier_list ',' IDENTIFIER      { free($3); $$ = 0; }
	;

type_name
	: specifier_qualifier_list                       { $$ = $1; }
	| specifier_qualifier_list abstract_declarator   { $$ = $1; }
	;

abstract_declarator
	: pointer                                        { $$ = $1; }
	| direct_abstract_declarator                     { $$ = 0; }
	| pointer direct_abstract_declarator             { $$ = $1; }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'                     { $$ = 0; }
	| '[' ']'                                         { $$ = 0; }
	| '[' constant_expression ']'                     { $$ = 0; }
	| direct_abstract_declarator '[' ']'              { $$ = 0; }
	| direct_abstract_declarator '[' constant_expression ']' { $$ = 0; }
	| '(' ')'                                         { $$ = 0; }
	| '(' parameter_type_list ')'                     { $$ = 0; }
	| direct_abstract_declarator '(' ')'              { $$ = 0; }
	| direct_abstract_declarator '(' parameter_type_list ')' { $$ = 0; }
	;

initializer
	: assignment_expression                            { $$ = $1; }
	| '{' initializer_list '}'                         { $$ = 0; }
	| '{' initializer_list ',' '}'                     { $$ = 0; }
	;

initializer_list
	: initializer                                      { $$ = 0; }
	| initializer_list ',' initializer                 { $$ = 0; }
	;

statement
	: labeled_statement                                { $$ = $1; }
	| compound_statement                               { $$ = $1; }
	| expression_statement                             { $$ = $1; }
	| selection_statement                              { $$ = $1; }
	| iteration_statement                              { $$ = $1; }
	| jump_statement                                   { $$ = $1; }
	;

labeled_statement
	: IDENTIFIER ':' statement                         { $$ = ast_new_stmt_label($1, $3, line, column); free($1); }
	| CASE constant_expression ':' statement           { $$ = ast_new_stmt_case($2, $4, line, column); }
	| DEFAULT ':' statement                            { $$ = ast_new_stmt_default($3, line, column); }
	;

compound_statement
	: '{' '}'                                          { $$ = ast_new_stmt_compound(0, line, column); }
	| '{' statement_list '}'                           { $$ = ast_new_stmt_compound($2, line, column); }
	| '{' declaration_list '}'                         { $$ = ast_new_stmt_compound($2, line, column); }
	| '{' declaration_list statement_list '}'          { $$ = ast_new_stmt_compound(ast_list_concat($2, $3), line, column); }
	;

declaration_list
	: declaration                                      { $$ = ast_list_append(0, $1); }
	| declaration_list declaration                     { $$ = ast_list_append($1, $2); }
	;

statement_list
	: statement                                        { $$ = ast_list_append(0, $1); }
	| statement_list statement                         { $$ = ast_list_append($1, $2); }
	;

expression_statement
	: ';'                                              { $$ = ast_new_stmt_expr(0, line, column); }
	| expression ';'                                   { $$ = ast_new_stmt_expr($1, line, column); }
	;

selection_statement
	: IF '(' expression ')' statement                  { $$ = ast_new_stmt_if($3, $5, 0, line, column); }
	| IF '(' expression ')' statement ELSE statement   { $$ = ast_new_stmt_if($3, $5, $7, line, column); }
	| SWITCH '(' expression ')' statement              { $$ = ast_new_stmt_switch($3, $5, line, column); }
	;

iteration_statement
	: WHILE '(' expression ')' statement               { $$ = ast_new_stmt_while($3, $5, line, column); }
	| DO statement WHILE '(' expression ')' ';'        { $$ = ast_new_stmt_dowhile($2, $5, line, column); }
	| FOR '(' expression_statement expression_statement ')' statement { $$ = ast_new_stmt_for($3, $4, 0, $6, line, column); }
	| FOR '(' expression_statement expression_statement expression ')' statement { $$ = ast_new_stmt_for($3, $4, $5, $7, line, column); }
	;

jump_statement
	: GOTO IDENTIFIER ';'                              { $$ = ast_new_stmt_goto($2, line, column); free($2); }
	| CONTINUE ';'                                     { $$ = ast_new_stmt_continue(line, column); }
	| BREAK ';'                                        { $$ = ast_new_stmt_break(line, column); }
	| RETURN ';'                                       { $$ = ast_new_stmt_return(0, line, column); }
	| RETURN expression ';'                            { $$ = ast_new_stmt_return($2, line, column); }
	;

translation_unit
	: external_declaration                             { $$ = ast_new_stmt_compound(ast_list_append(0, $1), line, column); parse_tree = $$; }
	| translation_unit external_declaration            { $$ = $1; $$->u.stmt.stmts = ast_list_append($$->u.stmt.stmts, $2); parse_tree = $$; }
	;

external_declaration
	: function_definition                              { $$ = $1; }
	| declaration                                      { $$ = ast_new_external($1, 0, line, column); }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement { $$ = ast_new_external(ast_new_decl($1, $2, 0, line, column), $4, line, column); }
	| declaration_specifiers declarator compound_statement { $$ = ast_new_external(ast_new_decl($1, $2, 0, line, column), $3, line, column); }
	| declarator declaration_list compound_statement { $$ = ast_new_external(ast_new_decl(0, $1, 0, line, column), $3, line, column); }
	| declarator compound_statement { $$ = ast_new_external(ast_new_decl(0, $1, 0, line, column), $2, line, column); }
	;

%%
#include <stdio.h>

extern char yytext[];
extern int line;
extern int column;
extern const char *yyfilename;
extern char current_line[];
extern int current_line_len;

static void print_caret_line(FILE *out)
{
	int i, vc;
	int target;
	vc = 0;
	/* Place caret under the offending character (column is 1-based visually) */
	target = column > 0 ? column - 1 : 0;
	for (i = 0; i < current_line_len && vc < target; i++)
	{
		if (current_line[i] == '\t')
		{
			/* keep tabs so caret aligns visually */
			fputc('\t', out);
			vc += 8 - (vc % 8);
		}
		else
		{
			fputc(' ', out);
			vc++;
		}
	}
	fputc('^', out);
	fputc('\n', out);
}

int yyerror(char *s)
{
	const char *fname = yyfilename ? yyfilename : "<stdin>";
	/* error header */
	fprintf(stderr, "%s:%d:%d: error: %s\n", fname, line, column, s);
	/* offending line */
	if (current_line_len > 0)
	{
		/* ensure it ends with a newline for readability */
		fwrite(current_line, 1, (size_t)current_line_len, stderr);
		fputc('\n', stderr);
	}
	/* caret */
	print_caret_line(stderr);
	return 0;
}

/* Accessor for the built AST */
struct ASTNode *parser_get_tree(void) { return parse_tree; }
