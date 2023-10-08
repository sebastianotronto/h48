#!/bin/sh

CC="cc -DDEBUG -std=c99 -pthread -pedantic -Wall -Wextra \
    -Wno-unused-parameter -Wno-unused-function -g3"
if [ $(uname) != "OpenBSD" ]; then
	CC="$CC -fsanitize=address -fsanitize=undefined"
fi
SRC="src/cube_array.c"
TESTBIN="test/run"
TESTOUT="test/last.out"
TESTERR="test/last.err"
CUBEOBJ="test/cube.o"

$CC -c $SRC -o $CUBEOBJ || exit 1

for t in test/*; do
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
