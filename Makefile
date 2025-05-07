include config.mk

all: nissy.o

nissy.s:
	${CC} ${MACROS} ${CFLAGS} -c -S -o nissy.s src/nissy.c

nissy.o:
	${CC} ${MACROS} ${CFLAGS} -c -o nissy.o src/nissy.c

libnissy.a: nissy.o
	ar rcs libnissy.a nissy.o

libnissy.so:
	${CC} ${MACROS} ${CFLAGS} -c -shared -o libnissy.so src/nissy.c

debugnissy.o:
	${CC} ${MACROS} ${DBGFLAGS} -c -o debugnissy.o src/nissy.c

wasmnissy.o:
	${EMCC} ${MACROS} ${WASMFLAGS} -c -o wasmnissy.o src/nissy.c

clean:
	rm -rf *.out *.o *.s *.so run debugrun

test: debugnissy.o
	CC="${CC} ${MACROS} ${DBGFLAGS}" OBJ=debugnissy.o ./test/test.sh

tool: nissy.o
	mkdir -p tables tools/results
	CC="${CC} ${MACROS} ${CFLAGS}" OBJ=nissy.o ./tools/tool.sh

debugtool: debugnissy.o
	mkdir -p tables tools/results
	CC="${CC} ${MACROS} ${DBGFLAGS}" OBJ=debugnissy.o ./tools/tool.sh

shell: nissy.o
	mkdir -p tables
	${CC} ${MACROS} ${CFLAGS} -o run nissy.o shell/shell.c

debugshell: debugnissy.o
	mkdir -p tables
	${CC} ${MACROS} ${DBGFLAGS} -o debugrun debugnissy.o shell/shell.c

python: nissy.o
	${CC} ${CFLAGS} -shared ${PYTHON3_INCLUDES} -o nissy_python_module.so \
		nissy.o python/nissy_module.c

.PHONY: all clean test tool debugtool shell debugshell python
