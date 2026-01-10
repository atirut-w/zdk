# AI Coding Agent Guide for ZDK

This repository is a CMake-based toolkit for Z80 development. Today, the only shipped component is a header-only library for the Z80 object file format. Keep guidance scoped to what’s present in this repo.

## Big Picture
- Purpose: foundational types/utilities for Z80 object files.
- Components: 
  - libzobj: header-only interface library exposing the `zobj` target with on-disk structure definitions in [libzobj/include/zobj.h](libzobj/include/zobj.h).
- Build system: CMake (3.22+), global C flags enforce strict C90 and warnings via [CMakeLists.txt](CMakeLists.txt).
- Format origin: derived from FreeBSD a.out (see https://man.freebsd.org/cgi/man.cgi?query=a.out); several field widths are reduced for Z80, but enum values and flags retain the same semantics as the original format.

## Build & Test
- Configure and build:
  - `cmake -S . -B build`
  - `cmake --build build -j`
- Targets:
  - `zobj` (INTERFACE): provides include path to `libzobj/include`.
- Output dirs: runtime output is configured to `build/bin` (useful when adding executables).
- Tests: `include(CTest)` is enabled in [CMakeLists.txt](CMakeLists.txt). If tests exist, run from build dir with `ctest`. No tests are currently defined in the source tree.

## Project Conventions
- Language: C90 with strict flags (`-std=c90 -pedantic -pedantic-errors -Wall -Wextra`) set globally in [CMakeLists.txt](CMakeLists.txt).
- Headers: fixed-width integer types from `<stdint.h>` and traditional header guards (see [libzobj/include/zobj.h](libzobj/include/zobj.h)).
- Library exposure: header-only via `add_library(zobj INTERFACE)` and `target_include_directories(zobj INTERFACE include)` in [libzobj/CMakeLists.txt](libzobj/CMakeLists.txt).
- Directory layout: public headers under `libzobj/include`.

## libzobj: API Essentials
- `ZObjExec`: on-disk file header
  - Fields: `magic`, `text`, `data`, `bss`, `syms`, `entry`, `trsize`, `drsize`.
  - Magic constants: `ZOBJ_OMAGIC`, `ZOBJ_NMAGIC`.
- `ZObjReloc`: relocation entry
  - Fields: `address`, `symbolnum`, `info`.
  - Flags: `ZOBJ_RELOC_PCREL`, `ZOBJ_RELOC_EXTERN`, `ZOBJ_RELOC_BASEREL`, `ZOBJ_RELOC_JMPTABLE`, `ZOBJ_RELOC_RELATIVE`, `ZOBJ_RELOC_COPY`.
  - Length mask: `ZOBJ_RELOC_LENGTH_MASK` with sizes `BYTE`, `WORD`, `LONG`.
- `ZObjSymbol`: symbol table entry
  - Name union: `un.name` (pointer) or `un.strx` (string table index).
  - Flags/types: `ZOBJ_SYM_UNDF`, `ABS`, `TEXT`, `DATA`, `BSS`, `FN`, `EXT`.
- `ZObjStrTab`: string table (`size`, `data`).

## Object File Layout
- Section order (a.out-style, Z80-sized fields):
  1. `ZObjExec` header
  2. Text segment: `hdr.text` bytes
  3. Data segment: `hdr.data` bytes
  4. Text relocation: `hdr.trsize` bytes (multiple of `sizeof(ZObjReloc)`)
  5. Data relocation: `hdr.drsize` bytes (multiple of `sizeof(ZObjReloc)`)
  6. Symbol table: `hdr.syms` bytes (multiple of `sizeof(ZObjSymbol)`)
  7. String table: `ZObjStrTab` — first a 16-bit `size`, followed by `size - sizeof(uint16_t)` bytes of data

- Offsets (no implicit padding unless producer adds it):
  - `off_text = sizeof(ZObjExec)`
  - `off_data = off_text + hdr.text`
  - `off_tr  = off_data + hdr.data`
  - `off_dr  = off_tr + hdr.trsize`
  - `off_sym = off_dr + hdr.drsize`
  - `off_str = off_sym + hdr.syms`

- Record sizes for readers/writers:
  - `sizeof(ZObjExec) = 16` bytes (8 x `uint16_t`)
  - `sizeof(ZObjReloc) = 4` bytes (`uint16_t` + `uint8_t` + `uint8_t`)
  - `sizeof(ZObjSymbol) = 8` bytes (2 + 1 + 1 + 2 + 2 via layout in [libzobj/include/zobj.h](libzobj/include/zobj.h))
  - `ZObjStrTab.size` is a 16-bit count including the `size` field itself (a.out-compatible pattern, width-reduced)

- Notes:
  - Magic: `ZOBJ_OMAGIC`/`ZOBJ_NMAGIC` mirror a.out semantics; field widths are reduced for Z80 but meanings are the same.
  - Relocation `info` encodes length via `ZOBJ_RELOC_LENGTH_MASK`; extract with `(info & ZOBJ_RELOC_LENGTH_MASK) >> 1`.
  - Treat endianness explicitly when reading/writing on-disk values; don’t assume host endianness matches file.

## Patterns & Guidance for Agents
- Treat structs in [libzobj/include/zobj.h](libzobj/include/zobj.h) as POD representations of on-disk records; when reading from a byte stream, handle endianness and alignment explicitly rather than assuming `memcpy` into the struct is always safe.
- Mirror original a.out semantics when interpreting flags, types, and relocation info; consult the a.out man page for meaning, using `zobj` names as Z80-sized analogs.
- Keep additions C90-compliant and warning-free under the global flags.
- When adding code:
  - Place new headers in `libzobj/include` (or create a new subdir with its own `CMakeLists.txt` and add it via `add_subdirectory(...)`).
  - Expose headers with an INTERFACE target and `target_include_directories` if the component is header-only.
  - Respect the configured `CMAKE_RUNTIME_OUTPUT_DIRECTORY` (`build/bin`) if you introduce executables.

## Quick Examples
- Check object magic:
  - C: `if (hdr.magic != ZOBJ_OMAGIC && hdr.magic != ZOBJ_NMAGIC) /* invalid */;`
- Decode relocation length from `info`:
  - C: `uint8_t len = (rel.info & ZOBJ_RELOC_LENGTH_MASK) >> 1; /* 0=BYTE,1=WORD,2=LONG */`

## Where to Look
- Build setup and flags: [CMakeLists.txt](CMakeLists.txt)
- Public API definitions: [libzobj/include/zobj.h](libzobj/include/zobj.h)
- lib target wiring: [libzobj/CMakeLists.txt](libzobj/CMakeLists.txt)

If parts of the architecture (e.g., assembler/linker tools) should be part of this repo, please point to their source locations so we can extend these instructions accordingly.
