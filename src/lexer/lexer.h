#ifndef COMP_LEXER_H
#define COMP_LEXER_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define LEXEME_LENGTH 32
#define TOKEN_LENGTH 128

const char *TOKENS[] = {"", "TK_IDENT", "TK_NUMBER", "TK_PLUS", "TK_MINUS", "TK_TIMES", "TK_SLASH", "TK_EQU",
						"TK_NEQ", "TK_LSS", "TK_LEQ", "TK_GTR", "TK_GEQ", "TK_LPARENT", "TK_RPARENT", "TK_LBRACK",
						"TK_RBRACK", "TK_PERIOD", "TK_COMMA", "TK_SEMICOLON", "TK_ASSIGN", "TK_PERCENT", "BEGIN", "CALL",
						"CONST", "DO", "ELSE", "END", "FOR", "IF", "PROCEDURE",
						"PROGRAM", "THEN", "TO", "VAR", "WHILE"
};

//	types of token
typedef enum ETokenType {
	TK_IDENT = 1, TK_NUMBER,
	TK_PLUS, TK_MINUS, TK_TIMES, TK_SLASH, TK_EQU, TK_NEQ, TK_LSS, TK_LEQ, TK_GTR, TK_GEQ,
	TK_LPARENT, TK_RPARENT, TK_LBRACK, TK_RBRACK, TK_PERIOD, TK_COMMA, TK_SEMICOLON, TK_ASSIGN, TK_PERCENT,
	BEGIN, CALL, CONST, DO, ELSE, END, FOR, IF, PROCEDURE, PROGRAM, THEN, TO, VAR, WHILE
} ETokenType;

char *GenerateToken(char *token, char *lexeme, ETokenType n); // generate a token to write into output stream
ETokenType IsKeyword(char *s); // check keyword, not case-sensitive, return corresponding ETokenType if true
ETokenType IsTwoOp(char *s); // check two-character operator, return ETokenType if true
ETokenType IsOneOp(char c); // check one-character operator, return ETokenType if true
static void str_up(char *s);  // uppercase string
// copy a single char and append null
#define char_cpy(s, c) {      \
    (s)[0] = (c);               \
    (s)[1] = '\0';              \
}

char *GenerateToken(char *token, char *lexeme, ETokenType n) {
	snprintf(token, TOKEN_LENGTH, "%4d %-10s //%s\n", n, lexeme, TOKENS[n]);
	return token;
}

ETokenType IsOneOp(char c) {
	switch (c) {
		case '+':
			return TK_PLUS;
		case '-':
			return TK_MINUS;
		case '*' :
			return TK_TIMES;
		case '/':
			return TK_SLASH;
		case '%':
			return TK_PERCENT;
		case '=':
			return TK_EQU;
		case '(':
			return TK_LPARENT;
		case ')':
			return TK_RPARENT;
		case '[':
			return TK_LBRACK;
		case ']':
			return TK_RBRACK;
		case '.':
			return TK_PERIOD;
		case ',':
			return TK_COMMA;
		case ';':
			return TK_SEMICOLON;
		case '<':
			return TK_LSS;
		case '>':
			return TK_GTR;
		default:
			return 0;
	}
}

ETokenType IsTwoOp(char *s) {
	if (!strcmp(s, "<>")) { return TK_NEQ; }
	if (!strcmp(s, "<=")) { return TK_LEQ; }
	if (!strcmp(s, ">=")) { return TK_GEQ; }
	if (!strcmp(s, ":=")) { return TK_ASSIGN; }
	return 0;
}

ETokenType IsKeyword(char *s) {
	char ss[LEXEME_LENGTH];
	strcpy(ss, s);
	str_up(ss);
	for (unsigned int i = BEGIN; i < sizeof(TOKENS) / sizeof(TOKENS[0]); ++i) {
		if (!strcmp(ss, TOKENS[i])) { return i; }
	}
	return 0;
}

static void str_up(char *s) { while ((*s = (char) toupper(*s))) { s++; }}

#endif //COMP_LEXER_H
