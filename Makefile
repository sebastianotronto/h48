# See LICENSE file for copyright and license details.

CFLAGS    = -std=c99 -pthread -pedantic -Wall -Wextra \
	    -Wno-unused-parameter -O3
DBGFLAGS  = -DDEBUG -std=c99 -pthread -pedantic -Wall -Wextra \
            -Wno-unused-parameter -Wno-unused-function -g3 \
	    -fsanitize=address -fsanitize=undefined

CC = cc

all: solve

cube.o: clean
	${CC} ${CFLAGS} -c -o cube.o src/*.c

debugcube.o:
	${CC} ${DBGFLAGS} -c -o debugcube.o src/*.c

clean:
	rm -rf solve

test: debugcube.o
	./test/test.sh

benchmark: cube.o
	./benchmark/bench.sh

.PHONY: all clean test benchmark
