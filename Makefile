# See LICENSE file for copyright and license details.

include config.mk

CFLAGS    = -std=c99 -pthread -pedantic -Wall -Wextra \
	    -Wno-unused-parameter -O3
DBGFLAGS  = -DDEBUG -std=c99 -pthread -pedantic -Wall -Wextra \
            -Wno-unused-parameter -Wno-unused-function -g3 \
	    -fsanitize=address -fsanitize=undefined

CC = cc

all: cube.o debugcube.o

cube.o: clean
	${CC} -D${CUBETYPE} ${CFLAGS} -c -o cube.o src/cube.c

debugcube.o:
	${CC} -D${CUBETYPE} ${DBGFLAGS} -c -o debugcube.o src/cube.c

clean:
	rm -rf *.o

test: debugcube.o
	./test/test.sh

benchmark: cube.o
	./benchmark/bench.sh

.PHONY: all clean test benchmark
