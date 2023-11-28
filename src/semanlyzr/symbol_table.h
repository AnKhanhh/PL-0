//	TODO: make entries in SymbolTable dynamically allocated
//	TODO: add argument number and type checks when procedure is called

#ifndef COMP_SCOPE_TABLE_H
#define COMP_SCOPE_TABLE_H
#endif //COMP_SCOPE_TABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include "../lexer/lexer.c"

#define MAX_ENTRIES 16

typedef enum {
	FNC = 1, IMM, MUTE, ARR
} Type;    // program is unique and is handled separately
char *TypeName[] = {"", "procedure", "constant", "variable", "array"};

typedef struct {
	char id[LEXEME_LENGTH];
	Type var_type;
	union {
		struct SymbolTable *sub_table; //	FNC
		long long number; //	IMM
		int arr_size; //	ARR
		bool init; //	MUTE
	};
} TableEntry;

typedef struct SymbolTable {
	char id[LEXEME_LENGTH];
	TableEntry entries[MAX_ENTRIES];
	struct SymbolTable *parent_table;    // parent = NULL means main program table
} SymbolTable;

//	malloc a new symbol table
SymbolTable *new_st(SymbolTable *parent, char *id);
//	recursively free all symbol tables start from root node
void free_st(SymbolTable *root);
//	search within local scope or globally, return Type of the first occurrence of ident or 0, can extract pointer to table and entry
Type search(SymbolTable *root, char *id, bool scoped, SymbolTable **pp_ST, TableEntry **pp_TE);
//	push entry into existing symbol table, return 0 if all entries is occupied 	( void pointer here is bad practice )
int push_ST(SymbolTable *root, char *id, Type var_type, void *p_value);
//	semantic check on declaration, void pointer is context-dependent, returns 0 on error
int semantic_dcl(SymbolTable *root, char *id, Type var_type, void *p_value);
//	semantic check on assigning value, returns 0 on error
int semantic_evk(SymbolTable *root, char *id, Type var_type);
//	semantic check on evaluating to an integer, returns 0 on error
int semantic_evl(SymbolTable *root, char *id, Type var_type);
void print_SymTab(SymbolTable *root, FILE *out);

SymbolTable *new_st(SymbolTable *parent, char *id) {
	SymbolTable *p_st = malloc(sizeof(SymbolTable));
	memset(p_st, 0, sizeof(SymbolTable));
	strncpy(p_st->id, id, LEXEME_LENGTH);
	p_st->parent_table = parent;
	return p_st;
}

void free_st(SymbolTable *root) {
	for ( int i = 0; i < MAX_ENTRIES; ++i ) {
		if ( root->entries[i].var_type == FNC ) { free_st(root->entries[i].sub_table); }
	}
	free(root);
}

Type search(SymbolTable *root, char *id, bool scoped, SymbolTable **pp_ST, TableEntry **pp_TE) {
	do {
		for ( int i = 0; root->entries[i].var_type != 0 && i < MAX_ENTRIES; ++i ) {
			if ( strncmp(id, root->entries[i].id, LEXEME_LENGTH) == 0 ) {
				if ( pp_ST != NULL) { *pp_ST = root; }
				if ( pp_TE != NULL) { *pp_TE = &( root->entries[i] ); }
				return root->entries[i].var_type;
			}
		}
	} while ( !scoped && ( root = root->parent_table ) != NULL);
	return 0;
}

int push_ST(SymbolTable *root, char *id, Type var_type, void *p_value) {
	TableEntry *p_table_entry = NULL;
	for ( int i = 0; i < MAX_ENTRIES; ++i ) {
		if ( root->entries[i].var_type == 0 ) {
			p_table_entry = &( root->entries[i] );
			break;
		}
	}
	if ( !p_table_entry ) { return 0; }

	switch ( var_type ) {
		case FNC: *(SymbolTable **) p_value = new_st(root, id);
			( *p_table_entry ).sub_table = *(SymbolTable **) p_value;
			break;
		case IMM: ( *p_table_entry ).number = *(long long *) p_value;
			break;
		case ARR: ( *p_table_entry ).arr_size = *(int *) p_value;
			break;
		case MUTE: break;   // initialized = false already due to memset
		default: puts("error in symbol table entry: unknown Type");
			break;
	}
	strncpy(( *p_table_entry ).id, id, LEXEME_LENGTH);
	( *p_table_entry ).var_type = var_type;
	return 1;
}

