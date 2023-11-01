#!/bin/sh

gcc -DDEBUG h48_to_src.c ../src/cube.c -o h48_to_src
gcc -DDEBUG invert.c ../src/cube.c -o invert

# Old version
genarray() {
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
}

genfuncs() {
	for f in transform_??_???.txt; do
		trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf 'static inline cube_t\ninline_trans_%s' "$trans"
		[ "$1" = "-i" ] && printf '_inverse'
		printf '(cube_t c)\n{\n'
		printf '\tcube_t ret;\n'
		printf '\tcube_t tn = {\n'
		./h48_to_src <"$f" | sed 's/^/\t/' | \
			tail -n 3 | head -n 2
		printf '\t};\n\tcube_t ti = {\n'
		./invert <"$f" | ./h48_to_src | sed 's/^/\t/' | \
			tail -n 3 | head -n 2
		printf '\t};\n\n'
		printf '\tret = compose(tn, c);\n'
		printf '\tret = compose(ret, ti);\n'
		if [ -n "$(echo "$trans" | grep "m")" ]; then
			printf '\tret = flipallcorners(ret);\n'
		fi
		printf '\n\treturn ret;\n}\n\n'
	done
}

genfuncs
rm -f h48_to_src invert
