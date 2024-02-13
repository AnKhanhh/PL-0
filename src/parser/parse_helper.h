#ifndef COMP_PARSE_HELPER_H
#define COMP_PARSE_HELPER_H

typedef struct {
	ETokenType token;
	union {
		char ident[LEXEME_LENGTH];
		long number;
	} tag;
} Symbol;    // terminal or non-terminal

// compare wrapper
#define accept_tk( sb, tk_cmp ) ((sb)->token == (tk_cmp))

// throw syntax error with token location
 void ParserThrow( const char *s, int count ) {
	fprintf(stderr, "syntax error: %s at token %d\n", s, count );
}

//	reads next token from stream into sb, return 0 on EOF or failure
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
	if (sb->token == TK_IDENT ) {
		strncpy( sb->tag.ident, strtok(NULL, " " ), LEXEME_LENGTH );
	} else if (sb->token == TK_NUMBER ) {
		sb->tag.number = strtol( strtok(NULL, " " ), NULL, 10 );
	}
	return 1;
}

#endif //COMP_PARSE_HELPER_H
