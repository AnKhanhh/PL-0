#ifndef COMP_LIST_SYMBOL_TABLE_H
#define COMP_LIST_SYMBOL_TABLE_H

#include <assert.h>
#include "../lexer/lexer.h"

const char *SB_IDENT_TYPE[] = {
		"", "SB_INT", "SB_CONST_INT", "SB_FUNCTION", "SB_ARRAY"
};

typedef struct SymbolEntry {
	char ident[LEXEME_LENGTH];
//	types of type table entry
	enum EIdentType {
		SB_INT = 1, SB_CONST_INT, SB_FUNCTION, SB_ARRAY
	} type;
	union {
		bool var_initialized;
		long const_value;
		long arr_size;
		struct {
			int param_count;                // n first entries in function table is parameters
			struct SymbolTable *scope_ptr;
		} func;
	} data;
} SymbolEntry;

typedef struct SymbolTable {
	char name[LEXEME_LENGTH];
	struct SymbolTable *parent;
	unsigned int entry_count;
	struct SymbolEntry *entries;
} SymbolTable;

//  allocate a new table, return pointer
SymbolTable *NewSymbolTable( SymbolTable *parent_table, char *table_name );

//  insert an entry into symbol table
SymbolTable *InsertEntry( SymbolTable *table, SymbolEntry entry, char *ident );

//  DFS free()
void FreeSymbolTables( SymbolTable *root );

//	DFS print
void PrintSymbolTable( SymbolTable *table, FILE *out );

SymbolTable *NewSymbolTable( SymbolTable *parent_table, char *table_name ) {
	SymbolTable *new_table = calloc( 1, sizeof( SymbolTable ));
	if( new_table == NULL) {
		perror( "allocation for symbol table failed" );
		exit( EXIT_FAILURE );
	}

	if( parent_table ) { new_table->parent = parent_table; }
	snprintf( new_table->name, LEXEME_LENGTH, "%s", table_name );
	return new_table;
}

SymbolTable *InsertEntry( SymbolTable *table, SymbolEntry entry, char *ident ) {
	assert( table != NULL );
//	temporary pointer to prevent leak in case of failure
	SymbolEntry *temp = realloc( table->entries, sizeof( *table->entries ) * (table->entry_count + 1));
	if( temp == NULL) {
		perror( "allocation for symbol table entry failed" );
		exit( EXIT_FAILURE );
	}

	table->entries = temp;
	table->entries[table->entry_count] = entry;
	snprintf( table->entries[table->entry_count].ident, LEXEME_LENGTH, "%s", ident );
	table->entry_count++;
	return table;
}

void FreeSymbolTables( SymbolTable *root ) {
	if( root->entries != NULL) {
		assert( root->entry_count != 0 );
		for( unsigned int i = 0; i < root->entry_count; ++i ) {
			if( root->entries[i].type == SB_FUNCTION ) {
				FreeSymbolTables( root->entries[i].data.func.scope_ptr );
			}
		}
		free( root->entries );
	}
	free( root );
}

void PrintSymbolTable( SymbolTable *table, FILE *out ) {
//	50 underscores
	static const char line[] = "__________________________________________________";
	char buffer[512];
	int size = sizeof buffer;
//	print table header
	snprintf( buffer, size, "%s\n"
							"parent scope_ptr: %llX \n"
							"scope_ptr: %llX - identifier: %s \n"
							"%s\n",
			  line, (long long) table->parent, (long long) table, table->name, line );
	fputs( buffer, out );
//	print table entries
	for( unsigned int i = 0; i < table->entry_count; ++i ) {
		SymbolEntry entry = table->entries[i];
		switch( entry.type ) {
			case SB_FUNCTION:
				snprintf( buffer, size,
						  "%-16s %-32s | scope_ptr: %llX \n",
						  SB_IDENT_TYPE[SB_FUNCTION], entry.ident, (long long) entry.data.func.scope_ptr );
				break;
			case SB_ARRAY:
				snprintf( buffer, size,
						  "%-16s %-32s | size: %ld \n",
						  SB_IDENT_TYPE[SB_ARRAY], entry.ident, entry.data.arr_size );
				break;
			case SB_INT:
				snprintf( buffer, size,
						  "%-16s %-32s | initialized: %d \n",
						  SB_IDENT_TYPE[SB_INT], entry.ident, entry.data.var_initialized );
				break;
			case SB_CONST_INT:
				snprintf( buffer, size,
						  "%-16s %-32s | scope_ptr: %ld \n",
						  SB_IDENT_TYPE[SB_FUNCTION], entry.ident, entry.data.const_value );
				break;
		}
		fputs( buffer, out );
	}
//	print table footer
	snprintf( buffer, size, "%s\n\n", line );
	fputs( buffer, out );
//	print other tables
	for( unsigned int i = 0; i < table->entry_count; ++i ) {
		SymbolEntry entry = table->entries[i];
		if( entry.type == SB_FUNCTION ) { PrintSymbolTable( entry.data.func.scope_ptr, out ); }
	}
}

#endif //COMP_LIST_SYMBOL_TABLE_H
