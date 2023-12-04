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

#include <stdbool.h>
#include <malloc.h>
#include "../lexer/lexer.c"

#ifndef COMP_TREE_GEN_H
#define COMP_TREE_GEN_H
#endif //COMP_TREE_GEN_H

//	max name length for symbol
#define SYMBOL_LENGTH              32
//	production list of each symbol expand by this amount every time it is full
#define SYMBOL_INITIAL_CAPACITY    8

//	Symbol can be terminal or non-terminal
typedef struct {
	char name[SYMBOL_LENGTH];
	//	associated value for number and indent symbol
	union {
		char ident[LEXEME_LENGTH];
		long value;
	} value;
} Symbol;

typedef struct ASTNode {
	Symbol symbol;
	struct ASTNode *parent;
	struct ASTNode **productions;
	int productions_size;
} ASTNode;

//	malloc() a new node
ASTNode *NewNode( Symbol sb );
//	insert node to first null production of root
ASTNode *InsertNode( Symbol sb, ASTNode *root );
// recursive post-order traversal free()
void FreePN( ASTNode *root );

ASTNode *NewNode( Symbol sb ) {
	ASTNode *p_new = malloc( sizeof( ASTNode ));
	memset( p_new, 0, sizeof( ASTNode ));
	p_new->symbol = sb;
	return p_new;
}

ASTNode *InsertNode( Symbol sb, ASTNode *root ) {
	ASTNode *child_node = NewNode( sb );
	if ( root == NULL) {
		return child_node;
	} else {
		child_node->parent = root;
		int end_index = root->productions_size - 1;
		for ( int i = end_index; i >= 0; --i ) {
			if ( root->productions[i] != NULL) {
				if ( i < end_index ) {
					root->productions[i] = child_node;
					return root->productions[i];
				} else {
					root->productions_size += SYMBOL_INITIAL_CAPACITY;
				}
			}
		}
		perror( "parse tree node full" );
		return NULL;
	}
}

void FreePN( ASTNode *root ) {
	for ( int i = 0; i < PROD_LENGTH; ++i ) {
		if ( root->productions[i] == NULL) { break; }
		FreePN( root->productions[i] );
	}
	free( root );
}

void printPN( ASTNode *root, FILE *out ) {

}
