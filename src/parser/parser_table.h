#ifndef COMP_PARSER_TABLE_H
#define COMP_PARSER_TABLE_H
#endif //COMP_PARSER_TABLE_H

#include "../semanlyzr/symbol_table.h"

#define SB_LENGTH 32

typedef union {
	char ident[LEXEME_LENGTH];
	long value;
} SymbolTag;

typedef struct {
	char name[SB_LENGTH];
	SymbolTag tag;
} Symbol;    //	terminal or non-terminal symbol

//	on recognizing a terminal symbol, returns 1 and call next_symbol()
int accept_terminal(FILE *in, Symbol *sb, int *tc, char *s);
//	reads a Symbol from the token list file, returns 0 on failure
int next_symbol(FILE *in, Symbol *sb, int *tc);
// issue error/ warning announcement at token in given position
void semantic_err(int count);

void term(FILE *in, Symbol *sb, int *tc, SymbolTable *root);
void factor(FILE *in, Symbol *sb, int *tc, SymbolTable *root);
void expression(FILE *in, Symbol *sb, int *tc, SymbolTable *root);
void condition(FILE *in, Symbol *sb, int *tc, SymbolTable *root);
void statement(FILE *in, Symbol *sb, int *tc, SymbolTable *root);
void block(FILE *in, Symbol *sb, int *tc, SymbolTable *root);
void program(FILE *in, Symbol *sb, int *tc, SymbolTable **p_root);

void term(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	factor(in, sb, tc, root);
	while ( accept_terminal( in, sb, tc, TOKENS[TIMES]) ||
			accept_terminal( in, sb, tc, TOKENS[SLASH]) ||
			accept_terminal( in, sb, tc, TOKENS[PERCENT])) {
		factor(in, sb, tc, root);
	}
}

void factor(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	Symbol holder = *sb;
	if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
		if ( accept_terminal( in, sb, tc, TOKENS[LBRACK])) {
			if ( !semantic_evl(root, holder.tag.ident, ARR)) { semantic_err(*tc); }
			expression(in, sb, tc, root);
			if ( accept_terminal( in, sb, tc, TOKENS[RBRACK])) {
			} else { printf("right bracket missing at %d \n", *tc); }
		} else if ( !semantic_evl(root, holder.tag.ident, MUTE)) { semantic_err(*tc); }
	} else if ( accept_terminal( in, sb, tc, TOKENS[NUMBER])) {
	} else if ( accept_terminal( in, sb, tc, TOKENS[LPARENT])) {
		expression(in, sb, tc, root);
		if ( accept_terminal( in, sb, tc, TOKENS[RPARENT])) {
		} else { printf("right parentheses missing at %d \n", *tc); }
	} else { printf("illegal factor production at %d \n", *tc); }
}

void expression(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	if ( accept_terminal( in, sb, tc, TOKENS[PLUS]) || accept_terminal( in, sb, tc, TOKENS[MINUS])) {}
	term(in, sb, tc, root);
	while ( accept_terminal( in, sb, tc, TOKENS[PLUS]) || accept_terminal( in, sb, tc, TOKENS[MINUS])) {
		term(in, sb, tc, root);
	}
}

void condition(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	if ( accept_terminal( in, sb, tc, TOKENS[ODD])) {
		expression(in, sb, tc, root);
	} else {
		expression(in, sb, tc, root);
		if ( accept_terminal( in, sb, tc, TOKENS[EQU]) || accept_terminal( in, sb, tc, TOKENS[GTR]) ||
			 accept_terminal( in, sb, tc, TOKENS[GEQ]) || accept_terminal( in, sb, tc, TOKENS[NEQ]) ||
			 accept_terminal( in, sb, tc, TOKENS[LSS]) || accept_terminal( in, sb, tc, TOKENS[LEQ])) {
			expression(in, sb, tc, root);
		} else { printf("missing comparison operator at %d", *tc); }
	}
}

void statement(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	Symbol holder = *sb;
	if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
		if ( accept_terminal( in, sb, tc, TOKENS[RBRACK])) {
			if ( !semantic_evk(root, holder.tag.ident, ARR)) { semantic_err(*tc); }
			expression(in, sb, tc, root);
			if ( accept_terminal( in, sb, tc, TOKENS[LBRACK])) {} else { printf( "missing bracket at %d \n", *tc); }
		} else { if ( !semantic_evk(root, holder.tag.ident, MUTE)) { semantic_err(*tc); }}
		if ( accept_terminal( in, sb, tc, TOKENS[ASSIGN])) {
			expression(in, sb, tc, root);
		} else { printf("missing assignment operator at %d", *tc); }
	} else if ( accept_terminal( in, sb, tc, TOKENS[CALL])) {
		holder = *sb;
		if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
			if ( !semantic_evl(root, holder.tag.ident, FNC)) { semantic_err(*tc); }
			if ( accept_terminal( in, sb, tc, TOKENS[LPARENT])) {
				do {
					expression(in, sb, tc, root);
				} while ( accept_terminal( in, sb, tc, TOKENS[COMMA]));
				if ( accept_terminal( in, sb, tc, TOKENS[RPARENT])) {
				} else { printf("missing parentheses at %d \n", *tc); }
			}
		}
	} else if ( accept_terminal( in, sb, tc, TOKENS[BEGIN])) {
		do {
			statement(in, sb, tc, root);
		} while ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON]));
		if ( accept_terminal( in, sb, tc, TOKENS[END])) {
		} else { printf("missing keyword END at %d", *tc); }
	} else if ( accept_terminal( in, sb, tc, TOKENS[IF])) {
		condition(in, sb, tc, root);
		if ( accept_terminal( in, sb, tc, TOKENS[THEN])) {
			statement(in, sb, tc, root);
		} else { printf("missing keyword THEN at %d \n", *tc); }
		if ( accept_terminal( in, sb, tc, TOKENS[ELSE])) { statement( in, sb, tc, root); }
	} else if ( accept_terminal( in, sb, tc, TOKENS[WHILE])) {
		condition(in, sb, tc, root);
		if ( accept_terminal( in, sb, tc, TOKENS[DO])) {
			statement(in, sb, tc, root);
		} else { printf(" missing keyword DO at %d \n", *tc); }
	} else if ( accept_terminal( in, sb, tc, TOKENS[FOR])) {
		holder = *sb;
		if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
			if ( !semantic_evk(root, holder.tag.ident, MUTE)) { semantic_err(*tc); }
			if ( accept_terminal( in, sb, tc, TOKENS[ASSIGN])) {
				expression(in, sb, tc, root);
				if ( accept_terminal( in, sb, tc, TOKENS[TO])) {
					expression(in, sb, tc, root);
					if ( accept_terminal( in, sb, tc, TOKENS[DO])) {
						statement(in, sb, tc, root);
					} else { printf("missing keyword TO at %d \n", *tc); }
				} else { printf("missing keyword TO at %d \n", *tc); }
			} else { printf("missing assignment operator at %d \n", *tc); }
		} else { printf("missing iterator at %d \n", *tc); }
	}
}

