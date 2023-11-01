/*
# Cube representation, moves, transformations and (tentatively) indexing

## Textual description

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

UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0 \ (no newline)
UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0

The cube after the moves R'U'F looks like this:

FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0 \ (no newline)
UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0

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

The thrid bit is needed because x+y and x+y+1 can exceed 3.  We can
apply this operation to multiple pairs of bits representing ternary
digits at once using binary operations.  See the function coapply()
below for the details.

## Inverting the cube

TODO

## Transformations (conjugations by full-cube rotations)

TODO

## Indexing (tentative)

TODO - subgroup description etc

*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "cube.h"

#define _error         0xFFFFFFFF

#define _eoblock       0x10ULL    /* 10000 */
#define _epblock       0x0FULL    /* 01111 */
#define _eblock        0x1FULL    /* 11111 */
#define _eblock2       0x3FFULL   /* 1111111111 */

#define _coblock       0x18ULL    /* 011000 */
#define _cpblock       0x07ULL    /* 000111 */
#define _cblock        0x3FULL    /* 111111 */
#define _cblock2       0xFFFULL   /* 111111111111 */

#define _eomask        0x842108421084210ULL /* 10000 repeated 12 times */
#define _comask        0x618618618618ULL    /* 011000 repeated 8 times */
#define _coonemask     0x208208208208ULL    /* 001000 repeated 8 times */
#define _coextramask   0x820820820820ULL    /* 100000 repeated 8 times */

#define _emask_u (_eblock2 | _eblock2 << 20ULL)
#define _emask_d (_eblock2 << 10ULL | _eblock2 << 30ULL)
#define _emask_r (_eblock << 20ULL | _eblock2 << 35ULL | _eblock << 55ULL)
#define _emask_l (_eblock2 << 45ULL | _eblock2 << 25ULL)
#define _emask_f (_eblock | _eblock << 15ULL | _eblock2 << 40ULL)
#define _emask_b (_eblock2 << 5ULL | _eblock2 << 50ULL)

#define _cmask_u (_cblock2 | _cblock2 << 24ULL)
#define _cmask_d (_cblock2 << 12ULL | _cblock2 << 36ULL)
#define _cmask_r (_cblock | _cblock << 18ULL | _cblock2 << 30ULL)
#define _cmask_l (_cblock2 << 6ULL | _cblock << 24ULL | _cblock << 42ULL)
#define _cmask_f (_cblock | _cblock << 12ULL | _cblock << 24ULL | _cblock << 36ULL)
#define _cmask_b (_cblock << 6ULL | _cblock << 18ULL | _cblock << 30ULL | _cblock << 42ULL)

#define _eomask_f (1ULL << 4ULL | 1ULL << 19ULL | 1ULL << 44ULL | 1ULL << 49ULL)
#define _eomask_b (1ULL << 9ULL | 1ULL << 14ULL | 1ULL << 54ULL | 1ULL << 59ULL)

#define _comask_r (2ULL << 3ULL | 1ULL << 33ULL | 2ULL << 21ULL | 1ULL << 39ULL)
#define _comask_l (1ULL << 27ULL | 2ULL << 9ULL | 2ULL << 15ULL | 1ULL << 45ULL)
#define _comask_f (1ULL << 3ULL | 1ULL << 15ULL | 2ULL << 27ULL | 2ULL << 39ULL)
#define _comask_b (1ULL << 9ULL | 1ULL << 21ULL | 2ULL << 33ULL | 2ULL << 45ULL)

static inline uint64_t coapply(uint64_t, uint64_t);
static uint64_t permsign(uint64_t *, int);
static uint64_t readep(char *);
static uint64_t readeo(char *);
static uint64_t readcp(char *);
static uint64_t readco(char *);
static uint64_t readmove(char);
static uint64_t readmodifier(char);

static char *edgestr[] = {
	"UF", "UB", "DB", "DF",
	"UR", "UL", "DL", "DR",
	"FR", "FL", "BL", "BR"
};
static char *cornerstr[] = {
	"UFR", "UBL", "DFL", "DBR",
	"UFL", "UBR", "DFR", "DBL"
};
static char *cornerstralt[] = {
	"URF", "ULB", "DLF", "DRB",
	"ULF", "URB", "DRF", "DLB"
};
static char *movestr[] = {
	[U] = "U", [U2] = "U2", [U3] = "U'",
	[D] = "D", [D2] = "D2", [D3] = "D'",
	[R] = "R", [R2] = "R2", [R3] = "R'",
	[L] = "L", [L2] = "L2", [L3] = "L'",
	[F] = "F", [F2] = "F2", [F3] = "F'",
	[B] = "B", [B2] = "B2", [B3] = "B'",
};

