#!/bin/sh

if [ -z "$TOOL" ]; then
	echo "No tool selected (TOOL variable must be set)"
	exit 1
fi

CC="cc -std=c99 -pedantic -Wall -Wextra \
    -Wno-unused-parameter -Wno-unused-function -O3 -D$CUBETYPE \
    -D_POSIX_C_SOURCE=199309L"

[ "$CUBETYPE" = "CUBE_AVX2" ] && CC="$CC -mavx2"

BIN="tools/run"
CUBEOBJ="cube.o"
d="$(date +'%Y-%m-%d-%H-%M-%S')"

for t in tools/*; do
	if [ ! -d "$t" ] || [ -z "$(echo "$t" | grep "$TOOL")" ]; then
		continue
	fi
	$CC -o $BIN $t/*.c $CUBEOBJ || exit 1;
	$BIN
	break
done

rm -rf $BIN $CUBEOBJ
