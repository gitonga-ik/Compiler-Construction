# Nobuglang Compiler For C--

Nobuglang is a compiler for the **c--** mini-language. It performs a full compilation pipeline from source code to Intermediate Code Generation (ICG) using a table-driven LL(1) parser and a Flex-based scanner.

## Features
- **Lexical Analysis:** Uses Flex to generate a fast scanner.
- **Syntactic Analysis:** Implements a table-driven LL(1) parser.
- **AST Generation:** Constructs an Abstract Syntax Tree from the concrete parse tree.
- **Intermediate Code Generation:** Produces Three-Address Code (TAC) for the target program.

---

## Compiler Architecture

The compiler operates in several distinct stages:

1. **Scanner (`scanner.l`):**
   The scanner is generated using Flex. It reads the raw source code and converts it into a stream of tokens. It identifies keywords, identifiers, literals, and operators.
   
2. **Parser (`ll1_parser.cpp`):**
   A table-driven LL(1) parser consumes the tokens produced by the scanner. It validates the syntax against the Nobuglang grammar and produces a Concrete Parse Tree.

3. **AST Builder (`ast_builder.cpp`):**
   The concrete parse tree is simplified into an Abstract Syntax Tree (AST), which represents the logical structure of the program without the syntactic overhead (like parentheses or semicolons).

4. **Intermediate Code Generator (`icg.cpp`):**
   The ICG traverses the AST and generates Three-Address Code (TAC), which is saved to `tac.out`.

---

## Alphabet (Tokens)

The C-- alphabet consists of the following symbols defined in `scanner.l`:

- **Keywords:** `int`, `string`, `if`, `else`, `while`, `cout`, `BEGIN`, `END`
- **Identifiers:** Sequences of letters, digits, and underscores starting with a letter or underscore.
- **Literals:**
  - **Integers:** Sequences of one or more digits.
  - **Strings:** Text enclosed in double quotes (e.g., `"hello"`).
- **Operators:**
  - Arithmetic: `+`, `-`
  - Assignment: `=`
  - Comparison: `==`, `<`, `>`
  - Output: `<<` (used with `cout`)
- **Punctuation:** `;`, `{`, `}`, `(`, `)`

---

## Grammar

The compiler implements the following LL(1) grammar (found in `ll1_parser.cpp`):

1.  `program -> BEGIN stmt_list END`
2.  `stmt_list -> stmt stmt_list' | epsilon`
3.  `stmt -> assignment | print_stmt | if_stmt | while_stmt`
4.  `assignment -> type id = expr ; | id = expr ;`
5.  `print_stmt -> cout << expr ;`
6.  `if_stmt -> if ( expr ) { stmt_list } else_part`
7.  `else_part -> else { stmt_list } | epsilon`
8.  `while_stmt -> while ( expr ) { stmt_list }`
9.  `expr -> arith_expr expr'`
10. `expr' -> == arith_expr | < arith_expr | > arith_expr | epsilon`
11. `arith_expr -> term arith_expr'`
12. `arith_expr' -> + term arith_expr' | - term arith_expr' | epsilon`
13. `term -> id | integer | string_literal | ( expr )`
14. `type -> int | string`

---

## Getting Started

### Prerequisites
- `g++` (C++17 or later)
- `flex`
- `make`

### Building the Compiler
To build the scanner and the ICG components, run:
```bash
make icg
```

### Running the Compiler
To compile an input file (e.g., `input.txt`):
1. **Scan the input:**
   ```bash
   ./scanner input.txt
   ```
   This generates `tokens.out`.

2. **Parse and Generate Code:**
   ```bash
   ./icg tokens.out
   ```
   This performs parsing, AST construction, and generates `tac.out`.

Alternatively, you can run the full pipeline using:
```bash
make all
```

## Files Overview
- `scanner.l`: Flex specification for the lexical analyzer.
- `ll1_parser.cpp`: Implementation of the LL(1) parser and main driver for the backend.
- `ast_builder.cpp` / `ast.cpp`: Logic for constructing and representing the AST.
- `icg.cpp`: Logic for generating Three-Address Code.
- `makefile`: Build instructions.
- `input.txt`: Sample source code in the c-- language.

---

Awwww.......You actually read it ♡(>ᴗ•)