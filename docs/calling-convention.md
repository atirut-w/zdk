# Calling Convention

!!! warning
    Calling convention may change during development.

The calling convention the ZDK C Compiler uses is loosely based on [`cdecl`](https://en.wikipedia.org/wiki/X86_calling_conventions#cdecl), with some compromises to adapt to the Z80 architecture. Other calling conventions may be supported in the (far) future.

## Register Usage
| Register Pair         | Usage                    |
| :-------------------- | :----------------------- |
| `AF`                  | Caller-saved             |
| `BC`                  | Not used by the compiler |
| `DE`                  | Caller-saved             |
| `HL`                  | Caller-saved             |
| `IX` (Frame Pointer)  | Callee-saved             |
| `IY` (Locals Pointer) | Callee-saved             |

Do note that while `IX` is used as a frame pointer, it also serves second purpose for indexing into arguments, while `IY` is only used for indexing into local variables.

## Caller Side
When a function is called, the caller must push the arguments to the stack right-to-left, and then call the function. The caller is responsible for removing the arguments from the stack after the call.

## Callee Side
The callee have to save the `IX`, `IY`, and other callee-saved registers if they are used, then allocate space for local variables if needed. The callee is responsible for cleaning up the stack frame before returning.

Single-byte return values are returned in the `A` register, while multi-byte return values are returned in `HL` (low word) and `DE` (high word).

### Examples
These example roughly outlines how the compiler generate a function's prologue and epilogue depending on how arguments and locals are used.

#### Callee (No Arguments & Locals)
```asm
    ; Return (char)10
    ld a, 10
    ret
```

#### Callee (No Locals)
```asm
    ; Create new stack frame
    push ix
    ld ix, 0
    add ix, sp

    ; Now you can access arguments using ix
    ld a, (ix+0)

    ; Clean up and return
    pop ix
    ret
```

#### Callee (With Locals)
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
    ld a, (ix+0)
    ; Accessing locals
    ld (iy+0), a

    ; Restore stack pointer, clean up, and return
    ld sp, ix
    pop ix
    pop iy
    ret
```
