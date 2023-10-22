#!/bin/sh

gcc -DDEBUG h48_to_src.c ../src/cube.c

for f in transform_??_???.txt; do
	trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
	printf '[%s] = ' "$trans"
	./a.out <"$f"
	printf ',\n'
done

rm a.out
