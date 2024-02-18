#ifndef COMP_RECURSIVE_DESCENT_PARSER_H
#define COMP_RECURSIVE_DESCENT_PARSER_H

#include "syntax_tree.h"
#include "expression_parser.h"

// recursive descent LL(1) parser
void program(FILE *in, Symbol *sb, int *tc, SyntaxTreeNode **root_ptr );
static void block(FILE *in, Symbol *sb, int *tc, SyntaxTreeNode *root );
static void statement(FILE *in, Symbol *sb, int *tc, SyntaxTreeNode *root );

void program(FILE *in, Symbol *sb, int *tc, SyntaxTreeNode **root_ptr ) {
	NextSymbol( in, sb, tc );
	if( accept_tk( sb, PROGRAM )) {
		NextSymbol( in, sb, tc );
		if( accept_tk(sb, TK_IDENT )) {
			*root_ptr = CreateNode(ND_PRG_DCL, ANN_IDENT, sb->tag.ident);
			NextSymbol( in, sb, tc );
			if( accept_tk(sb, TK_SEMICOLON )) {
				NextSymbol( in, sb, tc );
				block( in, sb, tc, *root_ptr );
				if( accept_tk(sb, TK_PERIOD )) {} else { ParserThrow("PROGRAM terminating token expected", *tc ); }
			} else { ParserThrow( "PROGRAM semicolon expected", *tc ); }
		} else { ParserThrow( "program name expected", *tc ); }
	} else { ParserThrow( "keyword PROGRAM expected", *tc ); }
}

void block(FILE *in, Symbol *sb, int *tc, SyntaxTreeNode *root ) {
	if( accept_tk( sb, CONST )) {
		SyntaxTreeNode *const_block = CreateNode(ND_CST_DCL, 0, NULL);
		InsertNode( root, const_block );
		do {
			NextSymbol( in, sb, tc );
			if( accept_tk(sb, TK_IDENT )) {
				SyntaxTreeNode *const_name = CreateNode(ND_IDENT, ANN_IDENT, sb->tag.ident);
				NextSymbol( in, sb, tc );
				if( accept_tk(sb, TK_EQU )) {
					SyntaxTreeNode *equals = CreateNode(ND_BINARY_OP, ANN_TOKEN, &(sb->token));
					InsertNode( const_block, equals );
					NextSymbol( in, sb, tc );
					if( accept_tk(sb, TK_NUMBER )) {
						InsertNode( equals, const_name );
						InsertNode(equals, CreateNode(ND_INTEGER, ANN_NUM, &(sb->tag.number)));
						NextSymbol( in, sb, tc );
					} else { ParserThrow( "CONST data expected", *tc ); }
				} else { ParserThrow( "equal operator expected", *tc ); }
			} else { ParserThrow( "CONST name expected", *tc ); }
		} while(accept_tk(sb, TK_COMMA ));
		if( accept_tk(sb, TK_SEMICOLON )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "missing CONST block semicolon", *tc ); }
	}
	if( accept_tk( sb, VAR )) {
		SyntaxTreeNode *var_blk = CreateNode(ND_VAR_DCL, 0, NULL);
		InsertNode( root, var_blk );
		do {
			NextSymbol( in, sb, tc );
			if( accept_tk(sb, TK_IDENT )) {
				SyntaxTreeNode *name = CreateNode(ND_IDENT, ANN_IDENT, sb->tag.ident);
				NextSymbol( in, sb, tc );
				if( accept_tk(sb, TK_LBRACK )) {
					SyntaxTreeNode *arr_dcl = CreateNode(ND_ARR_DCL, 0, NULL);
					InsertNode( var_blk, arr_dcl );
					NextSymbol( in, sb, tc );
					if( accept_tk(sb, TK_NUMBER )) {
						InsertNode( arr_dcl, name );
						InsertNode(arr_dcl, CreateNode(ND_INTEGER, ANN_NUM, &(sb->tag.number)));
						NextSymbol( in, sb, tc );
						if( accept_tk(sb, TK_RBRACK )) {
							NextSymbol( in, sb, tc );
						} else { ParserThrow( "right bracket expected", *tc ); }
					} else { ParserThrow( "array index expected", *tc ); }
				} else {
					InsertNode( var_blk, name );
				}
			} else { ParserThrow( "invalid name", *tc ); }
		} while(accept_tk(sb, TK_COMMA ));
		if( accept_tk(sb, TK_SEMICOLON )) {
			NextSymbol( in, sb, tc );
		} else { printf( "semicolon missing at %d \n", *tc ); }
	}
	while ( accept_tk( sb, PROCEDURE )) {
		NextSymbol( in, sb, tc );
		if( accept_tk(sb, TK_IDENT )) {
			SyntaxTreeNode *proc_blk = CreateNode(ND_PROC_DCL, ANN_IDENT, sb->tag.ident);
			InsertNode( root, proc_blk );
			NextSymbol( in, sb, tc );
			if( accept_tk(sb, TK_LPARENT )) {
				SyntaxTreeNode *proc_arg = CreateNode(ND_PARAM, 0, NULL);
				InsertNode( proc_blk, proc_arg );
				do {
					NextSymbol( in, sb, tc );
					if( accept_tk( sb, VAR )) { NextSymbol( in, sb, tc ); }
					if( accept_tk(sb, TK_IDENT )) {
						InsertNode(proc_arg, CreateNode(ND_IDENT, ANN_IDENT, sb->tag.ident));
						NextSymbol( in, sb, tc );
					} else { ParserThrow( "parameter name expected", *tc ); }
				} while(accept_tk(sb, TK_SEMICOLON ));
				if( accept_tk(sb, TK_RPARENT )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "right parentheses expected", *tc ); }
			}
			if( accept_tk(sb, TK_SEMICOLON )) {
				NextSymbol( in, sb, tc );
				block( in, sb, tc, proc_blk );
				if( accept_tk(sb, TK_SEMICOLON )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "procedure termination semicolon expected", *tc ); }
			} else { ParserThrow( "procedure declaration semicolon expected", *tc ); }
		} else { ParserThrow( "procedure name expected", *tc ); }
	}
	if( accept_tk( sb, BEGIN )) {
		SyntaxTreeNode *main_blk = CreateNode(ND_CODE_BLK, 0, NULL);
		InsertNode( root, main_blk );
		do {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, main_blk );
		} while(accept_tk(sb, TK_SEMICOLON ));
		if( accept_tk( sb, END )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "keyword END expected", *tc ); }
	} else { ParserThrow( "keyword BEGIN expected", *tc ); }
}

