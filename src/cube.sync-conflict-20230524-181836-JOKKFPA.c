/*
# Cube representation, moves, transformations and indexing

## String description

The functions readcube() and writecube() use the following format.
Each edge is represented by two letters denoting the sides it belongs to
and one number denoting its orientation (0 oriented, 1 mis-oriented).
Similarly, each corner is represented by three letters and a number
(0 oriented, 1 twisted clockwise, 2 twisted counter-clockwise).
Edge orientation is relative to the F / B axis, corner orientation is
relative to the U / D axis.

The correct order of the pieces is the same as that defined in the
section "Internal cube representation", except that pieces are read
left-to-right. Pieces are divided by slices, so the ordering is not the
most intuitive, but it is more convenient for the internal representation.

Whitespaces between pieces are ignored when reading the cube, and a
single whitespace character is added between pieces when writing.

For example, the solved cube looks like this:

UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0 UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0

The cube after the moves R'U'F looks like this:

FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0 UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0

More formats might be supported in the future.

## Internal cube representation

The cube_t data structure implemented in this file is designed to
efficiently perform common operations on a 3x3x3 Rubik's cube when
solving it with an iterative-deepening DFS search. It is not the most
general, complete, easy to read or compact one.  Since the cube can
be trivially reoriented before the search, we only encode permutations
of the cube that keep the center pieces in a fixed position (that is,
we do not encode the position of the centers).

The cube state is encoded in two 64-bit integers, one for edges and one
for centers. We explain how edges are encoded first, and the highlight
the few differences with corners afterwards.

For encoding edges, only the 60 least-significant bits are used. Each
edge described by 5 bits. The position of a 5-bit block in the 64-bit
integer determine the position of the edge piece in the cube, according
to the following table (least-significant bits on the right):

55-59 50-54 45-49 40-44 35-39 30-34 25-29 20-24 15-19 10-14  5-9   0-4
  BR    BL    FL    FR    DR    DL    UL    UR    DF    DB    UB    UF
ossee ossee ossee ossee ossee ossee ossee ossee ossee ossee ossee ossee

For each edge, the 4 least-significant bits ('ssee' in the table)
determine the piece. The two bits marked with 'ss' determine the internal
slice the piece belongs to, i.e. they are either '00' for M, '01' for
S or '10' for E. The other two bits (marked with 'ee') determine the
actual edge piece among the 4 in the same slice, and they are assigned
somewhat arbitarily. Using this representation and the ordering defined
in the table above, the edges are correctly permuted when these 4 bits
for each represent the numbers 0 to 11 in the correct order.

The last bit determines the orientation. The orientation of an edge
depends on its position, and it is defined being 0 if the edge can be
moved to its place in the solved orientation by permutations in the
subgroup <U, D, R, L, F2, B2>.

Corners are encoded in the 48 least-significant bits, and are described
by 6 bits each, their position being defined by the following table:

 35-39  30-34  25-29  20-24  15-19  10-14   5-9    0-4
  DBL    DFL    UBR    UFL    DBR    DFL    UBL    UFR
oooxcc oooxcc oooxcc oooxcc oooxcc oooxcc oooxcc oooxcc

The bit marked with an 'x' describes the axis the corner belongs to.
The 0 axis consists of the corners UFR, UBL, DFL and DBR, and the other
four corners form the axis marked with 1. Then two bits are needed to
identify the corner among the four of the same axis.  The last three bits
determine the orientation, where one corner is defined to be oriented
(marked with '000') if its top or bottom sticker faces the top or bottom
side. A corner a clockwise turn away from being oriented, thus requiring
a counter-clockwise turn to be oriented correctly, is marked with '001',
and a corner a counter-clockwise turn away is marked with '010'.  The most
significant bit is not used to determine the corner orientation, but it
must always be set to '0' to simplify the moving operations (see below).

## Basic moves

The 18 basic moves of the cube could be performed by applying a suitable
general permutation (see below), but they have instead been manually
implemented with a few simple operations each, to improve performance.

For each move we first permute the pieces. This amounts to shifting
around 4 blocks of bits for edges and 4 for corners. Since in some cases
adjacent pieces on the cube are also adjacent in the bit representation we
use, we can save some operations by shifting multiple blocks together.
For example, for the move U for edges we shift a block of 15 bits 5
positions to the left and a block of 5 bits 15 positions to the right.

There are some moves that change the orientation of the pieces. Namely,
the moves F, F', B and B' change the orientation of the edges and those
moves as well as R, R', L and L' change the orientation of the corners.
Edge orientation is easy to address: we simply xor the edge representation
by a bit mask with zeroes everywhere except for the 4 edges that need
to be flipped (i.e. the ones on the twisted face).

Corner orientation is harder to reproduce efficiently working only with
bitwise operations, as it involves performing operations modulo 3.
However, with the help of the extra bit we reserved, we are able to
do this using only two additions and 3 bitwise operations, without
any multiplication, division or modulo operation. The trick is
to use the following formula to sum two numbers x, y in {0,1,2}:

               ((x+y) + (x+y+1)/4) % 4

The thrid bit is needed because x+y and x+y+1 can exceed 3.
See below (in the code) for the details.

## Inverting the cube

TODO

## Transformations (conjugations by full-cube rotations)

TODO

## Indexing

TODO - subgroup description etc

Ideas for pruning (for another file?):
- Use corner separation + CO as main coordinate (~150k states)
  - for huge tables, htr corners can be used (6 times larger)
- Symmetry table, one entry or each main coordinate value with the
  following info:
  - index of the corresponding main symcoord (13 bits)
  - transtorep (6 bits)
  - base value for pruning table (5 bits, probably 4)
  - pruning value for only main coord, i.e. fallback (4 bits)
- To get a full coord for the cube:
  - get first coord c, get the transtorep
  - transform edges with transtorep
  - get second coordinate e
  - return c * MAXE + e
  - This is still too big, so divide by a power of 2 to get the hashed index
    - second coordinate: ep always, + varible number of eo bit (0 to 11)
- Generate table:
  - first probe for base value:
    - solve coord using fallback table for pruning for 10k random states or so
  - loop over all possible values (even if going for smaller table)
    - with inverse-index strategy or what?
  - use 1 bit per entry (more than base value or not)
*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "cube.h"

#define _error         0xFFFFFFFF

#define _esize         5ULL
#define _eoblock       0x10ULL /* 10000 */
#define _epblock       0x0FULL /* 01111 */
#define _eblock        0x1FULL /* 11111 */

