#include <stdio.h>
#include <stdlib.h>

#include "lexer.c"
#include "parser.c"

int main(int argc, char *argv[]) {
	// <input_file_path> <output_file_path>
	if ( argc != 3 ) {
		printf("wrong args count!");
		exit(EXIT_FAILURE);
	}
	FILE *f_in, *f_out;
	f_out = fopen(argv[2], "w");
	if (( f_in = fopen(argv[1], "r")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	get_token(f_in, f_out);

	fclose(f_in);
	fclose(f_out);
	return 0;
}
