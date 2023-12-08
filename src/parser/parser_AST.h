#ifndef COMP_PARSER_AST_H
#define COMP_PARSER_AST_H

//	an int counter is used to track token that produces syntax error,
//	so error message can be wrong for files with token number > INT_MAX

#include "syntax_tree.h"

typedef struct {
	ETokenType token;
	union {
		char ident[LEXEME_LENGTH];
		long number;
	} tag;
} Symbol;    // terminal or non-terminal

// compare wrapper
#define accept_tk( sb, tk_cmp ) ((sb)->token == (tk_cmp))

//	reads next token from stream into sb, return 0 on EOF or failure
int NextSymbol( FILE *in, Symbol *sb, int *tc );
// throw syntax error with token location
void ParserThrow( const char *s, int count ) {
	fprintf(stderr, "SYNTAX ERR: %s at token %d\n", s, count );
}

// recursive descent LL(1) parser functions
void program( FILE *in, Symbol *sb, int *tc, NodeAST **root_ptr );
void block( FILE *in, Symbol *sb, int *tc, NodeAST *root );
void statement( FILE *in, Symbol *sb, int *tc, NodeAST *root );
void term( FILE *in, Symbol *sb, int *tc );
void factor( FILE *in, Symbol *sb, int *tc );
void expression( FILE *in, Symbol *sb, int *tc );
void condition( FILE *in, Symbol *sb, int *tc );

void program( FILE *in, Symbol *sb, int *tc, NodeAST **root_ptr ) {
	if ( accept_tk( sb, PROGRAM )) {
		NextSymbol( in, sb, tc );
		if ( accept_tk( sb, IDENT )) {
			*root_ptr = CreateTreeNode( PRG_DCL, ANN_IDENT, sb->tag.ident );
			NextSymbol( in, sb, tc );
			if ( accept_tk( sb, SEMICOLON )) {
				NextSymbol( in, sb, tc );
				block( in, sb, tc, *root_ptr );
				if ( accept_tk( sb, PERIOD )) {
				} else { ParserThrow( "PROGRAM terminating symbol", *tc ); }
			} else { ParserThrow( "PROGRAM semicolon expected", *tc ); }
		} else { ParserThrow( "program name expected", *tc ); }
	} else { ParserThrow( "keyword PROGRAM expected", *tc ); }
}

