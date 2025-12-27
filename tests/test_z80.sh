#!/bin/bash
# Test script to verify Z80 output

set -e

TEST_DIR="$(dirname "$0")"
BUILD_DIR="${BUILD_DIR:-$TEST_DIR/../build}"
CC1="$BUILD_DIR/bin/cc1"

if [ ! -x "$CC1" ]; then
    echo "Error: cc1 not found at $CC1"
    exit 1
fi

# Create temporary directory
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

# Compile test file
"$CC1" -o "$TEMP_DIR/output.s" "$TEST_DIR/simple_main.c"

# Verify output contains expected Z80 instructions
echo "Checking Z80 assembly output..."

if ! grep -q "\.text" "$TEMP_DIR/output.s"; then
    echo "Error: Missing .text directive"
    exit 1
fi

if ! grep -q "\.globl main" "$TEMP_DIR/output.s"; then
    echo "Error: Missing .globl main directive"
    exit 1
fi

if ! grep -q "push ix" "$TEMP_DIR/output.s"; then
    echo "Error: Missing IX push in prologue"
    exit 1
fi

if ! grep -q "ld ix, 0" "$TEMP_DIR/output.s"; then
    echo "Error: Missing IX initialization"
    exit 1
fi

if ! grep -q "add ix, sp" "$TEMP_DIR/output.s"; then
    echo "Error: Missing IX frame pointer setup"
    exit 1
fi

if ! grep -q "ld hl, 0" "$TEMP_DIR/output.s"; then
    echo "Error: Missing return value in HL register"
    exit 1
fi

if ! grep -q "ld sp, ix" "$TEMP_DIR/output.s"; then
    echo "Error: Missing stack pointer restoration"
    exit 1
fi

if ! grep -q "pop ix" "$TEMP_DIR/output.s"; then
    echo "Error: Missing IX pop in epilogue"
    exit 1
fi

if ! grep -q "ret" "$TEMP_DIR/output.s"; then
    echo "Error: Missing ret instruction"
    exit 1
fi

echo "Z80 test passed!"
exit 0
