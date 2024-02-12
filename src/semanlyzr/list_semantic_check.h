#ifndef COMP_LIST_SEMANTIC_CHECK_H
#define COMP_LIST_SEMANTIC_CHECK_H

#include "list_symbol_table.h"
#include "/parser/syntax_tree.h"

// search for the nearest table entry with a similar name using strcmp()
// use scope_found to extract the pointer to the containing type table
static SymbolEntry *search_subroutine(SymbolTable *root, char *var_name, bool search_local_scope_only, SymbolTable **scope_found);

SymbolEntry *SearchLocal(SymbolTable *root, char *var_name, SymbolTable **scope_found) {
	return search_subroutine(root, var_name, true, scope_found);
}

SymbolEntry *SearchGlobal(SymbolTable *root, char *var_name, SymbolTable **scope_found) {
	return search_subroutine(root, var_name, false, scope_found);
}
//	all variable, constant, function and array share the same namespace
//	PL/0 is declarative, meaning it has a separate declaration section, simplifying declaration check
//	returns false on redeclaration error
bool OnDeclarationSemantic(NodeAST *node, SymbolTable *root);

bool OnAssignmentSemantic(NodeAST *node, SymbolTable *root);

bool OnEvaluationSemantic(NodeAST *node, SymbolTable *root);

bool OnForLoopSemantic(NodeAST *node, SymbolTable *root);

static SymbolEntry *search_subroutine(SymbolTable *root, char *var_name, bool search_local_scope_only, SymbolTable **scope_found) {
	assert(root != NULL);
	do {
		for (int i = 0; i < root->entry_count; ++i) {
			if (strncmp(root->entries[i].ident, var_name, LEXEME_LENGTH) != 0) { continue; }
			*scope_found = root;
			return &(root->entries[i]);
		}
//		recursively move to the scope above and search
	} while (!search_local_scope_only && (root = root->parent) != NULL);
	return NULL;
}

bool OnDeclarationSemantic(NodeAST *node, SymbolTable *root) {
	char *ident = NULL;
	enum EIdentType type = 0;
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
	switch (node->type) {
		case ND_NAME:
			assert(node->children[0] == NULL);
			ident = node->annotation->value.ident;
			type = SB_INT;
			break;
		case ND_ARR_DCL:
			assert(node->children[0]->type == ND_NAME && node->children[1]->type == ND_LITERAL);
			ident = node->children[0]->annotation->value.ident;
			type = SB_ARRAY;
			break;
		case ND_BINARY_OP:
			assert(node->annotation->value.token == TK_EQU);
			assert(node->children[0]->type == ND_NAME && node->children[1]->type == ND_LITERAL);
			ident = node->children[0]->annotation->value.ident;
			type = SB_CONST_INT;
			break;
		case ND_PROC_DCL:
			assert(node->children[0]->type == ND_FUNC_ARG);
			ident = node->annotation->value.ident;
			type = SB_FUNCTION;
			break;
		default:
			fprintf(stderr,
					"ERROR: declaration semantic check evoked on invalid token. scope: %s",
					root->table_name);
			return false;
	}
//	on name collision
	if (result_entry == SearchGlobal(root, ident, &result_table)) {
		assert(result_table != NULL);
//		redeclaration in the same scope is not allowed
		if (result_table == root) {
			printf("ERROR: redeclaration of variable in the same scope. \n"
				   "\t scope: %s, identifier: %s \n",
				   root->table_name, ident);
			return false;
//		variable shadowing is allowed
		} else {
			printf("WARNING: variable shadowing. \n"
				   "\t scope: %s, identifier: %s, type: %s - shadowing scope:%s, type: %s. \n",
				   root->table_name, ident, SB_IDENT_TYPE[type], result_table->table_name, SB_IDENT_TYPE[result_entry->type]);
		}

	}
	return true;
}

bool OnAssignmentSemantic(NodeAST *node, SymbolTable *root) {
	assert(node->type == ND_BINARY_OP && node->annotation->value.token == TK_ASSIGN);
	char *ident = NULL;
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
}


#endif //COMP_LIST_SEMANTIC_CHECK_H
