typedef enum {
        U = 0, U2, U3, D, D2, D3,
        R, R2, R3, L, L2, L3,
        F, F2, F3, B, B2, B3
} move_t;

typedef enum {
	UF_i = 0, UL_i, UB_i, UR_i, DF_i, DL_i, DB_i, DR_i,
	UF_m, UL_m, UB_m, UR_m, DF_m, DL_m, DB_m, DR_m,
	RU_i, RF_i, RD_i, RB_i, LU_i, LF_i, LD_i, LB_i,
	RU_m, RF_m, RD_m, RB_m, LU_m, LF_m, LD_m, LB_m,
	FU_i, FR_i, FD_i, FL_i, BU_i, BR_i, BD_i, BL_i,
	FU_m, FR_m, FD_m, FL_m, BU_m, BR_m, BD_m, BL_m
} trans_t;

typedef struct {
	uint8_t c[8];
	uint8_t e[12];
} cube_t;

extern cube_t solvedcube;

/*
The functions readcube() and writecube() use the following format.

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

More formats might be supported in the future.
*/

cube_t readcube(char *);
void writecube(cube_t, char *);

bool isconsistent(cube_t);
bool equal(cube_t, cube_t);
bool issolved(cube_t);
bool iserror(cube_t);

int readmoves(char *, move_t *);
void writemoves(move_t *, int, char *);

cube_t move(cube_t, move_t);
cube_t inverse(cube_t);
cube_t compose(cube_t, cube_t);
cube_t transform(cube_t, trans_t);
