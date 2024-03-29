### Semantic Analyzer

- **AST Parsing**: Parses the Abstract Syntax Tree (AST) generated by the parser and generates symbol tables to aid in
  semantic checks.
- **Symbol Tables**: The symbol tables generated during analysis are written to a `symbol_table` file, located in
  the `/output/` directory.
- **Semantic Checks**: `semantic_checks.h` implements declaration check, variable assignment check and variable evaluation / function call check.
- **Scoping**: Nested function definitions are not allowed and closure are not supported. Effectively, there are only 2 scopes: global scope and function local scopes.
- **Namespace**: To simplify implementation, function and variable share a single namespace.