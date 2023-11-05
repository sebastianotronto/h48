include config.mk

all: cube.o debugcube.o

cube.o: clean
	${CC} -D${CUBETYPE} ${CFLAGS} -c -o cube.o cube.c

debugcube.o: clean
	${CC} -D${CUBETYPE} ${DBGFLAGS} -c -o debugcube.o cube.c

clean:
	rm -rf *.o

test: debugcube.o
	CUBETYPE=${CUBETYPE} ./test/test.sh

benchmark: cube.o
	CUBETYPE=${CUBETYPE} ./benchmark/bench.sh

.PHONY: all clean test benchmark
