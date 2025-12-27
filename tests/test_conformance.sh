#!/bin/bash
# Conformance test - verify test cases are valid C90 code

set -e

TEST_DIR="$(dirname "$0")"

# Find C compiler
if command -v gcc >/dev/null 2>&1; then
    CC=gcc
elif command -v clang >/dev/null 2>&1; then
    CC=clang
else
    echo "Error: No C compiler (gcc or clang) found"
    exit 1
fi

echo "Using compiler: $CC"

# Test all C files in the tests directory
for test_file in "$TEST_DIR"/*.c; do
    if [ -f "$test_file" ]; then
        echo "Testing C90 conformance: $(basename "$test_file")"
        if ! "$CC" -fsyntax-only -std=c90 -pedantic -pedantic-errors "$test_file" 2>&1; then
            echo "Error: $(basename "$test_file") is not valid C90"
            exit 1
        fi
    fi
done

echo "All test files conform to C90 standard!"
exit 0
