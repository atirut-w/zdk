# C90 Compiler
A WIP C90 compiler written in C90. Yacc and Lex source files are based on the [ANSI C Yacc grammar](https://www.lysator.liu.se/c/ANSI-C-grammar-y.html) and the [ANSI C Lex specification](https://www.lysator.liu.se/c/ANSI-C-grammar-l.html).

> [!NOTE]
> The generated parser and lexer files are checked into VCS for portability reasons. You will need `bison` and `flex` only if you want to modify the grammar or the lexer specification.
