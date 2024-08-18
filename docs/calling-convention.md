# Calling Convention

!!! warning
    Calling convention may change during development.

The calling convention the ZDK C Compiler uses is loosely based on [`cdecl`](https://en.wikipedia.org/wiki/X86_calling_conventions#cdecl), with some compromises to adapt to the Z80 architecture. Other calling conventions may be supported in the (far) future.

- Arguments are passed right-to-left.
- The caller is responsible for cleaning up the stack after the call.
- The callee is responsible for saving and restoring the `IX` (frame pointer) and `IY` (locals pointer) registers, if used.

Registers other than `IX` and `IY` are caller-saved, meaning the callee can modify them without saving them.

Do note that while `IX` is used as a frame pointer, it also serves second purpose for indexing into arguments, while `IY` is only used for indexing into local variables.

## Examples
These example roughly outlines how the compiler generate a function's prologue and epilogue depending on how arguments and locals are used.

### Callee (No Arguments & Locals)
```asm
    ; Return (char)10
    ld a, 10
    ret
```

### Callee (No Locals)
```asm
    ; Create new stack frame
    push ix
    ld ix, 0
    add ix, sp

    ; Now you can access arguments using ix
    ld a, (ix+2)

    ; Clean up and return
    pop ix
    ret
```

### Callee (With Locals)
```asm
    ; Save old locals pointer
    push iy
    ; We need to save the stack frame, because allocating locals will require saving where it was into ix
    push ix
    ld ix, 0
    add ix, sp
    ; Allocate locals
    ld iy, -size
    add iy, sp
    ld sp, iy

    ; Accessing arguments
    ld a, (ix+4)
    ; Accessing locals
    ld (iy+0), a

    ; Restore stack pointer, clean up, and return
    ld sp, ix
    pop ix
    pop iy
    ret
```
