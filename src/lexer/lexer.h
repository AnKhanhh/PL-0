#ifndef COMP_LEXER_H
#define COMP_LEXER_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define LEXEME_LENGTH 32
#define TOKEN_LENGTH 128

const char *TOKENS[] = {"INVALID_TK", "IDENT", "NUMBER", "PLUS", "MINUS", "TIMES", "SLASH", "EQU",
						"NEQ", "LSS", "LEQ", "GTR", "GEQ", "LPARENT", "RPARENT", "LBRACK",
						"RBRACK", "PERIOD", "COMMA", "SEMICOLON", "ASSIGN", "PERCENT", "BEGIN", "CALL",
						"CONST", "DO", "ELSE", "END", "FOR", "IF", "PROCEDURE",
						"PROGRAM", "THEN", "TO", "VAR", "WHILE"
};

typedef enum {
	INVALID_TK = 0, IDENT, NUMBER,
	PLUS, MINUS, TIMES, SLASH, EQU, NEQ, LSS, LEQ, GTR, GEQ, LPARENT, RPARENT, LBRACK, RBRACK, PERIOD, COMMA,
	SEMICOLON, ASSIGN, PERCENT,
	BEGIN, CALL, CONST, DO, ELSE, END, FOR, IF, PROCEDURE, PROGRAM, THEN, TO, VAR, WHILE
} ETokenType;

char *GenerateToken( char *token, char *lexeme, ETokenType n ); // generate a token to write into output stream
 ETokenType IsKeyword( char *s ); // check keyword, not case-sensitive, return corresponding ETokenType if true
 ETokenType IsTwoOp( char *s ); // check two-character operator, return ETokenType if true
 ETokenType IsOneOp( char c ); // check one-character operator, return ETokenType if true
static void str_up( char *s );  // uppercase string
// copy a single char and append null
#define char_cpy( s, c ) {        \
    (s)[0] = (c);            \
    (s)[1] = '\0';            \
}

char *GenerateToken( char *token, char *lexeme, ETokenType n ) {
	snprintf( token, TOKEN_LENGTH, "%4d %-10s //%s\n", n, lexeme, TOKENS[n] );
	return token;
}

 ETokenType IsOneOp( char c ) {
	switch ( c ) {
		case '+':
			return PLUS;
		case '-':
			return MINUS;
		case '*' :
			return TIMES;
		case '/':
			return SLASH;
		case '%':
			return PERCENT;
		case '=':
			return EQU;
		case '(':
			return LPARENT;
		case ')':
			return RPARENT;
		case '[':
			return LBRACK;
		case ']':
			return RBRACK;
		case '.':
			return PERIOD;
		case ',':
			return COMMA;
		case ';':
			return SEMICOLON;
		case '<':
			return LSS;
		case '>':
			return GTR;
		default:
			return 0;
	}
}

 ETokenType IsTwoOp( char *s ) {
	if ( !strcmp( s, "<>" )) { return NEQ; }
	if ( !strcmp( s, "<=" )) { return LEQ; }
	if ( !strcmp( s, ">=" )) { return GEQ; }
	if ( !strcmp( s, ":=" )) { return ASSIGN; }
	return 0;
}

 ETokenType IsKeyword( char *s ) {
	char ss[LEXEME_LENGTH];
	strcpy( ss, s );
	str_up( ss );
	for ( unsigned int i = BEGIN; i < sizeof( TOKENS ) / sizeof( TOKENS[0] ); ++i ) {
		if ( !strcmp( ss, TOKENS[i] )) { return i; }
	}
	return 0;
}

static void str_up( char *s ) { while ((*s = (char) toupper( *s ))) { s++; }}

#endif //COMP_LEXER_H
