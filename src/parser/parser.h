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


#include "../lexer/lexer.c"

typedef struct {
	char name[TOKEN_LENGTH];
	char ident[LEXEME_LENGTH];
} Symbol;    //	represents either a terminal or non-terminal symbol

//struct SymbolStack {
//	Symbol symbols[TOKEN_LENGTH];
//	int depth;
//};

int next_symbol(FILE *in, Symbol *sb, int *tc); // generate a sb from formatted token list, returns 0 on failure
// all parser function have to read next symbol everytime accepting a terminal symbol
int sy_cmp(Symbol *sb, char *s);    // strcmp wrapper

int term(FILE *in, Symbol *sb, int *tc);
int factor(FILE *in, Symbol *sb, int *tc);
int expression(FILE *in, Symbol *sb, int *tc);
int condition(FILE *in, Symbol *sb, int *tc);
int statement(FILE *in, Symbol *sb, int *tc);
int block(FILE *in, Symbol *sb, int *tc);
int program(FILE *in, Symbol *sb, int *tc);

int term(FILE *in, Symbol *sb, int *tc) {
	factor(in, sb, tc);
	while ( sy_cmp(sb, tokens[TIMES]) || sy_cmp(sb, tokens[SLASH]) || sy_cmp(sb, tokens[PERCENT])) {
		next_symbol(in, sb, tc);
		factor(in, sb, tc);
	}
	return 1;
}

int factor(FILE *in, Symbol *sb, int *tc) {
	if ( sy_cmp(sb, tokens[IDENT])) {
		next_symbol(in, sb, tc);
		if ( sy_cmp(sb, tokens[LBRACK])) {
			next_symbol(in, sb, tc);
			expression(in, sb, tc);
			if ( sy_cmp(sb, tokens[RBRACK])) {
				next_symbol(in, sb, tc);
			} else {
				printf("right bracket missing at %d \n", *tc);
				return 0;
			}
		}
	} else if ( sy_cmp(sb, tokens[NUMBER])) {
		next_symbol(in, sb, tc);
	} else if ( sy_cmp(sb, tokens[LPARENT])) {
		next_symbol(in, sb, tc);
		expression(in, sb, tc);
		if ( sy_cmp(sb, tokens[RPARENT])) {
			next_symbol(in, sb, tc);
		} else {
			printf("right parentheses missing at %d \n", *tc);
			return 0;
		}
	} else {
		printf("<factor> - illegal production at %d \n", *tc);
		return 0;
	}
	return 1;
}

int expression(FILE *in, Symbol *sb, int *tc) {
	if ( sy_cmp(sb, tokens[PLUS]) || sy_cmp(sb, tokens[MINUS])) { next_symbol(in, sb, tc); }
	term(in, sb, tc);
	while ( sy_cmp(sb, tokens[PLUS]) || sy_cmp(sb, tokens[MINUS])) {
		next_symbol(in, sb, tc);
		term(in, sb, tc);
	}
	return 1;
}

int condition(FILE *in, Symbol *sb, int *tc) {
	if ( sy_cmp(sb, tokens[ODD])) {
		next_symbol(in, sb, tc);
		expression(in, sb, tc);
	} else {
		expression(in, sb, tc);
		if ( sy_cmp(sb, tokens[EQU]) || sy_cmp(sb, tokens[GTR]) || sy_cmp(sb, tokens[GEQ]) ||
			 sy_cmp(sb, tokens[NEQ]) || sy_cmp(sb, tokens[LSS]) || sy_cmp(sb, tokens[LEQ])) {
			next_symbol(in, sb, tc);
			expression(in, sb, tc);
		} else {
			printf("missing comparison operator at %d", *tc);
			return 0;
		}
	}
	return 1;
}

