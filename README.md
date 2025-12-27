# ZDK
Various things for Z80 development.

## C90 Compiler

ZDK includes a simple C90 compiler (`cc1`) and compiler driver (`cc`). The compiler can compile basic C90 programs like `int main() { return 0; }` to assembly for different target architectures.

### Supported Targets

- **AMD64**: x86-64 architecture using SystemV ABI
- **Z80**: Z80 processor with custom ABI (IX as frame pointer, 16-bit ints, returns in HL register)

### Building

The compiler uses CMake as its build system. The target architecture is selected at build time. All executables are placed in `build/bin/` for convenience.

#### Build for AMD64 (default)

```bash
cmake -S . -B build -DZDK_TARGET=amd64
cmake --build build
```

#### Build for Z80

```bash
cmake -S . -B build -DZDK_TARGET=z80
cmake --build build
```

### Usage

For convenience, add the build/bin directory to your PATH:

```bash
export PATH="$PWD/build/bin:$PATH"
```

#### Compiler Frontend (cc1)

The `cc1` compiler frontend reads C source code and generates assembly:

```bash
cc1 -o output.s input.c
```

Options:
- `-o <file>`: Write output to specified file
- `-h, --help`: Display help message

#### Compiler Driver (cc)

The `cc` compiler driver orchestrates compilation, assembly, and linking:

```bash
# Compile to assembly only
cc -S -o output.s input.c

# Compile and assemble to object file
cc -c -o output.o input.c

# Full compilation (compile, assemble, and link)
cc -o program input.c
```

Options:
- `-o <file>`: Write output to specified file
- `-S`: Compile only; do not assemble
- `-c`: Compile and assemble; do not link
- `-h, --help`: Display help message

**Note:** The driver assumes `cc1` is in your PATH. The full compilation pipeline calls the appropriate assembler (`as` for AMD64, `z80-unknown-none-elf-as` for Z80 if available) and linker (`ld` for AMD64, `z80-unknown-none-elf-ld` for Z80 if available).

### Testing

Run the test suite with CTest:

```bash
# Configure and build
cmake -S . -B build -DZDK_TARGET=amd64
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure
```

Tests include:
- **C90 Conformance Test**: Validates test cases against host compiler with `-fsyntax-only -std=c90 -pedantic -pedantic-errors`
- **Backend-Specific Tests**: Validates generated assembly for the selected target

### Example

Create a simple C program:

```c
int main() {
    return 0;
}
```

Compile it:

```bash
# Add build/bin to PATH
export PATH="$PWD/build/bin:$PATH"

# Compile to assembly
cc -S -o output.s input.c

# Or compile to object file
cc -c -o output.o input.c
```

The generated output will target the architecture specified during build configuration.

### Architecture Details

#### AMD64 Backend

- Follows SystemV ABI calling convention
- Uses `rbp` as frame pointer
- Returns integer values in `eax/rax` register
- Function prologue: `push rbp; mov rbp, rsp`
- Function epilogue: `pop rbp; ret`

#### Z80 Backend

- Uses `IX` register as frame pointer
- 16-bit integers
- Returns values in `HL` register pair
- Function prologue: `push ix; ld ix, 0; add ix, sp`
- Function epilogue: `ld sp, ix; pop ix; ret`

### Project Structure

```
zdk/
├── cc/               # Compiler driver
│   └── src/
│       └── main.cpp
├── cc1/              # Compiler frontend
│   ├── include/      # Header files
│   │   ├── ast.hpp
│   │   ├── codegen.hpp
│   │   ├── lexer.hpp
│   │   ├── parser.hpp
│   │   └── token.hpp
│   ├── reference/    # C90 grammar reference
│   │   ├── c90.l     # Lex grammar
│   │   └── c90.y     # Yacc grammar
│   └── src/
│       ├── ast/      # AST implementation
│       ├── codegen/  # Code generators (amd64, z80)
│       ├── lexer/    # Lexical analyzer
│       └── parser/   # Parser
└── tests/            # Test suite
    ├── simple_main.c
    ├── test_amd64.sh
    ├── test_z80.sh
    └── test_conformance.sh
```

### Implementation Details

#### Lexer

The lexer streams input from an `std::istream` with an internal 4KB buffer for memory efficiency. The buffer supports lookahead for tokenization.

#### Parser

The parser builds an Abstract Syntax Tree (AST) from tokens using recursive descent parsing.

#### Code Generation

Code generators implement the visitor pattern to traverse the AST and emit assembly code. Backends are conditionally compiled based on the `ZDK_TARGET` CMake option to reduce binary size.

