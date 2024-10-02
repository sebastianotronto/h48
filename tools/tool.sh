#!/bin/sh

if [ -z "$TOOL" ]; then
	echo "No tool selected (TOOL variable must be set)"
	exit 1
fi

CC="$CC -D_POSIX_C_SOURCE=199309L" # For timer

BIN="tools/run"
d="$(date +'%Y-%m-%d-%H-%M-%S')"

for t in tools/*; do
	if [ ! -d "$t" ] || ! (echo "$t" | grep -q "$TOOL"); then
		continue
	fi
	toolname="$(basename "$t" .c)"
	$CC -o $BIN "$t"/*.c "$OBJ" || exit 1;
	$BIN $TOOLARGS \
		| tee "tools/results/$toolname-$d.txt" "tools/results/last.out"
	break
done
