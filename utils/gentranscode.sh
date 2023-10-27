#!/bin/sh

gcc -DDEBUG h48_to_src.c ../src/cube.c -o h48_to_src
gcc -DDEBUG invert.c ../src/cube.c -o invert

for f in transform_??_???.txt; do
	trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
	printf '[%s] = ' "$trans"
	if [ "$1" = "-i" ]; then
		./invert <"$f" | ./h48_to_src
	else
		./h48_to_src <"$f"
	fi
	printf ',\n'
done

rm -f h48_to_src invert
