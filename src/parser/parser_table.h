#ifndef COMP_PARSER_TABLE_H
#define COMP_PARSER_TABLE_H
#endif //COMP_PARSER_TABLE_H

#include "../semanlyzr/symbol_table.h"

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
	while ( accept_terminal(in, sb, tc, tokens[TIMES]) ||
			accept_terminal(in, sb, tc, tokens[SLASH]) ||
			accept_terminal(in, sb, tc, tokens[PERCENT])) {
		factor(in, sb, tc, root);
	}
}

void factor(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	Symbol holder = *sb;
	if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
		if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
			if ( !semantic_evl(root, holder.tag.ident, ARR)) { semantic_err(*tc); }
			expression(in, sb, tc, root);
			if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
			} else { printf("right bracket missing at %d \n", *tc); }
		} else if ( !semantic_evl(root, holder.tag.ident, MUTE)) { semantic_err(*tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
	} else if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
		expression(in, sb, tc, root);
		if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
		} else { printf("right parentheses missing at %d \n", *tc); }
	} else { printf("illegal factor production at %d \n", *tc); }
}

void expression(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	if ( accept_terminal(in, sb, tc, tokens[PLUS]) || accept_terminal(in, sb, tc, tokens[MINUS])) {}
	term(in, sb, tc, root);
	while ( accept_terminal(in, sb, tc, tokens[PLUS]) || accept_terminal(in, sb, tc, tokens[MINUS])) {
		term(in, sb, tc, root);
	}
}

void condition(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	if ( accept_terminal(in, sb, tc, tokens[ODD])) {
		expression(in, sb, tc, root);
	} else {
		expression(in, sb, tc, root);
		if ( accept_terminal(in, sb, tc, tokens[EQU]) || accept_terminal(in, sb, tc, tokens[GTR]) ||
			 accept_terminal(in, sb, tc, tokens[GEQ]) || accept_terminal(in, sb, tc, tokens[NEQ]) ||
			 accept_terminal(in, sb, tc, tokens[LSS]) || accept_terminal(in, sb, tc, tokens[LEQ])) {
			expression(in, sb, tc, root);
		} else { printf("missing comparison operator at %d", *tc); }
	}
}

void statement(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	Symbol holder = *sb;
	if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
		if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
			if ( !semantic_evk(root, holder.tag.ident, ARR)) { semantic_err(*tc); }
			expression(in, sb, tc, root);
			if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {} else { printf("missing bracket at %d \n", *tc); }
		} else { if ( !semantic_evk(root, holder.tag.ident, MUTE)) { semantic_err(*tc); }}
		if ( accept_terminal(in, sb, tc, tokens[ASSIGN])) {
			expression(in, sb, tc, root);
		} else { printf("missing assignment operator at %d", *tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[CALL])) {
		holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			if ( !semantic_evl(root, holder.tag.ident, FNC)) { semantic_err(*tc); }
			if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
				do {
					expression(in, sb, tc, root);
				} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
				if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {
				} else { printf("missing parentheses at %d \n", *tc); }
			}
		}
	} else if ( accept_terminal(in, sb, tc, tokens[BEGIN])) {
		do {
			statement(in, sb, tc, root);
		} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
		if ( accept_terminal(in, sb, tc, tokens[END])) {
		} else { printf("missing keyword END at %d", *tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[IF])) {
		condition(in, sb, tc, root);
		if ( accept_terminal(in, sb, tc, tokens[THEN])) {
			statement(in, sb, tc, root);
		} else { printf("missing keyword THEN at %d \n", *tc); }
		if ( accept_terminal(in, sb, tc, tokens[ELSE])) { statement(in, sb, tc, root); }
	} else if ( accept_terminal(in, sb, tc, tokens[WHILE])) {
		condition(in, sb, tc, root);
		if ( accept_terminal(in, sb, tc, tokens[DO])) {
			statement(in, sb, tc, root);
		} else { printf(" missing keyword DO at %d \n", *tc); }
	} else if ( accept_terminal(in, sb, tc, tokens[FOR])) {
		holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			if ( !semantic_evk(root, holder.tag.ident, MUTE)) { semantic_err(*tc); }
			if ( accept_terminal(in, sb, tc, tokens[ASSIGN])) {
				expression(in, sb, tc, root);
				if ( accept_terminal(in, sb, tc, tokens[TO])) {
					expression(in, sb, tc, root);
					if ( accept_terminal(in, sb, tc, tokens[DO])) {
						statement(in, sb, tc, root);
					} else { printf("missing keyword TO at %d \n", *tc); }
				} else { printf("missing keyword TO at %d \n", *tc); }
			} else { printf("missing assignment operator at %d \n", *tc); }
		} else { printf("missing iterator at %d \n", *tc); }
	}
}