#define _csize         6ULL
#define _coblock       0x18ULL /* 011000 */
#define _cpblock       0x07ULL /* 000111 */
#define _cblock        0x3FULL /* 111111 */

#define _edge_uf       0ULL  /* 00 00 */
#define _edge_ub       1ULL  /* 00 01 */
#define _edge_db       2ULL  /* 00 10 */
#define _edge_df       3ULL  /* 00 11 */
#define _edge_ur       4ULL  /* 01 00 */
#define _edge_ul       5ULL  /* 01 01 */
#define _edge_dl       6ULL  /* 01 10 */
#define _edge_dr       7ULL  /* 01 11 */
#define _edge_fr       8ULL  /* 10 00 */
#define _edge_fl       9ULL  /* 10 01 */
#define _edge_bl       10ULL /* 10 10 */
#define _edge_br       11ULL /* 10 11 */

#define _corner_ufr    0ULL /* 0 00 */
#define _corner_ubl    1ULL /* 0 01 */
#define _corner_dfl    2ULL /* 0 10 */
#define _corner_dbr    3ULL /* 0 11 */
#define _corner_ufl    4ULL /* 1 00 */
#define _corner_ubr    5ULL /* 1 01 */
#define _corner_dfr    6ULL /* 1 10 */
#define _corner_dbl    7ULL /* 1 11 */

