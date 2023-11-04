# Set TYPE="AVX2" manually for advanced vector instructions
CUBETYPE = CUBE_${TYPE}

CFLAGS = -std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -O3 -mavx2
DBGFLAGS = -DDEBUG -std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -g3 -fsanitize=address -fsanitize=undefined -mavx2

CC = cc
all: cube.o debugcube.o

cube.o: clean
	${CC} -D${CUBETYPE} ${CFLAGS} -c -o cube.o src/cube.c

debugcube.o: clean
	${CC} -D${CUBETYPE} ${DBGFLAGS} -c -o debugcube.o src/cube.c

clean:
	rm -rf *.o

test: debugcube.o
	CUBETYPE=${CUBETYPE} ./test/test.sh

benchmark: cube.o
	CUBETYPE=${CUBETYPE} ./benchmark/bench.sh

.PHONY: all clean test benchmark
