#include <stdio.h>
#include "lexer/lexer.c"
#include "parser/parser.c"
#include "semanlyzr/semanlyzer.c"

// 	pass program argument to run
//	first argument is absolute path to the project folder, e.g. 'C:/path/compilers'
//	second argument is input file name, file should be placed in /input/, e.g. input.txt
int main(int argc, char *argv[]) {
	char in_path[128], l_out_path[128], p_out_path[128];
//	using unsanitized string is unsafe, but i'm lazy
	snprintf(in_path, sizeof(in_path), "%s/input/%s", argv[1], argv[2]);
	snprintf(l_out_path, sizeof(l_out_path), "%s/output/token_stream_%s", argv[1], argv[2]);
	snprintf(p_out_path, sizeof(p_out_path), "%s/output/AST_%s", argv[1], argv[2]);

	FILE *f_in, *f_lexer, *f_parser;
	if ((f_in = fopen(in_path, "r")) == NULL) { perror("input file"); }
	else if ((f_lexer = fopen(l_out_path, "w+")) == NULL) { perror("lexer file"); }
	else if ((f_parser = fopen(p_out_path, "w")) == NULL) { perror("parser file"); }
	else {
		LexicalAnalysis(f_in, f_lexer);
		SyntaxTreeNode *root = NULL;
		SyntaxAnalysis(f_lexer, f_parser, &root);

		FreeSyntaxTree(root);
//		FreeSymbolTables()
		fclose(f_in);
		fclose(f_lexer);
		fclose(f_parser);
		return 0;
	}
	return 1;
}
