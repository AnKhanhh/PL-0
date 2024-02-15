#ifndef COMPILERS_SEMANLYZER_H
#define COMPILERS_SEMANLYZER_H

#include "list_symbol_table.h"
#include "list_semantic_check.h"

void program_check(NodeAST *node, SymbolTable** table_ptr){
	if(node->type == ND_PRG_DCL){
		*table_ptr = NewSymbolTable(NULL,node->annotation->value.ident);
	}
}

#endif //COMPILERS_SEMANLYZER_H
