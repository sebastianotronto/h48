#!/bin/sh

printf '\tswitch (t) {\n'
for f in transform_??_???.txt; do
	t="$(echo $f | sed 's/.*_// ; s/\.txt//')"
	printf '\tcase %s:\n\t\treturn inline_trans_%s(c);\n' "$t" "$t"
done
printf '\t}\n'
