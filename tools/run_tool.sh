#!/bin/sh

if [ -z "$TOOL" ]; then
	echo "No tool selected (TOOL variable must be set)"
	exit 1
fi

CC="cc -std=c99 -pedantic -Wall -Wextra \
    -Wno-unused-parameter -Wno-unused-function -D$CUBETYPE \
    -D_POSIX_C_SOURCE=199309L"

if [ -n "$DEBUG" ]; then
	CC="$CC -fsanitize=address -g3"
	CUBEOBJ="debugcube.o"
else
	CC="$CC -O3"
	CUBEOBJ="cube.o"
fi

[ "$CUBETYPE" = "CUBE_AVX2" ] && CC="$CC -mavx2"

BIN="tools/run"
d="$(date +'%Y-%m-%d-%H-%M-%S')"

for t in tools/*; do
	if [ ! -d "$t" ] || [ -z "$(echo "$t" | grep "$TOOL")" ]; then
		continue
	fi
	toolname="$(basename "$t" .c)"
	$CC -o $BIN $t/*.c $CUBEOBJ || exit 1;
	$BIN | tee "tools/results/$toolname-$d.txt"
	break
done

rm -rf $BIN $CUBEOBJ
