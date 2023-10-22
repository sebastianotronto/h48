# Internal representation of the cube

The plan (TODO) is to have multiple implementations: some that
take advantage of advanced CPU instructions (SIMD) and a fallback
"array" representation that works on any architecture.

# Array representation (fallback)

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
