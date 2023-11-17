#!/bin/sh

type="${1:-src}"

gcc -DDEBUG h48_to_"$type".c ../cube.c -o h48_to_"$type"

lineavx() { printf '#define _move_cube_%s ' "$1"; }
linesrc() { printf '_static cube_fast_t _move_cube_%s = ' "$1"; }
sedavx() { sed '1,2s/$/ \\/ ; 3s/$/)/ ; 3q'; }
sedsrc() { sed '3s/$/ };/ ; 3q'; }

gen() {
	for f in move_??_*.txt; do
		move="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		line$type "$move"
		./h48_to_"$type" <"$f" | sed$type
	done
}

gen
rm -f h48_to_"$type" invert
