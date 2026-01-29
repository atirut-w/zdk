# ZDK

Z80 development tools (WIP). Currently includes:

- `cc1`: single-pass C90 compiler emitting Z80 GAS syntax
- `cc`: driver that orchestrates `cc1 -> as -> ld` (intended to work with GNU binutils today)

See `TOOLCHAIN.md` for toolchain layout, sysroot conventions, and driver behavior.

## Quick start (host)

Build:

```sh
cmake -S . -B build
cmake --build build -j
```

Compile + link a flat binary (Zeal-style) using GNU binutils in PATH:

```sh
cd cc1/test
../../build/bin/cc zeal.s start.c \
  -Wl,-Ttext=0x4000 -Wl,--oformat=binary \
  -o a.bin
```

## Install (optional)

```sh
cmake --install build --prefix /opt/zdk
```

Then create a sysroot skeleton:

```sh
sh /opt/zdk/cc/mksysroot.sh --prefix /opt/zdk --target z80-unknown-none-elf --name cc
```
