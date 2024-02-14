// recursive descent parser for program structure
// pratt (TDOP) parser for arithmetic expression

#include "recursive_descent_parser.h"

//	error message is tracked with a token counter, with max value <= MAX_INT
//	I don't intend to implement a line/column tracker in the near future

void SyntaxAnalysis( FILE *in, FILE *out, NodeAST **root_ptr ) {
	fseek( in, 0, SEEK_SET );

	int token_counter = 0;
	Symbol symbol = {0};
	program( in, &symbol, &token_counter, root_ptr );
	PrintSyntaxTree(*root_ptr,out);
}
