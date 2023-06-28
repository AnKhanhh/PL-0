#include "parser.h"

void syntax_analysis(FILE *in, FILE *out) {
	fseek(in,0,SEEK_SET);

	int token_counter = 0;
	Symbol symbol = {};

	next_symbol(in,&symbol, &token_counter);
	program(in, &symbol, &token_counter);
	puts("parsing complete!");
}