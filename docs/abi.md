# ABI Specification

This document describes the Application Binary Interface (ABI) for ZDK, a collection of tools, compilers, and libraries for Z80 development.

## Calling Convention

### Register Usage

- **A**: Scratch register, used for 8-bit return values
- **HL**: Scratch register, used for 16-bit return values
- **IX**: Frame pointer (callee-saved for non-leaf functions)
- **All other registers**: Caller-saved (BC, DE, IY, shadow registers)

### Argument Passing

Arguments are passed on the stack in right-to-left order (C calling convention). The caller is responsible for cleaning up the stack after the function returns.

- 8-bit values are pushed as 16-bit values with the high byte undefined
- 16-bit values are pushed as-is
- Arguments appear on the stack in left-to-right order relative to higher memory addresses

**Example**: For `func(a, b, c)`, the stack layout after the call is:
```
[saved IX (low byte)]         <- SP
[saved IX (high byte)]
[return address (low byte)]
[return address (high byte)]
[argument a (low byte)]
[argument a (high byte)]
[argument b (low byte)]
[argument b (high byte)]
[argument c (low byte)]
[argument c (high byte)]      <- highest address
```

### Return Values

- **8-bit values**: Returned in register A
- **16-bit values**: Returned in register HL
- **Structures**: An implicit pointer to caller-allocated storage is passed as the first argument. The function writes the result to this location and the caller receives the structure by value semantically.

**Example**: `Struct myFunc(int x, int y)` becomes `void myFunc(Struct *ret, int x, int y)` at the ABI level.

### Variadic Functions

Variadic functions follow C semantics:
- All fixed arguments are passed normally on the stack
- Variable arguments follow the fixed arguments on the stack in right-to-left order
- The callee does not know the number of variadic arguments passed
- The caller is responsible for determining argument count through format strings or other mechanisms

## Stack Frame

### Stack Properties

- The stack grows downward (toward lower memory addresses)
- Stack alignment is not required
- The stack pointer (SP) points to the last occupied stack location

### Frame Pointer Usage

**Leaf functions** (functions that do not call other functions) may omit the frame pointer setup and access arguments directly via SP offsets.

**Non-leaf functions** must save and restore IX to allow stack unwinding:

```asm
    push ix
    ld ix, 0
    add ix, sp
```

After this prologue, arguments can be accessed via positive offsets from IX, and local variables via negative offsets from IX.

### Local Variables

To allocate space for local variables, adjust the stack pointer downward:

```asm
    ld ix, 0
    add ix, sp
    ld hl, -N      ; N = total bytes for locals
    add hl, sp
    ld sp, hl
```

Local variables are accessed via negative offsets from IX:
```asm
    ld (ix-2), a   ; access first local byte
    ld (ix-1), a   ; access second local byte
```

### Function Epilogue

Before returning, restore the stack pointer and IX register:

```asm
    ld sp, ix
    pop ix
    ret
```

### Large Stack Frames

If the total size of local variables exceeds the indexing range of `(ix-n)` (i.e., beyond -128 to +127 bytes), the function implementation must choose an appropriate strategy to access far variables or arguments. Possible approaches include:
- Using additional registers as secondary pointers
- Computing effective addresses into HL and using indirect access
- Reorganizing frequently-accessed variables within the indexable range

## Caller Responsibilities

1. Push arguments onto the stack in right-to-left order
2. Call the function
3. Clean up arguments from the stack after return (adjust SP)
4. Save any caller-saved registers (BC, DE, HL, A, IY) if their values must be preserved across the call

## Callee Responsibilities

1. Save IX if the function is non-leaf
2. Set up stack frame if needed (for non-leaf functions or when local variables are used)
3. Allocate space for local variables
4. Execute function body
5. Place return value in A (8-bit) or HL (16-bit)
6. Restore stack pointer and IX before returning
7. Return using `ret` instruction
