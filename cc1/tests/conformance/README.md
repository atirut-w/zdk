# Conformance Tests

This directory contains conformance tests for the cc1 compiler frontend.

## Purpose

These tests document cc1's conformance (or non-conformance) to the C90 standard by comparing its behavior against a host compiler (gcc) with strict C90 flags: `-fsyntax-only -std=c90 -pedantic -pedantic-errors`.

**Important:** Failing tests indicate non-conformance in cc1. These tests are meant to:
- Document which C90 features are correctly implemented
- Identify where cc1 accepts invalid C90 code (semantic errors)
- Identify where cc1 rejects valid C90 code (missing features)

Fixing non-conformance is out of scope for these tests, but documenting it is essential.

## Test Structure

Tests are organized into three categories:

### Valid Tests (`valid_*.c`)
These files contain valid C90 code that should be successfully parsed by both the host compiler and cc1.
**Failing tests in this category indicate missing features in cc1.**

Examples:
- `valid_basic_function.c` - Basic function with return statement ✓
- `valid_prototype.c` - Function prototypes/forward declarations ✓
- `valid_function_call.c` - Function calls with arguments ✓
- `valid_char_literals.c` - Character literal expressions ✓
- `valid_string_literal.c` - String literals with const pointers ✓
- `valid_dereference.c` - Pointer dereference operations ✓
- `valid_postfix_inc.c` - Postfix increment operations ✓
- `valid_void_function.c` - Void function with empty return ✓
- `valid_function_params.c` - Functions with parameters ✓
- `valid_multiple_declarations.c` - Multiple global declarations ✗ (not supported)

### Invalid Tests (`invalid_*.c`)
These files contain invalid C90 code that should be rejected by both the host compiler and cc1.
**Failing tests in this category indicate semantic analysis bugs in cc1.**

Examples:
- `invalid_missing_semicolon.c` - Missing semicolon ✓
- `invalid_missing_brace.c` - Missing closing brace ✓
- `invalid_unterminated_string.c` - Unterminated string literal ✓
- `invalid_empty_char_literal.c` - Empty character literal ✓
- `invalid_void_return_value.c` - Return value in void function ✗ (cc1 accepts this incorrectly)

### Unsupported Tests (`unsupported_*.c`)
These files contain valid C90 code for features not yet implemented in cc1.
**These tests document known limitations and missing features.**

Examples:
- `unsupported_binary_operator.c` - Binary arithmetic operators ✗
- `unsupported_array.c` - Array declarations ✗
- `unsupported_if_statement.c` - Control flow (if statements) ✗
- `unsupported_while_loop.c` - Control flow (while loops) ✗

## Expected Test Results

Not all tests are expected to pass! The conformance test suite documents both:
- **What cc1 correctly implements** (passing tests)
- **What cc1 doesn't implement or implements incorrectly** (failing tests)

### Current Status

As of the initial implementation:
- **13/19 tests pass** (68%)
- **6/19 tests fail** (32%)

Failing tests breakdown:
- 1 semantic analysis bug (accepts invalid code)
- 4 unsupported C90 features (rejects valid code)
- 1 missing feature (global variable declarations)

This is expected and intentional - the tests serve as documentation of cc1's current capabilities.

## How Tests Work

The `run_test.sh` script:

1. Validates the test file against the host compiler with strict C90 flags
2. Preprocesses the file (removes comments, etc.) since cc1 expects preprocessed input
3. Runs cc1 on the preprocessed input
4. Compares exit codes:
   - **PASS**: Both compilers accept the input OR both reject it
   - **FAIL**: One compiler accepts while the other rejects

## Running Tests

Tests are integrated into the CMake/CTest build system:

```bash
# Configure and build
cmake -S . -B build
cmake --build build

# Run all tests
ctest --test-dir build

# Run only conformance tests
ctest --test-dir build -R cc1_conformance

# Run a specific test
ctest --test-dir build -R cc1_conformance_valid_basic_function
```

## Adding New Tests

To add a new conformance test:

1. Create a `.c` file in this directory with a descriptive name
   - Prefix with `valid_` for valid C90 code
   - Prefix with `invalid_` for invalid C90 code

2. Include comments describing what the test validates

3. Reconfigure CMake to pick up the new test:
   ```bash
   cmake -S . -B build
   ```

4. Run the tests to verify:
   ```bash
   ctest --test-dir build -R cc1_conformance
   ```

## Notes

- Test files contain C comments which are removed during preprocessing
- cc1 expects preprocessed input (no `#include`, `#define`, comments, etc.)
- The `-P` flag is used with gcc preprocessing to suppress line markers
- Tests focus on syntax and basic semantics that cc1 currently implements
