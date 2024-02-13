//	Pratt (TDOP) parser
//	inline function call is not supported

#ifndef COMP_PRATT_PARSER_H
#define COMP_PRATT_PARSER_H

#include "parse_helper.h"

typedef enum {
	PR_DEFAULT = 0, PR_RELATIONAL = 1, PR_EQUALITY = 2, PR_ADDITIVE = 3, PR_MULTIPLICATIVE = 4, PR_PREFIX = 6, PR_POSTFIX = 7
} EOperatorPrecedence;

// main expression parser
static NodeAST *parse_main(FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence);
// build a recursive call stack of prefix that ends when encounter a variable
static NodeAST *parse_sign(FILE *in, Symbol *sb, int *tc);
static NodeAST *parse_var(FILE *in, Symbol *sb, int *tc);
static NodeAST *parse_num(FILE *in, Symbol *sb, int *tc);
// parse grouping parentheses
static NodeAST *parse_grouping(FILE *in, Symbol *sb, int *tc);
// parse expression inside subscript
static NodeAST *parse_subscript(FILE *in, Symbol *sb, int *tc);
// get precedence of an infix operator, prefix and grouping precedence is handled in parse_main
static EOperatorPrecedence get_precedence(ETokenType token);

// expression parser to be called by main parser
NodeAST *expression(FILE *in, Symbol *sb, int *tc) {
	return parse_main(in, sb, tc, PR_DEFAULT);
}

static NodeAST *parse_main(FILE *in, Symbol *sb, int *tc, EOperatorPrecedence prev_precedence) {
	NodeAST *left_expression = NULL;
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
		if (sb->token == TK_LBRACK) {
			NodeAST *operator = CreateTreeNode(ND_SUBSCRIPT, 0, NULL);
			InsertNode(operator, left_expression);
			InsertNode(operator, parse_subscript(in, sb, tc));
		} else {
			NodeAST *operator = CreateTreeNode(ND_BINARY_OP, ANN_TOKEN, &(sb->token));
			NextSymbol(in, sb, tc);
			InsertNode(operator, left_expression);
			EOperatorPrecedence this_precedence = get_precedence(sb->token);

			InsertNode(operator, parse_main(in, sb, tc, this_precedence));

			left_expression = operator;
		}
	}
	return left_expression;
}

static NodeAST *parse_sign(FILE *in, Symbol *sb, int *tc) {
	NodeAST *prefix = CreateTreeNode(ND_UNARY_OP, ANN_TOKEN, &(sb->token));
	NextSymbol(in, sb, tc);
	NodeAST *right_expression = parse_main(in, sb, tc, PR_PREFIX);
	InsertNode(prefix, right_expression);
	return prefix;
}

NodeAST *parse_var(FILE *in, Symbol *sb, int *tc) {
	NodeAST *var = CreateTreeNode(ND_NAME, ANN_IDENT, sb->tag.ident);
	NextSymbol(in, sb, tc);
	return var;
}

NodeAST *parse_num(FILE *in, Symbol *sb, int *tc) {
	NodeAST *num = CreateTreeNode(ND_LITERAL, ANN_NUM, &(sb->tag.number));
	NextSymbol(in, sb, tc);
	return num;
}

static NodeAST *parse_grouping(FILE *in, Symbol *sb, int *tc) {
	NextSymbol(in, sb, tc);
	NodeAST *group = parse_main(in, sb, tc, PR_DEFAULT);
	if (accept_tk(sb, TK_RPARENT)) {
		NextSymbol(in, sb, tc);
		return group;
	} else {
		ParserThrow("expected \')\' to match (", *tc);
		return NULL;
	}
}

static NodeAST *parse_subscript(FILE *in, Symbol *sb, int *tc) {
	NextSymbol(in, sb, tc);
	NodeAST *group = parse_main(in, sb, tc, PR_POSTFIX);
	if (accept_tk(sb, TK_RBRACK)) {
		NextSymbol(in, sb, tc);
		return group;
	} else {
		ParserThrow("expected \']\' to match [", *tc);
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
