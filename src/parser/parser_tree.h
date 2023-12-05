#ifndef COMP_PARSER_TREE_H
#define COMP_PARSER_TREE_H

#include "syntax_tree.h"

typedef struct {
	char name[SB_LENGTH];
	Value associate;
} Symbol;

#endif //COMP_PARSER_TREE_H


