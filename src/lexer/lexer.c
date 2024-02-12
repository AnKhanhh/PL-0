#include "lexer.h"

void LexicalAnalysis( FILE *in, FILE *out ) {
	char lexeme[LEXEME_LENGTH], token[TOKEN_LENGTH], c = ' ';
	ETokenType t_name;
	while ( c != EOF) {
		token[0] = 0;
//			skip all whitespace
		if ( isspace( c )) {
			while ( isspace( c )) { c = (char) fgetc( in ); }
//			indent check:
		} else if ( isalpha( c )) {
			char_cpy( lexeme, c );
			int i = 0;
			while ( isalpha( lexeme[i] ) || isdigit( lexeme[i] ) || lexeme[i] == '_' ) {
				char x = (char) fgetc( in );
				if ( i < (int) sizeof lexeme ) { lexeme[++i] = x; }
			}
//			pop trailing character out of lexeme
			c = lexeme[i];
			lexeme[i] = 0;
			if ((t_name = IsKeyword( lexeme ))) {
				fputs( GenerateToken( token, lexeme, t_name ), out );
			} else {
				fputs(GenerateToken(token, lexeme, TK_IDENT ), out );
			}
//			integer check
		} else if ( isdigit( c )) {
			char_cpy( lexeme, c );
			unsigned int i = 0;
			while ( isdigit( lexeme[i] ) && i < sizeof lexeme ) {
				lexeme[++i] = (char) fgetc( in );
			}
			c = lexeme[i];
			lexeme[i] = 0;
			fputs(GenerateToken(token, lexeme, TK_NUMBER ), out );
//			operator check
		} else {
			lexeme[0] = c;
			char_cpy( lexeme + 1, (char) fgetc( in ));
			if ((t_name = IsTwoOp( lexeme ))) {
				fputs( GenerateToken( token, lexeme, t_name ), out );
				c = (char) fgetc( in );
			} else if ((t_name = IsOneOp( c ))) {
				c = lexeme[1];
				lexeme[1] = 0;
				fputs( GenerateToken( token, lexeme, t_name ), out );
//			all checks failed, invalid token
			} else {
				c = lexeme[1];
				lexeme[1] = 0;
				fputs( GenerateToken( token, lexeme, 0 ), out );
			}
		}
	}
}
