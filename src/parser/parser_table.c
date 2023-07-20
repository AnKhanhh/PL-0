#include "parser_table.h"

void syntax_analysis(FILE *in, FILE* out){
	fseek(in,0,SEEK_SET);

	int token_counter = 0;
	Symbol symbol = {};
	SymbolTable *root_table = NULL;

	next_symbol(in,&symbol, &token_counter);
	program(in, &symbol, &token_counter, &root_table);

	print_SymTab(root_table,out);
	free_st(root_table);
	puts("parsing complete!");
}