//	list of productions
//	1. <program> -> 'program' 'ident' ';' <block> '.'
//	2. <block> -> <con_blk> <var_blk> <pro_blk> 'begin' <statement> <add_stm> 'end'
//	3. <con_blk> -> 'const' 'ident' '=' 'number' <con_bd> ';'
//	4.             e
//	5. <var_blk> -> 'var' 'ident' <index> <var_bd> ';'
//	6.             e
//	7. <pro_blk> -> 'procedure' 'ident' <pro_bd> ';' <block> ';'
//	8.              e
//	9. <con_bd> -> ',' 'ident' '=' 'number' <con_bd>
//	10.            e
//	11. <var_bd> -> ',' 'ident' <index> <var_bd>
//	12.            e
//	13. <pro_bd> -> '(' <var> 'ident' <pro_add_id> ')'
//	14.             e
//	15. <pro_add_id> -> ';' <var> 'ident' <pro_add_id>
//	16.               e
//	17. <add_stm> -> ';' <statement> <add_stm>
//	18.             e
//	19. <var> -> 'var'
//	20.           e
//	21. <statement> -> 'ident' <index> ':=' <expression>
//	22.               'call' 'ident' <call_arg>
//	23.               'begin' <statement> <add_stm> 'end'
//	24.               'if' <condition> 'then' <statement> <con_else>
//	25.               'while' <condition> 'do' <statement>
//	26.               'for' 'ident' ':=' <expression> 'to' <expression> 'do' <statement>
//	27.               e
//	28. <index> -> '[' <expression> ']'
//	29.             e
//	30. <call_arg> -> '(' <expression> <call_add_arg> ')'
//	31.              e
//	32. <call_add_arg> -> ',' <expression> <call_add_arg>
//	33.                e
//	34. <con_else> -> 'else' <statement>
//	35.              e
//	36. <condition> -> 'odd' <expression>
//	37.                <expression> <cmp_op> <expression>
//	38. <cmp_op> -> '='
//	39.              '<>'
//	40.              '<'
//	41.              '<='
//	42.              '>'
//	43.              '>='
//	44. <expression> -> <sign_op> <term> <add_term>
//	45. <sign_op> -> '+'
//	46.              '-'
//	47.              e
//	48. <add_term> -> '+' <term> <add_term>
//	49.              '-' <term> <add_term>
//	50.              e
//	51. <term> -> <factor> <add_factor>
//	52. <add_factor> -> <mul_op> <factor> <add_factor>
//	53. <mul_op> -> *
//	54.              /
//	55.              %
//	56. <factor> -> 'ident' <index>
//	57.              'number'
//	58.              '(' <expression> ')'

#ifndef COMP_TREE_GEN_H
#define COMP_TREE_GEN_H

#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include "../lexer/lexer.h"

//	size of production list increment by this amount every time it hits limit
#define PROD_INITIAL_CAPACITY    2

typedef enum {
	INVALID_SB = 0, PRG_DCL, CST_BLK, VAR_BLK, ARR_DCL, PROC_DCL, MAIN,
	NAME, LITERAL, UNARY, BINARY
} ESymbolType;

//	associate value for number and indent symbol
typedef struct {
	union {
		char ident[LEXEME_LENGTH];
		long number;
		ETokenType token;
	} value;
	enum {
		ANN_NULL = 0, ANN_IDENT, ANN_NUM, ANN_TOKEN
	} type;
} Annotation;

//	Symbol can be terminal or non-terminal
typedef struct NodeAST {
	ESymbolType symbol;
	Annotation *annotation;
	//	struct NodeAST *parent;
	struct NodeAST **productions;
	int productions_size;
} NodeAST;

