#!/bin/sh

cc -mavx2 utils/detect-immintrin.c && detected="AVX2" && rm a.out
TYPE=${TYPE-"$detected"}

STD="-std=c99"
WFLAGS="-pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-function"
[ "$TYPE" = "AVX2" ] && AVX="-mavx2"
[ "$(uname)" = "Linux" ] && SAN="-fsanitize=address -fsanitize=undefined"

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
