#include <ctype.h>
#include <string.h>
#include <stdio.h>


#ifndef COMP_LEXER_H
#define COMP_LEXER_H
#endif

#define LEXEME_LENGTH 64
#define TOKEN_LENGTH 128

// NUMBER should only be integer
char *other_token[] = {"NONE", "IDENT", "NUMBER", "PLUS", "MINUS", "TIMES", "SLASH", "EQU",
					   "NEQ", "LSS", "LEQ", "GTR", "GEQ", "LPARENT", "RPARENT", "LBRACK",
					   "RBRACK", "PERIOD", "COMMA", "SEMICOLON", "ASSIGN", "PERCENT"
};
char *keyword_token[] = {"NONE", "BEGIN", "CALL", "CONST", "DO", "ELSE", "END", "FOR", "IF", "ODD",
						 "PROCEDURE", "PROGRAM", "THEN", "TO", "VAR", "WHILE"
};

typedef enum {
	NONE = 0, IDENT, NUMBER,
	PLUS, MINUS, TIMES, SLASH, EQU, NEQ, LSS, LEQ, GTR, GEQ, LPARENT, RPARENT, LBRACK, RBRACK, PERIOD, COMMA,
	SEMICOLON, ASSIGN, PERCENT,
	BEGIN, CALL, CONST, DO, ELSE, END, FOR, IF, ODD, PROCEDURE, PROGRAM, THEN, TO, VAR, WHILE
} token_name;

char *generate_token(char *token, char *lexeme, token_name n); // create a token (string) of token_name and lexeme
token_name is_keyword(char *s); // check keyword, not case-sensitive, return corresponding token_name if true
token_name is_binary(char *s); // check binary token, return token_name if true
token_name is_unary(char c); // check unary token, return token_name if true
void strupp(char *s);  // uppercase string
void charcpy(char *s, char c); // copy a single char and append null

char *generate_token(char *token, char *lexeme, token_name n) {
	int keyword_offset = sizeof(other_token) / 8 - 1;
	if ( n <= keyword_offset ) {
		snprintf(token, TOKEN_LENGTH, "%s( %s )\n", other_token[n], lexeme);
		return token;
	} else {
		snprintf(token, TOKEN_LENGTH, "%s( %s )\n", keyword_token[n - keyword_offset], lexeme);
		return token;
	}
}

token_name is_unary(char c) {
	switch ( c ) {
		case '+': return PLUS;
		case '-': return MINUS;
		case '*' : return TIMES;
		case '/': return SLASH;
		case '%': return PERCENT;
		case '=': return EQU;
		case '(': return LPARENT;
		case ')': return RPARENT;
		case '[': return LBRACK;
		case ']': return RBRACK;
		case '.':return PERIOD;
		case ',': return COMMA;
		case ';': return SEMICOLON;
		case '<': return LSS;
		case '>': return GTR;
		default: return 0;
	}
}

token_name is_binary(char *s) {
	if ( !strcmp(s, "<>")) return NEQ;
	if ( !strcmp(s, "<=")) return LEQ;
	if ( !strcmp(s, ">=")) return GEQ;
	if ( !strcmp(s, ":=")) return ASSIGN;
	return 0;
}

token_name is_keyword(char *s) {
	char ss[LEXEME_LENGTH];
	strcpy(ss, s);
	strupp(ss);
	for ( int i = 1; i < sizeof(keyword_token) / 8; ++i ) {
		if ( !strcmp(ss, keyword_token[i])) return i + BEGIN - 1; // enum offset
	}
	return 0;
}

void charcpy(char *s, char c) {
	s[0] = c;
	s[1] = 0;
}

void strupp(char *s) {
	while (( *s = (char) toupper(*s))) s++;
}
