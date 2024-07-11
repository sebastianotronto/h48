#!/bin/sh

re="${TEST:-$@}"

TESTBIN="test/run"
TESTOUT="test/last.out"
TESTERR="test/last.err"
CUBEOBJ="debugcube.o"

for t in test/*; do
	if [ -n "$re" ] && [ -z "$(echo "$t" | grep "$re")" ]; then
		continue
	fi
	if [ ! -d $t ]; then continue; fi
	$CC -o $TESTBIN $t/*.c $CUBEOBJ || exit 1;
	for cin in $t/*.in; do
		c=$(echo "$cin" | sed 's/\.in//')
		cout=$c.out
		printf "$c: "
		$TESTBIN < "$cin" > $TESTOUT 2> $TESTERR
		if diff $cout $TESTOUT; then
			printf "OK\n"
		else
			printf "Test failed! stderr:\n"
			cat $TESTERR
			exit 1
		fi
	done
done

echo "All tests passed!"
rm -rf $TESTBIN $TESTOUT $TESTERR $CUBEOBJ
