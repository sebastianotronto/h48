#!/bin/sh

if [ -z "$TOOL" ]; then
	echo "No tool selected (TOOL variable must be set)"
	exit 1
fi

CC="$CC -D_POSIX_C_SOURCE=199309L" # For timer

BIN="tools/run"
RESULTS="tools/results"
LAST="$RESULTS/last.out"
date="$(date +'%Y-%m-%d-%H-%M-%S')"

for t in tools/*; do
	if [ ! -d "$t" ] || ! (echo "$t" | grep -q "$TOOL"); then
		continue
	fi
	toolname="$(basename "$t" .c)"
	break
done

file="$RESULTS/$toolname-$date.txt"

$CC -o $BIN "$t"/*.c "$OBJ" || exit 1;

(
date +'%Y-%m-%d %H:%M'
echo ""
echo "======== config.mk ========"
cat config.mk
echo "==========================="
echo ""
echo "=== tool configuration ===="
echo "TOOL=$toolname"
echo "TOOLARGS=$TOOLARGS"
echo "CC=$CC"
echo "==========================="
echo ""
echo "======= tool output ======="
$BIN $TOOLARGS 
echo "==========================="
) | tee "$file" "$LAST"