void statement(FILE *in, Symbol *sb, int *tc, SyntaxTreeNode *root ) {
	if( accept_tk(sb, TK_IDENT )) {
		SyntaxTreeNode *ident = CreateNode(ND_IDENT, ANN_IDENT, sb->tag.ident);
		SyntaxTreeNode *left_side = NULL;
		NextSymbol( in, sb, tc );
		if( accept_tk(sb, TK_LBRACK )) {
			left_side = CreateNode(ND_SUBSCRIPT, 0, NULL);
			InsertNode(left_side, ident );
			NextSymbol( in, sb, tc );
			InsertNode( left_side, expression( in, sb, tc ));
			if( accept_tk(sb, TK_RBRACK )) {
				NextSymbol( in, sb, tc );
			} else { ParserThrow( "right bracket expected", *tc ); }
		} else { left_side = ident; }
		if( accept_tk(sb, TK_ASSIGN )) {
			SyntaxTreeNode *assignment = CreateNode(ND_BINARY_OP, ANN_TOKEN, &(sb->token));
			NextSymbol( in, sb, tc );
			InsertNode( root, assignment );
			InsertNode( assignment, left_side );
			InsertNode( assignment, expression( in, sb, tc ));
		} else { ParserThrow( "assignment operator expected", *tc ); }
	} else if( accept_tk( sb, CALL )) {
		NextSymbol( in, sb, tc );
		if( accept_tk(sb, TK_IDENT )) {
			SyntaxTreeNode *proc_evk = CreateNode(ND_FUNC_CALL, ANN_IDENT, sb->tag.ident);
			InsertNode( root, proc_evk );
			NextSymbol( in, sb, tc );
			if( accept_tk(sb, TK_LPARENT )) {
				do {
					NextSymbol( in, sb, tc );
					InsertNode( proc_evk, expression( in, sb, tc ));
				} while(accept_tk(sb, TK_COMMA ));
				if( accept_tk(sb, TK_RPARENT )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "right parentheses expected", *tc ); }
			}
		}
	} else if( accept_tk( sb, BEGIN )) {
		SyntaxTreeNode *code_blk = CreateNode(ND_CODE_BLK, 0, NULL);
		InsertNode( root, code_blk );
		do {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, code_blk );
		} while(accept_tk(sb, TK_SEMICOLON ));
		if( accept_tk( sb, END )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "keyword END expected at the end of code block", *tc ); }
	} else if( accept_tk( sb, IF )) {
		NextSymbol( in, sb, tc );
		SyntaxTreeNode *conditional = CreateNode(ND_CONDITIONAL, 0, NULL);
		InsertNode( root, conditional );
		SyntaxTreeNode *cond_exp = expression(in, sb, tc );
		InsertNode( conditional, cond_exp );
		if( accept_tk( sb, THEN )) {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, conditional );
		} else { ParserThrow( "keyword THEN expected after IF", *tc ); }
		if( accept_tk( sb, ELSE )) {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, conditional );
		}
	} else if( accept_tk( sb, WHILE )) {
		NextSymbol( in, sb, tc );
		SyntaxTreeNode *while_loop = CreateNode(ND_WHILE_LOOP, 0, NULL);
		InsertNode( root, while_loop );
		InsertNode( while_loop, expression( in, sb, tc ));
		if( accept_tk( sb, DO )) {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, while_loop );
		} else { ParserThrow( " keyword DO expected after WILLE", *tc ); }
	} else if( accept_tk( sb, FOR )) {
		NextSymbol( in, sb, tc );
		SyntaxTreeNode *for_loop = CreateNode(ND_FOR_LOOP, 0, NULL);
		InsertNode( root, for_loop );
		if( accept_tk(sb, TK_IDENT )) {
			InsertNode(for_loop, CreateNode(ND_IDENT, ANN_IDENT, sb->tag.ident));
			NextSymbol( in, sb, tc );
			if( accept_tk(sb, TK_ASSIGN )) {
				NextSymbol( in, sb, tc );
				InsertNode( for_loop, expression( in, sb, tc ));
				if( accept_tk( sb, TO )) {
					NextSymbol( in, sb, tc );
					InsertNode( for_loop, expression( in, sb, tc ));
					if( accept_tk( sb, DO )) {
						NextSymbol( in, sb, tc );
						statement( in, sb, tc, for_loop );
					} else { ParserThrow( "keyword DO expected after TO", *tc ); }
				} else { ParserThrow( "keyword TO expected after FROM", *tc ); }
			} else { ParserThrow( "assignment operator expected in FOR loop", *tc ); }
		} else { ParserThrow( "invalid iterator name", *tc ); }
	}
}

#endif //COMP_RECURSIVE_DESCENT_PARSER_H
