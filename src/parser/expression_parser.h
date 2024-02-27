//	Pratt (TDOP) parser
//	inline function call is not supported

#ifndef COMP_PRATT_PARSER_H
#define COMP_PRATT_PARSER_H

#include "parsing_helper.h"

typedef enum {
	PR_DEFAULT = 0, PR_RELATIONAL = 1, PR_EQUALITY = 2, PR_ADDITIVE = 3, PR_MULTIPLICATIVE = 4, PR_PREFIX = 6, PR_POSTFIX = 7
} EOperatorPrecedence;

// main expression parser
static SyntaxTreeNode *parse_main(FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence);
// build a recursive call stack of prefix that ends when encounter a variable
static SyntaxTreeNode *parse_sign(FILE *in, Symbol *sb, int *tc);
static SyntaxTreeNode *parse_var(FILE *in, Symbol *sb, int *tc);
static SyntaxTreeNode *parse_num(FILE *in, Symbol *sb, int *tc);

// parse grouping parentheses
static SyntaxTreeNode *parse_grouping(FILE *in, Symbol *sb, int *tc);

// parse expression inside subscript
static SyntaxTreeNode *parse_subscript(FILE *in, Symbol *sb, int *tc);

// get precedence of an infix operator, prefix and grouping precedence is handled in parse_main
static EOperatorPrecedence get_precedence(ETokenType token);

// expression parser to be called by main parser
SyntaxTreeNode *expression(FILE *in, Symbol *sb, int *tc) {
	SyntaxTreeNode * expression = CreateNode(ND_EXPRESSION, 0, NULL);
	InsertNode(expression,parse_main(in, sb, tc, PR_DEFAULT));
	return expression;
}

static SyntaxTreeNode *parse_main(FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence) {
	SyntaxTreeNode *left_expression = NULL;
//	all parse implicitly call NextSymbol()
	switch (sb->token) {
		case TK_PLUS:
		case TK_MINUS:
			left_expression = parse_sign(in, sb, tc);
			break;
		case TK_IDENT:
			left_expression = parse_var(in, sb, tc);
			break;
		case TK_NUMBER:
			left_expression = parse_num(in, sb, tc);
			break;
		case TK_LPARENT:
			left_expression = parse_grouping(in, sb, tc);
			break;
		default:
			printf("compiler error: expected expression at token %d\n", *tc);
	}
//	binary operators handled here
	while (prev_precedence < get_precedence(sb->token)) {
//		subscripting is handled separately, need improvement
		if (sb->token == TK_LBRACK) {
			SyntaxTreeNode *operator = CreateNode(ND_SUBSCRIPT, 0, NULL);
			InsertNode(operator, left_expression);
			InsertNode(operator, parse_subscript(in, sb, tc));

			left_expression = operator;
		} else {
			SyntaxTreeNode *operator = CreateNode(ND_BINARY_OP, ANN_TOKEN, &(sb->token));
			NextSymbol(in, sb, tc);
			InsertNode(operator, left_expression);
			EOperatorPrecedence this_precedence = get_precedence(sb->token);

			InsertNode(operator, parse_main(in, sb, tc, this_precedence));

			left_expression = operator;
		}
	}
	return left_expression;
}

static SyntaxTreeNode *parse_sign(FILE *in, Symbol *sb, int *tc) {
	SyntaxTreeNode *prefix = CreateNode(ND_UNARY_OP, ANN_TOKEN, &(sb->token));
	NextSymbol(in, sb, tc);
	SyntaxTreeNode *right_expression = parse_main(in, sb, tc, PR_PREFIX);
	InsertNode(prefix, right_expression);
	return prefix;
}

SyntaxTreeNode *parse_var(FILE *in, Symbol *sb, int *tc) {
	SyntaxTreeNode *var = CreateNode(ND_IDENT, ANN_IDENT, sb->tag.ident);
	NextSymbol(in, sb, tc);
	return var;
}

SyntaxTreeNode *parse_num(FILE *in, Symbol *sb, int *tc) {
	SyntaxTreeNode *num = CreateNode(ND_INTEGER, ANN_NUM, &(sb->tag.number));
	NextSymbol(in, sb, tc);
	return num;
}

static SyntaxTreeNode *parse_grouping(FILE *in, Symbol *sb, int *tc) {
	NextSymbol(in, sb, tc);
	SyntaxTreeNode *group = parse_main(in, sb, tc, PR_DEFAULT);
	if (accept_tk(sb, TK_RPARENT)) {
		NextSymbol(in, sb, tc);
		return group;
	} else {
		SyntaxError( "expected \')\' to match (", *tc );
		return NULL;
	}
}

static SyntaxTreeNode *parse_subscript(FILE *in, Symbol *sb, int *tc) {
	NextSymbol(in, sb, tc);
	SyntaxTreeNode *group = parse_main(in, sb, tc, PR_POSTFIX);
	if (accept_tk(sb, TK_RBRACK)) {
		NextSymbol(in, sb, tc);
		return group;
	} else {
		SyntaxError( "expected \']\' to match [", *tc );
		return NULL;
	}
}

static EOperatorPrecedence get_precedence(ETokenType token) {
	switch (token) {
		case TK_EQU:
		case TK_NEQ:
			return PR_EQUALITY;
		case TK_LSS:
		case TK_GTR:
		case TK_LEQ:
		case TK_GEQ:
			return PR_RELATIONAL;
		case TK_PLUS:
		case TK_MINUS:
			return PR_ADDITIVE;
		case TK_TIMES:
		case TK_SLASH:
		case TK_PERCENT:
			return PR_MULTIPLICATIVE;
		case TK_LBRACK:
			return PR_POSTFIX;
		case TK_RPARENT:
		default:
			return PR_DEFAULT;
	}
}

#endif //COMP_PRATT_PARSER_H
