#!/bin/sh

if [ -z "$TOOL" ]; then
	echo "No tool selected (TOOL variable must be set)"
	exit 1
fi

CC="$CC -D_POSIX_C_SOURCE=199309L"

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
