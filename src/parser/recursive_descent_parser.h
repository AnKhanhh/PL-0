#ifndef COMP_RECURSIVE_DESCENT_PARSER_H
#define COMP_RECURSIVE_DESCENT_PARSER_H

//	an int counter is used to track token that produces syntax error,
//	so error message can be wrong for files with token number > INT_MAX

#include "syntax_tree.h"
#include "Pratt_parser.h"

// recursive descent LL(1) parser functions, arithmetic expression parsing is in Prat_parser.h
void program( FILE *in, Symbol *sb, int *tc, NodeAST **root_ptr );
void block( FILE *in, Symbol *sb, int *tc, NodeAST *root );
void statement( FILE *in, Symbol *sb, int *tc, NodeAST *root );

void program( FILE *in, Symbol *sb, int *tc, NodeAST **root_ptr ) {
	NextSymbol( in, sb, tc );
	if( accept_tk( sb, PROGRAM )) {
		NextSymbol( in, sb, tc );
		if( accept_tk( sb, IDENT )) {
			*root_ptr = CreateTreeNode( PRG_DCL, ANN_IDENT, sb->tag.ident );
			NextSymbol( in, sb, tc );
			if( accept_tk( sb, SEMICOLON )) {
				NextSymbol( in, sb, tc );
				block( in, sb, tc, *root_ptr );
				if( accept_tk( sb, PERIOD )) {} else { ParserThrow( "PROGRAM terminating token expected", *tc ); }
			} else { ParserThrow( "PROGRAM semicolon expected", *tc ); }
		} else { ParserThrow( "program name expected", *tc ); }
	} else { ParserThrow( "keyword PROGRAM expected", *tc ); }
}

void block( FILE *in, Symbol *sb, int *tc, NodeAST *root ) {
	if( accept_tk( sb, CONST )) {
		NodeAST *const_block = CreateTreeNode( CST_DCL, ANN_NULL, NULL);
		InsertNode( root, const_block );
		do {
			NextSymbol( in, sb, tc );
			if( accept_tk( sb, IDENT )) {
				NodeAST *const_name = CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident );
				NextSymbol( in, sb, tc );
				if( accept_tk( sb, EQU )) {
					NodeAST *equals = CreateTreeNode( BIN_OP, ANN_TOKEN, &( sb->token ));
					InsertNode( const_block, equals );
					NextSymbol( in, sb, tc );
					if( accept_tk( sb, NUMBER )) {
						InsertNode( equals, const_name );
						InsertNode( equals, CreateTreeNode( LITERAL, ANN_NUM, &( sb->tag.number )));
						NextSymbol( in, sb, tc );
					} else { ParserThrow( "CONST value expected", *tc ); }
				} else { ParserThrow( "equal operator expected", *tc ); }
			} else { ParserThrow( "CONST name expected", *tc ); }
		} while(accept_tk( sb, COMMA ));
		if( accept_tk( sb, SEMICOLON )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "missing CONST block semicolon", *tc ); }
	}
	if( accept_tk( sb, VAR )) {
		NodeAST *var_blk = CreateTreeNode( VAR_DCL, ANN_NULL, NULL);
		InsertNode( root, var_blk );
		do {
			NextSymbol( in, sb, tc );
			if( accept_tk( sb, IDENT )) {
				NodeAST *name = CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident );
				NextSymbol( in, sb, tc );
				if( accept_tk( sb, LBRACK )) {
					NodeAST *arr_dcl = CreateTreeNode( ARR_DCL, ANN_NULL, NULL);
					InsertNode( var_blk, arr_dcl );
					NextSymbol( in, sb, tc );
					if( accept_tk( sb, NUMBER )) {
						InsertNode( arr_dcl, name );
						InsertNode( arr_dcl, CreateTreeNode( ARR_DCL, LITERAL, &( sb->tag.number )));
						NextSymbol( in, sb, tc );
						if( accept_tk( sb, RBRACK )) {
							NextSymbol( in, sb, tc );
						} else { ParserThrow( "right bracket expected", *tc ); }
					} else { ParserThrow( "array index expected", *tc ); }
				} else {
					InsertNode( var_blk, name );
				}
			} else { ParserThrow( "invalid name", *tc ); }
		} while(accept_tk( sb, COMMA ));
		if( accept_tk( sb, SEMICOLON )) {
			NextSymbol( in, sb, tc );
		} else { printf( "semicolon missing at %d \n", *tc ); }
	}
	if( accept_tk( sb, PROCEDURE )) {
		NextSymbol( in, sb, tc );
		if( accept_tk( sb, IDENT )) {
			NodeAST *proc_blk = CreateTreeNode( PROC_DCL, ANN_IDENT, sb->tag.ident );
			InsertNode( root, proc_blk );
			NextSymbol( in, sb, tc );
			if( accept_tk( sb, LPARENT )) {
				NodeAST *proc_arg = CreateTreeNode( PROC_ARG, ANN_NULL, NULL);
				InsertNode( proc_blk, proc_arg );
				do {
					NextSymbol( in, sb, tc );
					if( accept_tk( sb, VAR )) { NextSymbol( in, sb, tc ); }
					if( accept_tk( sb, IDENT )) {
						InsertNode( proc_arg, CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident ));
						NextSymbol( in, sb, tc );
					} else { ParserThrow( "parameter name expected", *tc ); }
				} while(accept_tk( sb, SEMICOLON ));
				if( accept_tk( sb, RPARENT )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "right parentheses expected", *tc ); }
			}
			if( accept_tk( sb, SEMICOLON )) {
				NextSymbol( in, sb, tc );
				block( in, sb, tc, proc_blk );
				if( accept_tk( sb, SEMICOLON )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "procedure termination semicolon expected", *tc ); }
			} else { ParserThrow( "procedure declaration semicolon expected", *tc ); }
		} else { ParserThrow( "procedure name expected", *tc ); }
	}
	if( accept_tk( sb, BEGIN )) {
		NodeAST *main_blk = CreateTreeNode( CODE_BLK, ANN_NULL, NULL);
		InsertNode( root, main_blk );
		do {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, main_blk );
		} while(accept_tk( sb, SEMICOLON ));
		if( accept_tk( sb, END )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "keyword END expected", *tc ); }
	} else { ParserThrow( "keyword BEGIN expected", *tc ); }
}

