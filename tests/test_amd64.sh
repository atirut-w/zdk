#!/bin/bash
# Test script to verify AMD64 output

set -e

TEST_DIR="$(dirname "$0")"
BUILD_DIR="${BUILD_DIR:-$TEST_DIR/../build}"
CC1="$BUILD_DIR/cc1/cc1"

if [ ! -x "$CC1" ]; then
    echo "Error: cc1 not found at $CC1"
    exit 1
fi

# Create temporary directory
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

# Compile test file
"$CC1" -o "$TEMP_DIR/output.s" "$TEST_DIR/simple_main.c"

# Verify output contains expected AMD64 instructions
echo "Checking AMD64 assembly output..."

if ! grep -q "\.text" "$TEMP_DIR/output.s"; then
    echo "Error: Missing .text directive"
    exit 1
fi

if ! grep -q "\.globl main" "$TEMP_DIR/output.s"; then
    echo "Error: Missing .globl main directive"
    exit 1
fi

if ! grep -q "pushq %rbp" "$TEMP_DIR/output.s"; then
    echo "Error: Missing function prologue"
    exit 1
fi

if ! grep -q "movq %rsp, %rbp" "$TEMP_DIR/output.s"; then
    echo "Error: Missing frame pointer setup"
    exit 1
fi

if ! grep -q "movl \$0, %eax" "$TEMP_DIR/output.s"; then
    echo "Error: Missing return value setup"
    exit 1
fi

if ! grep -q "popq %rbp" "$TEMP_DIR/output.s"; then
    echo "Error: Missing function epilogue"
    exit 1
fi

if ! grep -q "ret" "$TEMP_DIR/output.s"; then
    echo "Error: Missing ret instruction"
    exit 1
fi

echo "AMD64 test passed!"
exit 0
