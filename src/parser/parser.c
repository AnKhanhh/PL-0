#include "recursive_descent_parser.h"

void SyntaxAnalysis( FILE *in, FILE *out, NodeAST **root_ptr ) {
	fseek( in, 0, SEEK_SET );

	int token_counter = 0;
	Symbol symbol = {0};
	program( in, &symbol, &token_counter, root_ptr );
	PrintSyntaxTree(*root_ptr,out);
}
