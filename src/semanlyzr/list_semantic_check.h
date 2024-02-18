#ifndef COMP_LIST_SEMANTIC_CHECK_H
#define COMP_LIST_SEMANTIC_CHECK_H

#include "list_symbol_table.h"
#include "../parser/syntax_tree.h"

// search for the nearest table entry using strcmp()
// use scope_found to extract the pointer to the containing type table
static SymbolEntry *search_helper(SymbolTable *root, char *var_name, bool search_local_scope_only, SymbolTable **scope_found);

//SymbolEntry *SearchLocalScope(SymbolTable *root, char *var_name, SymbolTable **scope_found) {
//	return search_helper(root, var_name, true, scope_found);
//}

SymbolEntry *SearchGlobalScope(SymbolTable *root, char *var_name, SymbolTable **scope_found) {
	return search_helper(root, var_name, false, scope_found);
}

//	check declaration
bool DeclarationCheck(SyntaxTreeNode *node, SymbolTable *root);

//	check assignment operation
bool AssignmentCheck(SyntaxTreeNode *node, SymbolTable *root);

//	check variable evaluation
bool EvaluationCheck(SyntaxTreeNode *node, SymbolTable *root);

//	check on function invocation
bool FunctionCallCheck(SyntaxTreeNode *node, SymbolTable *root);

bool ConditionalCheck(SyntaxTreeNode *node, SymbolTable *root);

static SymbolEntry *search_helper(SymbolTable *root, char *var_name, bool search_local_scope_only, SymbolTable **scope_found) {
	assert(root != NULL);
	do {
		for (int i = 0; i < root->entry_count; ++i) {
			if (strncmp(root->entries[i].ident, var_name, LEXEME_LENGTH) != 0) { continue; }
			if (scope_found) { *scope_found = root; }
			return &(root->entries[i]);
		}
//		recursively move to the scope above and search
	} while (!search_local_scope_only && (root = root->parent) != NULL);
	return NULL;
}

bool DeclarationCheck(SyntaxTreeNode *node, SymbolTable *root) {
	char *ident = NULL;
	enum EIdentType type = 0;
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
	switch (node->type) {
//		on declaration of variable integer
		case ND_IDENT:
			assert(node->children[0] == NULL);
			ident = node->annotation->data.ident;
			type = SB_INT;
			break;
//		one-dimensional integer array
		case ND_ARR_DCL:
			assert(node->children[0]->type == ND_IDENT && node->children[1]->type == ND_INTEGER);
			ident = node->children[0]->annotation->data.ident;
			type = SB_ARRAY;
			if (node->children[1]->annotation->data.number < 1) {
				printf("Array initialized with non-positive size. \n"
					   "\t scope: %s, identifier: %s \n",
					   root->name, ident);
			}
			break;
//		constant
		case ND_BINARY_OP:
			assert(node->annotation->data.token == TK_EQU);
			assert(node->children[0]->type == ND_IDENT && node->children[1]->type == ND_INTEGER);
			ident = node->children[0]->annotation->data.ident;
			type = SB_CONST_INT;
			break;
//		function
		case ND_PROC_DCL:
			assert(node->children[0]->type == ND_PARAM);
			ident = node->annotation->data.ident;
			type = SB_FUNCTION;
			break;
		default:
			fprintf(stderr,
					"compiler error: declaration semantic check on invalid token - scope: %s",
					root->name);
			return false;
	}
//	on name collision
	if (result_entry == SearchGlobalScope(root, ident, &result_table)) {
		assert(result_table != NULL);
//		redeclaration in the same scope is not allowed
		if (result_table == root) {
			printf("Redeclaration of variable in the same scope. \n"
				   "\t scope: %s, identifier: %s \n", root->name, ident);
			return false;
//		variable shadowing is allowed
		} else {
			printf("Warning: variable shadowing. \n"
				   "\t scope: %s, identifier: %s, type: %s - shadowing scope:%s, type: %s. \n",
				   root->name, ident, SB_IDENT_TYPE[type], result_table->name, SB_IDENT_TYPE[result_entry->type]);
		}

	}
	return true;
}

bool AssignmentCheck(SyntaxTreeNode *node, SymbolTable *root) {
	assert(node->type == ND_BINARY_OP && node->annotation->data.token == TK_ASSIGN);
	char *ident = NULL;
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
//	check identifier
	if (node->children[0]->type == ND_IDENT) {
		ident = node->children[0]->annotation->data.ident;
	} else if (node->children[0]->type == ND_SUBSCRIPT) {
		ident = node->children[0]->children[0]->annotation->data.ident;
	} else {
		printf("Expected identifier. \n"
			   "\t scope: %s. \n", root->name);
		return false;
	}
//	check if identifier is declared
	if (result_entry == SearchGlobalScope(root, ident, &result_table)) {
		assert(result_table != NULL);
//		check if type is assignable
		if (result_entry->type == SB_INT || result_entry->type == SB_ARRAY) {
			return true;
		} else {
			printf("Type is not assignable. \n"
				   "\t scope: %s, identifier: %s, type: %s \n",
				   root->name, ident, SB_IDENT_TYPE[result_entry->type]);
		}
	} else {
		printf("Use of undeclared identifier. \n"
			   "\t scope: %s, identifier: %s, type: %s \n",
			   root->name, ident, SB_IDENT_TYPE[result_entry->type]);
	}
	return false;
}

bool EvaluationCheck(SyntaxTreeNode *node, SymbolTable *root) {
	assert(node->type == ND_IDENT && node->child_count == 0);
	char *ident = node->annotation->data.ident;
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
	if (result_entry == SearchGlobalScope(root, ident, &result_table)) {
		assert(result_table != NULL);
		switch (result_entry->type) {
			case SB_INT:
				if (!result_entry->data.var_initialized) {
					printf("warning: uninitialized variable. \n"
						   "\t scope: %s, identifier: %s \n", root->name, ident);
				}
//			all array must be accessed with subscripting operator
			case SB_ARRAY:
				if (node->parent->type != ND_SUBSCRIPT) {
					printf("Incompatible conversion of array pointer to integer. \n"
						   "\t scope: %s, identifier: %s \n", root->name, ident);
				}
				break;
			case SB_CONST_INT:
				return true;
//			no pointer support, so using pointer as integer will result in error
			case SB_FUNCTION:
				printf("Incompatible conversion of function pointer to integer. \n"
					   "\t scope: %s, identifier: %s \n", root->name, ident);
				break;
			default:
				printf("Expected expression. \n"
					   "\t scope: %s, identifier: %s \n", root->name, ident);
		}
	} else {
		printf("Use of undeclared identifier. \n"
			   "\t scope: %s, identifier: %s \n", root->name, ident);
	}
	return false;
}

bool FunctionCallCheck(SyntaxTreeNode *node, SymbolTable *root) {
	assert(node->type == ND_FUNC_CALL);
	char *ident = node->annotation->data.ident;
	SymbolTable *result_table = NULL;
	SymbolEntry *result_entry = NULL;
	if (!(result_entry = SearchGlobalScope(root, ident, &result_table))
		|| result_entry->type != SB_FUNCTION) {
		printf("Call to undeclared function. \n"
			   "\t scope: %s, identifier: %s \n", root->name, ident);
	} else if (node->child_count != result_entry->data.func.param_count) {
		printf("Function have %d parameters, called with %d arguments. \n"
			   "\t scope: %s, identifier: %s \n",
			   result_entry->data.func.param_count, node->child_count, root->name, ident);
	} else { return true; }
	return false;
}

#endif //COMP_LIST_SEMANTIC_CHECK_H
