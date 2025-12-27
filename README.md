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
The main compiler that translates C source code to Z80 assembly. Supports:

**Implemented Features:**
- Function definitions with parameters (`int`, `void` types)
- Function prototypes (forward declarations)
- Function calls with multiple arguments
- Character literals (e.g., `'A'`, `'\n'`) evaluated as `int` type
- Multiple statements in function bodies
- Return statements (including empty returns for `void` functions)
- Stack frames using IX register as frame pointer
- Standard Z80 calling convention:
  - Arguments pushed right-to-left
  - Return values in HL register (16-bit)
  - Caller cleans up stack

**Type System:**
- `int` type is 16-bit, returned in HL register pair
- `void` return type for functions
- Character literals are `int` type

**Limitations:**
- Input is assumed to be preprocessed (no `#include`, `#define`, etc.)
- No local variables yet
- No complex expressions (only literals and function calls)
- No control flow (if, while, for, etc.)
- No operators (arithmetic, logical, etc.)

Usage:
```bash
cc1 [input_file] [-o output_file]
```

Example:
```bash
cc1 input.c -o output.s      # Compile to assembly
echo "int main() { return 42; }" | cc1 -o output.s  # From stdin
```

Example program:
```c
int bdos(int func, int arg);

int main() {
  bdos(2, 'H');
  bdos(2, 'i');
  bdos(2, '\n');
  return 0;
}

void _start() {
  main();
  bdos(0, 0);
}
```

## Building

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

The built executables will be in:
- `build/bin/cc` - Compiler driver
- `build/bin/cc1` - Compiler

## Testing

Create a simple test program:
```c
int main() { return 0; }
```

Compile it:
```bash
./build/bin/cc test.c -o test
```

Verify the output:
```bash
z80-unknown-none-elf-objdump -d test
```

