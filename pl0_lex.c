#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void lexical_analysis(FILE *in, FILE *out);
char *generate_token(char *token, char *ident, token_name n); // create a token composed of token_name and lexeme
token_name is_other(char *s); // check non-keyword token, return corresponding token_name or 0
token_name is_keyword(char *s); // check keyword, not case-sensitive, return corresponding token_name or 0
void strupp(char *s);  // uppercase string

// <input_file_path> <output_file_path>
int main(int argc, char *argv[]) {
	FILE *f_in, *f_out;
	f_out = fopen(argv[2], "w");
	if (( f_in = fopen(argv[1], "r")) == NULL) {
		perror("fopen() failed");
		exit(EXIT_FAILURE);
	}

	lexical_analysis(f_in,f_out);

	fclose(f_in);
	fclose(f_out);
	return 0;
}

void lexical_analysis(FILE *in, FILE *out) {
	char lexeme[LEXEME_LENGTH], token[TOKEN_LENGTH],c;
	token_name t_name;
	while (( lexeme[0] = fgetc(in)) != EOF) {
		lexeme[1] = 0;
		token[0] = 0;
		if ( isalpha(lexeme[0])) {
			int i;
			for ( i = 0; isalpha(lexeme[i]); lexeme[++i] = fgetc(in)) {} // parse word
			lexeme[i] = 0;
			if ( is_keyword(lexeme)) fputs(generate_token(token, lexeme, is_keyword(lexeme)), out);
			else fputs(generate_token(token, lexeme, IDENT), out);
		}
	}
}

char *generate_token(char *token, char *ident, token_name n) {
	int keyword_offset = sizeof(other_token) / 8 - 1;
	if ( n <= keyword_offset ) {
		snprintf(token, TOKEN_LENGTH, "%s( %s )\n", other_token[n], ident);
		return token;
	} else {
		snprintf(token, TOKEN_LENGTH, "%s( %s )\n", keyword_token[n - keyword_offset], ident);
		return token;
	}
}

token_name is_other(char *s) {
	if ( strlen(s) == 1 )
		switch ( s[0] ) {
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
		}
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
		if ( !strcmp(ss, keyword_token[i])) return i + 21; // enum offset
	}
	return 0;
}

void strupp(char *s) {
	while (( *s = toupper(*s))) s++;
}