cube_t solvedcube = { .e = 0x5A928398A418820ULL, .c = 0x1C61440C2040ULL };
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
	uint64_t x, p[12], sum, co;
	bool found[12];
	int i;

	sum = 0;

	/* Check for EP consistency */
	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0, x = cube.e; i < 12; i++, x >>= 5ULL) {
		p[i] = x & _epblock;
		if (p[i] >= 12)
			goto inconsistent_ep;
		found[p[i]] = true;
	}
	for (i = 0; i < 12; i++)
		if (!found[i])
			goto inconsistent_ep;
	sum = permsign(p, 12);

	/* Check for CP consistency */
	for (i = 0; i < 8; i++)
		found[i] = false;
	for (i = 0, x = cube.c; i < 8; i++, x >>= 6ULL) {
		p[i] = x & _cpblock;
		if (p[i] >= 8)
			goto inconsistent_cp;
		found[p[i]] = true;
	}
	for (i = 0; i < 8; i++)
		if (!found[i])
			goto inconsistent_cp;
	sum += permsign(p, 8);

	/* Check permutation parity */
	if (sum % 2 != 0)
		goto inconsistent_parity;

	/* Check for EO parity */
	for (i = 0, sum = 0, x = cube.e; i < 12; i++, x >>= 5ULL)
		sum += (x & _eoblock) >> 4ULL;
	if (sum % 2 != 0)
		goto inconsistent_eo;

	/* Check for CO parity */
	for (i = 0, sum = 0, x = cube.c; i < 8; i++, x >>= 6ULL) {
		co = (x & _coblock) >> 3ULL;
		if (co > 2)
			goto inconsistent_co3;
		sum += co;
	}
	if (sum % 3 != 0)
		goto inconsistent_co;

	/* Check that CO extra bit is zero */
	if (cube.c & _coextramask)
		goto inconsistent_coextra;

	return true;

inconsistent_ep:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent EP\n");
#endif
	goto inconsistent_return;
inconsistent_cp:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent CP\n");
#endif
	goto inconsistent_return;
inconsistent_parity:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent parity\n");
#endif
	goto inconsistent_return;
inconsistent_eo:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent EO\n");
#endif
	goto inconsistent_return;
inconsistent_co3:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent CO=3\n");
#endif
	goto inconsistent_return;
inconsistent_co:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent CO\n");
#endif
	goto inconsistent_return;
inconsistent_coextra:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent extra bit for CO\n");
#endif
	goto inconsistent_return;
inconsistent_return:
	return false;
}

bool
issolved(cube_t cube)
{
	return cube.c == solvedcube.c && cube.e == solvedcube.e;
}


static uint64_t
readep(char *str)
{
	uint64_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

#ifdef DEBUG
	fprintf(stderr, "Error reading EP\n");
#endif
	return _error;
}

static uint64_t
readeo(char *str)
{
	if (*str == '0')
		return 0ULL;
	if (*str == '1')
		return 1ULL;

#ifdef DEBUG
	fprintf(stderr, "Error reading EO\n");
#endif
	return _error;
}

static uint64_t
readcp(char *str)
{
	uint64_t c;

	for (c = 0; c < 8; c++)
		if (!strncmp(str, cornerstr[c], 3) ||
		    !strncmp(str, cornerstralt[c], 3))
			return c;

#ifdef DEBUG
	fprintf(stderr, "Error reading CP\n");
#endif
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

#ifdef DEBUG
	fprintf(stderr, "Error reading CO\n");
#endif
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
		ret.e |= (piece << (i * 5ULL)) | (orient << (i * 5ULL + 4ULL));
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
		ret.c |= (piece << (i * 6ULL)) | (orient << (i * 6ULL + 3ULL));
	}

	return ret;

readcube_error:
#ifdef DEBUG
	fprintf(stderr, "readcube error\n");
#endif
	return errorcube;
}

