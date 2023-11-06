# Prototype for a new optimal solver

Work in progress. There is some documentation at the bottom of this page,
but do not believe it. Everything is in a state of flux and can change
without notice.

## Building and running tests

First run

```
$ ./configure.sh
```

or `TYPE=AVX2 ./configure.sh` if you want to use AVX2 instructions.

Then

```
$ make test
```

to run the tests. You can also run only the tests that match a chosen
regex, for example:

```
$ TEST=coord make test
```

You can also run

```
$ make benchmark
```

for benchmarks.

## TODO:

### Simple solver

* tests
* finish implementation
* benchmarks

### Coordinates

* [done] eo
* co
* ep
* epsep
* cp
* cpsep
* cphtr

What about symcoord?

### Solving

All solving functions take a cube and some parameters as input.

* Depth [uint, <= 20]: all solvers work at fixed depth. The caller
  implementation can implement an A* search.
* max [int]: the maximum number of solutions to find. Set to a negative
  value for all solutions.
* sol [move_t *]: the array for returning the solutions. The caller
  should make sure that it can hold at least max * depth values.
* Table [uint8_t *]: table with all the necessare pre-computed info.
  The table can be generated with a companion function, but reading
  from and writing to file is delegated to the caller implementation.

Implement the following solvers:
* Slow: basic solver without any table.
* H48: one-bit-per-entry table + fallback, 48 symmetries and so on.
  See planner.
* nxopt31: mostly for comparison.
* other nxopt solvers: make generic and take the type as parameter.
* Step solver: take a coordinate function and a moveset as a parameter.

### cube.h changes

* Consider removing zerocube() from the api
* prefix public functions with nissy_ or something similar
* move() that takes a string (alg) as input

### Documentation and interface

* inline some documentation as comments in source code
* README.md (maybe convert to txt?) becomes the reference documentation

### Optimizations

* Trans: don't do full compose, for some trans composing perm is enough.
  Split out sumco() as a separate function and refactor, optimize.
* Use multi-move (up to 4/5 moves at once)
* CO is the worst part of moving, transforming and inverting. Try basing
  everything on representing the cube without CO and apply it only at the
  end to check that it is actually solved.
* see if vcube's method to flip all corners is better
* find a better way for computing the inverse?
* Improve avx2 instructions in general

## Internal representation of the cube

The plan (TODO) is to have multiple implementations: some that
take advantage of advanced CPU instructions (SIMD) and a fallback
"array" representation that works on any architecture.

### Array representation (fallback)

In this implementation of the cube.h interface, the cube is represented
by two arrays of 8-bit unsigned integers, one for centers and one for
corners.  The 4 leas-significant digits of each bit determine the piece,
the other 4 are used for orientation or kept to 0.

Edges:
        xxxopppp (x = unused, o = orientation, p = piece)

Corners:
        xooxpppp (x = unused, o = orientation, p = piece)

The two bits for CO are shifted to make it possible to perform mod 3
operations (sum, inverse) using only addition and bitwise operators.
See below for details.

The third bit is needed because x+y+1 can exceed 4.

### AVX2

Work in progress


## Textual representation of the cube

The functions readcube() and writecube() use different formats to read
and write a cube to text. Not all formats are supported for both input
and output.

### H48 - standard format for h48 (read, write)

Each edge is represented by two letters denoting the sides it belongs to
and one number denoting its orientation (0 oriented, 1 mis-oriented).
Similarly, each corner is represented by three letters and a number
(0 oriented, 1 twisted clockwise, 2 twisted counter-clockwise).
Edge orientation is relative to the F / B axis, corner orientation is
relative to the U / D axis.

The pieces are ordered such that the solved cube looks like this:

UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0
UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0

Whitespace (including newlines) between pieces is ignored when reading
the cube, and a single whitespace character is added between pieces
when writing.

The cube after the moves R'U'F looks like this:

FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0
UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0

### SRC - representation of the object in C code for cube_array (write)

The exact format depends on the internal cube representation (TODO: actually
this is false, because I need all formats for code generation; also adapating
tests is hard). It is guaranteed that, if OUT is the output in this format,
the line

cube_t cube = OUT;

is interpreted correctly by h48.


## Transformations

Transformations can be either simple rotations or a rotation composed
with a mirroring.

Simple rotations are denoted by two letters corresponding to the faces
to be moved to the U and F positions, respectively. For example FD is
the rotation that brings the F face on top and the D face on front.

A composed rotation + mirror is obtained by applying the corresponding
rotation to the solved cube mirrored along the M plane.

For example, to apply the transformation RBm (mirrored RB) to a cube C:
	1a. Apply a mirror along the M plane to the solved cube
	1b. Rotate the mirrored cube with z' y2
	3.  Apply the cube C to the transformed solved cube
	4.  Apply the transformations of step 1a and 1b in reverse

The orientation of pieces after a rotation ignores the new position
of centers.  A rotated cube can technically be inconsistent, because
the parity of the edge permutation has to be adjusted considering the
parity of the centers, which we ignore.

The utility script mirror.sh transforms a solved, rotated cube to its
mirrored and rotated version.