void block( FILE *in, Symbol *sb, int *tc, NodeAST *root ) {
	if ( accept_tk( sb, CONST )) {
		NodeAST *const_block = CreateTreeNode( CST_BLK, ANN_NULL, NULL);
		InsertNode( root, const_block );
		do {
			NextSymbol( in, sb, tc );
			if ( accept_tk( sb, IDENT )) {
				NodeAST *const_name = CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident );
				NextSymbol( in, sb, tc );
				if ( accept_tk( sb, EQU )) {
					ETokenType tk_type = EQU;
					NodeAST *equals = CreateTreeNode( BINARY, ANN_TOKEN, &tk_type );
					InsertNode( const_block, equals );
					NextSymbol( in, sb, tc );
					if ( accept_tk( sb, NUMBER )) {
						InsertNode( equals, const_name );
						InsertNode( equals, CreateTreeNode( LITERAL, ANN_NUM, &(sb->tag.number)));
						NextSymbol( in, sb, tc );
					} else { ParserThrow( "CONST value expected", *tc ); }
				} else { ParserThrow( "equal operator expected", *tc ); }
			} else { ParserThrow( "CONST name expected", *tc ); }
		} while (accept_tk( sb, COMMA ));
		if ( accept_tk( sb, SEMICOLON )) {} else { ParserThrow( "missing CONST block semicolon", *tc ); }
	}
	if ( accept_tk( sb, VAR )) {
		NodeAST *var_blk = CreateTreeNode( VAR_BLK, ANN_NULL, NULL);
		InsertNode( root, var_blk );
		do {
			NextSymbol( in, sb, tc );
			if ( accept_tk( sb, IDENT )) {
				Symbol temp_sb = *sb;
				NextSymbol( in, sb, tc );
				if ( accept_tk( sb, LBRACK )) {
					NodeAST *arr_dcl = CreateTreeNode( ARR_DCL, ANN_IDENT, temp_sb.tag.ident );
					InsertNode( var_blk, arr_dcl );
					NextSymbol( in, sb, tc );
					if ( accept_tk( sb, NUMBER )) {
						InsertNode( arr_dcl, CreateTreeNode( ARR_DCL, LITERAL, &(sb->tag.number)));
						NextSymbol( in, sb, tc );
						if ( accept_tk( sb, RBRACK )) {
							NextSymbol( in, sb, tc );
						} else { ParserThrow( "right bracket expected", *tc ); }
					} else { ParserThrow( "array index expected", *tc ); }
				} else {
					InsertNode( var_blk, CreateTreeNode( NAME, ANN_IDENT, temp_sb.tag.ident ));
				}
			} else { ParserThrow( "invalid name", *tc ); }
		} while (accept_tk( sb, COMMA ));
		if ( accept_tk( sb, SEMICOLON )) {} else { printf( "semicolon missing at %d \n", *tc ); }
	}
	if ( accept_tk( sb, PROCEDURE )) {
		NextSymbol( in, sb, tc );
		if ( accept_tk( sb, IDENT )) {
			NodeAST *proc_blk = CreateTreeNode( PROC_DCL, ANN_IDENT, sb->tag.ident );
			InsertNode( root, proc_blk );
			NextSymbol( in, sb, tc );
			if ( accept_tk( sb, LPARENT )) {
				do {
					NextSymbol( in, sb, tc );
					if ( accept_tk( sb, VAR )) { NextSymbol( in, sb, tc ); }
					if ( accept_tk( sb, IDENT )) {
						InsertNode( proc_blk, CreateTreeNode( NAME, ANN_IDENT, sb->tag.ident ));
						NextSymbol( in, sb, tc );
					} else { ParserThrow( "parameter name expected", *tc ); }
				} while (accept_tk( sb, SEMICOLON ));
				if ( accept_tk( sb, RPARENT )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "right parentheses expected", *tc ); }
			}
			if ( accept_tk( sb, SEMICOLON )) {
				NextSymbol( in, sb, tc );
				block( in, sb, tc, proc_blk );
				if ( accept_tk( sb, SEMICOLON )) {
					NextSymbol( in, sb, tc );
				} else { ParserThrow( "procedure termination semicolon expected", *tc ); }
			} else { ParserThrow( "procedure declaration semicolon expected", *tc ); }
		} else { ParserThrow( "procedure name expected", *tc ); }
	}
	if ( accept_tk( sb, BEGIN )) {
		NodeAST *main_blk = CreateTreeNode( MAIN, ANN_NULL, NULL);
		InsertNode( root, main_blk );
		do {
			NextSymbol( in, sb, tc );
			statement( in, sb, tc, main_blk );
		} while (accept_tk( sb, SEMICOLON ));
		if ( accept_tk( sb, END )) {
			NextSymbol( in, sb, tc );
		} else { ParserThrow( "keyword END expected", *tc ); }
	} else { ParserThrow( "keyword BEGIN expected", *tc ); }
}

int NextSymbol( FILE *in, Symbol *sb, int *tc ) {
	char line[1024];
	if ( fgets( line, sizeof line, in ) == NULL) {
		puts( "end of token list reached!" );
		return 0;
	}
	memset( sb, 0, sizeof( Symbol ));
	sb->token = strtol( strtok( line, " " ), NULL, 10 );
//	token counter to pinpoint syntax error
	(*tc)++;
	if ( sb->token == IDENT ) {
		strncpy( sb->tag.ident, strtok(NULL, " " ), LEXEME_LENGTH );
	} else if ( sb->token == NUMBER ) {
		sb->tag.number = strtol( strtok(NULL, " " ), NULL, 10 );
	}
	return 1;
}


#endif //COMP_PARSER_AST_H