void
writecube(cube_t cube, char *buf)
{
	char *errormsg;
	uint64_t piece, orien, x;
	size_t len;
	int i;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	for (i = 0, x = cube.e; i < 12; i++, x >>= 5ULL) {
		piece = x & _epblock;
		orien = (x & _eoblock) >> 4ULL;
		buf[4*i    ] = edgestr[piece][0];
		buf[4*i + 1] = edgestr[piece][1];
		buf[4*i + 2] = orien + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0, x = cube.c; i < 8; i++, x >>= 6ULL) {
		piece = x & _cpblock;
		orien = (x & _coblock) >> 3ULL;
		buf[48 + 5*i    ] = cornerstr[piece][0];
		buf[48 + 5*i + 1] = cornerstr[piece][1];
		buf[48 + 5*i + 2] = cornerstr[piece][2];
		buf[48 + 5*i + 3] = orien + '0';
		buf[48 + 5*i + 4] = ' ';
	}

	buf[48+39] = '\0';

	return;

writecube_error:
#ifdef DEBUG
	fprintf(stderr, "writecube error, see stdout for details\n");
#endif
	len = strlen(errormsg);
	memcpy(buf, errormsg, len);
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
		if (*b == '\0')
			return n;
		if ((r = readmove(*b)) == _error)
			goto readmoves_error;
		m[n] = (move_t)r;
		if ((r = readmodifier(*(b+1))) != 0) {
			b++;
			m[n] += r;
		}
		n++;
	}

	return n;

readmoves_error:
#ifdef DEBUG
	fprintf(stderr, "readmoves error\n");
#endif
	return -1;
}

void
writemoves(move_t *m, int n, char *buf)
{
	int i;
	size_t len;
	char *b, *s;

	for (i = 0, b = buf; i < n; i++, b++) {
		s = movestr[m[i]];
		len = strlen(s);
		memcpy(b, s, len);
		b += len;	
		*b = ' ';
	}
	*b = '\0';
}


static inline uint64_t
coapply(uint64_t c, uint64_t m)
{
	uint64_t z, b;

	z = c + m;
	b = ((z + _coonemask) & _coextramask) >> 2ULL;

	return (z + b) & ~_coextramask;
}

cube_t
move(cube_t c, move_t m)
{
	cube_t ret = {0};

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "move error, inconsistent cube\n");
		goto move_error;
	}