#define ESHIFT(i)      ((i) * _esize)
#define EOSHIFT(i)     (4ULL + (i) * _esize)
#define EMASK(i)       (_eblock << ESHIFT(i))
#define EOMASK(i)      (_eoblock << ESHIFT(i))
#define EPMASK(i)      (_epblock << ESHIFT(i))
#define ESOLVED(ee)    (_edge_##ee << (_edge_##ee * _esize))
#define EDGEAT(e, i)   (((e) & EMASK(i)) >> ESHIFT(i))
#define EOAT(e, i)     (((e) & EOMASK(i)) >> EOSHIFT(i))
#define EPAT(e, i)     (((e) & EPMASK(i)) >> ESHIFT(i))

#define CSHIFT(i)      ((i) * _csize)
#define COSHIFT(i)     (3ULL + (i) * _csize)
#define CMASK(i)       (_cblock << CSHIFT(i))
#define COMASK(i)      (_coblock << CSHIFT(i))
#define CPMASK(i)      (_cpblock << CSHIFT(i))
#define CSOLVED(ccc)   (_corner_##ccc << (_corner_##ccc * _csize))
#define CORNERAT(c, i) (((c) & CMASK(i)) >> CSHIFT(i))
#define COAT(c, i)     (((c) & COMASK(i)) >> COSHIFT(i))
#define CPAT(c, i)     (((c) & CPMASK(i)) >> CSHIFT(i))

#define _emask_u       (EMASK(uf) | EMASK(ul) | EMASK(ub) | EMASK(ur))
#define _emask_d       (EMASK(df) | EMASK(dl) | EMASK(db) | EMASK(dr))
#define _emask_r       (EMASK(ur) | EMASK(dr) | EMASK(fr) | EMASK(br))
#define _emask_l       (EMASK(ul) | EMASK(dl) | EMASK(fl) | EMASK(bl))
#define _emask_f       (EMASK(uf) | EMASK(df) | EMASK(fr) | EMASK(fl))
#define _emask_b       (EMASK(ub) | EMASK(db) | EMASK(br) | EMASK(bl))

#define _cmask_u       (CMASK(ufr) | CMASK(ufl) | CMASK(ubl) | CMASK(ubr))
#define _cmask_d       (CMASK(dfr) | CMASK(dfl) | CMASK(dbl) | CMASK(dbr))
#define _cmask_r       (CMASK(ufr) | CMASK(dfr) | CMASK(ubr) | CMASK(dbr))
#define _cmask_l       (CMASK(ufl) | CMASK(dfl) | CMASK(ubl) | CMASK(dbl))
#define _cmask_f       (CMASK(ufr) | CMASK(ufl) | CMASK(dfr) | CMASK(dfl))
#define _cmask_b       (CMASK(ubr) | CMASK(ubl) | CMASK(dbr) | CMASK(dbl))

#define _eomask        (EOMASK(uf) | EOMASK(ul) | EOMASK(ub) | EOMASK(ur) \
                        EOMASK(df) | EOMASK(dl) | EOMASK(db) | EOMASK(dr) \
                        EOMASK(fr) | EOMASK(fl) | EOMASK(bl) | EOMASK(br))
#define _comask        (COMASK(ufr) | COMASK(ufl) | COMASK(ubl) | COMASK(ubr) \
                        COMASK(dfr) | COMASK(dfl) | COMASK(dbl) | COMASK(dbr))

static uint64_t permsign(uint64_t *, int);
static uint64_t readep(char *);
static uint64_t readeo(char *);
static uint64_t readcp(char *);
static uint64_t readco(char *);
static uint64_t readmove(char);
static uint64_t readmodifier(char);

static char *edgestr[] = {
	[_edge_uf] = "UF",
	[_edge_ub] = "UB",
	[_edge_db] = "DB",
	[_edge_df] = "DF",
	[_edge_ur] = "UR",
	[_edge_ul] = "UL",
	[_edge_dl] = "DL",
	[_edge_dr] = "DR",
	[_edge_fr] = "FR",
	[_edge_fl] = "FL",
	[_edge_bl] = "BL",
	[_edge_br] = "BR"
};
static char *cornerstr[] = {
	[_corner_ufr] = "UFR",
	[_corner_ubl] = "UBL",
	[_corner_dfl] = "DFL",
	[_corner_dbr] = "DBR",
	[_corner_ufl] = "UFL",
	[_corner_ubr] = "UBR",
	[_corner_dfr] = "DFR",
	[_corner_dbl] = "DBL"
};
static char *movestr[] = {
	[U] = "U", [U2] = "U2", [U3] = "U'",
	[D] = "D", [D2] = "D2", [D3] = "D'",
	[R] = "R", [R2] = "R2", [R3] = "R'",
	[L] = "L", [L2] = "L2", [L3] = "L'",
	[F] = "F", [F2] = "F2", [F3] = "F'",
	[B] = "B", [B2] = "B2", [B3] = "B'",
};

