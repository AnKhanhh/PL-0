### Semantic Analyzer

- **AST Parsing**: Parses the Abstract Syntax Tree (AST) generated by the parser and generates symbol tables to aid in semantic checks.
- **Symbol Tables**: The symbol tables generated during analysis are written to a `symbol_table` file, located in the `/output/` directory.
- **Semantic Checks**: Implemented in `semantic_checks.h`, the Semantic Analyzer performs three types of checks:
    1. **Declaration Check**: Ensures variables, constants and function are declared before use.
    2. **Variable Assignment Check**: Validates assignments to variables, ensuring compatibility of types.
    3. **Evaluation Check**: Validates variable access and function calls to ensure correct evaluation.