# C90 Compiler Driver

The `cc` driver orchestrates the complete compilation process from C source code to Z80 executable.

## Architecture

The driver coordinates three main stages:

1. **Compilation**: Calls `cc1` to compile C source to Z80 assembly
2. **Assembly**: Calls `z80-unknown-none-elf-as` to assemble to object file
3. **Linking**: Calls `z80-unknown-none-elf-ld` to link into executable

## Usage

```bash
cc [options] file.c
```

### Options

- `-o <file>` - Specify output file name
- `-S` - Stop after compilation; output assembly (.s)
- `-c` - Stop after assembly; output object file (.o)
- `-h, --help` - Display help information

### Examples

```bash
# Compile to executable
cc main.c -o program

# Generate assembly only
cc main.c -S -o main.s

# Generate object file only
cc main.c -c -o main.o
```

## Implementation Notes

- Uses temporary files for intermediate stages (cleaned up automatically)
- Requires `cc1` and Z80 binutils in PATH
- Currently assumes preprocessed input (TODO: integrate cpp)

## TODO

- Integrate C preprocessor (cpp) for `#include`, `#define`, etc.
- Add support for multiple input files
- Add library linking support (-l flag)
- Add include path support (-I flag)

