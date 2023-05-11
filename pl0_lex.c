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

void get_token(FILE *in, FILE *out);
char *generate_token(char *token, char *lexeme, token_name n); // create a token (string) of token_name and lexeme
token_name is_keyword(char *s); // check keyword, not case-sensitive, return corresponding token_name if true
token_name is_binary(char *s); // check binary token, return token_name if true
token_name is_unary(char c); // check unary token, return token_name if true
void strupp(char *s);  // uppercase string
void charcpy(char *s, char c); // copy a single char and append null

int main(int argc, char *argv[]) {
	// <input_file_path> <output_file_path>
	if ( argc != 3 ) {
		printf("wrong args count!");
		exit(EXIT_FAILURE);
	}
	FILE *f_in, *f_out;
	f_out = fopen(argv[2], "w");
	if (( f_in = fopen(argv[1], "r")) == NULL) {
		perror("fopen() failed");
		exit(EXIT_FAILURE);
	}

	get_token(f_in, f_out);

	fclose(f_in);
	fclose(f_out);
	return 0;
}

void get_token(FILE *in, FILE *out) {
	char lexeme[LEXEME_LENGTH], token[TOKEN_LENGTH], c = ' ';
	token_name t_name;
	while ( c != EOF) {
		token[0] = 0;
		while ( isspace(c)) c = (char) fgetc(in);
		if(c == EOF) break;
		if ( isalpha(c)) {
			charcpy(lexeme, c);
			int i = 0;
			for ( ; isalpha(lexeme[i]) || isdigit(lexeme[i]) || lexeme[i] == '_'; lexeme[++i] = (char) fgetc(in)) {}
			c = lexeme[i];
			lexeme[i] = 0;
			if (( t_name = is_keyword(lexeme))) {
				fputs(generate_token(token, lexeme, t_name), out);
			} else {
				fputs(generate_token(token, lexeme, IDENT), out);
			}
		} else if ( isdigit(c)) {
			charcpy(lexeme, c);
			int i = 0;
			for ( ; isdigit(lexeme[i]); lexeme[++i] = (char) fgetc(in)) {} // parse number
			c = lexeme[i];
			lexeme[i] = 0;
			fputs(generate_token(token, lexeme, NUMBER), out);
		} else {
			lexeme[0] = c;
			charcpy(lexeme + 1, (char) fgetc(in));
			if (( t_name = is_binary(lexeme))) {
				fputs(generate_token(token, lexeme, t_name), out);
				c = (char) fgetc(in);
			} else if (( t_name = is_unary(c))) {
				c = lexeme[1];
				lexeme[1] = 0;
				fputs(generate_token(token, lexeme, t_name), out);
			} else {
				c = lexeme[1];
				lexeme[1] = 0;
				fputs(generate_token(token, lexeme, NONE), out);
			}
		}
	}
}

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
		if ( !strcmp(ss, keyword_token[i])) return i + 21; // enum offset
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