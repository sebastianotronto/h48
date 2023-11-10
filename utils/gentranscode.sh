#!/bin/sh

type="${1:-src}"

gcc -DDEBUG h48_to_"$type".c ../cube.c -o h48_to_"$type"
gcc -DDEBUG invert.c ../cube.c -o invert

# Old version
genarray() {
	for f in transform_??_???.txt; do
		trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf '[%s] = ' "$trans"
		if [ "$1" = "-i" ]; then
			./invert <"$f" | ./h48_to_"$type"
		else
			./h48_to_"$type" <"$f"
		fi
		printf ',\n'
	done
}

genfuncs() {
	for f in transform_??_???.txt; do
		trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf 'static inline cube_fast_t\n_trans_%s' "$trans"
		printf '(cube_fast_t c)\n{\n'
		printf '\tcube_fast_t ret;\n\n'
		printf '\tcube_fast_t tn = '
		./h48_to_"$type" <"$f" | sed '2,4s/^/\t/'
		printf ';\n\tcube_fast_t ti = '
		./invert <"$f" | ./h48_to_"$type" | sed '2,4 s/^/\t/'
		printf ';\n\n\tret = compose_fast(tn, c);\n'
		printf '\tret = compose_fast(ret, ti);\n'
		if [ -n "$(echo "$trans" | grep "m")" ]; then
			printf '\tret = invertco_fast(ret);\n'
		fi
		printf '\n\treturn ret;\n}\n\n'
	done
}

genfuncs
rm -f h48_to_"$type" invert
