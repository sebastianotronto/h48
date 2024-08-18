#!/bin/sh

cc -DDEBUG h48_to_lst.c ../src/nissy.c -o h48_to_lst

gen() {
	for f in cubes/move_??_*.txt; do
		move="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf '#define _move_cube_%s fastcube( \\\n    ' "$move"
		./h48_to_lst <"$f"
		printf ')\n'
	done
}

gen
rm -f h48_to_lst invert
