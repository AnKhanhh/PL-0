#include <ctype.h>
#include <string.h>
#include <stdio.h>


#ifndef COMP_LEXER_H
#define COMP_LEXER_H
#endif

#define LEXEME_LENGTH 32
#define TOKEN_LENGTH 128

char *tokens[] = {"NONE", "IDENT", "NUMBER", "PLUS", "MINUS", "TIMES", "SLASH", "EQU",
				  "NEQ", "LSS", "LEQ", "GTR", "GEQ", "LPARENT", "RPARENT", "LBRACK",
				  "RBRACK", "PERIOD", "COMMA", "SEMICOLON", "ASSIGN", "PERCENT", "BEGIN", "CALL",
				  "CONST", "DO", "ELSE", "END", "FOR", "IF", "ODD", "PROCEDURE",
				  "PROGRAM", "THEN", "TO", "VAR", "WHILE"
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
void str_up(char *s);  // uppercase string
void char_cpy(char *s, char c); // copy a single char and append null

char *generate_token(char *token, char *lexeme, token_name n) {
		snprintf(token, TOKEN_LENGTH, "%-16s %s\n", tokens[n], lexeme);
		return token;
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
	str_up(ss);
	for ( int i = BEGIN; i < sizeof(tokens) / sizeof(tokens[0]); ++i ) {
		if ( !strcmp(ss, tokens[i])) return i;
	}
	return 0;
}

void char_cpy(char *s, char c) {
	s[0] = c;
	s[1] = 0;
}

void str_up(char *s) {
	while (( *s = (char) toupper(*s))) s++;
}
