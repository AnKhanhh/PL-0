### File Organization
The source code is organized into directories corresponding to typical stages of compiling. Directory names are self-explanatory and keep the project modular.

### Language Features
- **Data Types**: The compiler supports only integer and 1D array data types.
- **PL/0 Syntax**: PL/0 is a declarative language, meaning variables and constants must be declared in a separate code section. This simplifies the front end implementation.
- **Functions**: To keep the compiler minimalistic, functions in PL/0 do not have return statements and all parameters are expected to be integer type.
