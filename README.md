# ZDK
Compiler frontend, IR compiler, and other tools for Z80 development.

## Dependencies
- [GNU binutils](https://www.gnu.org/software/binutils/) (`--target=z80-elf`) for assembling and linking. Refer to the [LURe build script](https://github.com/lure-sh/lure-repo/blob/master/binutils-z80/lure.sh) for an example of how to build binutils for the Z80 target, or use LURe to build it for you.
- ANTLR4 for generating parsers. Your distribution of choice should already have a package for ANTLR4, but you can also download from [the official website](https://www.antlr.org/).
- Clang for generating IR.

> [!WARNING]
> Ubuntu's package for ANTLR4 is outdated and generates invalid C++ code. You should download the JAR from the official website and use that instead.

## General Goals
- [ ] An LLVM IR compiler that can compile most Clang-generated IR.
- [ ] A frontend for Clang, ZDK LLC, and GNU Assembler.
- [ ] Standard library.
