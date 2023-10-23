#!/bin/sh

BENCHBIN="benchmark/run"
BENCHOUT="benchmark/results.txt"
CUBEOBJ="cube.o"

cc -std=c99 -pthread -O3 -o $BENCHBIN benchmark/bench.c $CUBEOBJ || exit 1;

$BENCHBIN | tee $BENCHOUT

echo ""
echo "Results saved to $BENCHOUT"

rm -rf $BENCHBIN $CUBEOBJ
