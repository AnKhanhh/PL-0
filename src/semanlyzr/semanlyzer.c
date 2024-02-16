//  nested code block is only allowed inside function definition, which simplifies scoping
//	since functions don't have return statement, closure or inline function call is not supported
//	all arrays is 1-dimensional

//	all variable, constant, function and array share the same namespace
//	PL/0 is declarative, meaning it has a separate declaration section, simplifying declaration check

//	right now recursion is syntactically valid

#include <stdio.h>
#include "semanlyzer.h"

void SemanticAnalysis(SyntaxTreeNode *root_node, FILE *out) {
	SymbolTable *root_table = NULL;
	program_semantic(root_node, &root_table);
	PrintSymbolTable(root_table, out);
}