cube_t solvedcube = {
	.e = ESOLVED(uf) | ESOLVED(ul) | ESOLVED(ub) | ESOLVED(ur) |
	     ESOLVED(df) | ESOLVED(dl) | ESOLVED(db) | ESOLVED(dr) |
	     ESOLVED(fr) | ESOLVED(fl) | ESOLVED(bl) | ESOLVED(br),
	.c = CSOLVED(ufr) | CSOLVED(ufl) | CSOLVED(ubl) | CSOLVED(ubr) |
	     CSOLVED(dfr) | CSOLVED(dfl) | CSOLVED(dbl) | CSOLVED(dbr),
};
cube_t errorcube = { .e = _error, .c = _error };


static uint64_t
permsign(uint64_t *a, int n)
{
	int i, j;
	uint64_t ret;

	ret = 0;

	for (i = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

bool
isconsistent(cube_t cube)
{
	uint64_t p[12], sum;
	bool found[12];
	int i;

	sum = 0;

	/* Check for EP consistency */
	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		p[i] = EPAT(cube.e, i);
		found[p[i]] = true;
	}
	for (i = 0; i < 12; i++)
		if (!found[i])
			return false;
	sum = permsign(p, 12);

	/* Check for CP consistency */
	for (i = 0; i < 8; i++)
		found[i] = false;
	for (i = 0; i < 8; i++) {
		p[i] = CPAT(cube.c, i);
		found[p[i]] = true;
	}
	for (i = 0; i < 8; i++)
		if (!found[i])
			return false;
	sum += permsign(p, 8);

	/* Check permutation parity */
	if (sum % 2 != 0)
		return false;

	/* Check for EO parity */
	for (i = 0, sum = 0; i < 12; i++)
		sum += EOAT(cube.e, i);
	if (sum % 2 != 0)
		return false;

	/* Check for CO parity */
	for (i = 0, sum = 0; i < 8; i++)
		sum += COAT(cube.c, i);
	if (sum % 3 != 0)
		return false;

	/* Check that CO extra bit is zero */
	for (i = 0; i < 8; i++)
		if (cube.c & (1ULL << (5 + i * _csize)))
			return false;

	return true;
}

bool
issolved(cube_t cube)
{
	return cube.c == solvedcube.c && cube.e == solvedcube.e;
}


static uint64_t
readep(char *str)
{
	if (!strncmp(str, "UF", 2))
		return _edge_uf;
	if (!strncmp(str, "UL", 2))
		return _edge_ul;
	if (!strncmp(str, "UB", 2))
		return _edge_ub;
	if (!strncmp(str, "UR", 2))
		return _edge_ur;
	if (!strncmp(str, "DF", 2))
		return _edge_df;
	if (!strncmp(str, "DL", 2))
		return _edge_dl;
	if (!strncmp(str, "DB", 2))
		return _edge_db;
	if (!strncmp(str, "DR", 2))
		return _edge_dr;
	if (!strncmp(str, "FR", 2))
		return _edge_fr;
	if (!strncmp(str, "FL", 2))
		return _edge_fl;
	if (!strncmp(str, "BL", 2))
		return _edge_bl;
	if (!strncmp(str, "BR", 2))
		return _edge_br;

	return _error;
}

static uint64_t
readeo(char *str)
{
	if (*str == '0')
		return 0ULL;
	if (*str == '1')
		return 1ULL;

	return _error;
}

static uint64_t
readcp(char *str)
{
	if (!strncmp(str, "UFR", 3) || !strncmp(str, "URF", 3))
		return _corner_ufr;
	if (!strncmp(str, "UFL", 3) || !strncmp(str, "ULF", 3))
		return _corner_ufl;
	if (!strncmp(str, "UBL", 3) || !strncmp(str, "ULB", 3))
		return _corner_ubl;
	if (!strncmp(str, "UBR", 3) || !strncmp(str, "URB", 3))
		return _corner_ubr;
	if (!strncmp(str, "DFR", 3) || !strncmp(str, "DRF", 3))
		return _corner_dfr;
	if (!strncmp(str, "DFL", 3) || !strncmp(str, "DLF", 3))
		return _corner_dfl;
	if (!strncmp(str, "DBL", 3) || !strncmp(str, "DLB", 3))
		return _corner_dbl;
	if (!strncmp(str, "DBR", 3) || !strncmp(str, "DRB", 3))
		return _corner_dbr;

	return _error;
}

static uint64_t
readco(char *str)
{
	if (*str == '0')
		return 0ULL;
	if (*str == '1')
		return 1ULL;
	if (*str == '2')
		return 2ULL;

	return _error;
}

cube_t
readcube(char *buf)
{
	int i;
	uint64_t piece, orient;
	cube_t ret = {0};
	char *b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			goto readcube_error;
		b += 2;
		if ((orient = readeo(b)) == _error)
			goto readcube_error;
		b++;
		ret.e |= (piece << ESHIFT(i)) | (orient << EOSHIFT(i));
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			goto readcube_error;
		b += 3;
		if ((orient = readco(b)) == _error)
			goto readcube_error;
		b++;
		ret.c |= (piece << CSHIFT(i)) | (orient << COSHIFT(i));
	}

	return ret;

readcube_error:
	return errorcube;
}

