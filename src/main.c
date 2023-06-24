#include <stdio.h>
#include <stdlib.h>
#include "parser/parser.c"

#define INPUT_FILE_PATH "C:/Users/admin/CLionProjects/comp/input/input1.txt"
#define LEXER_FILE_PATH "C:/Users/admin/CLionProjects/comp/output/l_output1.txt"

int main(int argc, char *argv[]) {
	FILE *f_in, *f_lexer;
	if (( f_in = fopen(INPUT_FILE_PATH, "r")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	if ( (f_lexer = fopen(LEXER_FILE_PATH, "r+")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	lexical_analysis(f_in, f_lexer);
	syntax_analysis(f_lexer);

	fclose(f_in);
	fclose(f_lexer);
	return 0;
}
