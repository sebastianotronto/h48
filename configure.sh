#!/bin/sh

greparch() {
	$CC -march=native -dM -E - </dev/null 2>/dev/null | grep "$1"
}

grepsan() {
	$CC -fsanitize="$1" -dM -E -x c - </dev/null 2>/dev/null | grep "SANITIZE"
}

detectthreads() {
	echo 16 # TODO: choose based on system
}

detectarch() {
	[ -n "$(greparch __AVX2__)" ] && detected="AVX2"
	[ -n "$(greparch __ARM_NEON)" ] && detected="NEON"
	[ -z "$detected" ] && detected="PORTABLE"

	echo "$detected"
}

validatecc() {
	if ! (command -v "$CC" >/dev/null 2>&1) ; then
		echo "Error: compiler '$CC' not found"
		exit 1
	fi
}

validatethreads() {
	if [ "$THREADS" -le 0 ] || [ "$THREADS" -gt 128 ]; then
		echo "Error: number of threads must be between 1 and 128"
		exit 1
	fi
}

validatearch() {
	case "$ARCH" in
	AVX2|NEON|PORTABLE)
		;;
	*)
		echo "Error: architecture '$ARCH' not supported"
		exit 1
		;;
	esac
}

CC=${CC:-cc}
THREADS=${THREADS-"$(detectthreads)"}
ARCH=${ARCH-"$(detectarch)"}

validatecc
validatethreads
validatearch

STD="-std=c99"
WFLAGS="-pedantic -Wall -Wextra"
WNOFLAGS="-Wno-unused-parameter -Wno-unused-function -Wno-unknown-pragmas"

[ "$ARCH" = "AVX2" ] && AVX="-mavx2"
[ -n "$(grepsan address)" ] && ADDR="-fsanitize=address"
[ -n "$(grepsan undefined)" ] && UNDEF="-fsanitize=undefined"
SAN="$ADDR $UNDEF"
LIBS="-lpthread"

CFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $AVX -O3"
DBGFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $SAN $AVX -g3 -DDEBUG"
MACROS="-DTHREADS=$THREADS -D$ARCH"

echo "Selected architecture: $ARCH"
echo "Number of threads: $THREADS"
echo "Compiler: $CC"

{
echo "ARCH = $ARCH";
echo "";
echo "CFLAGS = $CFLAGS";
echo "DBGFLAGS = $DBGFLAGS";
echo "MACROS = $MACROS"
echo "";
echo "CC = $CC"
} > config.mk
