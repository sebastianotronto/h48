# See LICENSE file for copyright and license details.

CFLAGS    = -std=c99 -pthread -pedantic -Wall -Wextra \
	    -Wno-unused-parameter -O3
DBGFLAGS  = -std=c99 -pthread -pedantic -Wall -Wextra \
            -Wno-unused-parameter -Wno-unused-function -g3 \
	    -fsanitize=address -fsanitize=undefined

CC = cc

all: solve

solve: clean
	${CC} ${CFLAGS} -o solve src/*.c

debug:
	${CC} ${DBGFLAGS} -o solve src/*.c

clean:
	rm -rf solve

test:
	./test/test.sh

.PHONY: all debug clean test
