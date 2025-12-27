#!/bin/sh
# Conformance test runner
# Compares cc1's behavior against host compiler (gcc) with strict C90 flags

set -e

CC1="$1"
TEST_FILE="$2"

if [ -z "$CC1" ] || [ -z "$TEST_FILE" ]; then
    echo "Usage: $0 <cc1_executable> <test_file>"
    exit 1
fi

if [ ! -f "$CC1" ]; then
    echo "Error: cc1 executable not found: $CC1"
    exit 1
fi

if [ ! -f "$TEST_FILE" ]; then
    echo "Error: test file not found: $TEST_FILE"
    exit 1
fi

# Create a temporary file for preprocessed output
PREPROCESSED=$(mktemp)
trap "rm -f $PREPROCESSED" EXIT

# Test with host compiler (syntax check on original file)
HOST_EXIT=0
gcc -fsyntax-only -std=c90 -pedantic -pedantic-errors "$TEST_FILE" 2>/dev/null || HOST_EXIT=$?

# Preprocess the file for cc1 (cc1 expects preprocessed input)
# Use -P to suppress line markers which cc1 doesn't handle
# Note: Preprocessing may fail for invalid input files, but we still want to
# continue and let cc1 try to parse the output, hence || true
gcc -E -P -std=c90 -pedantic -pedantic-errors "$TEST_FILE" -o "$PREPROCESSED" 2>/dev/null || true

# Test with cc1 on preprocessed input
CC1_EXIT=0
"$CC1" "$PREPROCESSED" -o /dev/null 2>/dev/null || CC1_EXIT=$?

# Compare results
if [ $HOST_EXIT -eq 0 ] && [ $CC1_EXIT -eq 0 ]; then
    echo "PASS: Both accept $TEST_FILE"
    exit 0
elif [ $HOST_EXIT -ne 0 ] && [ $CC1_EXIT -ne 0 ]; then
    echo "PASS: Both reject $TEST_FILE"
    exit 0
elif [ $HOST_EXIT -eq 0 ] && [ $CC1_EXIT -ne 0 ]; then
    echo "FAIL: Host accepts but cc1 rejects $TEST_FILE"
    exit 1
else
    echo "FAIL: Host rejects but cc1 accepts $TEST_FILE"
    exit 1
fi
