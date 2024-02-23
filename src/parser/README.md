### Syntax Analysis

- **LL(1) Parser**: The input program's structure is parsed by an LL(1) parser, which is implemented in `./recursive_descent_parser.h`.
- **Mathematical Expressions**: Mathematical expressions are parsed using a [Pratt parser](https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html), implemented in `./expression_parser.h`.
- **Debugging Output**: The Syntax Analysis stage emits a human-readable syntax_tree file in the `/output/` directory, aiding in debugging efforts. Refers to `/doc/syntax_tree_structure.txt` for more info.
- **Grammar**: for grammar overview, as well as the general structure of the recursive descent parser, refers to `/doc/language_structure.pdf`