#endif

	switch (m) {
	case U:
		ret.e = c.e & ~_emask_u;
		ret.e |=
		    (c.e & _eblock) << 25ULL |
		    (c.e & _eblock << 5ULL) << 15ULL |
		    (c.e & _eblock2 << 20ULL) >> 20ULL;

		ret.c = c.c & ~_cmask_u;
		ret.c |=
		    (c.c & _cblock2) << 24ULL |
		    (c.c & _cblock << 24ULL) >> 18ULL |
		    (c.c & _cblock << 30ULL) >> 30ULL;

		return ret;
	case U2:
		ret.e = c.e & ~_emask_u;
		ret.e |=
		    (c.e & (_eblock | _eblock << 20ULL)) << 5ULL |
		    (c.e & (_eblock << 5ULL | _eblock << 25ULL)) >> 5ULL;

		ret.c = c.c & ~_cmask_u;
		ret.c |=
		    (c.c & (_cblock | _cblock << 24ULL)) << 6ULL |
		    (c.c & (_cblock << 6ULL | _cblock << 30ULL)) >> 6ULL;

		return ret;
	case U3:
		ret.e = c.e & ~_emask_u;
		ret.e |=
		    (c.e & _eblock2) << 20ULL |
		    (c.e & _eblock << 25ULL) >> 25ULL |
		    (c.e & _eblock << 20ULL) >> 15ULL;

		ret.c = c.c & ~_cmask_u;
		ret.c |=
		    (c.c & _cblock) << 30ULL |
		    (c.c & _cblock << 6ULL) << 18ULL |
		    (c.c & _cblock2 << 24ULL) >> 24ULL;

		return ret;
	case D:
		ret.e = c.e & ~_emask_d;
		ret.e |=
		    (c.e & _eblock2 << 10ULL) << 20ULL |
		    (c.e & _eblock << 30ULL) >> 15ULL |
		    (c.e & _eblock << 35ULL) >> 25ULL;

		ret.c = c.c & ~_cmask_d;
		ret.c |=
		    (c.c & _cblock2 << 12ULL) << 24ULL |
		    (c.c & _cblock << 36ULL) >> 18ULL |
		    (c.c & _cblock << 42ULL) >> 30ULL;

		return ret;
	case D2:
		ret.e = c.e & ~_emask_d;
		ret.e |=
		    (c.e & (_eblock << 10ULL | _eblock << 30ULL)) << 5ULL |
		    (c.e & (_eblock << 15ULL | _eblock << 35ULL)) >> 5ULL;

		ret.c = c.c & ~_cmask_d;
		ret.c |=
		    (c.c & (_cblock << 12ULL | _cblock << 36ULL)) << 6ULL |
		    (c.c & (_cblock << 18ULL | _cblock << 42ULL)) >> 6ULL;

		return ret;
	case D3:
		ret.e = c.e & ~_emask_d;
		ret.e |=
		    (c.e & _eblock << 10ULL) << 25ULL |
		    (c.e & _eblock << 15ULL) << 15ULL |
		    (c.e & _eblock2 << 30ULL) >> 20ULL;

		ret.c = c.c & ~_cmask_d;
		ret.c |=
		    (c.c & _cblock << 12ULL) << 30ULL |
		    (c.c & _cblock << 18ULL) << 18ULL |
		    (c.c & _cblock2 << 36ULL) >> 24ULL;

		return ret;
	case R:
		ret.e = c.e & ~_emask_r;
		ret.e |=
		    (c.e & _eblock << 20ULL) << 35ULL |
		    (c.e & _eblock << 55ULL) >> 20ULL |
		    (c.e & _eblock << 35ULL) << 5ULL |
		    (c.e & _eblock << 40ULL) >> 20ULL;

		ret.c = c.c & ~_cmask_r;
		ret.c |=
		    (c.c & _cblock) << 30ULL |
		    (c.c & _cblock << 30ULL) >> 12ULL |
		    (c.c & _cblock << 18ULL) << 18ULL |
		    (c.c & _cblock << 36ULL) >> 36ULL;

		ret.c = coapply(ret.c, _comask_r);

		return ret;
	case R2:
		ret.e = c.e & ~_emask_r;
		ret.e |=
		    (c.e & (_eblock << 20ULL | _eblock << 40ULL)) << 15ULL |
		    (c.e & (_eblock << 35ULL | _eblock << 55ULL)) >> 15ULL;

		ret.c = c.c & ~_cmask_r;
		ret.c |=
		    (c.c & _cblock) << 18ULL |
		    (c.c & _cblock << 18ULL) >> 18ULL |
		    (c.c & _cblock << 30ULL) << 6ULL |
		    (c.c & _cblock << 36ULL) >> 6ULL;

		return ret;
	case R3:
		ret.e = c.e & ~_emask_r;
		ret.e |=
		    (c.e & (_eblock << 20ULL | _eblock << 35ULL)) << 20ULL |
		    (c.e & _eblock << 55ULL) >> 35ULL |
		    (c.e & _eblock << 40ULL) >> 5ULL;

		ret.c = c.c & ~_cmask_r;
		ret.c |=
		    (c.c & _cblock) << 36ULL |
		    (c.c & _cblock << 30ULL) >> 30ULL |
		    (c.c & _cblock << 18ULL) << 12ULL |
		    (c.c & _cblock << 36ULL) >> 18ULL;

		ret.c = coapply(ret.c, _comask_r);

		return ret;
	case L:
		ret.e = c.e & ~_emask_l;
		ret.e |=
		    (c.e & _eblock2 << 25ULL) << 20ULL |
		    (c.e & _eblock << 45ULL) >> 15ULL |
		    (c.e & _eblock << 50ULL) >> 25ULL;

		ret.c = c.c & ~_cmask_l;
		ret.c |=
		    (c.c & _cblock << 6ULL) << 18ULL |
		    (c.c & _cblock << 12ULL) << 30ULL |
		    (c.c & _cblock << 24ULL) >> 12ULL |
		    (c.c & _cblock << 42ULL) >> 36ULL;

		ret.c = coapply(ret.c, _comask_l);

		return ret;
	case L2:
		ret.e = c.e & ~_emask_l;
		ret.e |=
		    (c.e & (_eblock << 25ULL | _eblock << 45ULL)) << 5ULL |
		    (c.e & (_eblock << 30ULL | _eblock << 50ULL)) >> 5ULL;

		ret.c = c.c & ~_cmask_l;
		ret.c |=
		    (c.c & _cblock << 6ULL) << 6ULL |
		    (c.c & _cblock << 12ULL) >> 6ULL |
		    (c.c & _cblock << 24ULL) << 18ULL |
		    (c.c & _cblock << 42ULL) >> 18ULL;

		return ret;
	case L3:
		ret.e = c.e & ~_emask_l;
		ret.e |=
		    (c.e & _eblock << 25ULL) << 25ULL |
		    (c.e & _eblock << 30ULL) << 15ULL |
		    (c.e & _eblock2 << 45ULL) >> 20ULL;

		ret.c = c.c & ~_cmask_l;
		ret.c |=
		    (c.c & _cblock << 6ULL) << 36ULL |
		    (c.c & _cblock << 12ULL) << 12ULL |
		    (c.c & _cblock << 24ULL) >> 18ULL |
		    (c.c & _cblock << 42ULL) >> 30ULL;

		ret.c = coapply(ret.c, _comask_l);

		return ret;
	case F:
		ret.e = c.e & ~_emask_f;
		ret.e |=
		    (c.e & _eblock) << 40ULL |
		    (c.e & _eblock << 15ULL) << 30ULL |
		    (c.e & _eblock << 40ULL) >> 25ULL |
		    (c.e & _eblock << 45ULL) >> 45ULL;

		ret.e ^= _eomask_f;

		ret.c = c.c & ~_cmask_f;
		ret.c |=
		    (c.c & _cblock) << 36ULL |
		    (c.c & (_cblock << 24ULL | _cblock << 36ULL)) >> 24ULL |
		    (c.c & _cblock << 12ULL) << 12ULL;

		ret.c = coapply(ret.c, _comask_f);
		
		return ret;
	case F2:
		ret.e = c.e & ~_emask_f;
		ret.e |=
		    (c.e & _eblock) << 15ULL |
		    (c.e & _eblock << 15ULL) >> 15ULL |
		    (c.e & _eblock << 40ULL) << 5ULL |
		    (c.e & _eblock << 45ULL) >> 5ULL;

		ret.c = c.c & ~_cmask_f;
		ret.c |=
		    (c.c & (_cblock | _cblock << 24ULL)) << 12ULL |
		    (c.c & (_cblock << 12ULL | _cblock << 36ULL)) >> 12ULL;

		return ret;
	case F3:
		ret.e = c.e & ~_emask_f;
		ret.e |=
		    (c.e & _eblock) << 45ULL |
		    (c.e & _eblock << 15ULL) << 25ULL |
		    (c.e & _eblock << 40ULL) >> 40ULL |
		    (c.e & _eblock << 45ULL) >> 30ULL;

		ret.e ^= _eomask_f;

		ret.c = c.c & ~_cmask_f;
		ret.c |=
		    (c.c & (_cblock | _cblock << 12ULL)) << 24ULL |
		    (c.c & _cblock << 24ULL) >> 12ULL |
		    (c.c & _cblock << 36ULL) >> 36ULL;

		ret.c = coapply(ret.c, _comask_f);
		
		return ret;
	case B:
		ret.e = c.e & ~_emask_b;
		ret.e |=
		    (c.e & _eblock2 << 5ULL) << 45ULL |
		    (c.e & _eblock << 50ULL) >> 40ULL |
		    (c.e & _eblock << 55ULL) >> 50ULL;

		ret.e ^= _eomask_b;

		ret.c = c.c & ~_cmask_b;
		ret.c |=
		    (c.c & _cblock << 6ULL) << 36ULL |
		    (c.c & _cblock << 18ULL) << 12ULL |
		    (c.c & (_cblock << 30ULL | _cblock << 42ULL)) >> 24ULL;

		ret.c = coapply(ret.c, _comask_b);

		return ret;
	case B2:
		ret.e = c.e & ~_emask_b;
		ret.e |=
		    (c.e & (_eblock << 5ULL | _eblock << 50ULL)) << 5ULL |
		    (c.e & (_eblock << 10ULL | _eblock << 55ULL)) >> 5ULL;

		ret.c = c.c & ~_cmask_b;
		ret.c |=
		    (c.c & (_cblock << 6ULL | _cblock << 30ULL)) << 12ULL |
		    (c.c & (_cblock << 18ULL | _cblock << 42ULL)) >> 12ULL;

		return ret;
	case B3:
		ret.e = c.e & ~_emask_b;
		ret.e |=
		    (c.e & _eblock << 5ULL) << 50ULL |
		    (c.e & _eblock << 10ULL) << 40ULL |
		    (c.e & _eblock2 << 50ULL) >> 45ULL;

		ret.e ^= _eomask_b;

		ret.c = c.c & ~_cmask_b;
		ret.c |=
		    (c.c & (_cblock << 6ULL | _cblock << 18ULL)) << 24ULL |
		    (c.c & _cblock << 30ULL) >> 12ULL |
		    (c.c & _cblock << 42ULL) >> 36ULL;

		ret.c = coapply(ret.c, _comask_b);

		return ret;
	default:
		goto move_unknown;
	}

move_unknown:
#ifdef DEBUG
	fprintf(stderr, "move error, unknow move\n");
#endif
	goto move_error;
move_error:
	return errorcube;
}
