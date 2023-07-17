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

#include <stdlib.h>
#include "../semanlyzr/symbol_stack.h"

#define SYMBOL_LENGTH 32

typedef struct {
	char name[SYMBOL_LENGTH];
	union {
		char ident[LEXEME_LENGTH];
		long long value;
	} tag;
} Symbol;    //	terminal or non-terminal

//	on recognizing a terminal symbol, returns 1 and call next_symbol()
int accept_terminal(FILE *in, Symbol *sb, int *tc, char *s);
//	reads a Symbol from the token list file, returns 0 on failure
int next_symbol(FILE *in, Symbol *sb, int *tc);

void term(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);
void factor(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);
void expression(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);
void condition(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);
void statement(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);
void block(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);
void program(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset);

void term(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	factor(in, sb, tc, pvs, offset);
	while ( accept_terminal(in, sb, tc, tokens[TIMES]) ||
			accept_terminal(in, sb, tc, tokens[SLASH]) ||
			accept_terminal(in, sb, tc, tokens[PERCENT])) {
		factor(in, sb, tc, pvs, offset);
	}
}

void factor(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	Symbol sb_holder = *sb;
	if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
		if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
			semantic_evl(*tc - 2, pvs, offset, sb_holder.tag.ident, true);
			expression(in, sb, tc, pvs, offset);
			if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
			} else { printf("right bracket missing at %d \n", *tc); }
		} else { semantic_evl(*tc - 1, pvs, offset, sb_holder.tag.ident, false); }
	} else if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
	} else if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
		expression(in, sb, tc, pvs, offset);
		if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
		} else { printf("right parentheses missing at %d \n", *tc); }
	} else { printf("<factor> - illegal production at %d \n", *tc); }
}

void expression(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	if ( accept_terminal(in, sb, tc, tokens[PLUS]) || accept_terminal(in, sb, tc, tokens[MINUS])) {
	}
	term(in, sb, tc, pvs, offset);
	while ( accept_terminal(in, sb, tc, tokens[PLUS]) || accept_terminal(in, sb, tc, tokens[MINUS])) {
		term(in, sb, tc, pvs, offset);
	}
}

void condition(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	if ( accept_terminal(in, sb, tc, tokens[ODD])) {
		expression(in, sb, tc, pvs, offset);
	} else {
		expression(in, sb, tc, pvs, offset);
		if ( accept_terminal(in, sb, tc, tokens[EQU]) || accept_terminal(in, sb, tc, tokens[GTR]) ||
			 accept_terminal(in, sb, tc, tokens[GEQ]) || accept_terminal(in, sb, tc, tokens[NEQ]) ||
			 accept_terminal(in, sb, tc, tokens[LSS]) || accept_terminal(in, sb, tc, tokens[LEQ])) {
			expression(in, sb, tc, pvs, offset);
		} else { printf("missing comparison operator at %d", *tc); }
	}
}

