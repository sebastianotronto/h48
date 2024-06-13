#!/bin/sh

re="${RUN:-$@}"

CC="cc -std=c99 -pedantic -Wall -Wextra \
    -Wno-unused-parameter -Wno-unused-function -O3 -D$CUBETYPE \
    -D_POSIX_C_SOURCE=199309L"

[ "$CUBETYPE" = "CUBE_AVX2" ] && CC="$CC -mavx2"

BIN="benchmark/run"
RES="benchmark/results"
CUBEOBJ="cube.o"
d="$(date +'%Y-%m-%d-%H-%M-%S')"

mkdir -p "$RES"

for t in benchmark/*; do
	if [ -n "$re" ] && [ -z "$(echo "$t" | grep "$re")" ]; then
		continue
	fi
	if [ ! -d "$t" ] || [ "$t" = "benchmark/results" ]; then continue; fi
	$CC -o $BIN $t/*.c $CUBEOBJ || exit 1;
	$BIN | tee "$RES/results-$d.txt" "$RES/results-last.txt"
done

rm -rf $BIN $CUBEOBJ
