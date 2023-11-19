#!/bin/sh

detectarch() { cc -march=native -dM -E - </dev/null | grep "$1"; }
detectsan() { cc -fsanitize=$1 -dM -E -x c - </dev/null | grep "SANITIZE"; }

[ -n "$(detectarch __AVX2__)" ] && detected="AVX2"

TYPE=${TYPE-"$detected"}

echo "Using CUBE_$TYPE"

STD="-std=c99"
WFLAGS="-pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-function"
[ "$TYPE" = "AVX2" ] && AVX="-mavx2"

[ -n "$(detectsan address)" ] && ADDR="-fsanitize=address"
[ -n "$(detectsan undefined)" ] && UNDEF="-fsanitize=undefined"
SAN="$ADDR $UNDEF"

CFLAGS="$STD $WFLAGS $AVX -O3"
DBGFLAGS="$STD $WFLAGS $AVX -g3 -DDEBUG"

{
echo "CUBETYPE = CUBE_$TYPE";
echo "";
echo "CFLAGS = $CFLAGS";
echo "DBGFLAGS = $DBGFLAGS";
echo "";
echo "CC = cc"
} > config.mk
