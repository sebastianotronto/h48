#!/bin/sh

for f in $@; do
	if [ ! -f "$f" ]; then
		echo "File $f does not exist"
		exit 1
	fi
done

for f in $@; do
	while read -r line; do
		#c=$(cat "$f")
		c="$line"
		cc="$(./run-old convert -fin H48 -fout B32 -cubestr "$c" 2>/dev/null)"
		if [ -z "$cc" ]; then
			echo "$line"
		else
			echo "$cc=A"
		fi
	done < "$f" > "$f.new"
	mv "$f.new" "$f"
done