int statement(FILE *in, Symbol *sb, int *tc) {
	if ( sy_cmp(sb, tokens[IDENT])) {
		next_symbol(in, sb, tc);
		if ( sy_cmp(sb, tokens[RBRACK])) {
			next_symbol(in, sb, tc);
			expression(in, sb, tc);
			if ( sy_cmp(sb, tokens[LBRACK])) {
				next_symbol(in, sb, tc);
			} else {
				printf("missing bracket at %d \n", *tc);
				return 0;
			}
		}
		if ( sy_cmp(sb, tokens[ASSIGN])) {
			next_symbol(in, sb, tc);
			expression(in, sb, tc);
		} else {
			printf("missing assignment operator at %d", *tc);
			return 0;
		}
	} else if ( sy_cmp(sb, tokens[CALL])) {
		next_symbol(in, sb, tc);
		if ( sy_cmp(sb, tokens[IDENT])) {
			next_symbol(in, sb, tc);
			if ( sy_cmp(sb, tokens[LPARENT])) {
				do {
					next_symbol(in, sb, tc);
					expression(in, sb, tc);
				} while ( sy_cmp(sb, tokens[COMMA]));
				if ( sy_cmp(sb, tokens[RPARENT])) {
					next_symbol(in, sb, tc);
				} else {
					printf("missing parentheses at %d \n", *tc);
					return 0;
				}
			}
		}
	} else if ( sy_cmp(sb, tokens[BEGIN])) {
		do {
			next_symbol(in, sb, tc);
			statement(in, sb, tc);
		} while ( sy_cmp(sb, tokens[SEMICOLON]));
		if ( sy_cmp(sb, tokens[END])) {
			next_symbol(in, sb, tc);
		} else {
			printf("missing keyword END at %d", *tc);
			return 0;
		}
	} else if ( sy_cmp(sb, tokens[IF])) {
		next_symbol(in, sb, tc);
		condition(in, sb, tc);
		if ( sy_cmp(sb, tokens[THEN])) {
			next_symbol(in, sb, tc);
			statement(in, sb, tc);
		} else {
			printf("missing keyword THEN at %d \n", *tc);
			return 0;
		}
		if ( sy_cmp(sb, tokens[ELSE])) {
			next_symbol(in, sb, tc);
			statement(in, sb, tc);
		}
	} else if ( sy_cmp(sb, tokens[WHILE])) {
		next_symbol(in, sb, tc);
		condition(in, sb, tc);
		if ( sy_cmp(sb, tokens[DO])) {
			next_symbol(in, sb, tc);
			statement(in, sb, tc);
		} else {
			printf(" missing keyword DO at %d \n", *tc);
			return 0;
		}
	} else if ( sy_cmp(sb, tokens[FOR])) {
		next_symbol(in, sb, tc);
		if ( sy_cmp(sb, tokens[IDENT])) {
			next_symbol(in, sb, tc);
			if ( sy_cmp(sb, tokens[ASSIGN])) {
				next_symbol(in, sb, tc);
				expression(in, sb, tc);
				if ( sy_cmp(sb, tokens[TO])) {
					next_symbol(in, sb, tc);
					expression(in, sb, tc);
					if ( sy_cmp(sb, tokens[DO])) {
						next_symbol(in, sb, tc);
						statement(in, sb, tc);
					} else {
						printf("missing keyword TO at %d \n", *tc);
						return 0;
					}
				} else {
					printf("missing keyword TO at %d \n", *tc);
					return 0;
				}

			} else {
				printf("missing assignment operator at %d \n", *tc);
				return 0;
			}
		} else {
			printf("missing iterator at %d \n", *tc);
			return 0;
		}
	}
	return 1;
}

