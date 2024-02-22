### 1. What is this?
This is a handwritten compiler for the [PL/0](https://en.wikipedia.org/wiki/PL/0) language that I'm working on .The purpose of this project is to make a complete compiler without relying on any tool ( e.g. flex, bison, LLVM ).
### 2. Approach:
- This is the first time I'm writing a compiler, I picked PL/0  for its simplicity. 
- Optimization is a very complex topic, so I don't intend to implement a dedicated optimizer.
### 3. Navigation:
- Miscellaneous notes is stored inside /doc/. As I refactor my code quite frequently, I will reorganize them when I finish this project.
- source code inside /src/.
- /input/ store test files, and /output/ store human-readable generated debug files.