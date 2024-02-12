//  PL/0 only allow nested code block inside function definition, which greatly simplifies scoping
//	for now, all array is 2d and size cannot be expression
#include <stdio.h>
#include "list_semantic_check.h"
#include "list_symbol_table.h"
#include "../parser/syntax_tree.h"

void SemanticAnalysis(NodeAST* root_AST, FILE* out) {

}
