# C90 Compiler

The `cc1` compiler translates C source code to Z80 assembly language.

## Current Capabilities

The compiler currently supports a minimal subset of C90:

```c
int main() { return <number>; }
```

### Supported Features

- Function definition: `int <name>() { ... }`
- Return statement: `return <number>;`
- Integer literals
- 16-bit `int` type (returned in HL register pair)

### Type System

- `int` is 16-bit (matching Z80's 16-bit register pairs)
- Return values are placed in HL register (Z80 convention)

## Usage

```bash
cc1 [input_file] [-o output_file]
```

### Options

- `input_file` - C source file to compile (if omitted, reads from stdin)
- `-o output_file` - Output assembly file (if omitted, writes to stdout)
- `-h, --help` - Display help information

### Examples

```bash
# Compile file to assembly
cc1 main.c -o main.s

# Read from stdin, write to stdout
echo "int main() { return 42; }" | cc1

# Read from file, write to stdout
cc1 main.c > main.s
```

## Output Format

The compiler generates Z80 assembly in GAS (GNU Assembler) format:

```asm
.section .text
.global main
main:
    ld hl, 42
    ret
```

## Architecture

The compiler consists of three main components:

1. **Lexer** (`lexer.c`) - Tokenizes the input
2. **Parser** (`parser.c`) - Builds an Abstract Syntax Tree (AST)
3. **Code Generator** (`codegen.c`) - Emits Z80 assembly from AST

## Implementation Notes

- Written in C90 for portability
- Input is assumed to be preprocessed
- No error recovery (fails on first syntax error)
- Memory is allocated but not extensively error-checked

## TODO

- Integrate with preprocessor for `#include`, `#define`, etc.
- Add support for function parameters
- Add support for local variables
- Add support for expressions (arithmetic, logical, etc.)
- Add support for control flow (if, while, for, etc.)
- Add support for more types (char, short, long, pointers, etc.)
- Improve error messages with better diagnostics

