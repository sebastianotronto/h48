#!/bin/sh

type="${1:-src}"

gcc -DDEBUG h48_to_"$type".c ../cube.c -o h48_to_"$type"
gcc -DDEBUG invert.c ../cube.c -o invert

lineavx() { printf '#define _trans_cube_%s ' "$1"; }
linesrc() { printf '_static cube_fast_t _trans_cube_%s = ' "$1"; }
sedavx() { sed '1,2s/$/ \\/ ; 3s/$/)/ ; 3q'; }
sedsrc() { sed '3s/$/ };/ ; 3q'; }

gen() {
	for f in transform_??_???.txt; do
		trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		line$type "$trans"
		./h48_to_"$type" <"$f" | sed$type
		line$type "${trans}_inverse"
		./invert <"$f" | ./h48_to_"$type" | sed$type
	done
}

gen
rm -f h48_to_"$type" invert
