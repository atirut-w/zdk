# ZDK
Various things for Z80 development.

## Components

### cc - C90 Compiler Driver
The compiler driver orchestrates the compilation process from C source to Z80 executable. It coordinates calling:
- `cc1` - the actual compiler (C to Z80 assembly)
- `z80-unknown-none-elf-as` - the assembler (assembly to object file)
- `z80-unknown-none-elf-ld` - the linker (object file to executable)

Usage:
```bash
cc [options] file.c
```

Options:
- `-o <file>` - Place output into `<file>`
- `-S` - Compile only; output assembly
- `-c` - Compile and assemble; output object file
- `-h, --help` - Display help information

Example:
```bash
cc input.c -o output         # Full compilation to executable
cc input.c -S -o output.s    # Compile to assembly only
cc input.c -c -o output.o    # Compile and assemble only
```

### cc1 - C90 Compiler
The main compiler that translates C source code to Z80 assembly. Currently supports minimal C90:
- `int main() { return <number>; }`
- `int` type is 16-bit, returned in HL register pair
- Input is assumed to be preprocessed (no `#include`, `#define`, etc.)

Usage:
```bash
cc1 [input_file] [-o output_file]
```

Example:
```bash
cc1 input.c -o output.s      # Compile to assembly
echo "int main() { return 42; }" | cc1 -o output.s  # From stdin
```

## Building

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

The built executables will be in:
- `build/cc/cc` - Compiler driver
- `build/cc1/cc1` - Compiler

## Testing

Create a simple test program:
```c
int main() { return 0; }
```

Compile it:
```bash
./build/cc/cc test.c -o test
```

Verify the output:
```bash
z80-unknown-none-elf-objdump -d test
```

## Current Limitations

- Only supports `int main() { return <number>; }` programs
- No preprocessor support yet (TODO)
- No function parameters or local variables
- No expressions beyond integer literals
- No control flow (if, while, for, etc.)