int block(FILE *in, Symbol *sb, int *tc) {
	if ( sy_cmp(sb, tokens[CONST])) {
		do {
			next_symbol(in, sb, tc);
			if ( sy_cmp(sb, tokens[IDENT])) {
				next_symbol(in, sb, tc);
				if ( sy_cmp(sb, tokens[EQU])) {
					next_symbol(in, sb, tc);
					if ( sy_cmp(sb, tokens[NUMBER])) { next_symbol(in, sb, tc); }
					else { printf("missing CONST value at %d \n", *tc); }
				} else { printf("missing equal operator at %d \n", *tc); }
			} else { printf("missing CONST name at %d \n", *tc); }
		} while ( sy_cmp(sb, tokens[COMMA]));
		if ( sy_cmp(sb, tokens[SEMICOLON])) {
			next_symbol(in, sb, tc);
		} else { printf("missing semicolon at %d \n", *tc); }
	}
	if ( sy_cmp(sb, tokens[VAR])) {
		do {
			next_symbol(in, sb, tc);
			if ( sy_cmp(sb, tokens[IDENT])) {
				next_symbol(in, sb, tc);
				if ( sy_cmp(sb, tokens[LBRACK])) {
					next_symbol(in, sb, tc);
					if ( sy_cmp(sb, tokens[NUMBER])) {
						next_symbol(in, sb, tc);
						if ( sy_cmp(sb, tokens[RBRACK])) {
							next_symbol(in, sb, tc);
						} else { printf("missing bracket at %d \n", *tc); }
					} else { printf("missing array index at %d \n", *tc); }
				}
			} else { printf("missing variable name at %d \n", *tc); }
		} while ( sy_cmp(sb, tokens[COMMA]));
		if ( sy_cmp(sb, tokens[SEMICOLON])) {
			next_symbol(in, sb, tc);
		} else { printf("semicolon missing at %d \n", *tc); }
	}
	if ( sy_cmp(sb, tokens[PROCEDURE])) {
		next_symbol(in, sb, tc);
		if ( sy_cmp(sb, tokens[IDENT])) {
			next_symbol(in, sb, tc);
			if ( sy_cmp(sb, tokens[LPARENT])) {
				do {
					next_symbol(in, sb, tc);
					if ( sy_cmp(sb, tokens[VAR])) {
						next_symbol(in, sb, tc);
					}
					if ( sy_cmp(sb, tokens[IDENT])) {
						next_symbol(in, sb, tc);
					} else { printf("variable name missing at %d \n", *tc); }
				} while ( sy_cmp(sb, tokens[SEMICOLON]));
				if ( sy_cmp(sb, tokens[RPARENT])) {
					next_symbol(in, sb, tc);
				} else { printf("parentheses missing at %d \n", *tc); }
			}
			if ( sy_cmp(sb, tokens[SEMICOLON])) {
				next_symbol(in, sb, tc);
				block(in, sb, tc);
				if ( sy_cmp(sb, tokens[SEMICOLON])) {
					next_symbol(in, sb, tc);
				} else { printf("semicolon missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("procedure name missing at %d \n", *tc); }
	}
	if ( sy_cmp(sb, tokens[BEGIN])) {
		do {
			next_symbol(in, sb, tc);
			statement(in, sb, tc);
		} while ( sy_cmp(sb, tokens[SEMICOLON]));
		if ( sy_cmp(sb, tokens[END])) {
			next_symbol(in, sb, tc);
			return 1;
		} else { printf("keyword END missing at %d \n", *tc); }
		return 1;
	} else { printf("keyword BEGIN missing at %d \n", *tc); }
	return 0;
}

int program(FILE *in, Symbol *sb, int *tc) {
	if ( sy_cmp(sb, tokens[PROGRAM])) {
		next_symbol(in, sb, tc);
		if ( sy_cmp(sb, tokens[IDENT])) {
			next_symbol(in, sb, tc);
			if ( sy_cmp(sb, tokens[SEMICOLON])) {
				next_symbol(in, sb, tc);
				block(in, sb, tc);
				if ( sy_cmp(sb, tokens[PERIOD])) {
					next_symbol(in, sb, tc);
					return 1;
				} else { printf("terminating symbol (period) missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("program name missing at %d \n", *tc); }
	} else { printf("keyword PROGRAM missing at %d \n", *tc); }
	return 0;
}

int sy_cmp(Symbol *sb, char *s) {
	return !strcmp(sb->name, s);
}

int next_symbol(FILE *in, Symbol *sb, int *tc) {
	char line[1024];
	if ( fgets(line, sizeof line, in) == NULL) {
		return 0;
	}
	memset(sb, 0, sizeof(*sb));
	strncpy(sb->name, strtok(line, " "), TOKEN_LENGTH);
	( *tc )++;    //	increment the token counter while parsing through token list
	if ( sy_cmp(sb, tokens[IDENT]) || sy_cmp(sb, tokens[NUMBER])) {
		strncpy(sb->ident, strtok(NULL, " "), LEXEME_LENGTH);
		*strchr(sb->ident, '\n') = 0;    //	remove trailing newline from file
	}
	return 1;

}