void block(FILE *in, Symbol *sb, int *tc, SymbolTable *root) {
	if ( accept_terminal(in, sb, tc, tokens[CONST])) {
		do {
			Symbol holder = *sb;
			if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
				if ( accept_terminal(in, sb, tc, tokens[EQU])) {
					long long number = sb->tag.value;
					if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
						if ( !semantic_dcl(root, holder.tag.ident, IMM, &number)) { semantic_err(*tc); }
					} else { printf("missing CONST value at %d \n", *tc); }
				} else { printf("missing equal operator at %d \n", *tc); }
			} else { printf("missing CONST name at %d \n", *tc); }
		} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
		if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {} else { printf("missing semicolon at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[VAR])) {
		do {
			Symbol holder = *sb;
			if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
				if ( accept_terminal(in, sb, tc, tokens[LBRACK])) {
					int arr_size = sb->tag.value;
					if ( accept_terminal(in, sb, tc, tokens[NUMBER])) {
						if ( accept_terminal(in, sb, tc, tokens[RBRACK])) {
							if ( !semantic_dcl(root, holder.tag.ident, ARR, &arr_size)) { semantic_err(*tc); }
						} else { printf("missing bracket at %d \n", *tc); }
					} else { printf("missing array index at %d \n", *tc); }
				} else { if ( !semantic_dcl(root, holder.tag.ident, MUTE, NULL)) { semantic_err(*tc); }}
			} else { printf("missing variable name at %d \n", *tc); }
		} while ( accept_terminal(in, sb, tc, tokens[COMMA]));
		if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {} else { printf("semicolon missing at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[PROCEDURE])) {
		Symbol holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			SymbolTable *sub_block = NULL;
			if ( !semantic_dcl(root, holder.tag.ident, FNC, &sub_block)) { semantic_err(*tc); }

			if ( accept_terminal(in, sb, tc, tokens[LPARENT])) {
				do {
					if ( accept_terminal(in, sb, tc, tokens[VAR])) {}
					holder = *sb;
					if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
						if ( !semantic_dcl(sub_block, holder.tag.ident, MUTE, NULL)) { semantic_err(*tc); }
					} else { printf("variable name missing at %d \n", *tc); }
				} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
				if ( accept_terminal(in, sb, tc, tokens[RPARENT])) {} else { printf("parentheses missing at %d \n", *tc); }
			}
			if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				block(in, sb, tc, sub_block);
				if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				} else { printf("semicolon missing at %d \n", *tc); }
			} else { printf("semicolon missing at %d \n", *tc); }
		} else { printf("procedure name missing at %d \n", *tc); }
	}
	if ( accept_terminal(in, sb, tc, tokens[BEGIN])) {
		do {
			statement(in, sb, tc, root);
		} while ( accept_terminal(in, sb, tc, tokens[SEMICOLON]));
		if ( accept_terminal(in, sb, tc, tokens[END])) {} else { printf("keyword END missing at %d \n", *tc); }
	} else { printf("keyword BEGIN missing at %d \n", *tc); }
}

void program(FILE *in, Symbol *sb, int *tc, SymbolTable **p_root) {
	if ( accept_terminal(in, sb, tc, tokens[PROGRAM])) {
		Symbol holder = *sb;
		if ( accept_terminal(in, sb, tc, tokens[IDENT])) {
			*p_root = new_st(NULL, holder.tag.ident);
			if ( accept_terminal(in, sb, tc, tokens[SEMICOLON])) {
				block(in, sb, tc, *p_root);
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
		sb->tag.value = strtol(strtok(NULL, " \n"), NULL, 10);
	}
	return 1;
}

void semantic_err(int count) { printf(" - at token %d \n", count); }
