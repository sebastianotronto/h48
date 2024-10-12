include config.mk

all: nissy.o

nissy.s:
	${CC} ${MACROS} ${CFLAGS} -c -S -o nissy.s src/nissy.c

nissy.o:
	${CC} ${MACROS} ${CFLAGS} -c -o nissy.o src/nissy.c

nissy.so:
	${CC} ${MACROS} ${CFLAGS} -c -shared -o nissy.so src/nissy.c

debugnissy.o:
	${CC} ${MACROS} ${DBGFLAGS} -c -o debugnissy.o src/nissy.c

clean:
	rm -rf *.o *.so run

test: debugnissy.o
	CC="${CC} ${MACROS} ${DBGFLAGS}" OBJ=debugnissy.o ./test/test.sh

tool: nissy.o
	mkdir -p tools/results
	CC="${CC} ${MACROS} ${CFLAGS}" OBJ=nissy.o ./tools/tool.sh

debugtool: debugnissy.o
	mkdir -p tools/results
	CC="${CC} ${MACROS} ${DBGFLAGS}" OBJ=debugnissy.o ./tools/tool.sh

shell: nissy.o
	mkdir -p tables
	${CC} ${MACROS} ${CFLAGS} -o run nissy.o shell/shell.c

debugshell: debugnissy.o
	mkdir -p tables
	${CC} ${MACROS} ${DBGFLAGS} -o run debugnissy.o shell/shell.c

shelltest: debugshell
	./shell/test.sh

.PHONY: all clean test tool debugtool shell debugshell shelltest
