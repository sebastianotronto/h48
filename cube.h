/******************************************************************************
Cube type definition

Each piece is represented by an (unsigned) 8-bit integer. The 4
least-significant bits determine which piece it is, the other 4 determine
the orientation.

Edges are numbered as follows (see also cube.c):
UF=0 UB=1 DB=2 DF=3 UR=4 UL=5 DL=6 DR=7 FR=8 FL=9 BL=10 BR=11

Corners are numbered as follows:
UFR=0 UBL=1 DFL=2 DBR=3 UFL=4 UBR=5 DFR=6 DBL=7

The orientation of the edges is with respect to F/B, the orientation of
corners is with respect to U/D.

The permutation of the center pieces is not stored. This means that the
cube is assumed to be in a fixed orientation.

TODO: encode centers?

The exact cube type structure depends on your system's configuration. If
you operate on the cube only via the functions provided below, you don't
need to worry about this.
******************************************************************************/

#ifdef CUBE_AVX2
typedef __m256i cube_t;
#else
typedef struct {
	uint8_t c[8];   /* Corners */
	uint8_t e[12];  /* Edges   */
} cube_t;
#endif

/* Returns a copy of the solved cube */
cube_t solvedcube(void);

/* Basic checks on the cube */
bool issolvable(cube_t);
bool equal(cube_t, cube_t);
bool issolved(cube_t);

/* Apply the second cube on the first as a move sequence */
cube_t compose(cube_t, cube_t);

/* Invert the cube */
cube_t inverse(cube_t);

/* All functions can return an error value, use iserror() to check this */
bool iserror(cube_t);

/******************************************************************************
Moves and transformations

Moves and transformations are represented each as an (unsigned) 8 bit integer.

Moves are numbered as follows:
U=0  U2=1  U'=2  D=3  D2=4  D'=5
R=6  R2=7  R'=8  L=9  L2=10 L'=11
F=12 F2=13 F'=14 B=15 B2=16 B'=17

TODO: NISS

TODO: Extend the moveset?

Transformations can be either simple rotations or a rotation composed
with a mirroring.  A composed rotation + mirror is obtained by applying
the corresponding rotation to the solved cube mirrored along the M plane.

For example, to apply the transformation RBm (mirrored RB) to a cube C:
	1. Apply a mirror along the M plane to the solved cube
	2. Rotate the mirrored cube with z' y2
	3. Apply the cube C to the transformed solved cube
	4. Apply the transformations of step 1a and 1b in reverse

See cube.c for a full list of transformations.
******************************************************************************/

typedef uint8_t move_t;
typedef uint8_t trans_t;

/* Apply a move or a transformation on the cube */
cube_t move(cube_t, move_t);
cube_t transform(cube_t, trans_t);

/******************************************************************************
Read / write utilities

Reading and writing is not done directly via stdin / stdout, but via an
array of char (called buf in the prototypes below).

Multiple representations of the cube as text are supported, although
not all of them are supported for both reading and writing. See below
for details. More formats may be supported in the future.

Moves are read using the standard notation. Each move (U, D, R, L, F,
B) can be followed by a modifier (1, 2, 3, '). Whitespace (spaces, tabs,
newlines) are ignored. Parantheses and other notation is not supported.
TODO: parantheses for NISS

For how transformations are read or written, see cube.c.
******************************************************************************/

/* The different formats for reading or writing the cube */
typedef enum {
	H48, /* H48 is a human-readable format.
	      *
	      * Each edge is represented by two letters denoting the sides it
	      * belongs to and one number denoting its orientation (0 oriented,
	      * 1 mis-oriented). Similarly, each corner is represented by three
	      * letters and a number (0 oriented, 1 twisted clockwise, 2
	      * twisted counter-clockwise).
	      *
	      * The solved cube looks like this:
	      *
	      * UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0
	      * UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0
	      *
	      * The cube after the moves R'U'F looks like this:
	      *
	      * FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0
	      * UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0
	      *
	      * Whitespace (including newlines) between pieces is ignored when
	      * reading the cube. A single whitespace character is added
	      * between pieces when writing.
	      */
	SRC, /* The SRC format can be used to generate code for internal use.
	      *
	      * In cube.c, a type called cube_array_t is defined and used for
	      * basic, non-performance-critical methods. If OUT is the output
	      * in SRC format, the following line can be used to declare a new
	      * cube object:
	      *
	      * cube_array_t cube = OUT
	      */
	AVX, /* The AVX format is analogous to SRC, but for the AVX2 internal
	      * representation of the cube.
	      */
} format_t;

/* Reads a cube from buf in the specified format, and return it.
 * Supported formats: H48.
 */
cube_t readcube(format_t format, char *buf);

/* Write the given cube to buf in the specified format.
 * Supported formats: H48, SRC, AVX.
 */
void writecube(format_t format, cube_t cube, char *buf);

/* Utilities for reading and writing moves */
int readmoves(char *buf, move_t *moves);
void writemoves(move_t *moves, int n, char *buf);
trans_t readtrans(char *buf);
void writetrans(trans_t trans, char *buf);

/******************************************************************************
Coordinates

The coordinate functions compute one aspect of the cube (for example,
the edge orientation) and they return it as an integer. They are used
for example to build pruning tables for various solving methods.
******************************************************************************/

int16_t coord_eo(cube_t); /* Edge orientation */

/******************************************************************************
Solvers

Solvers return -1 in case of error, the number of solutions otherwise

TODO
******************************************************************************/

int solve_generic(cube_t, int (*)(cube_t), uint8_t, int, move_t *);
