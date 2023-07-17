#ifndef COMP_VAR_STACK_H
#define COMP_VAR_STACK_H
#endif //COMP_VAR_STACK_H

///	TODO: add check for calling uninitialized var

#include <stdbool.h>
#include "../lexer/lexer.c"

//	all types share the same namespace, variable shadowing allowed
typedef enum {
	PRG, FNC, IMM, MUTE, ARR
} type;    //	program, function, immutable, mutable, array
char *vt_name[] = {"program", "procedure", "constant", "variable", "array"};

typedef struct {
	char name[LEXEME_LENGTH];
	type var_type;
} VarStack[128];

//	offset being the topmost empty position of stack
int push_vs(VarStack *pvs, int *p_off, type t, char *s);
//	scan for first occurrence of ident in range, return its offset or -1
int search_vs(VarStack *pvs, int start, int end, char *s);
//	check over namespace for ident occurrence of different var_type, returns offset position or -1
int check_namespace(VarStack (*pvs), int end, char *s, type type);
//	check within a scope for ident occurrence of same var_type, returns offset position or -1
int check_scope(VarStack (*pvs), int start, int end, char *s, type type);
void semantic_dcl(int c, VarStack (*pvs), int offset, int *p_cur_offset, char *ident, type type);
void semantic_evk(int c, VarStack (*pvs), int offset, int cur_offset, char *ident, type type);
void semantic_evl(int c, VarStack (*pvs), int cur_offset, char *ident, bool is_arr);

int push_vs(VarStack *pvs, int *p_off, type t, char *s) {
	if ( *p_off >= 128 ) { return 0; }
	else {
		strncpy(( *pvs )[*p_off].name, s, LEXEME_LENGTH);
		( *pvs )[*p_off].var_type = t;
		( *p_off )++;
		return 1;
	}
}

int search_vs(VarStack *pvs, int start, int end, char *s) {
	int i;
	for ( i = end - 1; i >= start; --i ) {
		if ( !strcmp(( *pvs )[i].name, s)) { return i; }
	}
	return -1;
}

int check_namespace(VarStack (*pvs), int end, char *s, type type) {
	for ( int i = search_vs(pvs, 0, end, s); i != -1; i = search_vs(pvs, 0, end, s)) {
		if (( *pvs )[i].var_type != type ) { return i; }
		else { end = i; }
	}
	return -1;
}

int check_scope(VarStack (*pvs), int start, int end, char *s, type type) {
	for ( int i = search_vs(pvs, start, end, s); i != -1; i = search_vs(pvs, start, end, s)) {
		if (( *pvs )[i].var_type == type ) { return i; }
		else { end = i; }
	}
	return -1;
}

void semantic_dcl(int c, VarStack (*pvs), int offset, int *p_cur_offset, char *ident, type type) {
	if ( check_namespace(pvs, *p_cur_offset, ident, type) != -1 ) {
		printf("var_type of %s redeclared as %s at %d \n", ident, vt_name[type], c);
	} else if ( check_scope(pvs, offset, *p_cur_offset, ident, type) != -1 ) {
		printf("redeclaration of %s %s at %d \n", vt_name[type], ident, c);
	} else { push_vs(pvs, p_cur_offset, type, ident); }
}

void semantic_evk(int c, VarStack (*pvs), int offset, int cur_offset, char *ident, type type) {
	if ( check_namespace(pvs, cur_offset, ident, type) != -1 ) {
		printf("var_type mismatch: cannot resolve %s %s at %d \n", vt_name[type], ident, c);
	} else if ( check_scope(pvs, 0, cur_offset, ident, type) == -1 ) {
		printf("no declaration of %s %s at %d \n", vt_name[type], ident, c);
	}
}

void semantic_evl(int c, VarStack (*pvs), int cur_offset, char *ident, bool is_arr) {
	if ( is_arr && check_scope(pvs, 0, cur_offset, ident, ARR) == -1 ) {
		printf("unable to access element: %s at %d \n", ident, c);
	} else if ( check_scope(pvs, 0, cur_offset, ident, IMM) == -1 &&
				check_scope(pvs, 0, cur_offset, ident, MUTE) == -1 ) {
		printf("unable to evaluate to value: %s at %d \n", ident, c);
	}
}
