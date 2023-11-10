#!/bin/sh

CC="cc -std=c99 -O3 -D$CUBETYPE"
if [ "$CUBETYPE" = "CUBE_AVX2" ]; then
        CC="$CC -mavx2"
fi

BENCHBIN="benchmark/run"
BENCHDIR="benchmark/results"
CUBEOBJ="cube.o"

$CC -D_POSIX_C_SOURCE=199309L -o $BENCHBIN benchmark/bench.c $CUBEOBJ || exit 1

d="$(date +'%Y-%m-%d-%H-%M-%S')" 
mkdir -p "$BENCHDIR"
$BENCHBIN | tee "$BENCHDIR/results-$d.txt" "$BENCHDIR/results.txt"

echo ""
echo "Results saved to $BENCHDIR/results.txt"

rm -rf $BENCHBIN $CUBEOBJ
