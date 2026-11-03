# C Lexical Analyzer and Syntax Checker

A lightweight C Lexical Analyzer and Syntax Checker written in C. It performs lexical scanning, token classification, and basic syntax validation on C source files.

---

## Features

### 1. Lexical Analysis (`lexer.c` / `lexer.h`)
- **Token Classification**:
  - `PRE-PROCESSOR`: Directives such as `#include`, `#define`, `#ifdef`, etc.
  - `HEADER`: System header files enclosed in `<...>` (e.g., `<stdio.h>`).
  - `RESERVED KEYWORD`: C reserved types and qualifiers (`int`, `char`, `float`, `const`, `static`, etc.).
  - `NON RESERVED KEYWORD`: Control flow and statements (`if`, `else`, `while`, `for`, `return`, etc.).
  - `IDENTIFIER`: Variable and function names (`main`, `x`, `msg`, etc.).
  - `NUMERIC CONSTANT`: Integers and floating-point literals (with lexical error detection for malformed numbers like `12.3.4`).
  - `CHARACTER CONSTANT`: Single-character literals (`'A'`, `'\n'`).
  - `STRING LITERAL`: Double-quoted string constants (`"Hello, world!"`).
  - `OPERATOR`: Single-character and compound operators (`+`, `-`, `*`, `/`, `==`, `!=`, `<=`, `>=`, `++`, `--`, `&&`, `||`, `->`).
  - `OPEN_BRACE` / `CLOSE_BRACE`: Parentheses, curly braces, and square brackets (`()`, `{}`, `[]`).
  - `TERMINATING OPERATOR`: Statement terminator (`;`).
  - `LEXICAL_ERROR`: Malformed tokens (invalid numbers, unclosed characters/headers).
- **Comment Handling**: Automatically strips single-line comments (`//`) and multi-line comments (`/* ... */`).
- **Line Tracking**: Maintains line numbers for precise error reporting.

### 2. Syntax Validation (`syntax.c` / `syntax.h`)
- **Header Validation**: Checks if included header files are recognized standard C libraries.
- **Required Header Verification**: Warns if library functions (e.g. `printf`, `malloc`, `strlen`, `sqrt`) are called without including the required headers (`<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<math.h>`).
- **Delimiter Matching**: Uses a stack to detect mismatched, unclosed, or unexpected parentheses, curly braces, and square brackets.
- **Semicolon Validation**:
  - Detects missing semicolons across declarations, assignments, returns, increments/decrements, and function calls.
  - Supports pointer variable declarations (e.g. `char *msg = "Hello";`) and multiple declarations on one line.
  - Properly skips `for (...)` loop headers and function definition parameter blocks.

### 3. Multi-File CLI Support (`main.c`)
- Supports running against **one or more files** passed via command-line arguments.
- Safely reports file-open errors and continues processing remaining files.
- Automatically falls back to `input.c` if no command-line arguments are provided.

---

## Project Structure

```text
Lexical/
├── main.c       # CLI entry point, file processor, and output formatter
├── lexer.c      # Lexical scanner and token extraction
├── lexer.h      # Token types, Token struct, and lexer prototypes
├── syntax.c     # Syntax analysis rules and checks
├── syntax.h     # Syntax analysis prototypes
├── input.c      # Sample input C source file for testing
├── Makefile     # Build automation rules
└── README.md    # Project documentation
```

---

## Build Instructions

### Prerequisites
- GCC (`gcc`)
- GNU Make (`make`)

### Compiling with Make
```bash
make
```

### Compiling Manually with GCC
```bash
gcc -Wall -Wextra main.c lexer.c syntax.c -o lexer
```

---

## Usage

### 1. Default (Single File: `input.c`)
If no arguments are provided, it automatically analyzes `input.c`:
```bash
./lexer
# or
make run
```

### 2. Specific File
```bash
./lexer sample.c
```

### 3. Multiple Files
Pass any number of files separated by spaces:
```bash
./lexer input1.c input2.c check.c
```

### 4. Wildcard Pattern
```bash
./lexer input*.c
```

### 5. Cleaning Build Files
Remove generated object files and binaries:
```bash
make clean
```

---

## Sample Output

```text
======================================================
 FILE: input.c
======================================================
------------------------------------------------------
Line No    Token                : Token Type
------------------------------------------------------
1          #include             : PRE-PROCESSOR
1          <stdio.h>            : HEADER
3          int                  : RESERVED KEYWORD
3          main                 : IDENTIFIER
3          (                    : OPEN_BRACE
3          )                    : CLOSE_BRACE
4          {                    : OPEN_BRACE
5          int                  : RESERVED KEYWORD
5          x                    : IDENTIFIER
5          =                    : OPERATOR
5          10                   : NUMERIC CONSTANT
5          ;                    : TERMINATING OPERATOR
...
------------------------------------------------------
```

If syntax errors are detected, they are displayed directly above the token table:
```text
Syntax Error: Missing required header <stdio.h>
Syntax Error: Missing semicolon near line 6
Syntax Error: Mismatched delimiter ')' near line 8
```
