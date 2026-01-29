# cc1

Single-pass C90 compiler front-end targeting Z80, emitting GNU as (GAS) syntax.

Current state: bring-up. Supports enough C to compile `cc1/test/start.c`.

## ABI (current)

- External symbol names: leading underscore (e.g. `start` -> `_start`).
- Stack args: pushed right-to-left; each argument occupies a 16-bit stack slot.
  - `char` is 8-bit; for `char` params the caller places the value in `L` and
    pushes `HL` with `H` intentionally left undefined (keeps stack aligned).
- Return:
  - `char` in `A`
  - `int`/pointer in `HL`
- Prologue/epilogue:
  - `push ix; ld ix,0; add ix,sp` ... `ld sp,ix; pop ix; ret`

## Layout

- `cc1/include/cc1/*.h`: public headers
- `cc1/src/*`: implementation

## Usage

`cc1 <input.c> [-o <out.s>]`
