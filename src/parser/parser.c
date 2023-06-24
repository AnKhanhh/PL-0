#include "parser.h"

void syntax_analysis(FILE *in) {
	int token_counter = 0;
	int *tc = &token_counter;

	Symbol symbol = {};
	Symbol *sb = &symbol;

	program(in, sb, tc);
	puts("parsing complete!");
}