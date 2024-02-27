// recursive descent parser for program structure
// pratt (TDOP) parser for arithmetic expression

#include "recursive_descent_parser.h"

//	error message is tracked with a token counter, with max data <= MAX_INT
//	substitute for a line/column tracker

void SyntaxAnalysis(FILE *in, FILE *out, SyntaxTreeNode **root_ptr ) {
	fseek( in, 0, SEEK_SET );

	int token_counter = 0;
	Symbol symbol = {0};
	program( in, &symbol, &token_counter, root_ptr );
	PrintSyntaxTree(*root_ptr,out);
	fflush(out);
}
