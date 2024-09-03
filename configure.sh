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

TYPE=${TYPE-"$detected"}

STD="-std=c99"
WFLAGS="-pedantic -Wall -Wextra"
# -Wstringop-overflow seems to be causing problems when combined with -O3
# Someone else complained here: https://access.redhat.com/solutions/6755371
WNOFLAGS="-Wno-unused-parameter -Wno-unused-function -Wno-stringop-overflow"
[ "$TYPE" = "AVX2" ] && AVX="-mavx2"
[ -n "$(detectsan address)" ] && ADDR="-fsanitize=address"
[ -n "$(detectsan undefined)" ] && UNDEF="-fsanitize=undefined"
SAN="$ADDR $UNDEF"
LIBS="-lpthread"

CFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $AVX -O3"
DBGFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $SAN $AVX -g3 -DDEBUG"

echo "Cube type: CUBE_$TYPE"
echo "Compiler: ${CC:-cc}"

{
echo "CUBETYPE = CUBE_$TYPE";
echo "";
echo "CFLAGS = $CFLAGS";
echo "DBGFLAGS = $DBGFLAGS";
echo "";
echo "CC = ${cc}"
} > config.mk
