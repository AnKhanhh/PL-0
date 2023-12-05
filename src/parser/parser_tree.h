#ifndef COMP_PARSER_TREE_H
#define COMP_PARSER_TREE_H

#include "syntax_tree.h"

typedef struct {
	char name[SB_LENGTH];
	Value associate;
} Symbol;    // terminal or non-terminal

//	on recognizing a terminal symbol, returns 1 and call NextSymbol()
int AcceptTerminal( FILE *in, Symbol *sb, int *tc, const char *s);
//	reads a Symbol from the token list file, returns 0 on failure
int NextSymbol( FILE *in, Symbol *sb, int *tc);
// throw current token number to stderr
void ThrowToken(int count){ fprintf(stderr," - at token %d", count);}

void term(FILE *in, Symbol *sb, int *tc);
void factor(FILE *in, Symbol *sb, int *tc);
void expression(FILE *in, Symbol *sb, int *tc);
void condition(FILE *in, Symbol *sb, int *tc);
void statement(FILE *in, Symbol *sb, int *tc);
void block(FILE *in, Symbol *sb, int *tc);
void program(FILE *in, Symbol *sb, int *tc);

int AcceptTerminal( FILE *in, Symbol *sb, int *tc, const char *s) {
	if ( !strcmp(sb->name, s)) {
		NextSymbol( in, sb, tc );
		return 1;
	} else { return 0; }
}


int NextSymbol( FILE *in, Symbol *sb, int *tc ) {
	char line[1024];
	if ( fgets( line, sizeof line, in ) == NULL) {
		puts( "end of token list reached!" );
		return 0;
	}
	memset( sb, 0, sizeof( Symbol ));
	strncpy( sb->name, strtok( line, " " ), SB_LENGTH );
//	token counter to pinpoint syntax error
	(*tc)++;
	if ( !strcmp( sb->name, TOKENS[IDENT] )) {
		strncpy( sb->associate.ident, strtok(NULL, " \n" ), LEXEME_LENGTH );
	} else if ( !strcmp( sb->name, TOKENS[NUMBER] )) {
		sb->associate.number = strtol( strtok(NULL, " \n" ), NULL, 10 );
	}
	return 1;
}


#endif //COMP_PARSER_TREE_H


