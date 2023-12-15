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

// return a node represent variable
static NodeAST *parse_name( FILE *in, Symbol *sb, int *tc );
// build a recursive call stack of prefix that end when encounter a variable
static NodeAST *parse_prefix( FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence );
// main expression parser
static NodeAST *parse_expression( FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence );
// get precedence of current operator token, throw
static EOperatorPrecedence this_precedence( ETokenType token, int *tc );
// expression parser to be called by main parser
NodeAST *expression( FILE *in, Symbol *sb, int *tc ) {
	return parse_expression( in, sb, tc, PR_DEFAULT );
}

static NodeAST *parse_expression( FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence ) {
	NodeAST *left_expression = NULL;
//	check for prefix
	if ( accept_tk( sb, PLUS ) || accept_tk( sb, MINUS )) {
		left_expression = parse_prefix( in, sb, tc, PR_DEFAULT );
	} else if ( accept_tk( sb, IDENT )) {
		left_expression = parse_name( in, sb, tc );
	} else { ParserThrow( "WARNING: expression not started with a variable", *tc ); }

	NodeAST *operator = NULL;
	while ( prev_precedence < this_precedence( sb->token, tc )) {
		operator = CreateTreeNode( BIN_OP, ANN_TOKEN, &(sb->token));
		InsertNode( operator, left_expression );
		NodeAST *right_expression = parse_expression( in, sb, tc, this_precedence( sb->token, tc ));
		InsertNode( operator, right_expression );
		NextSymbol( in, sb, tc );
	}
	return operator;
}

NodeAST *parse_name( FILE *in, Symbol *sb, int *tc ) {
	NodeAST *name = CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident );
	NextSymbol( in, sb, tc );
	return name;
}

static NodeAST *parse_prefix( FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence ) {
	NodeAST *prefix = CreateTreeNode( U_OP, ANN_TOKEN, &(sb->tag.number));
	NextSymbol( in, sb, tc );
	NodeAST *right_expression = parse_expression( in, sb, tc, prev_precedence );
	InsertNode( prefix, right_expression );
	return prefix;
}

static EOperatorPrecedence this_precedence( ETokenType token, int *tc ) {
	switch ( token ) {
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
			ParserThrow("WARNING: evoking precedence for non-operator", *tc);
			return PR_DEFAULT;
	}
}

#endif //COMP_PRATT_PARSER_H
