#!/bin/sh

printf '\tswitch (m) {\n'
for f in cubes/move_??_*.txt; do
	t="$(echo $f | sed 's/.*_// ; s/\.txt//')"
	printf '\tcase %s:\n\t\treturn _move(%s, c);\n' "$t" "$t"
done
printf '\t}\n'
