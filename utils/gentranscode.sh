#!/bin/sh

cc -DDEBUG h48_to_lst.c ../src/nissy.c -o h48_to_lst
cc -DDEBUG invert.c ../src/nissy.c -o invert

lineavx() { printf '#define _trans_cube_%s ' "$1"; }
linesrc() { printf '_static cube_fast_t _trans_cube_%s = ' "$1"; }
sedavx() { sed '1,2s/$/ \\/ ; 3s/$/)/ ; 3q'; }
sedsrc() { sed '3s/$/ };/ ; 3q'; }

gen() {
	for f in cubes/transform_??_???.txt; do
		trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf '#define _trans_cube_%s fastcube( \\\n    ' "$trans"
		./h48_to_lst <"$f"
		printf ')\n'
		printf '#define _trans_cube_%s_inverse fastcube( \\\n    ' \
			"$trans"
		./invert <"$f" | ./h48_to_lst
		printf ')\n'
	done
}

gen
rm -f h48_to_lst invert
