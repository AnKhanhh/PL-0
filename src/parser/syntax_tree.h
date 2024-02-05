//  TODO: InsertNode pass child by value

#ifndef COMP_TREE_GEN_H
#define COMP_TREE_GEN_H

#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include "../lexer/lexer.h"

//	size of production list increment by this amount every time it hits limit
#define PROD_INITIAL_CAPACITY    2

typedef enum ESymbolType {
	INVALID_SB = 0, PRG_DCL, CST_DCL, VAR_DCL, ARR_DCL, PROC_DCL, CODE_BLK,
	NAME, LITERAL, U_OP, BIN_OP, SUBSCRIPT, PROC_EVK, PROC_ARG, CONDITIONAL,
	FOR_LOOP, WHILE_LOOP
} ESymbolType;

const char *SYMBOL[] = {
		"INVALID_SB", "PRG_DCL", "CST_DCL", "VAR_DCL", "ARR_DCL", "PROC_DCL", "CODE_BLK",
		"NAME", "LITERAL", "U_OP", "BIN_OP", "SUBSCRIPT", "PROC_EVK", "PROC_ARG", "CONDITIONAL",
		"FOR_LOOP", "WHILE_LOOP"
};
//	associate value for number and indent symbol
typedef struct Annotation {
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
	struct NodeAST **children;
	int child_count;
} NodeAST;

//	insert new node to production list, return pointer to topmost node or null if insertion failed
NodeAST *InsertNode( NodeAST *root, NodeAST *child );
// create a node
NodeAST *CreateTreeNode( ESymbolType sb_type, int ann_type, void *ann_ptr );
// recursive post-order traversal free()
void FreeSyntaxTree( NodeAST *root );
// print AST to file
void PrintSyntaxTree( NodeAST *root, FILE *out );

NodeAST *InsertNode( NodeAST *root, NodeAST *child ) {
	if( root == NULL || child == NULL) { return child; }
	else if(root->child_count == 0 ) {
		root->child_count = PROD_INITIAL_CAPACITY;
		if(( root->children = calloc(PROD_INITIAL_CAPACITY, sizeof( NodeAST * ))) == NULL) {
			fprintf(stderr, "PARSER ERR: calloc() failure for %zu bytes\n", PROD_INITIAL_CAPACITY * sizeof( NodeAST * ));
		}
		root->children[0] = child;
		return root;
	} else {
//		child_node->parent = root;
		int end_index = root->child_count - 1;
		int i;
		for( i = end_index; i >= 0; --i ) { if(root->children[i] != NULL) { break; }}
		if( i < end_index ) {
			root->children[i + 1] = child;
		} else {
			root->child_count += PROD_INITIAL_CAPACITY;
			root->children = realloc(root->children, root->child_count * sizeof( NodeAST * ));
			root->children[end_index + 1] = child;
			for(int j = end_index + 2; j < root->child_count; ++j ) { root->children[j] = NULL; }
			return root;
		}
	}
	return NULL;
}

NodeAST *CreateTreeNode( ESymbolType sb_type, int ann_type, void *ann_ptr ) {
	NodeAST *new_node = calloc( 1, sizeof( NodeAST ));
	new_node->symbol = sb_type;
	if( !ann_type ) {
		assert( ann_ptr == NULL );
		return new_node;
	}
	new_node->annotation = calloc( 1, sizeof( Annotation ));
	if( ann_type == ANN_IDENT ) {
		strncpy( new_node->annotation->value.ident, ann_ptr, LEXEME_LENGTH );
	} else if( ann_type == ANN_NUM ) {
		new_node->annotation->value.number = *(long *) ann_ptr;
	} else if( ann_type == ANN_TOKEN ) {
		new_node->annotation->value.token = *(ETokenType *) ann_ptr;
	}
	new_node->annotation->type = ann_type;
	return new_node;
}

void FreeSyntaxTree( NodeAST *root ) {
	for(int i = 0; i < root->child_count; ++i ) {
		if(root->children[i] == NULL) { break; }
		FreeSyntaxTree( root->children[i] );
	}
	free( root->annotation );
	free( root->children );
	free( root );
}

static void SubPrintTree( NodeAST *root, FILE *out, int depth ) {
	for( int i = 0; i < depth; ++i ) { fputs( "\t", out ); }
	fprintf( out, "%s", SYMBOL[root->symbol] );
	if( root->annotation != NULL) {
		int type = root->annotation->type;
		if( type == ANN_IDENT ) {
			fprintf( out, " : %s", root->annotation->value.ident );
		} else if( type == ANN_NUM ) {
			fprintf( out, " : %ld", root->annotation->value.number );
		} else if( type == ANN_TOKEN ) {
			fprintf( out, " : %s", TOKENS[root->annotation->value.token] );
		} else {
			fprintf(stderr, "PARSER ERR: invalid node annotation in symbol %s, depth %d \n",
					SYMBOL[root->symbol], depth );
		}
	}
	if(root->child_count == 0 ) {
		assert(root->children == NULL );
		fputs( "\n", out );
	} else {
		assert(root->children != NULL );
		fputs( " {\n", out );
		for(int i = 0; i < root->child_count; ++i ) {
			NodeAST *product_ptr = root->children[i];
			if( product_ptr == NULL) { break; }
			SubPrintTree( product_ptr, out, depth + 1 );
		}
		for( int i = 0; i < depth; ++i ) { fputs( "\t", out ); }
		fputs( "}\n", out );
	}
}

void PrintSyntaxTree( NodeAST *root, FILE *out ) {
	SubPrintTree( root, out, 0 );
}

#endif //COMP_TREE_GEN_H
