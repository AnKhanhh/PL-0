//	EBNF production list
//	----------------------------------------------------------------------------------------------
//	program = 'PROGRAM' ident ';' block '.'
//	block = [	'CONST' ident '=' number  {',' ident '=' number}  ';'	]
//			[	'VAR' ident {',' ident} ';'	]
//			[	'PROCEDURE' ident  [ '(' ['VAR'] ident {';' ['VAR'] ident} ')' ]  ';' block ';'	]
//			'BEGIN' statement {';' statement} 'END'
//	statement = [	ident  [ '[' expression ']' ]  ':=' expression	|
//					'CALL'  ident  [ '(' expression  {',' expression} ')' ]	|
//					'BEGIN'  statement  {';' statement}  'END'	|
//					'IF' condition 'THEN' statement 'ELSE' statement	|
//					'WHILE' condition 'DO' statement	|
//					'FOR'  ident ':=' expression 'TO' expression 'DO'	]
//	condition = 'ODD' expression	|
//				expression	('='|'<'|'<='|'>'|'>='|'<>')	expression
//	factor = ident	{ '[' expression ']' }	|  number  |  '(' expression ')'
//	expression = ['+'|'-'] term  { ('+'|'-')  term }
//	term = factor  { ('*'|'%'|'/')  factor }
//	----------------------------------------------------------------------------------------------


#include "../lexer/lexer.h"

#define S_LEN 1024

typedef struct {
	char name[TOKEN_LENGTH];
	char ident[LEXEME_LENGTH];
} Symbol;    //	can be both terminal and non-terminal

//struct SymbolStack {
//	char symbols[TOKEN_LENGTH];
//	int depth;
//};

int next_symbol(FILE *in, Symbol *sb); // generate a sb from formatted token list, returns 0 on failure
// all parser function have to read next symbol everytime accepting a terminal symbol
int sbcmp(Symbol *sb, char *s);    // strcmp wrapper

int term(FILE *in, Symbol *sb);
int factor(FILE *in, Symbol *sb);
int expression(FILE *in, Symbol *sb);

int term(FILE *in, Symbol *sb) {
	factor(in, sb);
	while ( sbcmp(sb, other_token[TIMES]) ||
			sbcmp(sb, other_token[SLASH]) ||
			sbcmp(sb, other_token[PERCENT])) {
		next_symbol(in, sb);
		factor(in, sb);
	}
	return 1;
}

int factor(FILE *in, Symbol *sb) {
	if ( sbcmp(sb, other_token[IDENT])) {
		next_symbol(in, sb);
		if ( sbcmp(sb, other_token[LBRACK])) {
			next_symbol(in, sb);
			expression(in, sb);
			if ( sbcmp(sb, other_token[RBRACK])) {
				next_symbol(in, sb);
			} else {
				puts("right bracket missing!");
				return 0;
			}
		}
	} else if ( sbcmp(sb, other_token[NUMBER])) {
		next_symbol(in, sb);
	} else if ( sbcmp(sb, other_token[LPARENT])) {
		next_symbol(in, sb);
		expression(in, sb);
		if ( sbcmp(sb, other_token[RPARENT])) {
			next_symbol(in, sb);
		} else {
			puts("right parentheses missing!");
			return 0;
		}
	} else  return 0;
	return 1;
}

int expression(FILE *in, Symbol *sb) {
	if ( sbcmp(sb, other_token[PLUS]) ||
		 sbcmp(sb, other_token[MINUS])) {
		next_symbol(in, sb);
	}
	term(in, sb);
	return 1;
}

int sbcmp(Symbol *sb, char *s) {
	if ( strcmp(sb->name, s) == 0 ) { return 1; }
	else { return 0; }
}

int next_symbol(FILE *in, Symbol *sb) {
	char line[S_LEN];
	if ( fgets(line, sizeof line, in) == NULL) {
		return 0;
	} else {
		strcpy(sb->name, strtok(line, " "));
		if ( sbcmp(sb, other_token[IDENT]) ||
			 sbcmp(sb, other_token[NUMBER])) {
			strcpy(sb->ident, strtok(NULL, " "));
		}
		return 1;
	}
}