#!/bin/sh

printf '\tswitch (t) {\n'
for f in transform_??_???.txt; do
	t="$(echo $f | sed 's/.*_// ; s/\.txt//')"
	mirror_or_rotation="$(echo $t | grep m)"
	if [ -z "$mirror_or_rotation" ]; then
		m="rotation"
	else
		m="mirrored"
	fi
	printf '\tcase %s:\n\t\treturn _trans_%s(%s, c);\n' "$t" "$m" "$t"
done
printf '\t}\n'