void statement( FILE *in, Symbol *sb, int *tc, NodeAST *root ) {
	if( accept_tk( sb, IDENT )) {
		NodeAST *name = CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident );
		NodeAST *left_side = NULL;
		NextSymbol( in, sb, tc );
		if( accept_tk( sb, RBRACK )) {
			left_side = CreateTreeNode( SUBSCRIPT, ANN_NULL, NULL);
			InsertNode( left_side, name );
			NextSymbol( in, sb, tc );
			InsertNode( left_side, expression( in, sb, tc ));
			if( accept_tk( sb, LBRACK )) {
				NextSymbol( in, sb, tc );
			} else { ParserThrow( "right bracket expected", *tc ); }
		} else { left_side = name; }
		if( accept_tk( sb, ASSIGN )) {
			NodeAST *assignment = CreateTreeNode( BIN_OP, ANN_TOKEN, &( sb->token ));
			NextSymbol( in, sb, tc );
			InsertNode( root, assignment );
			InsertNode( assignment, left_side );
			InsertNode( assignment, expression( in, sb, tc ));
		} else { ParserThrow( "assignment operator expected", *tc ); }
	} else if( accept_tk( sb, CALL )) {
		NextSymbol( in, sb, tc );
		if( accept_tk( sb, IDENT )) {
			NodeAST *proc_evk = CreateTreeNode( PROC_EVK, ANN_IDENT, sb->tag.ident );
			InsertNode( root, proc_evk );
			NextSymbol( in, sb, tc );
			if( accept_tk( sb, LPARENT )) {
				do {
					NextSymbol( in, sb, tc );
					InsertNode( proc_evk, expression( in, sb, tc ));
				} while(accept_tk( sb, COMMA ));
				if( accept_tk( sb, RPARENT )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "right parentheses expected", *tc ); }
			}
		}
	} else if( accept_tk( sb, BEGIN )) {
		NodeAST *code_blk = CreateTreeNode( CODE_BLK, ANN_NULL, NULL);
		InsertNode( root, code_blk );
		do {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, code_blk );
		} while(accept_tk( sb, SEMICOLON ));
		if( accept_tk( sb, END )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "keyword END expected at the end of code block", *tc ); }
	} else if( accept_tk( sb, IF )) {
		NextSymbol( in, sb, tc );
		NodeAST *conditional = CreateTreeNode( CONDITIONAL, ANN_NULL, NULL);
		InsertNode( root, conditional );
		NodeAST *cond_exp = expression( in, sb, tc );
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
		NodeAST *while_loop = CreateTreeNode( WHILE_LOOP, ANN_NULL, NULL);
		InsertNode( root, while_loop );
		InsertNode( while_loop, expression( in, sb, tc ));
		if( accept_tk( sb, DO )) {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, while_loop );
		} else { ParserThrow( " keyword DO expected after WILLE", *tc ); }
	} else if( accept_tk( sb, FOR )) {
		NextSymbol( in, sb, tc );
		NodeAST *for_loop = CreateTreeNode( FOR_LOOP, ANN_NULL, NULL);
		InsertNode( root, for_loop );
		if( accept_tk( sb, IDENT )) {
			InsertNode( for_loop, CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident ));
			NextSymbol( in, sb, tc );
			if( accept_tk( sb, ASSIGN )) {
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
