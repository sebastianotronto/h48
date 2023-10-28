include config.mk

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
