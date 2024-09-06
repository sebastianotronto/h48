#!/bin/sh

cc=${CC:-cc}

detectarch() {
	${cc} -march=native -dM -E - </dev/null 2>/dev/null | grep "$1"
}
detectsan() {
	${cc} -fsanitize=$1 -dM -E -x c - </dev/null 2>/dev/null \
		| grep "SANITIZE"
}

[ -n "$(detectarch __AVX2__)" ] && detected="AVX2"
[ -n "$(detectarch __ARM_NEON)" ] && detected="NEON"
[ -z "$detected" ] && detected="PORTABLE"

ARCH=${ARCH-"$detected"}

case "$ARCH" in
AVX2|NEON|PORTABLE)
	;;
*)
	echo "Error: architecture $ARCH not supported"
	exit 1
	;;
esac

STD="-std=c99"
WFLAGS="-pedantic -Wall -Wextra"
WNOFLAGS="-Wno-unused-parameter -Wno-unused-function -Wno-unknown-pragmas"

[ "$ARCH" = "AVX2" ] && AVX="-mavx2"
[ -n "$(detectsan address)" ] && ADDR="-fsanitize=address"
[ -n "$(detectsan undefined)" ] && UNDEF="-fsanitize=undefined"
SAN="$ADDR $UNDEF"
LIBS="-lpthread"

CFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $AVX -O3"
DBGFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $SAN $AVX -g3 -DDEBUG"

echo "Selected architecture: $ARCH"
echo "Compiler: ${CC:-cc}"

{
echo "ARCH = $ARCH";
echo "";
echo "CFLAGS = $CFLAGS";
echo "DBGFLAGS = $DBGFLAGS";
echo "";
echo "CC = ${cc}"
} > config.mk
