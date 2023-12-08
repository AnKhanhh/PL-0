#include "lexer.h"

//									_____ skip space
// flow: check EOF ___ is not? -> /______ check word _____ check keyword
//							  	  \				 	 \____ is ident
//								   \_____ check number ___ is not -> illegal (INVALID_TK)
//									\____ check binary symbol
//									 \___ check unary symbol ___ is not? -> illegal (INVALID_TK)

//	TODO: token stream represented by corresponding enum to improve compilation time

void lexical_analysis( FILE *in, FILE *out ) {
	char lexeme[LEXEME_LENGTH], token[TOKEN_LENGTH], c = ' ';
	ETokenType t_name;
	while ( c != EOF) {
		token[0] = 0;
		if ( isspace( c )) {
			while ( isspace( c )) { c = (char) fgetc( in ); }
		} else if ( isalpha( c )) {
			char_cpy( lexeme, c );
			int i = 0;
			while ((isalpha( lexeme[i] ) || isdigit( lexeme[i] ) || lexeme[i] == '_') && i < sizeof lexeme ) {
				lexeme[++i] = (char) fgetc( in );
			}
			c = lexeme[i];
			lexeme[i] = 0;

			if ((t_name = is_keyword( lexeme ))) {
				fputs( generate_token( token, lexeme, t_name ), out );
			} else {
				fputs( generate_token( token, lexeme, IDENT ), out );
			}

		} else if ( isdigit( c )) {
			char_cpy( lexeme, c );
			int i = 0;
			while ( isdigit( lexeme[i] ) && i < sizeof lexeme ) {
				lexeme[++i] = (char) fgetc( in );
			}
			c = lexeme[i];
			lexeme[i] = 0;
			fputs( generate_token( token, lexeme, NUMBER ), out );

		} else {
			lexeme[0] = c;
			char_cpy( lexeme + 1, (char) fgetc( in ));
			if ((t_name = is_binary( lexeme ))) {
				fputs( generate_token( token, lexeme, t_name ), out );
				c = (char) fgetc( in );
			} else if ((t_name = is_unary( c ))) {
				c = lexeme[1];
				lexeme[1] = 0;
				fputs( generate_token( token, lexeme, t_name ), out );
			} else {
				c = lexeme[1];
				lexeme[1] = 0;
				fputs( generate_token( token, lexeme, INVALID_TK ), out );
			}
		}
	}
}
