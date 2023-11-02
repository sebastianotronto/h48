#!/bin/sh

type="${1:-src}"

gcc -DDEBUG h48_to_"$type".c ../src/cube.c -o h48_to_"$type"

genfuncs() {
	for f in move_??_*.txt; do
		move="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf 'static inline cube_t\ninline_move_%s' "$move"
		printf '(cube_t c)\n{\n'
		printf '\tcube_t m = '
		./h48_to_"$type" <"$f" | sed '2,4s/^/\t/'
		printf ';\n\n\treturn compose(c, m);\n}\n\n'
	done
}

genfuncs
rm -f h48_to_"$type" invert
