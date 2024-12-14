#!/bin/sh

# The following environment variables can be used to configure the build:
#
# CC="compiler"
# Specify the compiler to use.
# By default, cc will be used.
# The string "compiler" must be the name of an executable in $PATH.
#
# ARCH="architecture"
# You can use this variable to build for a different architecture, for example
# if you want to cross-compile or to use the portable version.
# By default, the build script will detect which architecture it is running on.
# The string "architecture" must be one of "AVX2", "NEON" or "PORTABLE".
#
# THREADS=n
# The maximum number of threads to use for multi-threaded operations.
# This is also used as default value in case an operation allows
# specifying how many threads to use.
# By default, 8 threads will be used (TODO: in the future this will be
# determined base on the system).
# The number n must be between 1 and 128.
#
# SANITIZE="option1,option2,..."
# Add the options "-fsanitize=option1", "-fsanitize=option2", ... to the
# compilation command when compiling in debug mode.
# By default, "-fsanitize=address" and "-fsanitize=undefined" will be used.
# If this variable is set, the default is overridden. No check is performed
# on the availability of any sanitizer used, make sure the ones you use are
# available on your system.
#
# Examples
#
# 1. Build using clang and 4 threads
#    CC=clang THREADS=4 ./configure.sh && make
#
# 2. Build using thread and undefined behavior sanitizers when in debug mode
#    SANITIZE="thread,undefined" ./configure.sh && make

greparch() {
	$CC -march=native -dM -E - </dev/null 2>/dev/null | grep "$1"
}

detectthreads() {
	echo 8 # TODO: choose based on system
}

detectarch() {
	[ -n "$(greparch __AVX2__)" ] && detected="AVX2"
	[ -n "$(greparch __ARM_NEON)" ] && detected="NEON"
	[ -z "$detected" ] && detected="PORTABLE"

	echo "$detected"
}

validatecc() {
	if ! (command -v "$CC" >/dev/null 2>&1) ; then
		echo "Error: compiler '$CC' not found"
		exit 1
	fi
}

validatethreads() {
	if [ "$THREADS" -le 0 ] || [ "$THREADS" -gt 128 ]; then
		echo "Error: number of threads must be between 1 and 128"
		exit 1
	fi
}

validatearch() {
	case "$ARCH" in
	AVX2|NEON|PORTABLE)
		;;
	*)
		echo "Error: architecture '$ARCH' not supported"
		echo "Supported architectures: AVX2, NEON, PORTABLE"
		exit 1
		;;
	esac
}

CC=${CC:-cc}
THREADS=${THREADS-"$(detectthreads)"}
ARCH=${ARCH-"$(detectarch)"}

validatecc
validatethreads
validatearch

STD="-std=c11"
WFLAGS="-pedantic -Wall -Wextra -Werror"
WNOFLAGS="-Wno-unused-parameter -Wno-unused-function -Wno-unknown-pragmas"
WNOFLAGS="$WNOFLAGS -Wno-unused-command-line-argument"

[ "$ARCH" = "AVX2" ] && AVX="-mavx2"

if [ -n "$SANITIZE" ]; then
	# Use the user-specified comma-separated sanitizers
	for san in $(echo "$SANITIZE" | tr ',' '\n'); do
		SAN="$SAN -fsanitize=$san"
	done
else
	SAN="-fsanitize=address -fsanitize=undefined"
fi
LIBS="-lpthread"

CFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $AVX -O3 -fPIC"
DBGFLAGS="$STD $LIBS $WFLAGS $WNOFLAGS $SAN $AVX -g3 -DDEBUG -fPIC"
MACROS="-DTHREADS=$THREADS -D$ARCH"

if (command -v "python3-config" >/dev/null 2>&1) ; then
	PYTHON3_INCLUDES="$(python3-config --includes)"
	PYTHON3="version $(echo "$PYTHON3_INCLUDES" | sed 's/.*3\./3./')"
else
	PYTHON3_INCLUDES=""
	PYTHON3="Not found, Python shell won't be available"
fi

echo "Compiler: $CC"
echo "Selected architecture: $ARCH"
echo "Number of threads: $THREADS"
echo "Sanitizer options (debug build only): $SAN"
echo "Python3 development libraries: $PYTHON3"

{
echo "ARCH = $ARCH";
echo "";
echo "CFLAGS = $CFLAGS";
echo "";
echo "DBGFLAGS = $DBGFLAGS";
echo "";
echo "MACROS = $MACROS"
echo "";
echo "PYTHON3_INCLUDES = $PYTHON3_INCLUDES"
echo "CC = $CC"
} > config.mk
