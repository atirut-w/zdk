# Toolchain Notes

Default target triplet: `z80-unknown-none-elf`.

Toolchain name: by default, the `cc` driver uses its own executable name to
pick toolchain directories. This allows multiple toolchains to coexist via
renaming/symlinking the driver.

## Layout

- `PREFIX/bin/cc`: driver
- `PREFIX/libexec/<cc-name>/cc1`: C compiler proper (preferred)
- `PREFIX/libexec/zdk/cc1`: fallback legacy location
- `PREFIX/bin/as`, `PREFIX/bin/ld`: (future) ZDK binutils-style tools
- `PREFIX/bin/z80-unknown-none-elf-as`, `PREFIX/bin/z80-unknown-none-elf-ld`: GNU binutils (optional)
- `PREFIX/z80-unknown-none-elf/<cc-name>/sysroot/`: target sysroot (preferred)
- `PREFIX/z80-unknown-none-elf/sysroot/`: fallback

## Driver goals

- Unix-y, target-agnostic behavior.
- Pass linker flags via `-Wl,ARG[,ARG...]` (including `--oformat=binary`).
- Default sysroot: `PREFIX/<triplet>/<cc-name>/sysroot` when present, else
  `PREFIX/<triplet>/sysroot`.
- Tool selection:
  - Prefer tools next to the driver, then `PREFIX/bin/as|ld`, then `PREFIX/bin/<tool-prefix>as|ld`.
  - Allow overrides via `--tool-prefix=` and env `ZDK_CC1/ZDK_AS/ZDK_LD`.

## Sysroot builder

Use the POSIX `sh` script: `cc/mksysroot.sh`.

Creates the directory skeleton:

- `SYSROOT/usr/include`
- `SYSROOT/usr/lib`

## Getting Running (Host)

Prereqs:

- GNU binutils for Z80 in `PATH` (at minimum `z80-unknown-none-elf-as` and `z80-unknown-none-elf-ld`)

Build:

```sh
cmake -S . -B build
cmake --build build -j
```

Run without installing:

- `build/bin/cc` will try, in order:
  - tools next to itself
  - `PREFIX/bin/as|ld` (future ZDK tools)
  - `z80-unknown-none-elf-as|ld` via `--tool-prefix` (default)

Example (Zeal flat binary):

```sh
cd cc1/test
../../build/bin/cc zeal.s start.c \
  -Wl,-Ttext=0x4000 -Wl,--oformat=binary \
  -o a.bin
```

Notes:

- `cc` is OS-neutral; Zeal/FUZIX behaviors are driven via `-Wl,...` flags and sysroot contents.
- No preprocessor stage yet (`-E` not implemented).

## Install + Sysroot

Install:

```sh
cmake --install build --prefix /opt/zdk
```

Create a named sysroot skeleton (matches the `cc` executable name):

```sh
sh /opt/zdk/cc/mksysroot.sh --prefix /opt/zdk --target z80-unknown-none-elf --name cc
```

The driver will prefer:

- `PREFIX/z80-unknown-none-elf/cc/sysroot` (if present)
- then `PREFIX/z80-unknown-none-elf/sysroot`
