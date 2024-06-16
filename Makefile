include config.mk

all: cube.o debugcube.o

cube.s: clean
	${CC} -D${CUBETYPE} ${CFLAGS} -c -S -o cube.s src/cube.c

cube.o: clean
	${CC} -D${CUBETYPE} ${CFLAGS} -c -o cube.o src/cube.c

debugcube.o: clean
	${CC} -D${CUBETYPE} ${DBGFLAGS} -c -o debugcube.o src/cube.c

clean:
	rm -rf *.o run

test: debugcube.o
	CUBETYPE=${CUBETYPE} TEST=${TEST} ./test/test.sh

benchmark: cube.o
	CUBETYPE=${CUBETYPE} ./benchmark/benchmark.sh

shell: cube.o
	mkdir -p tables
	${CC} ${CFLAGS} -o run cube.o shell.c

.PHONY: all clean test benchmark shell
