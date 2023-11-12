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

TODO: define EO and CO better, explain how to use them
TODO: encode centers?

The exact cube type structure depends on your system's configuration. If
you operate on the cube only via the functions provided below, you don't
need to worry about this.
******************************************************************************/

typedef struct {
	uint8_t corner[8];
	uint8_t edge[12];
} cube_t;

/* Returns a copy of the solved cube */
cube_t solvedcube(void);

/* Basic checks on the cube */
bool isconsistent(cube_t);
bool issolvable(cube_t);
bool issolved(cube_t);
bool equal(cube_t, cube_t);

/* All functions can return an error value, use iserror() to check this */
bool iserror(cube_t);

/* Apply the second cube on the first as a move sequence */
cube_t compose(cube_t, cube_t);

/* Invert the cube */
cube_t inverse(cube_t);

/* Check if a cube represent a valid state (possibly unsolvable) */

/* TODO comment on these and the format for moves and trans */
/* For trans, only one trans is supported */
cube_t applymoves(cube_t, char *);
cube_t applytrans(cube_t, char *);

/******************************************************************************
Read / write utilities

Reading and writing is not done directly via stdin / stdout, but via an
array of char (called buf in the prototypes below).

Multiple representations of the cube as text are supported:

- H48: a human-readable format.
  Each edge is represented by two letters denoting the sides it
  belongs to and one number denoting its orientation (0 oriented, 1
  mis-oriented). Similarly, each corner is represented by three letters and
  a number (0 oriented, 1 twisted clockwise, 2 twisted counter-clockwise).

  The solved cube looks like this:

  UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0
  UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0

  The cube after the moves R'U'F looks like this:

  FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0
  UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0

  Whitespace (including newlines) between pieces is ignored when reading the
  cube. A single whitespace character is added between pieces when writing.

- SRC: format used to generate code for internal use.
  In cube.c, a type called cube_array_t is defined and used for basic,
  non-performance-critical methods. If OUT is the output in SRC format,
  the following line can be used to declare a new cube object:

cube_array_t cube = OUT
      
- AVX: analogue to SRC, but for the AVX2 internal representation of the cube.

Not all formats are supported for both reading and writing. More formats
may be supported in the future.
******************************************************************************/

/* Reads a cube from buf in the specified format, and return it.
 * Supported formats: "H48".
 */
cube_t readcube(char *format, char *buf);

/* Write the given cube to buf in the specified format.
 * Supported formats: "H48", "SRC", "AVX".
 */
void writecube(char *format, cube_t cube, char *buf);

/******************************************************************************
Solvers

The solutions are returned as a newline-separated list of characters. Moves
are separated by single spaces.

Unless specified otherwise, all the solutions are not trivially simplifiable.
This means that sequences like U U2 or R L R will not appear in any solution.
Moreover, two consecutive parallel moves are always going to be sorted in
increasing order. For example, L R2 may never appear in a solution, but R2 L
could.
******************************************************************************/

int64_t solve(
	cube_t cube,     /* The cube to solve. Must be solvable. */
	char *solver,    /* Supported solvers:
			  * "optimal" - currently the same as "simple"
			  * "simple"  - a simple, slow solver using no tables
			  */
	char *options,   /* Some solvers accept extra options,
			  * like "!filter".
			  */
	char *nisstype,  /* Can be "normal", "inverse", "mixed" or "linear". */
	int8_t minmoves, /* The minimum number of moves. Must be >= 0. */
	int8_t maxmoves, /* The maximum number of moves. If negative, the
			  * maximum length is unlimited.
			  */
	int64_t maxsols, /* The maximum number of solutions. */
	int8_t optimal,  /* All solutions at most "optimal" moves from the
			  * shortest solution (respecting minmoves) are found.
			  * If negative, this parameter is ignored.
			  */
	void *data,      /* Some solvers require extra data to function
			  * properly (for example, pruning tables). This data
			  * can be generated with gendata(), see below.
			  */
	char *solutions  /* The solutions (return parameter) */
);

/* Solving n cubes optimally, one solutions per cube. Options are similar
 * to solve().
 */
void multisolve(int n, cube_t *cube, char *solver, void *data, char *sols);

/* Returns the number of bytes written to data, -1 in case of error.
 * TODO: write down how much memory every solver requires. */
int64_t gendata(char *solver, void *data);
