#ifndef COMPILERS_SEMANLYZER_H
#define COMPILERS_SEMANLYZER_H

#include "symbol_table.h"
#include "semantic_checks.h"

//	functions used to traverse the AST and perform semantic checks

void program_semantic( SyntaxTreeNode *node, SymbolTable **table_ptr );
static void block_semantic( SyntaxTreeNode *node, SymbolTable *table, bool main_block );

//	iterate through all children and perform check
static void statement_semantic( SyntaxTreeNode *node, SymbolTable *table );

//	evaluation check on all variable in expression
static void expression_semantic( SyntaxTreeNode *node, SymbolTable *table );
//	DFS recursion
static void recursive_expression_semantic( SyntaxTreeNode *node, SymbolTable *table );

void program_semantic( SyntaxTreeNode *node, SymbolTable **table_ptr ) {
	assert( node->type == ND_PRG_DCL );
	*table_ptr = NewSymbolTable(NULL, node->annotation->data.ident );
//	SymbolEntry entry = {
//			.type = SB_INT,
//			.data.var_initialized = false
//	};
//	InsertEntry(*table_ptr, entry, node->annotation->data.ident);
	block_semantic( node, *table_ptr, true );
}

void block_semantic( SyntaxTreeNode *node, SymbolTable *table, bool main_block ) {
	int i = 0;
	SyntaxTreeNode **child_arr = node->children;
//	insert constant to table
	if( child_arr[i]->type == ND_CST_DCL ) {
		for( int j = 0; j < child_arr[i]->child_count; ++j ) {
			SyntaxTreeNode *const_node = child_arr[i]->children[j];
			assert( const_node->type == ND_BINARY_OP );
			if( DeclarationCheck( const_node, table )) {
				SymbolEntry entry = {
						.type = SB_CONST_INT,
						.data.const_value = const_node->children[1]->annotation->data.number
				};
				InsertEntry( table, entry, const_node->children[0]->annotation->data.ident );
			}
		}
		i++;
	}
//	integer and int array are declared in the same section
	if( child_arr[i]->type == ND_VAR_DCL ) {
		for( int j = 0; j < child_arr[i]->child_count; ++j ) {
			SyntaxTreeNode *var_node = child_arr[i]->children[j];
			assert( var_node->type == ND_IDENT || var_node->type == ND_ARR_DCL );
			if( DeclarationCheck( var_node, table )) {
				if( var_node->type == ND_IDENT ) {
					SymbolEntry entry = {
							.type = SB_INT,
							.data.var_initialized = false
					};
					InsertEntry( table, entry, var_node->annotation->data.ident );
				}
			} else {
				if( var_node->type == ND_ARR_DCL ) {
					SymbolEntry entry = {.type = SB_ARRAY};
					entry.data.arr_size = var_node->children[1]->annotation->data.number;
					InsertEntry( table, entry, var_node->children[0]->annotation->data.ident );
				}
			}
		}
		i++;
	}
//	iterate through function definitions, creating a table for each
	while( main_block && child_arr[i]->type == ND_PROC_DCL ) {
		SyntaxTreeNode *func_node = child_arr[i];
		if( DeclarationCheck( func_node, table )) {
			SymbolEntry entry = {.type = SB_FUNCTION};
			entry.data.func.scope_ptr = NewSymbolTable( table, func_node->annotation->data.ident );
//			check number of parameter
			if( func_node->children[0]->type == ND_PARAM ) {
				entry.data.func.param_count = func_node->children[0]->child_count;
			}
			InsertEntry( table, entry, func_node->annotation->data.ident );
//			depends on whether function has parameter, definition will be the first or second child node
			block_semantic(
					func_node->children[entry.data.func.param_count + 1],
					entry.data.func.scope_ptr, false );
		}
		i++;
	}
//	this block only contains statements, no declaration allowed
	assert( child_arr[i]->type == ND_CODE_BLK );
	statement_semantic( child_arr[i], table );
}

static void statement_semantic( SyntaxTreeNode *node, SymbolTable *table ) {
	for( int j = 0; j < node->child_count; ++j ) {
		SyntaxTreeNode *stm_node = node->children[j];
		switch( stm_node->type ) {
			case ND_BINARY_OP:
				AssignmentCheck( stm_node, table );
				expression_semantic( stm_node->children[1], table );
//				check for mathematical expression within subscripting operator
				if( stm_node->children[0]->type == ND_SUBSCRIPT ) {
					expression_semantic( stm_node->children[0]->children[1], table );
				}
				break;
			case ND_CONDITIONAL:
				expression_semantic( stm_node->children[0], table );
				statement_semantic( stm_node, table );
				break;
			case ND_FOR_LOOP:
				LoopAssignmentCheck( stm_node, table );
				expression_semantic( stm_node->children[1], table );
				expression_semantic( stm_node->children[2], table );
				statement_semantic( stm_node, table );
				break;
			case ND_WHILE_LOOP:
				expression_semantic( stm_node->children[0], table );
				statement_semantic( stm_node, table );
				break;
			case ND_CODE_BLK:
				statement_semantic( stm_node, table );
				break;
			case ND_FUNC_CALL:
				FunctionCallCheck( stm_node, table );
//				semantic check on all function parameter
				for( int i = 0; i < stm_node->child_count; ++i ) {
					expression_semantic( stm_node->children[i], table );
				}
				break;
//			skip unrecognized nodes
			default:
				continue;
		}
	}
}

static void expression_semantic( SyntaxTreeNode *node, SymbolTable *table ) {
	assert( node->type == ND_EXPRESSION && node->child_count == 1 );
	recursive_expression_semantic( node->children[0], table );
}

static void recursive_expression_semantic( SyntaxTreeNode *node, SymbolTable *table ) {
	if( node == NULL) { return; }
	if( node->type == ND_IDENT ) {
		EvaluationCheck( node, table );
		if( node->child_count != 0 ) {
			perror( "compiler error: expression tree generated with identifier having children. \n" );
		}
	} else {
		for( int i = 0; i < node->child_count; ++i ) {
			recursive_expression_semantic( node->children[0], table );
		}
	}

}

#endif //COMPILERS_SEMANLYZER_H