void
writecube(cube_t cube, char *buf)
{
	char *errormsg;
	uint64_t piece;
	size_t len;
	int i;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	for (i = 0; i < 12; i++) {
		piece = EPAT(cube.e, i);
		buf[4*i    ] = edgestr[piece][0];
		buf[4*i + 1] = edgestr[piece][1];
		buf[4*i + 2] = EOAT(cube.e, i) + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = CPAT(cube.c, i);
		buf[48 + 5*i    ] = cornerstr[piece][0];
		buf[48 + 5*i + 1] = cornerstr[piece][1];
		buf[48 + 5*i + 2] = cornerstr[piece][2];
		buf[48 + 5*i + 3] = COAT(cube.c, i) + '0';
		buf[48 + 5*i + 4] = ' ';
	}

	buf[48+39] = '\0';

	return;

writecube_error:
	len = strlen(errormsg);
	strcpy(buf, errormsg);
	buf[len] = '\n';
	buf[len+1] = '\0';
}


static uint64_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return U;
	case 'D':
		return D;
	case 'R':
		return R;
	case 'L':
		return L;
	case 'F':
		return F;
	case 'B':
		return B;
	default:
		return _error;
	}
}

static uint64_t
readmodifier(char c)
{
	switch (c) {
	case '1': /* Fallthrough */
	case '2': /* Fallthrough */
	case '3':
		return c - '0' - 1;
	case '\'':
		return 2;
	default:
		return 0;
	}
}

int
readmoves(char *buf, move_t *m)
{
	int n;
	uint64_t r;
	char *b;

	for (b = buf, n = 0; *b != '\0'; b++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((r = readmove(*b)) == _error)
			return -1;
		m[n] = (move_t)r;
		if ((r = readmodifier(*(b+1))) != 0) {
			b++;
			m[n] += r;
		}
		n++;
	}

	return n;
}

void
writemoves(move_t *m, int n, char *buf)
{
	int i;
	char *b, *s;

	for (i = 0, b = buf; i < n; i++, b++) {
		s = movestr[m[i]];
		strcpy(b, s);
		b += strlen(s);	
		*b = ' ';
	}
	*b = '\0';
}


cube_t
move(move_t m, cube_t c)
{
	/* TODO - not implemented yet */

	cube_t ret = {0};

	switch (m) {
	case U:
		return ret;
	default:
		return ret;
	}
}
