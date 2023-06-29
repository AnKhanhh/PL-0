//	production list in EBNF
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
//	TODO: return values of symbol functions are not consistent
//	TODO: add a function to handles error, and another to accept terminal symbols

#include <stdlib.h>
#include "../lexer/lexer.c"

#define SYMBOL_LENGTH 32

typedef union {
	char ident[LEXEME_LENGTH];
	long value;
} SymbolTag;

typedef struct {
	char name[SYMBOL_LENGTH];
	SymbolTag tag;
} Symbol;    //	terminal or non-terminal symbol

//	on recognizing a terminal symbol, returns 1 and call next_symbol()
int accept_terminal(FILE *in, Symbol *sb, int *tc, char *s);
//	reads a Symbol from the token list file, returns 0 on failure
int next_symbol(FILE *in, Symbol *sb, int *tc);

void term(FILE *in, Symbol *sb, int *tc);
void factor(FILE *in, Symbol *sb, int *tc);
int expression(FILE *in, Symbol *sb, int *tc);
int condition(FILE *in, Symbol *sb, int *tc);
int statement(FILE *in, Symbol *sb, int *tc);
int block(FILE *in, Symbol *sb, int *tc);
int program(FILE *in, Symbol *sb, int *tc);

void term(FILE *in, Symbol *sb, int *tc) {
	factor(in, sb, tc);
	while ( accept_terminal(in, sb, tc, tokens[TIMES]) ||
			accept_terminal(in, sb, tc, tokens[SLASH]) ||
			accept_terminal(in, sb, tc, tokens[PERCENT])) {
		factor(in, sb, tc);
	}
}

void factor(FILE *in, Symbol *sb, int *tc) {
	if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
		if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
			expression(in, sb, tc);
			if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
			} else { printf("right bracket missing at %d \n", *tc); }
		}
	} else if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
	} else if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
		expression(in, sb, tc);
		if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
		} else { printf("right parentheses missing at %d \n", *tc); }
	} else { printf("<factor> - illegal production at %d \n", *tc); }
	return 1;
}

int expression(FILE *in, Symbol *sb, int *tc) {
	if ( accept_terminal(in, sb, tc, tokens[PLUS]) || accept_terminal(in, sb, tc, tokens[MINUS])) {
	}
	term(in, sb, tc);
	while ( accept_terminal(in, sb, tc, tokens[PLUS]) || accept_terminal(in, sb, tc, tokens[MINUS])) {
		term(in, sb, tc);
	}
	return 1;
}

int condition(FILE *in, Symbol *sb, int *tc) {
	if ( accept_terminal(in, sb, tc, tokens[ODD])) {
		expression(in, sb, tc);
	} else {
		expression(in, sb, tc);
		if ( accept_terminal(in, sb, tc, tokens[EQU]) || accept_terminal(in, sb, tc, tokens[GTR]) ||
			 accept_terminal(in, sb, tc, tokens[GEQ]) || accept_terminal(in, sb, tc, tokens[NEQ]) ||
			 accept_terminal(in, sb, tc, tokens[LSS]) || accept_terminal(in, sb, tc, tokens[LEQ])) {
			expression(in, sb, tc);
		} else {
			printf("missing comparison operator at %d", *tc);
			return 0;
		}
	}
	return 1;
}

