#!/bin/sh

re="${TEST:-$@}"

TESTBIN="test/run"
TESTOUT="test/last.out"
TESTERR="test/last.err"

for t in test/*; do
	if [ -n "$re" ] && !(echo "$t" | grep -q "$re"); then
		continue
	fi
	
	# Verify if $t is a directory and if its name starts with three digits
	if [ -d "$t" ] && basename "$t" | grep -Eq '^[0-9]{3}'; then
		$CC -o $TESTBIN "$t"/*.c $OBJ || exit 1
		for cin in "$t"/*.in; do
			c=$(echo "$cin" | sed 's/\.in//')
			cout="$c.out"
			printf "%s: " "$c"
			$TESTBIN < "$cin" > $TESTOUT 2> $TESTERR
			if diff "$cout" "$TESTOUT"; then
				printf "OK\n"
			else
				printf "Test failed! stderr:\n"
				cat $TESTERR
				exit 1
			fi
		done
	fi
done

echo "All tests passed!"
