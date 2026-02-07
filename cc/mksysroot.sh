#!/bin/sh
set -eu

prefix=""
target="z80-unknown-elf"
name="cc"
out=""

usage() {
	cat <<'EOF'
usage: mksysroot.sh --prefix <PREFIX> [--target <triplet>] [--name <toolchain>] [--out <SYSROOT>]

Creates a sysroot skeleton:
  SYSROOT/usr/include
  SYSROOT/usr/lib

Default SYSROOT:
  <PREFIX>/<triplet>/<toolchain>/sysroot
EOF
}

while [ $# -gt 0 ]; do
	case "$1" in
	--prefix)
		shift
		[ $# -gt 0 ] || {
			usage >&2
			exit 2
		}
		prefix="$1"
		;;
	--target)
		shift
		[ $# -gt 0 ] || {
			usage >&2
			exit 2
		}
		target="$1"
		;;
	--name)
		shift
		[ $# -gt 0 ] || {
			usage >&2
			exit 2
		}
		name="$1"
		;;
	--out)
		shift
		[ $# -gt 0 ] || {
			usage >&2
			exit 2
		}
		out="$1"
		;;
	-h | --help)
		usage
		exit 0
		;;
	*)
		echo "mksysroot.sh: unknown argument: $1" >&2
		usage >&2
		exit 2
		;;
	esac
	shift
done

if [ -z "$prefix" ]; then
	echo "mksysroot.sh: missing --prefix" >&2
	usage >&2
	exit 2
fi

if [ -z "$out" ]; then
	out="$prefix/$target/$name/sysroot"
fi

mkdir -p "$out/usr/include" "$out/usr/lib"

echo "created sysroot: $out" >&2