void block(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	if ( accept_terminal( in, sb, tc, TOKENS[CONST])) {
		do {
			Symbol holder = *sb;
			if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
				if ( accept_terminal( in, sb, tc, TOKENS[EQU])) {
					long long number = sb->tag.value;
					if ( accept_terminal( in, sb, tc, TOKENS[NUMBER])) {
						if ( !semantic_dcl(root, holder.tag.ident, IMM, &number)) { semantic_err(*tc); }
					} else { printf("missing CONST value at %d \n", *tc); }
				} else { printf("missing equal operator at %d \n", *tc); }
			} else { printf("missing CONST name at %d \n", *tc); }
		} while ( accept_terminal( in, sb, tc, TOKENS[COMMA]));
		if ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON])) {} else { printf( "missing semicolon at %d \n", *tc); }
	}
	if ( accept_terminal( in, sb, tc, TOKENS[VAR])) {
		do {
			Symbol holder = *sb;
			if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
				if ( accept_terminal( in, sb, tc, TOKENS[LBRACK])) {
					int arr_size = sb->tag.value;
					if ( accept_terminal( in, sb, tc, TOKENS[NUMBER])) {
						if ( accept_terminal( in, sb, tc, TOKENS[RBRACK])) {
							if ( !semantic_dcl(root, holder.tag.ident, ARR, &arr_size)) { semantic_err(*tc); }
						} else { printf("missing bracket at %d \n", *tc); }
					} else { printf("missing array index at %d \n", *tc); }
				} else { if ( !semantic_dcl(root, holder.tag.ident, MUTE, NULL)) { semantic_err(*tc); }}
			} else { printf("missing variable name at %d \n", *tc); }
		} while ( accept_terminal( in, sb, tc, TOKENS[COMMA]));
		if ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON])) {} else { printf( "semicolon missing at %d \n", *tc); }
	}
	if ( accept_terminal( in, sb, tc, TOKENS[PROCEDURE])) {
		Symbol holder = *sb;
		if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
			SymbolTable *sub_block = NULL;
			if ( !semantic_dcl(root, holder.tag.ident, FNC, &sub_block)) { semantic_err(*tc); }

			if ( accept_terminal( in, sb, tc, TOKENS[LPARENT])) {
				do {
					if ( accept_terminal( in, sb, tc, TOKENS[VAR])) {}
					holder = *sb;
					if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
						if ( !semantic_dcl(sub_block, holder.tag.ident, MUTE, NULL)) { semantic_err(*tc); }
					} else { printf("variable name missing at %d \n", *tc); }
				} while ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON]));
				if ( accept_terminal( in, sb, tc, TOKENS[RPARENT])) {} else { printf( "parentheses missing at %d \n", *tc); }
			}
			if ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON])) {
				block(in, sb, tc, sub_block);
				if ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON])) {
				} else { printf("semicolon missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("procedure name missing at %d \n", *tc); }
	}
	if ( accept_terminal( in, sb, tc, TOKENS[BEGIN])) {
		do {
			statement(in, sb, tc, root);
		} while ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON]));
		if ( accept_terminal( in, sb, tc, TOKENS[END])) {} else { printf( "keyword END missing at %d \n", *tc); }
	} else { printf("keyword BEGIN missing at %d \n", *tc); }
}

void program(FILE *in, Symbol *sb, int *tc, SymbolTable **p_root) {
	if ( accept_terminal( in, sb, tc, TOKENS[PROGRAM])) {
		Symbol holder = *sb;
		if ( accept_terminal( in, sb, tc, TOKENS[IDENT])) {
			*p_root = new_st(NULL, holder.tag.ident);
			if ( accept_terminal( in, sb, tc, TOKENS[SEMICOLON])) {
				block(in, sb, tc, *p_root);
				if ( accept_terminal( in, sb, tc, TOKENS[PERIOD])) {
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
	strncpy( sb->name, strtok(line, " "), SB_LENGTH);
	( *tc )++;    //	increment the token counter while parsing through token list
	if ( !strcmp( sb->name, TOKENS[IDENT])) {
		strncpy(sb->tag.ident, strtok(NULL, " \n"), LEXEME_LENGTH);
	} else if ( !strcmp( sb->name, TOKENS[NUMBER])) {
		sb->tag.value = strtol(strtok(NULL, " \n"), NULL, 10);
	}
	return 1;
}

void semantic_err(int count) { printf(" - at token %d \n", count); }
