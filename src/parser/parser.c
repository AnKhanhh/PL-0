#include "parser.h"

void syntax_analysis(FILE *in, FILE *out) {
	fseek(in,0,SEEK_SET);

	int token_counter = 0, vs_offset = 0;
	Symbol symbol = {};
	VarStack var_stack = {};

	next_symbol(in,&symbol, &token_counter);
	program(in, &symbol, &token_counter, &var_stack, vs_offset);
	puts("parsing complete!");
}