//  nested code block is only allowed inside function definition, which simplifies scoping
//	since functions don't have return statement, closure or inline function call is not supported
//	all arrays is 1-dimensional

//	all variable, constant, function and array share the same namespace
//	PL/0 is declarative, meaning it has a separate declaration section, simplifying declaration check

//	since token line and column are not tracked, error messages are emitted with identifier name and scope

#include <stdio.h>
#include "semanlyzer.h"

void SemanticAnalysis( FILE *out, SyntaxTreeNode *root_node, SymbolTable **root_table ) {
	program_semantic( root_node, root_table );
	PrintSymbolTable( *root_table, out );
	fflush(out);
}
