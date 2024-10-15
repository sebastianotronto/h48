#!/bin/sh

SHELLBIN="./debugrun"
TESTDIR="./shell/testcases"
TESTOUT="shell/lasttest.out"
TESTERR="shell/lasttest.err"

for cin in "$TESTDIR"/*.in; do
	c=$(echo "$cin" | sed 's/\.in//')
	cout="$c.out"
	printf "%s: " "$c"
	(cat "$cin" | xargs "$SHELLBIN") > $TESTOUT 2> $TESTERR
	if diff "$cout" "$TESTOUT"; then
		printf "OK\n"
	else
		printf "Test failed! stderr:\n"
		cat $TESTERR
		exit 1
	fi
done

echo "All tests passed!"
