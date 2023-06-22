#include "lexer.h"

#define S_LEN 1024

typedef struct {
	char name[TOKEN_LENGTH];
	char ident[LEXEME_LENGTH];
} Symbol;

//struct SymbolStack {
//	char symbols[TOKEN_LENGTH];
//	int depth;
//};

int deconstruct_token(FILE *in, Symbol *symbol); // generate a symbol from formatted token list, returns 0 on failure



int deconstruct_token(FILE *in, Symbol *symbol) {
	char line[S_LEN];
	if ( fgets(line, sizeof line, in) == NULL) {
		return 0;
	} else {
		strcpy(symbol->name, strtok(line, " "));
		if ( strcmp(symbol->name, other_token[IDENT]) == 0 ||
			 strcmp(symbol->name, other_token[NUMBER]) == 0 ) {
			strcpy(symbol->ident, strtok(NULL, " "));
		}
		return 1;
	}
}