int statement(FILE *in, Symbol *sb, int *tc) {
	if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
		if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
			expression(in, sb, tc);
			if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
			} else {
				printf("missing bracket at %d \n", *tc);
				return 0;
			}
		}
		if ( accept_terminal(in, sb, tc, tokens[ASSIGN])) {
			expression(in, sb, tc);
		} else {
			printf("missing assignment operator at %d", *tc);
		}
	} else if ( accept_terminal(in, sb, tc, tokens[CALL])) {
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
				do {
					expression(in, sb, tc);
				} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
				if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
				} else { printf("missing parentheses at %d \n", *tc); }
			}
		}
	} else if ( accept_terminal(in, sb, tc, tokens[BEGIN])) {
		do {
			statement(in, sb, tc);
		} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
		if ( accept_terminal(in, sb, tc, tokens[END])) {
		} else {
			printf("missing keyword END at %d", *tc);
			return 0;
		}
	} else if ( accept_terminal(in, sb, tc, tokens[IF])) {
		condition(in, sb, tc);
		if ( accept_terminal(in, sb, tc, tokens[THEN])) {
			statement(in, sb, tc);
		} else { printf("missing keyword THEN at %d \n", *tc); }
		if ( accept_terminal(in, sb, tc, tokens[ELSE])) {
			statement(in, sb, tc);
		}
	} else if ( accept_terminal(in, sb, tc, tokens[WHILE])) {
		condition(in, sb, tc);
		if ( accept_terminal(in, sb, tc, tokens[DO])) {
			statement(in, sb, tc);
		} else {
			printf(" missing keyword DO at %d \n", *tc);
			return 0;
		}
	} else if ( accept_terminal(in, sb, tc, tokens[FOR])) {
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			if ( accept_terminal(in, sb, tc, tokens[ASSIGN])) {
				expression(in, sb, tc);
				if ( accept_terminal(in, sb, tc, tokens[TO])) {
					expression(in, sb, tc);
					if ( accept_terminal(in, sb, tc, tokens[DO])) {
						statement(in, sb, tc);
					} else { printf("missing keyword TO at %d \n", *tc); }
				} else { printf("missing keyword TO at %d \n", *tc); }
			} else { printf("missing assignment operator at %d \n", *tc); }
		} else { printf("missing iterator at %d \n", *tc); }
	}
	return 1;
}

int block(FILE *in, Symbol *sb, int *tc) {
	if ( accept_terminal(in, sb, tc, tokens[CONST])) {
		do {
			if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
				if ( accept_terminal(in, sb, tc, tokens[EQU])) {
					if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {}
					else { printf("missing CONST value at %d \n", *tc); }
				} else { printf("missing equal operator at %d \n", *tc); }
			} else { printf("missing CONST name at %d \n", *tc); }
		} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
		if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
		} else { printf("missing semicolon at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[VAR])) {
		do {
			if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
				if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
					if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
						if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
						} else { printf("missing bracket at %d \n", *tc); }
					} else { printf("missing array index at %d \n", *tc); }
				}
			} else { printf("missing variable name at %d \n", *tc); }
		} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
		if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
		} else { printf("semicolon missing at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[PROCEDURE])) {
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
				do {
					if ( accept_terminal(in, sb, tc, tokens[VAR])) {
					}
					if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
					} else { printf("variable name missing at %d \n", *tc); }
				} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
				if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
				} else { printf("parentheses missing at %d \n", *tc); }
			}
			if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				block(in, sb, tc);
				if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				} else { printf("semicolon missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("procedure name missing at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[BEGIN])) {
		do {
			statement(in, sb, tc);
		} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
		if ( accept_terminal(in, sb, tc, tokens[END])) {
			return 1;
		} else { printf("keyword END missing at %d \n", *tc); }
		return 1;
	} else { printf("keyword BEGIN missing at %d \n", *tc); }
	return 0;
}

int program(FILE *in, Symbol *sb, int *tc) {
	if ( accept_terminal(in, sb, tc, tokens[PROGRAM])) {
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				block(in, sb, tc);
				if ( accept_terminal(in, sb, tc, tokens[PERIOD])) {
					return 1;
				} else { printf("program terminating symbol missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("program name missing at %d \n", *tc); }
	} else { printf("keyword PROGRAM missing at %d \n", *tc); }
	return 0;
}

int accept_terminal(FILE *in, Symbol *sb, int *tc, char *s) {
	if ( !strcmp(sb->name, s)) {
		next_symbol(in, sb, tc);
		return 1;
	} else { return 0; }
}

int next_symbol(FILE *in, Symbol *sb, int *tc) {
	char line[1024];
	if ( fgets(line, sizeof line, in) == NULL) {
		puts("end of token list reached!");
		return 0;
	}
	memset(sb, 0, sizeof(*sb));
	strncpy(sb->name, strtok(line, " "), SYMBOL_LENGTH);
	( *tc )++;    //	increment the token counter while parsing through token list
	if ( !strcmp(sb->name, tokens[IDENT])) {
		strncpy(sb->tag.ident, strtok(NULL, " \n"), LEXEME_LENGTH);
	} else if ( !strcmp(sb->name, tokens[NUMBER])) {
		strtol(strtok(NULL, " \n"), NULL, 10);
	}
	return 1;
}
