# ZDK
C compiler and libraries for Z80 development.

The compiler itself is loosely based on Nora Sandler's book on writing a C compiler, but with some key differences:
- The semantic analysis is bundled into IR generation.
- Uses LLVM IR.
- Uses an ANTLR4-generated parser.
- Register allocation is implemented early on as a consequence of using LLVM IR.
- Uses a bottom-up approach to register allocation.

## Build Dependencies
- ANTLR4 for generating the C parser. Your distribution of choice should already have a package for ANTLR4, but you can also download from [the official website](https://www.antlr.org/).

> [!WARNING]
> Ubuntu's package for ANTLR4 is outdated and generates invalid C++ code. You should download the JAR from the official website and use that instead.

## Dependencies
- [GNU binutils](https://www.gnu.org/software/binutils/) (`--target=z80-elf`) for assembling and linking. Refer to the [LURe build script](https://github.com/lure-sh/lure-repo/blob/master/binutils-z80/lure.sh) for an example of how to build binutils for the Z80 target, or use LURe to build it for you.
- LLVM for intermediate representation.

## General Goals
- [ ] C90 compiler.
- [ ] A platform-agnostic implementation of the C90 standard library.
