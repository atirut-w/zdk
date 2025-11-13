D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "y.tab.h"

/* ANSI C90 prototypes */
void count(void);
int yywrap(void);
void comment(void);
int check_type(void);
void handle_linemarker(void);

/* Error reporting state (C90-compatible) */
int line = 1;
int column = 0;
char current_line[1024];
int current_line_len = 0;
const char *yyfilename = 0; /* Set by the driver (main) */

/* Track and own current filename set by linemarkers */
static char *owned_filename = 0;

/* expose yylval for setting semantic values */
extern YYSTYPE yylval;
extern int yyleng;
extern char *yytext;

/* C90-safe strdup */
static char *xstrdup(const char *s) {
	size_t n;
	char *d;
	if (!s) return 0;
	n = strlen(s);
	d = (char*)malloc(n + 1);
	if (!d) return 0;
	memcpy(d, s, n + 1);
	return d;
}

%}

%%

^[ \t]*#[ \t]*{D}+[^\n]*\n?    { handle_linemarker(); }

"/*"			{ comment(); }

"auto"			{ count(); return(AUTO); }
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"char"			{ count(); return(CHAR); }
"const"			{ count(); return(CONST); }
"continue"		{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"enum"			{ count(); return(ENUM); }
"extern"		{ count(); return(EXTERN); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"goto"			{ count(); return(GOTO); }
"if"			{ count(); return(IF); }
"int"			{ count(); return(INT); }
"long"			{ count(); return(LONG); }
"register"		{ count(); return(REGISTER); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"			{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"static"		{ count(); return(STATIC); }
"struct"		{ count(); return(STRUCT); }
"switch"		{ count(); return(SWITCH); }
"typedef"		{ count(); return(TYPEDEF); }
"union"			{ count(); return(UNION); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"volatile"		{ count(); return(VOLATILE); }
"while"			{ count(); return(WHILE); }

{L}({L}|{D})*		{ count(); yylval.str = xstrdup(yytext); return(check_type()); }

0[xX]{H}+{IS}?		{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }
0{D}+{IS}?		{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }
{D}+{IS}?		{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }

{D}+{E}{FS}?		{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ count(); yylval.str = xstrdup(yytext); return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ count(); yylval.str = xstrdup(yytext); return(STRING_LITERAL); }

"..."			{ count(); return(ELLIPSIS); }
">>="			{ count(); return(RIGHT_ASSIGN); }
"<<="			{ count(); return(LEFT_ASSIGN); }
"+="			{ count(); return(ADD_ASSIGN); }
"-="			{ count(); return(SUB_ASSIGN); }
"*="			{ count(); return(MUL_ASSIGN); }
"/="			{ count(); return(DIV_ASSIGN); }
"%="			{ count(); return(MOD_ASSIGN); }
"&="			{ count(); return(AND_ASSIGN); }
"^="			{ count(); return(XOR_ASSIGN); }
"|="			{ count(); return(OR_ASSIGN); }
">>"			{ count(); return(RIGHT_OP); }
"<<"			{ count(); return(LEFT_OP); }
"++"			{ count(); return(INC_OP); }
"--"			{ count(); return(DEC_OP); }
"->"			{ count(); return(PTR_OP); }
"&&"			{ count(); return(AND_OP); }
"||"			{ count(); return(OR_OP); }
"<="			{ count(); return(LE_OP); }
">="			{ count(); return(GE_OP); }
"=="			{ count(); return(EQ_OP); }
"!="			{ count(); return(NE_OP); }
";"			{ count(); return(';'); }
("{"|"<%")		{ count(); return('{'); }
("}"|"%>")		{ count(); return('}'); }
"," 			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
("["|"<:")		{ count(); return('['); }
("]"|":>")		{ count(); return(']'); }
"."			{ count(); return('.'); }
"&"			{ count(); return('&'); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"-"			{ count(); return('-'); }
"+"			{ count(); return('+'); }
"*"			{ count(); return('*'); }
"/"			{ count(); return('/'); }
"%"			{ count(); return('%'); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }
"^"			{ count(); return('^'); }
"|"			{ count(); return('|'); }
"?"			{ count(); return('?'); }

[ \t\v\n\f]		{ count(); }
.			{ /* ignore bad characters */ }

%%

int yywrap(void)
{
	return 1;
}


void comment(void)
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0)
	{
		if (c == '\n')
		{
			current_line_len = 0;
			current_line[0] = '\0';
			line++;
			column = 0;
		}
		else if (c == '\t')
		{
			int add = 8 - (column % 8);
			column += add;
			if (current_line_len < (int)sizeof(current_line) - 1)
			{
				current_line[current_line_len++] = '\t';
				current_line[current_line_len] = '\0';
			}
		}
		else
		{
			column++;
			if (current_line_len < (int)sizeof(current_line) - 1)
			{
				current_line[current_line_len++] = c;
				current_line[current_line_len] = '\0';
			}
		}
	}

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}
	if (c != 0)
	{
		/* account for the '/' closing the comment */
		column++;
		if (current_line_len < (int)sizeof(current_line) - 1)
		{
			current_line[current_line_len++] = '/';
			current_line[current_line_len] = '\0';
		}
	}
}

void count(void)
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
	{
		char ch = yytext[i];
		if (ch == '\n')
		{
			current_line_len = 0;
			current_line[0] = '\0';
			line++;
			column = 0;
		}
		else if (ch == '\t')
		{
			int add = 8 - (column % 8);
			column += add;
			if (current_line_len < (int)sizeof(current_line) - 1)
			{
				current_line[current_line_len++] = '\t';
				current_line[current_line_len] = '\0';
			}
		}
		else
		{
			column++;
			if (current_line_len < (int)sizeof(current_line) - 1)
			{
				current_line[current_line_len++] = ch;
				current_line[current_line_len] = '\0';
			}
		}
	}
}

int check_type(void)
{
/*
* pseudo code --- this is what it should check
*
*    if (yytext == type_name)
*        return(TYPE_NAME);
*
*    return(IDENTIFIER);
*/

/*
*    it actually will only return IDENTIFIER
*/

	return(IDENTIFIER);
}

/*
 * Handle GCC-style "linemarker" lines that look like:
 *   # linenum "filename" flags
 * Flags (1=new file, 2=returning to file, 3=system header, 4=extern "C").
 * We treat 3/4 as no-ops. We update 'line' and current filename.
 */
void handle_linemarker(void)
{
	const char *p = yytext;
	const char *end = yytext + yyleng;
	long n = 0;
	int has_filename = 0;
	char *new_filename = 0;

	/* Match leading spaces and '#'. */
	while (p < end && (*p == ' ' || *p == '\t')) p++;
	if (p < end && *p == '#') p++; else goto done;
	while (p < end && (*p == ' ' || *p == '\t')) p++;

	/* Parse line number */
	if (p < end && *p >= '0' && *p <= '9') {
		char *q;
		n = strtol((char*)p, &q, 10);
		if ((const char*)q == p) goto done; /* no digits parsed */
		p = q;
	} else {
		goto done;
	}

	/* Optional whitespace */
	while (p < end && (*p == ' ' || *p == '\t')) p++;

	/* Optional filename in quotes */
	if (p < end && *p == '"') {
		const char *start;
		const char *q;
		start = ++p; /* after opening quote */
		q = p;
		while (q < end) {
			if (*q == '\\') {
				/* skip escaped char */
				q++;
				if (q < end) q++;
				else break;
			} else if (*q == '"') {
				break;
			} else if (*q == '\n' || *q == '\r') {
				/* malformed; bail */
				break;
			} else {
				q++;
			}
		}
		if (q < end && *q == '"') {
			size_t len = (size_t)(q - start);
			char *buf = (char*)malloc(len + 1);
			size_t i = 0;
			if (buf) {
				/* copy, handling simple backslash escapes */
				const char *s = start;
				while (s < q) {
					if (*s == '\\' && (s + 1) < q) {
						s++;
					}
					buf[i++] = *s++;
				}
				buf[i] = '\0';
				new_filename = buf;
				has_filename = 1;
			}
			p = q + 1; /* after closing quote */
		}
	}

	/* Skip whitespace */
	while (p < end && (*p == ' ' || *p == '\t')) p++;

	/* Parse optional flags (1,2,3,4...) separated by spaces). We only care about 1,2. */
	{
		int flag_new = 0;
		int flag_return = 0;
		while (p < end && *p >= '0' && *p <= '9') {
			char *q;
			long flag = strtol((char*)p, &q, 10);
			if ((const char*)q == p) break;
			if (flag == 1) flag_new = 1;
			else if (flag == 2) flag_return = 1;
			/* flags 3 and 4 are ignored */
			p = q;
			while (p < end && (*p == ' ' || *p == '\t')) p++;
		}

		/* Update filename: if a filename was provided, use it. */
		if (has_filename) {
			if (owned_filename) {
				free(owned_filename);
				owned_filename = 0;
			}
			owned_filename = new_filename; /* take ownership */
			new_filename = 0;
			yyfilename = owned_filename;
		}

		/* Update line number to apply to the following line of code. */
		if (n > 0 && n < 2147483647L) {
			line = (int)n;
		}
		column = 0;
		current_line_len = 0;
		current_line[0] = '\0';
	}

done:
	if (new_filename) free(new_filename);
}