//	insert new node to first empty spot on production list, return null on failure
NodeAST *InsertNode( NodeAST *root, NodeAST *child );
// create a node, call safe_calloc()
NodeAST *CreateTreeNode( ESymbolType sb_type, int ann_type, void *ann_ptr );
// recursive post-order traversal free()
void FreeSyntaxTree( NodeAST *root );
// print AST to file
void PrintSyntaxTree( NodeAST *root, FILE *out );
// calloc() with error message
#define safe_calloc( num, size ) ({                                                        \
    void *p = calloc((num), (size));                                                       \
    if (p == NULL) {                                                                       \
        fprintf(stderr, "PARSER ERR: calloc() failure for %zu bytes\n", (num) * (size));   \
    }                                                                                      \
    p;                                                                                     \
})

NodeAST *InsertNode( NodeAST *root, NodeAST *child ) {
	assert( root != NULL );
	if ( root->productions_size == 0 ) {
		root->productions_size = PROD_INITIAL_CAPACITY;
		root->productions = safe_calloc( PROD_INITIAL_CAPACITY, sizeof( NodeAST * ));
		root->productions[0] = child;
		return child;
	} else {
//		child_node->parent = root;
		int end_index = root->productions_size - 1;
		int i;
		for ( i = end_index; i >= 0; --i ) { if ( root->productions[i] != NULL) { break; }}
		if ( i < end_index ) {
			root->productions[i + 1] = child;
		} else {
			root->productions_size += PROD_INITIAL_CAPACITY;
			root->productions = realloc( root->productions, root->productions_size * sizeof( NodeAST * ));
			root->productions[end_index + 1] = child;
			for ( int j = end_index + 2; j < root->productions_size; ++j ) { root->productions[j] = NULL; }
			return child;
		}
	}
	return NULL;
}

NodeAST *CreateTreeNode( ESymbolType sb_type, int ann_type, void *ann_ptr ) {
	NodeAST *new_node = calloc( 1, sizeof( NodeAST ));
	if ( !ann_type ) {
		assert( ann_ptr == NULL );
		return new_node;
	}
	new_node->annotation = calloc( 1, sizeof( Annotation ));
	if ( ann_type == ANN_IDENT ) {
		strncpy( new_node->annotation->value.ident, ann_ptr, LEXEME_LENGTH );
	} else if ( ann_type == ANN_NUM ) {
		new_node->annotation->value.number = *(long *) ann_ptr;
	} else if ( ann_type == ANN_TOKEN ) {
		new_node->annotation->value.token = *(ETokenType *) ann_ptr;
	}
	new_node->annotation->type = ann_type;
	return new_node;
}

void FreeSyntaxTree( NodeAST *root ) {
	for ( int i = 0; i < root->productions_size; ++i ) {
		if ( root->productions[i] == NULL) { break; }
		FreeSyntaxTree( root->productions[i] );
	}
	free( root->annotation );
	free( root->productions );
	free( root );
}

static void SubPrintTree( NodeAST *root, FILE *out, int depth ) {
//	for ( int i = 0; i < depth; ++i ) { fprintf( out, "\t" ); }
//	fprintf( out, "%s", TOKENS[root->token] );
//	if ( !root->value ) {
//		if ( root->token == IDENT ) {
//			fprintf( out, ": %s\n", root->value->ident );
//		} else if ( root->token == NUMBER ) {
//			fprintf( out, ": %ld\n", root->value->number );
//		} else {
//			fprintf(stderr, "PARSER ERR: invalid associative value at token %s, depth %d",
//					TOKENS[root->token], depth );
//		}
//	}
//	fputs( "\n", out );
////	productions_size = 0 means productions = NULL
//	for ( int i = 0; i < root->productions_size; ++i ) {
//		NodeAST *product_ptr = root->productions[i];
//		if ( product_ptr == NULL) { break; }
//		SubPrintTree( product_ptr, out, depth + 1 );
//	}
}

void PrintSyntaxTree( NodeAST *root, FILE *out ) {
	SubPrintTree( root, out, 0 );
}

#endif //COMP_TREE_GEN_H
