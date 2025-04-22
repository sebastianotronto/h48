# H48: prototype for a new optimal solver and nissy backend

**Warning**: this library is work in progress, breaking changes can
happen without notice.

H48 is an experimental Rubik's cube solver. The main goal is experimenting
with various optimal solving methods and pruning tables, with
[nxopt](https://github.com/rokicki/cube20src/blob/master/nxopt.md) and
[vcube](https://github.com/Voltara/vcube) as inspiration and benchmark
reference.

In the future this project may evolve as a new "back-end" for the classic
[nissy](https://github.com/sebastianotronto/nissy-classic).

## Building

First run the configuration script to detect the system configuration.
This is going to select a C compiler and architecture-specific optimizations.

```
$ ./configure.sh
```

These settings can be overridden, for example:

```
$ THREADS=3 CC=gcc ./configure.sh   # Use 3 threads and compile with gcc
```

All the configuration-time options are described in the `configure.sh` script.

Once the configuration is done, you can build with make

```
$ make
```

## Running tests

This project includes a suite of "unit" test. They can be run with:

```
$ make test
```

To run only a subset of the tests, set the `TEST` variable to a regular
expression that matches only the name of the tests you want to run:

```
$ TEST=coord make test
```

Each subfolder of the test folder contains a test. A test can consist
of multiple test cases (.in files). Running a test means compiling and
running the corresponding test against each test case. When a test case
is run, the .in file is read a the output of the program is compared
to the corresponding .out filei using diff(1). If the two differ, the
difference is printed out and no other test is run.

The results of the last test case run is saved in test/last.out (standard
output, the results compared with the .out files) and test/last.err
(standard error).

Tests are always run in debug mode: this means that optimizations are
disabled and some extra logging is enabled.

See the test folder and test/test.sh for details.

## Running "tools"

In the tools folder there are some small programs that test various
functionality of the H48 library. They work similarly to test, but they
are not run in debug mode by default.

To run a tool you must select it with the environment variable `TOOL`.
For example the command:

```
TOOL=stats make tool
```

Will run the stats_tables_h48 tool.

To pass some arguments to a tool, use the `TOOLARGS` variable:

```
TOOL=gendata TOOLARGS="h48 0;2;20" make tool
```

Like for tests, the value of the `TOOL` variable can be any regular
expression matching the name of the tool. Unlike tests, one and
only one tool will be selected for each run.  The content of the
`TOOLARGS` variable is used directly as command line arguments for
the chosen tool.

Each tool run is automatically timed, so these tools can be used as
benchmark.  The output as well as the time of the run are saved to a
file in the tools/results folder.

To build and run a tool in debug mode, use `make debugtool`.

## User interfaces

User interfaces for this software are in an experimental stage. Some
examples are included in this repository, but they are not polished.
If you are a developer and you are interested in contributing, have a
look at the examples and let me know!

## Command-line interface

The `shell` folder contains the code for a rudimentary shell that can
be used to run commands manually. The user experience in not amazing,
as the commands require quite verbose options.

To build the shell run:

```
$ make shell
```

This will create an executable called `run`.
Optionally, you can run some tests:

```
$ make shelltest
```

Then you can for example get a cube from a sequence of moves:

```
$ ./run frommoves -moves "R' U' F"
JLQWSVUH=ZLCUABGIVTKH=A
```

The cube format is meant to be easy to copy-paste and read for the
software, but not necessarily intuitive for the user. See below for a
detaileed description.

You can also get a random cube

```
$ ./run randomcube
WDSQREVX=VBKYDUCJXWAb=A
```

To solve a cube you can use:

```
$ ./run solve -solver h48h0k4 -n 1 -M 4 -cube "JLQWSVUH=ZLCUABGIVTKH=A"
F' U' R
```

Or alternatively:

```
$ ./run solve_scramble -solver h48h0k4 -n 1 -M 4 -moves "R' U' F"
F' U' R
```

For a full list of available command, use `./run help`.

### QT

A draft for a QT-based GUI is contained in the `qt` folder. In order
to build it you need QT libraries and headers (for QML / QT Quick
development) and CMake. If your environment is set up, you can
build the QT GUI with

```
$ make qt
```

And then run it with

```
$ ./nissyqt
```

## Using this software as a library

This tool has been developed as a library, so it can be easily included
in other programs. For this reason, some bindings for languages other
than C are available.

The API is documented in the public header file `src/nissy.h`.

### C

To use this in a C project, simly include `src/nissy.h`. The tools
in the `tools/` folder are a good example for how to use this.

NOTE: this project is developed using the C11 standard. If you are using
an older version of C, you can write your own header file.

### C++

The `cpp` folder contains a C++ header `nissy.h` (C++20 standard) and an
implementation file `nissy.cpp`. This interface wraps the calls to the
C functions in an object-oriented C++ interface for more convenient use.

The `cpp/examples` folder contains some examples for how to use this
interface. You can build them with e.g.

```
g++ -std=c++20 cpp/nissy.cpp cpp/examples/solve_h48h3k2.cpp nissy.o
```

NOTE: If you prefer to use a C-style API, you'll have to write
your own header, similar to the `extern "C" {}` block at the top of
`cpp/nissy.cpp`. The C header `src/nissy.h` cannot be compiled as C++,
as it uses features of C that are not compatible with it.

### Python

The `python` folder contains a Python module. The API provided by
this module follows the C API quite closely, except its functions
sometimes return strings instead of writing to `char *` buffers.

To build the Python module you need the Python development headers
installed. You can check this from the output of `./configure.sh`:

```
$ ./configure.sh
...
Python3 development libraries: version 3.12
```

Then to build the module:

```
$ make python
```

And to import it

```
$ python                                  # In the main folder
>>> import nissy_python_module as nissy   # In the python shell
```

From here you can call the library functions directly, for example:

```
>>> nissy.compose('NEORSQLH=ZFCYUAGLHTKB=A', 'NEORSQLH=ZFCYUAGLHTKB=A')
'ASTUGFBH=DACXEZGBLIKF=A'
```

The `python/examples` folder contains some examples, that you
can run for example with:

```
$ python python/examples/solve.py
```

You can access the documentation for the Python module from within
a Python interpreter with `help(nissy)`. Cross-check this documentation
with the comments in nissy.h for more details.

NOTE: Support for the Python module is still rudimentary.

## Cube format

This format is a "base 32" encoding of the cube. It is not meant to be
human-readable, but it is compact while still being plain text. Each
piece, including the orientation value, is encoded as a number from 0
to 31, and this number is then converted to an uppercase letter (0-26)
or to a lowercase letter (27-31).

The format looks like this:

```
cccccccc=eeeeeeeeeeee=r
```

Where the first 8 characters represent the corner, the 12 characters
after the first 12 represent the edges and the last character represents
the orientation of the cube with respect to the base orientation.

Edges are numbered as follows (see also constants.h):

```
UF=0 UB=1 DB=2 DF=3 UR=4 UL=5 DL=6 DR=7 FR=8 FL=9 BL=10 BR=11
```

Corners are numbered as follows:

```
UFR=0 UBL=1 DFL=2 DBR=3 UFL=4 UBR=5 DFR=6 DBL=7
```

At the moment, the only orientation supported is the standard one, as wide
moves, rotations and slice moves are not supported.

In this format, the solved cube looks like this:

```
ABCDEFGH=ABCDEFGHIJKL=A
```

The cube after the move F looks like this:

```
MBODSFQH=ZBCYEFGHQTKL=A
```

A cube in B32 format is always 23 characters long (24 if the terminating
null character is included).