void statement(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	Symbol sb_holder = *sb;
	if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
		if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
			semantic_evk(*tc - 2, pvs, offset, sb_holder.tag.ident, ARR);
			expression(in, sb, tc, pvs, offset);
			if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
			} else { printf("missing bracket at %d \n", *tc); }
		} else { semantic_evk(*tc - 1, pvs, offset, sb_holder.tag.ident, MUTE); }
		if ( accept_terminal(in, sb, tc, tokens[ASSIGN])) {
			expression(in, sb, tc, pvs, offset);
		} else { printf("missing assignment operator at %d", *tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[CALL])) {
		sb_holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			semantic_evk(*tc - 1, pvs, offset, sb_holder.tag.ident, FNC);
			if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
				do {
					expression(in, sb, tc, pvs, offset);
				} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
				if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
				} else { printf("missing parentheses at %d \n", *tc); }
			}
		}
	} else if ( accept_terminal(in, sb, tc, tokens[BEGIN])) {
		do {
			statement(in, sb, tc, pvs, offset);
		} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
		if ( accept_terminal(in, sb, tc, tokens[END])) {
		} else { printf("missing keyword END at %d", *tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[IF])) {
		condition(in, sb, tc, pvs, offset);
		if ( accept_terminal(in, sb, tc, tokens[THEN])) {
			statement(in, sb, tc, pvs, offset);
		} else { printf("missing keyword THEN at %d \n", *tc); }
		if ( accept_terminal(in, sb, tc, tokens[ELSE])) {
			statement(in, sb, tc, pvs, offset);
		}
	} else if ( accept_terminal(in, sb, tc, tokens[WHILE])) {
		condition(in, sb, tc, pvs, offset);
		if ( accept_terminal(in, sb, tc, tokens[DO])) {
			statement(in, sb, tc, pvs, offset);
		} else { printf(" missing keyword DO at %d \n", *tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[FOR])) {
		sb_holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			semantic_evk(*tc - 1, pvs, offset, sb_holder.tag.ident, MUTE);
			if ( accept_terminal(in, sb, tc, tokens[ASSIGN])) {
				expression(in, sb, tc, pvs, offset);
				if ( accept_terminal(in, sb, tc, tokens[TO])) {
					expression(in, sb, tc, pvs, offset);
					if ( accept_terminal(in, sb, tc, tokens[DO])) {
						statement(in, sb, tc, pvs, offset);
					} else { printf("missing keyword TO at %d \n", *tc); }
				} else { printf("missing keyword TO at %d \n", *tc); }
			} else { printf("missing assignment operator at %d \n", *tc); }
		} else { printf("missing iterator at %d \n", *tc); }
	}
}

void block(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	int cur_offset = offset;
	Symbol sb_holder;
	if ( accept_terminal(in, sb, tc, tokens[CONST])) {
		do {
			sb_holder = *sb;
			if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
				semantic_dcl(*tc - 1, pvs, offset, &cur_offset, sb_holder.tag.ident, IMM);
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
			sb_holder = *sb;
			if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
				if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
					semantic_dcl(*tc - 2, pvs, offset, &cur_offset, sb_holder.tag.ident, ARR);
					if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
						if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
						} else { printf("missing bracket at %d \n", *tc); }
					} else { printf("missing array index at %d \n", *tc); }
				} else {
					semantic_dcl(*tc - 1, pvs, offset, &cur_offset, sb_holder.tag.ident, MUTE);
				}
			} else { printf("missing variable name at %d \n", *tc); }
		} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
		if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
		} else { printf("semicolon missing at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[PROCEDURE])) {
		sb_holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			semantic_dcl(*tc - 1, pvs, offset, &cur_offset, sb_holder.tag.ident, FNC);
			if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
				do {
					if ( accept_terminal(in, sb, tc, tokens[VAR])) {}
					sb_holder = *sb;
					if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
						semantic_dcl(*tc - 1, pvs, offset, &cur_offset, sb_holder.tag.ident, MUTE);
					} else { printf("variable name missing at %d \n", *tc); }
				} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
				if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
				} else { printf("parentheses missing at %d \n", *tc); }
			}
			if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				block(in, sb, tc, pvs, cur_offset);
				if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				} else { printf("semicolon missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("procedure name missing at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[BEGIN])) {
		do {
			statement(in, sb, tc, pvs, cur_offset);
		} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
		if ( accept_terminal(in, sb, tc, tokens[END])) {
		} else { printf("keyword END missing at %d \n", *tc); }
	} else { printf("keyword BEGIN missing at %d \n", *tc); }
}

void program(FILE *in, Symbol *sb, int *tc, VarStack (*pvs), int offset) {
	int cur_offset = offset;
	Symbol sb_holder;
	if ( accept_terminal(in, sb, tc, tokens[PROGRAM])) {
		sb_holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			push_vs(pvs, &cur_offset, PRG, sb_holder.tag.ident);
			if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				block(in, sb, tc, pvs, cur_offset);
				if ( accept_terminal(in, sb, tc, tokens[PERIOD])) {
				} else { printf("program terminating symbol missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("program name missing at %d \n", *tc); }
	} else { printf("keyword PROGRAM missing at %d \n", *tc); }
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
		strtoll(strtok(NULL, " \n"), NULL, 10);
	}
	return 1;
}
