#ifndef COMP_LIST_SEMANTIC_CHECK_H
#define COMP_LIST_SEMANTIC_CHECK_H

#include "list_symbol_table.h"
#include "/parser/syntax_tree.h"

// search for the nearest table entry with the name equals var_name
// can pass in scope_found to extract the symbol table containing the result
static SymbolEntry *search_subroutine(SymbolTable *root, char *var_name, bool search_local_scope_only, SymbolTable **scope_found);

static bool var_dcl_check(NodeAST *node, SymbolTable *root);

static bool arr_dcl_check(NodeAST *node, SymbolTable *root);

static bool const_dcl_check(NodeAST *node, SymbolTable *root);

static bool func_dcl_check(NodeAST *node, SymbolTable *root);

SymbolEntry *SearchLocal(SymbolTable *root, char *var_name, SymbolTable **scope_found) {
	return search_subroutine(root, var_name, true, scope_found);
}

SymbolEntry *SearchGlobal(SymbolTable *root, char *var_name, SymbolTable **scope_found) {
	return search_subroutine(root, var_name, false, scope_found);
}
//	except for the program name, all variable, constant, function and array share the same namespace
//	PL/0 only allow declaration before the main logic, greatly simplify declaration check
//	return false on error
bool OnDeclarationSemantic(NodeAST *node, SymbolTable *root);

bool OnEvocationSemantic(NodeAST *node, SymbolTable *root);

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
//		recursively move to parent symbol table and search
	} while (!search_local_scope_only && (root = root->parent) != NULL);
	return NULL;
}

static bool var_dcl_check(NodeAST *node, SymbolTable *root) {
	assert(node->child_count == 0);
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
//	on name collision
	if (result_entry == SearchGlobal(root, node->annotation->value.ident, &result_table)) {
		assert(result_table != NULL);
		char *ident = node->annotation->value.ident;
		if (result_table == root) {
			printf("ERROR: redeclaration of variable in the same scope. \n"
				   "\t scope: %s, identifier: %s \n",
				   root->table_name, ident);
			return false;
//		allow variable shadowing of same type
		} else {
			if (result_entry->type == SB_INT) {
				printf("WARNING: variable shadowing. \n"
					   "\t scope: %s, identifier: %s shadowing scope:%s \n",
					   root->table_name, ident, result_table->table_name);
			} else {
				printf("ERROR: redeclaration of variable into a different type. \n"
					   " \t scope: %s, identifier: %s. from scope: %s, of type %s into %s \n",
					   root->table_name, ident, result_table->table_name, IDENTTYPE[result_entry->type], IDENTTYPE[SB_INT]);
				return false;
			}
		}
		return true;
	}
}

static bool arr_dcl_check(NodeAST *node, SymbolTable *root) {
	assert(node->child_count == 2);
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
	if (result_entry == SearchGlobal(root, node->children[0]->annotation->value.ident, &result_table)) {
		char *ident = node->children[0]->annotation->value.ident;
		if (result_table == root) {
			printf("ERROR: redeclaration of variable in the same scope. \n"
				   "\t scope: %s, identifier: %s \n",
				   root->table_name, ident);
//		allow variable shadowing of same type
		} else {
			if (result_entry->type == SB_CONST_INT) {
				printf("WARNING: variable shadowing. \n"
					   "\t scope: %s, identifier: %s shadowing scope:%s \n",
					   root->table_name, ident, result_table->table_name);
			} else {
				printf("ERROR: redeclaration of variable into a different type. \n"
					   " \t scope: %s, identifier: %s. from scope: %s, of type %s into %s \n",
					   root->table_name, ident, result_table->table_name, IDENTTYPE[result_entry->type], IDENTTYPE[SB_CONST_INT]);
			}
		}

	}

}

bool OnDeclarationSemantic(NodeAST *node, SymbolTable *root) {
	switch (node->symbol) {
//		due to syntax tree structure, var_dcl contains arr_dcl
		case NAME:
			return var_dcl_check(node, root);
		case ARR_DCL:
			return arr_dcl_check(node, root);
		case CST_DCL:
			return const_dcl_check(node, root);
		case PROC_DCL:
			return func_dcl_check(node, root);
		default:
			fprintf(stderr,
					"ERR: declaration semantic check evoked on invalid token. scope: %s", root->table_name);
			return false;
	}
}

#endif //COMP_LIST_SEMANTIC_CHECK_H
