#ifndef COMP_TREE_GEN_H
#define COMP_TREE_GEN_H

#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include "../lexer/lexer.h"

//	size of children increment by this amount
#define NODE_INITIAL_CAP    2

typedef enum ESymbolType {
	ND_PRG_DCL = 1, ND_CST_DCL, ND_VAR_DCL, ND_ARR_DCL, ND_PROC_DCL, ND_CODE_BLK,
	ND_IDENT, ND_INTEGER, ND_UNARY_OP, ND_BINARY_OP, ND_SUBSCRIPT, ND_FUNC_CALL, ND_PARAM,
	ND_CONDITIONAL, ND_FOR_LOOP, ND_WHILE_LOOP, ND_EXPRESSION
} ETreeNodeType;

const char *TREE_NODE_NAME[] = {
		"", "ND_PRG_DCL", "ND_CST_DCL", "ND_VAR_DCL", "ND_ARR_DCL", "ND_PROC_DCL", "ND_CODE_BLK",
		"ND_IDENT", "ND_INTEGER", "ND_UNARY_OP", "ND_BINARY_OP", "ND_SUBSCRIPT", "ND_FUNC_CALL", "ND_PARAM",
		"ND_CONDITIONAL", "ND_FOR_LOOP", "ND_WHILE_LOOP", "ND_EXPRESSION"
};
//	associate data for number and indent type
typedef struct TreeAnnotation {
	union {
		char ident[LEXEME_LENGTH];
		long number;
		ETokenType token;
	} data;
	enum EAnnotationType {
		ANN_IDENT = 1, ANN_NUM, ANN_TOKEN
	} type;
} TreeAnnotation;

//	Symbol can be terminal or non-terminal
typedef struct SyntaxTreeNode {
	ETreeNodeType type;
	TreeAnnotation *annotation;
	struct SyntaxTreeNode *parent;
	struct SyntaxTreeNode **children;
	int child_count;
} SyntaxTreeNode;

//	insert new node to production list, return pointer to topmost node or null if insertion failed
SyntaxTreeNode *InsertNode(SyntaxTreeNode *root, SyntaxTreeNode *child);
// create a node
SyntaxTreeNode *CreateNode(ETreeNodeType sb_type, enum EAnnotationType ann_type, void *ann_ptr);
// recursive post-order traversal free()
void FreeSyntaxTree(SyntaxTreeNode *root);
// print AST to file
void PrintSyntaxTree(SyntaxTreeNode *root, FILE *out);

SyntaxTreeNode *InsertNode(SyntaxTreeNode *root, SyntaxTreeNode *child) {
	if (root == NULL || child == NULL) { return child; }
	else if (root->child_count == 0) {
		root->child_count = NODE_INITIAL_CAP;
		if ((root->children = calloc(NODE_INITIAL_CAP, sizeof(SyntaxTreeNode *))) == NULL) {
			fprintf(stderr, "compiler error: calloc() failure for %zu bytes\n", NODE_INITIAL_CAP * sizeof(SyntaxTreeNode *));
		}
		root->children[0] = child;
		return root;
	} else {
		child->parent = root;
		int end_index = root->child_count - 1;
		int i;
		for (i = end_index; i >= 0; --i) { if (root->children[i] != NULL) { break; }}
		if (i < end_index) {
			root->children[i + 1] = child;
		} else {
			root->child_count += NODE_INITIAL_CAP;
			root->children = realloc(root->children, root->child_count * sizeof(SyntaxTreeNode *));
			root->children[end_index + 1] = child;
			for (int j = end_index + 2; j < root->child_count; ++j) { root->children[j] = NULL; }
			return root;
		}
	}
	return NULL;
}

SyntaxTreeNode *CreateNode(ETreeNodeType sb_type, enum EAnnotationType ann_type, void *ann_ptr) {
	SyntaxTreeNode *new_node = calloc(1, sizeof(SyntaxTreeNode));
	new_node->type = sb_type;
	if (!ann_type) {
		assert(ann_ptr == NULL);
		return new_node;
	}
	new_node->annotation = calloc(1, sizeof(TreeAnnotation));
	if (ann_type == ANN_IDENT) {
		snprintf(new_node->annotation->data.ident, LEXEME_LENGTH, "%s", (char *) ann_ptr);
	} else if (ann_type == ANN_NUM) {
		new_node->annotation->data.number = *(long *) ann_ptr;
	} else if (ann_type == ANN_TOKEN) {
		new_node->annotation->data.token = *(ETokenType *) ann_ptr;
	}
	new_node->annotation->type = ann_type;
	return new_node;
}

void FreeSyntaxTree(SyntaxTreeNode *root) {
	for (int i = 0; i < root->child_count; ++i) {
		if (root->children[i] == NULL) { break; }
		FreeSyntaxTree(root->children[i]);
	}
	free(root->annotation);
	free(root->children);
	free(root);
}

static void SubPrintTree(SyntaxTreeNode *root, FILE *out, int depth) {
	for (int i = 0; i < depth; ++i) { fputs("\t", out); }
	fprintf(out, "%s", TREE_NODE_NAME[root->type]);
	if (root->annotation != NULL) {
		int type = root->annotation->type;
		if (type == ANN_IDENT) {
			fprintf(out, " : %s", root->annotation->data.ident);
		} else if (type == ANN_NUM) {
			fprintf(out, " : %ld", root->annotation->data.number);
		} else if (type == ANN_TOKEN) {
			fprintf(out, " : %s", TOKENS[root->annotation->data.token]);
		} else {
			fprintf(stderr, "compiler error: invalid node annotation in type %s, depth %d \n",
					TREE_NODE_NAME[root->type], depth);
		}
	}
	if (root->child_count == 0) {
		assert(root->children == NULL);
		fputs("\n", out);
	} else {
		assert(root->children != NULL);
		fputs(" {\n", out);
		for (int i = 0; i < root->child_count; ++i) {
			SyntaxTreeNode *product_ptr = root->children[i];
			if (product_ptr == NULL) { break; }
			SubPrintTree(product_ptr, out, depth + 1);
		}
		for (int i = 0; i < depth; ++i) { fputs("\t", out); }
		fputs("}\n", out);
	}
}

void PrintSyntaxTree(SyntaxTreeNode *root, FILE *out) {
	SubPrintTree(root, out, 0);
}

#endif //COMP_TREE_GEN_H
