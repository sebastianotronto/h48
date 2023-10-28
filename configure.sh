#!/bin/sh

CFLAGS="-std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -O3"
DBGFLAGS="-DDEBUG -std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -g3 -fsanitize=address -fsanitize=undefined"

if [ "$TYPE" = "AVX2" ]; then
	CFLAGS="$CFLAGS -mavx2"
	DBGFLAGS="$DBGFLAGS -mavx2"
fi

{
echo "CUBETYPE = CUBE_$TYPE";
echo "";
echo "CFLAGS = $CFLAGS";
echo "DBGFLAGS = $DBGFLAGS";
echo "";
echo "CC = cc"
} > config.mk