int semantic_dcl(SymbolTable *root, char *id, Type var_type, void *p_value) {
	Type result = search(root, id, true, NULL, NULL);
	if ( result ) {
		printf("redefinition into %s %s in block %s from type %s", TypeName[var_type], id, root->id, TypeName[result]);
		return 0;
	} else if ( var_type == ARR && *(int *) p_value < 0 ) {
		printf("length of %s %s in block %s must be a positive value", TypeName[var_type], id, root->id);
		return 0;
	} else { return push_ST(root, id, var_type, p_value); }
}

int semantic_evk(SymbolTable *root, char *id, Type var_type) {
	SymbolTable *p_ST = NULL;
	TableEntry *p_TE = NULL;
	Type result = search(root, id, false, &p_ST, &p_TE);

	if ( !result ) { printf("found no declaration of %s %s in block %s", TypeName[var_type], id, root->id); }
	else if ( result == IMM ) { printf("%s is immutable", id); }
	else if ( result != var_type ) {
		printf("type mismatch: %s %s in block %s cannot be resolved into %s as declared in block %s",
			   TypeName[var_type], id, root->id, TypeName[result], p_ST->id);
	} else {
		if ( var_type == MUTE ) { p_TE->init = true; }
		return 1;
	}
	return 0;
}

int semantic_evl(SymbolTable *root, char *id, Type var_type) {
	SymbolTable *p_ST = NULL;
	TableEntry *p_TE = NULL;
	Type result = search(root, id, false, &p_ST, &p_TE);
	if ( !result ) { printf("found no declaration of %s %s in block %s", TypeName[var_type], id, root->id); }
	switch ( result ) {
		case MUTE: if ( p_TE->init == false ) { printf("warning: %s %s in block %s is not initialized", TypeName[var_type], id, root->id); }
			return 0;
		case ARR:
		case IMM:
		case FNC:
			if ( result != IMM && result != var_type ) {
				printf("type mismatch: %s %s in block %s cannot be resolved into %s as declared in block %s",
					   TypeName[var_type], id, root->id, TypeName[result], p_ST->id);
				return 0;
			}
			return 1;
		default: return 0;
	}
}

void print_SymTab(SymbolTable *root, FILE *out) {
	int buff_size = LEXEME_LENGTH * MAX_ENTRIES;    //	approximation of necessary buffer length
	char horizontal[] = "____________________";    //	20 underscores
	char buffer[buff_size];
	snprintf(buffer, buff_size, "%s\n%s\n PARENT_ADDR: %llX \n SELF_ADDR: %llX \n ID: %s \n%s\n",
			 horizontal, horizontal, (long long) root->parent_table, (long long) root, root->id, horizontal);
	fputs(buffer, out);
	for ( int i = 0; i < MAX_ENTRIES && root->entries[i].var_type != 0; ++i ) {
		switch ( root->entries[i].var_type ) {
			case FNC:
				snprintf(buffer, buff_size, "%-16s %-32s || \t addr: %llX \n",
						 TypeName[root->entries[i].var_type], root->entries[i].id, (long long) root->entries[i].sub_table);
				break;
			case IMM:
				snprintf(buffer, buff_size, "%-16s %-32s || \t value: %lld \n",
						 TypeName[root->entries[i].var_type], root->entries[i].id, root->entries[i].number);
				break;
			case MUTE:
				snprintf(buffer, buff_size, "%-16s %-32s || \t %s \n",
						 TypeName[root->entries[i].var_type], root->entries[i].id, root->entries[i].init ? "initialized" : "uninitialized");
				break;
			case ARR:
				snprintf(buffer, buff_size, "%-16s %-32s || \t length: %d \n",
						 TypeName[root->entries[i].var_type], root->entries[i].id, root->entries[i].arr_size);
				break;
		}
		fputs(buffer, out);
	}
	snprintf(buffer, buff_size, "%s\n%s\n\n", horizontal, horizontal);
	fputs(buffer, out);

	for ( int i = 0; i < MAX_ENTRIES && root->entries[i].var_type != 0; ++i ) {
		if ( root->entries[i].var_type == FNC ) { print_SymTab(root->entries[i].sub_table, out); }
	}
}
