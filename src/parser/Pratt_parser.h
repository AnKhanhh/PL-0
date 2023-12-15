// pratt parser for arithmetic expression only
// PL0 don't have return type so inline function call no supported
// TODO: implement exponent and increment/decrement token, support postfix operator

#ifndef COMP_PRATT_PARSER_H
#define COMP_PRATT_PARSER_H

#include "parse_helper.h"

typedef enum {
	PR_DEFAULT = 0, PR_ASSIGN = 1, PR_ADD = 3, PR_SUB = 3, PR_MULTI = 4, PR_DIV = 4, PR_MOD = 4,
	PR_EXP = 5, PR_PREFIX = 6, PR_POSTFIX = 7
} EOperatorPrecedence;

// main expression parser
static NodeAST *parse_main( FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence );
// build a recursive call stack of prefix that end when encounter a variable
static NodeAST *parse_sign( FILE *in, Symbol *sb, int *tc );
// parse a single token into a symbol
static NodeAST *parse_var( FILE *in, Symbol *sb, int *tc );
static NodeAST *parse_num( FILE *in, Symbol *sb, int *tc );
// parse grouping parentheses
static NodeAST *parse_grouping( FILE *in, Symbol *sb, int *tc );
// get precedence of an infix operator
// prefix and grouping precedence is handled implicitly
static EOperatorPrecedence get_precedence( ETokenType token, const int *tc );
// expression parser to be called by main parser
NodeAST *expression( FILE *in, Symbol *sb, int *tc ) {
	return parse_main( in, sb, tc, PR_DEFAULT );
}

static NodeAST *parse_main( FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence ) {
	NodeAST *left_expression = NULL;
//	all parse implicitly call NextSymbol()
	switch( sb->token ) {
		case PLUS:
		case MINUS:
			left_expression = parse_sign( in, sb, tc );
			break;
		case IDENT:
			left_expression = parse_var( in, sb, tc );
			break;
		case NUMBER:
			left_expression = parse_num( in, sb, tc );
			break;
		case LPARENT:
			left_expression = parse_grouping( in, sb, tc );
			break;
		default:
			printf( "Pratt parser debug: expression not started with a variable at token %d\n", *tc );
	}

	while( prev_precedence < get_precedence( sb->token, tc )) {
		NodeAST *operator = CreateTreeNode( BIN_OP, ANN_TOKEN, &( sb->token ));
		NextSymbol( in, sb, tc );
		InsertNode( operator, left_expression );

		EOperatorPrecedence this_precedence = get_precedence( sb->token, tc );
		InsertNode( operator, parse_main( in, sb, tc, this_precedence ));
		left_expression = operator;
	}
	return left_expression;
}

static NodeAST *parse_sign( FILE *in, Symbol *sb, int *tc ) {
	NodeAST *prefix = CreateTreeNode( U_OP, ANN_TOKEN, &( sb->tag.number ));
	NextSymbol( in, sb, tc );
	NodeAST *right_expression = parse_main( in, sb, tc, PR_PREFIX );
	InsertNode( prefix, right_expression );
	return prefix;
}

NodeAST *parse_var( FILE *in, Symbol *sb, int *tc ) {
	NodeAST *var = CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident );
	NextSymbol( in, sb, tc );
	return var;
}

NodeAST *parse_num( FILE *in, Symbol *sb, int *tc ) {
	NodeAST *num = CreateTreeNode( LITERAL, ANN_NUM, &( sb->tag.number ));
	NextSymbol( in, sb, tc );
	return num;
}

static NodeAST *parse_grouping( FILE *in, Symbol *sb, int *tc ) {
	NextSymbol( in, sb, tc );
	NodeAST *group = parse_main( in, sb, tc, PR_DEFAULT );
	if( accept_tk( sb, RPARENT )) {
		NextSymbol( in, sb, tc );
		return group;
	} else {
		ParserThrow( "expression grouping expected to end here", *tc );
		return NULL;
	}
}

static EOperatorPrecedence get_precedence( ETokenType token, const int *tc ) {
	switch( token ) {
		case PLUS:
			return PR_ADD;
		case MINUS:
			return PR_SUB;
		case TIMES:
			return PR_MULTI;
		case SLASH:
			return PR_DIV;
		case PERCENT:
			return PR_MOD;
		default:
			printf( "Pratt parser debug: an expression ended at token %d\n", *tc );
			return PR_DEFAULT;
	}
}

#endif //COMP_PRATT_PARSER_H
