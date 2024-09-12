include config.mk

all: cube.o debugcube.o

cube.s: clean
	${CC} ${MACROS} ${CFLAGS} -c -S -o cube.s src/nissy.c

cube.o: clean
	${CC} ${MACROS} ${CFLAGS} -c -o cube.o src/nissy.c

debugcube.o: clean
	${CC} ${MACROS} ${DBGFLAGS} -c -o debugcube.o src/nissy.c

clean:
	rm -rf *.o run

test: debugcube.o
	CC="${CC} ${MACROS} ${DBGFLAGS}" ./test/test.sh

tool: cube.o
	mkdir -p tools/results
	CC="${CC} ${MACROS} ${CFLAGS}" CUBEOBJ=cube.o ./tools/run_tool.sh

debugtool: debugcube.o
	mkdir -p tools/results
	CC="${CC} ${MACROS} ${DBGFLAGS}" CUBEOBJ=debugcube.o ./tools/run_tool.sh

shell: cube.o
	mkdir -p tables
	${CC} ${MACROS} ${CFLAGS} -o run cube.o shell.c

debugshell: debugcube.o
	mkdir -p tables
	${CC} ${MACROS} ${DBGFLAGS} -o run debugcube.o shell.c

.PHONY: all clean test tool debugtool shell debugshell
