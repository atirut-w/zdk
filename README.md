# ZDK
C compiler and libraries for Z80 development.

> [!WARNING]
> This project is currently half in limbo, especially the compiler. I have decided to shift focus to compiling LLVM IR instead.

## Build Dependencies
- ANTLR4 for generating the C parser. Your distribution of choice should already have a package for ANTLR4, but you can also download from [the official website](https://www.antlr.org/).

> [!WARNING]
> Ubuntu's package for ANTLR4 is outdated and generates invalid C++ code. You should download the JAR from the official website and use that instead.

## Dependencies
- [GNU binutils](https://www.gnu.org/software/binutils/) (`--target=z80-elf`) for assembling and linking. Refer to the [LURe build script](https://github.com/lure-sh/lure-repo/blob/master/binutils-z80/lure.sh) for an example of how to build binutils for the Z80 target, or use LURe to build it for you.
- Clang for validating input code.

## General Goals
- [ ] C90 compiler.
- [ ] A platform-agnostic implementation of the C90 standard library.
