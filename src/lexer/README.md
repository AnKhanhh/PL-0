### Lexical Analyzer

- The Lexical Analyzer tokenizes input files, generating a token stream file located in the `/output/` directory.
- This token_stream file serves as input for the parser, where it is used to construct the Abstract Syntax Tree (AST).
- Tokens are constructed from input stream one character at a time, and whitespaces are skipped. Refers to `/doc/accepted_tokens.txt`.
