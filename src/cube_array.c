/*
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
*/

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "constants.h"
#include "cube.h"

#define _c_ufr      0U
#define _c_ubl      1U
#define _c_dfl      2U
#define _c_dbr      3U
#define _c_ufl      4U
#define _c_ubr      5U
#define _c_dfr      6U
#define _c_dbl      7U

#define _e_uf       0U
#define _e_ub       1U
#define _e_db       2U
#define _e_df       3U
#define _e_ur       4U
#define _e_ul       5U
#define _e_dl       6U
#define _e_dr       7U
#define _e_fr       8U
#define _e_fl       9U
#define _e_bl       10U
#define _e_br       11U

#define _eoshift    4U
#define _coshift    5U

#define _pbits      0xFU
#define _eobit      0x10U
#define _cobits     0xF0U
#define _cobits2    0x60U
#define _ctwist_cw  0x20U
#define _ctwist_ccw 0x40U
#define _eflip      0x10U
#define _error      0xFFU

static char *cornerstr[] = {
	[_c_ufr] = "UFR",
	[_c_ubl] = "UBL",
	[_c_dfl] = "DFL",
	[_c_dbr] = "DBR",
	[_c_ufl] = "UFL",
	[_c_ubr] = "UBR",
	[_c_dfr] = "DFR",
	[_c_dbl] = "DBL"
};

static char *cornerstralt[] = {
	[_c_ufr] = "URF",
	[_c_ubl] = "ULB",
	[_c_dfl] = "DLF",
	[_c_dbr] = "DRB",
	[_c_ufl] = "ULF",
	[_c_ubr] = "URB",
	[_c_dfr] = "DRF",
	[_c_dbl] = "DLB"
};

static char *edgestr[] = {
	[_e_uf] = "UF",
	[_e_ub] = "UB",
	[_e_db] = "DB",
	[_e_df] = "DF",
	[_e_ur] = "UR",
	[_e_ul] = "UL",
	[_e_dl] = "DL",
	[_e_dr] = "DR",
	[_e_fr] = "FR",
	[_e_fl] = "FL",
	[_e_bl] = "BL",
	[_e_br] = "BR"
};

static char *movestr[] = {
	[U]  = "U",
	[U2] = "U2",
	[U3] = "U'",
	[D]  = "D",
	[D2] = "D2",
	[D3] = "D'",
	[R]  = "R",
	[R2] = "R2",
	[R3] = "R'",
	[L]  = "L",
	[L2] = "L2",
	[L3] = "L'",
	[F]  = "F",
	[F2] = "F2",
	[F3] = "F'",
	[B]  = "B",
	[B2] = "B2",
	[B3] = "B'",
};

cube_t solvedcube = {
	.c = {
		[_c_ufr] = _c_ufr,
		[_c_ubl] = _c_ubl,
		[_c_dfl] = _c_dfl,
		[_c_dbr] = _c_dbr,
		[_c_ufl] = _c_ufl,
		[_c_ubr] = _c_ubr,
		[_c_dfr] = _c_dfr,
		[_c_dbl] = _c_dbl
	},
	.e = {
		[_e_uf] = _e_uf,
		[_e_ub] = _e_ub,
		[_e_db] = _e_db,
		[_e_df] = _e_df,
		[_e_ur] = _e_ur,
		[_e_ul] = _e_ul,
		[_e_dl] = _e_dl,
		[_e_dr] = _e_dr,
		[_e_fr] = _e_fr,
		[_e_fl] = _e_fl,
		[_e_bl] = _e_bl,
		[_e_br] = _e_br
	}
};

static cube_t errorcube = { .e = {0}, .c = {0} };

static bool isconsistent(cube_t);
static uint8_t readco(char *);
static uint8_t readcp(char *);
static uint8_t readeo(char *);
static uint8_t readep(char *);
static uint8_t readmove(char);
static uint8_t readmodifier(char);
static int permsign(uint8_t *, int);

static uint8_t
readco(char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _ctwist_cw;
	if (*str == '2')
		return _ctwist_ccw;

#ifdef DEBUG
	fprintf(stderr, "Error reading CO\n");
#endif
	return _error;
}

static uint8_t
readcp(char *str)
{
	uint8_t c;

	for (c = 0; c < 8; c++)
		if (!strncmp(str, cornerstr[c], 3) ||
		    !strncmp(str, cornerstralt[c], 3))
			return c;

#ifdef DEBUG
	fprintf(stderr, "Error reading CP\n");
#endif
	return _error;
}

static uint8_t
readeo(char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _eflip;

#ifdef DEBUG
	fprintf(stderr, "Error reading EO\n");
#endif
	return _error;
}

static uint8_t
readep(char *str)
{
	uint8_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

#ifdef DEBUG
	fprintf(stderr, "Error reading EP\n");
#endif
	return _error;
}

cube_t
readcube(char *buf)
{
	int i;
	uint8_t piece, orient;
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
		ret.e[i] = piece | orient;
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
		ret.c[i] = piece | orient;
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
	uint8_t piece, orient;
	size_t len;
	int i;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	for (i = 0; i < 12; i++) {
		piece = cube.e[i] & _pbits;
		orient = (cube.e[i] & _eobit) >> _eoshift;
		buf[4*i    ] = edgestr[piece][0];
		buf[4*i + 1] = edgestr[piece][1];
		buf[4*i + 2] = orient + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = cube.c[i] & _pbits;
		orient = (cube.c[i] & _cobits) >> _coshift;
		buf[48 + 5*i    ] = cornerstr[piece][0];
		buf[48 + 5*i + 1] = cornerstr[piece][1];
		buf[48 + 5*i + 2] = cornerstr[piece][2];
		buf[48 + 5*i + 3] = orient + '0';
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


static uint8_t
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

static uint8_t
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


static int
permsign(uint8_t *a, int n)
{
	int i, j;
	uint8_t ret = 0;

	for (i = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += (a[i] & _pbits) > (a[j] & _pbits) ? 1 : 0;

	return ret % 2;
}

static bool
isconsistent(cube_t c)
{
	uint8_t i, p, e;
	bool found[12];

	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		p = c.e[i] & _pbits;
		e = c.e[i] & ~_pbits;
		if (p >= 12)
			goto inconsistent_ep;
		if (e != 0 && e != _eobit)
			goto inconsistent_eo;
		found[p] = true;
	}
	for (i = 0; i < 12; i++)
		if (!found[i])
			goto inconsistent_ep;

	for (i = 0; i < 8; i++)
		found[i] = false;
	for (i = 0; i < 8; i++) {
		p = c.c[i] & _pbits;
		e = c.c[i] & ~_pbits;
		if (p >= 8)
			goto inconsistent_cp;
		if (e != 0 && e != _ctwist_cw && e != _ctwist_ccw)
			goto inconsistent_co;
		found[p] = true;
	}
	for (i = 0; i < 8; i++)
		if (!found[i])
			goto inconsistent_co;

	return true;

inconsistent_ep:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent EP\n");
#endif
	return false;
inconsistent_cp:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent CP\n");
#endif
	return false;
inconsistent_eo:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent EO\n");
#endif
	return false;
inconsistent_co:
#ifdef DEBUG
	fprintf(stderr, "Inconsistent CO\n");
#endif
	return false;
}

bool
issolvable(cube_t cube)
{
	int8_t i, eo, co;

#ifdef DEBUG
	if (!isconsistent(cube))
		goto issolvable_inconsistent;
#endif

	if (permsign(cube.e, 12) != permsign(cube.c, 8))
		goto issolvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++)
		eo += (cube.e[i] & _eobit) >> _eoshift;
	if (eo % 2 != 0)
		goto issolvable_eo;

	co = 0;
	for (i = 0; i < 8; i++)
		co += (cube.c[i] & _cobits) >> _coshift;
	if (co % 3 != 0)
		goto issolvable_co;

	return true;

issolvable_inconsistent:
#ifdef DEBUG
	fprintf(stderr, "issolvable: cube is inconsistent\n");
#endif
	return false;
issolvable_parity:
#ifdef DEBUG
	fprintf(stderr, "EP and CP parities are different\n");
#endif
	return false;
issolvable_eo:
#ifdef DEBUG
	fprintf(stderr, "Odd number of flipped edges\n");
#endif
	return false;
issolvable_co:
#ifdef DEBUG
	fprintf(stderr, "Sum of corner orientation is not multiple of 3\n");
#endif
	return false;
}

bool
equal(cube_t cube1, cube_t cube2)
{
	uint8_t i;

	for (i = 0; i < 12; i++)
		if (cube1.e[i] != cube2.e[i])
			return false;

	for (i = 0; i < 8; i++)
		if (cube1.c[i] != cube2.c[i])
			return false;

	return true;
}

bool
issolved(cube_t cube)
{
	return equal(cube, solvedcube);
}

bool
iserror(cube_t cube)
{
	return equal(cube, errorcube);
}

cube_t
move(cube_t c, move_t m)
{
	cube_t ret;
	uint8_t aux, auy, auz;

#ifdef DEBUG
	if (!isconsistent(c))
		goto move_inconsistent;
#endif

#define PERM4(r, i, j, k, l) \
    aux = r[i];              \
    r[i] = r[l];             \
    r[l] = r[k];             \
    r[k] = r[j];             \
    r[j] = aux;
#define PERM22(r, i, j, k, l) \
    aux = r[i];               \
    r[i] = r[j];              \
    r[j] = aux;               \
    aux = r[k];               \
    r[k] = r[l];              \
    r[l] = aux;
#define CO(a, b)                         \
    aux = (a & _cobits) + (b & _cobits); \
    auy = (aux + _ctwist_cw) >> 2U;      \
    auz = (aux + auy) & _cobits2;        \
    a = (a & _pbits) | auz;
#define CO4(r, i, j, k, l) \
    CO(r[i], _ctwist_cw)   \
    CO(r[j], _ctwist_cw)   \
    CO(r[k], _ctwist_ccw)  \
    CO(r[l], _ctwist_ccw)
#define EO4(r, i, j, k, l) \
    r[i] ^= _eobit;        \
    r[j] ^= _eobit;        \
    r[k] ^= _eobit;        \
    r[l] ^= _eobit;

	ret = c;

	switch (m) {
	case U:
		PERM4(ret.e, _e_uf, _e_ul, _e_ub, _e_ur)
		PERM4(ret.c, _c_ufr, _c_ufl, _c_ubl, _c_ubr)

		return ret;
	case U2:
		PERM22(ret.e, _e_uf, _e_ub, _e_ul, _e_ur)
		PERM22(ret.c, _c_ufr, _c_ubl, _c_ufl, _c_ubr)

		return ret;
	case U3:
		PERM4(ret.e, _e_uf, _e_ur, _e_ub, _e_ul)
		PERM4(ret.c, _c_ufr, _c_ubr, _c_ubl, _c_ufl)

		return ret;
	case D:
		PERM4(ret.e, _e_df, _e_dr, _e_db, _e_dl)
		PERM4(ret.c, _c_dfr, _c_dbr, _c_dbl, _c_dfl)

		return ret;
	case D2:
		PERM22(ret.e, _e_df, _e_db, _e_dr, _e_dl)
		PERM22(ret.c, _c_dfr, _c_dbl, _c_dbr, _c_dfl)

		return ret;
	case D3:
		PERM4(ret.e, _e_df, _e_dl, _e_db, _e_dr)
		PERM4(ret.c, _c_dfr, _c_dfl, _c_dbl, _c_dbr)

		return ret;
	case R:
		PERM4(ret.e, _e_ur, _e_br, _e_dr, _e_fr)
		PERM4(ret.c, _c_ufr, _c_ubr, _c_dbr, _c_dfr)

		CO4(ret.c, _c_ubr, _c_dfr, _c_ufr, _c_dbr)

		return ret;
	case R2:
		PERM22(ret.e, _e_ur, _e_dr, _e_fr, _e_br)
		PERM22(ret.c, _c_ufr, _c_dbr, _c_ubr, _c_dfr)

		return ret;
	case R3:
		PERM4(ret.e, _e_ur, _e_fr, _e_dr, _e_br)
		PERM4(ret.c, _c_ufr, _c_dfr, _c_dbr, _c_ubr)

		CO4(ret.c, _c_ubr, _c_dfr, _c_ufr, _c_dbr)

		return ret;
	case L:
		PERM4(ret.e, _e_ul, _e_fl, _e_dl, _e_bl)
		PERM4(ret.c, _c_ufl, _c_dfl, _c_dbl, _c_ubl)

		CO4(ret.c, _c_ufl, _c_dbl, _c_dfl, _c_ubl)

		return ret;
	case L2:
		PERM22(ret.e, _e_ul, _e_dl, _e_fl, _e_bl)
		PERM22(ret.c, _c_ufl, _c_dbl, _c_ubl, _c_dfl)

		return ret;
	case L3:
		PERM4(ret.e, _e_ul, _e_bl, _e_dl, _e_fl)
		PERM4(ret.c, _c_ufl, _c_ubl, _c_dbl, _c_dfl)

		CO4(ret.c, _c_ufl, _c_dbl, _c_dfl, _c_ubl)

		return ret;
	case F:
		PERM4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
		PERM4(ret.c, _c_ufr, _c_dfr, _c_dfl, _c_ufl)

		EO4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
		CO4(ret.c, _c_ufr, _c_dfl, _c_dfr, _c_ufl)

		return ret;
	case F2:
		PERM22(ret.e, _e_uf, _e_df, _e_fr, _e_fl)
		PERM22(ret.c, _c_ufr, _c_dfl, _c_ufl, _c_dfr)

		return ret;
	case F3:
		PERM4(ret.e, _e_uf, _e_fl, _e_df, _e_fr)
		PERM4(ret.c, _c_ufr, _c_ufl, _c_dfl, _c_dfr)

		EO4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
		CO4(ret.c, _c_ufr, _c_dfl, _c_dfr, _c_ufl)

		return ret;
	case B:
		PERM4(ret.e, _e_ub, _e_bl, _e_db, _e_br)
		PERM4(ret.c, _c_ubr, _c_ubl, _c_dbl, _c_dbr)

		EO4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
		CO4(ret.c, _c_ubl, _c_dbr, _c_dbl, _c_ubr)

		return ret;
	case B2:
		PERM22(ret.e, _e_ub, _e_db, _e_br, _e_bl)
		PERM22(ret.c, _c_ubr, _c_dbl, _c_ubl, _c_dbr)

		return ret;
	case B3:
		PERM4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
		PERM4(ret.c, _c_ubr, _c_dbr, _c_dbl, _c_ubl)

		EO4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
		CO4(ret.c, _c_ubl, _c_dbr, _c_dbl, _c_ubr)

		return ret;
	default:
		goto move_unknown;
	}

move_inconsistent:
	fprintf(stderr, "move error, inconsistent cube\n");
	goto move_error;
move_unknown:
	fprintf(stderr, "mover error, unknown move\n");
	goto move_error;
move_error:
	return errorcube;
}

cube_t
inverse(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret = {0};

#ifdef DEBUG
	if (!isconsistent(c))
		goto inverse_inconsistent;
#endif

	for (i = 0; i < 12; i++) {
		piece = c.e[i & _pbits];
		orien = piece & _eobit;
		ret.e[piece & _pbits] = i | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = c.c[i & _pbits];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.c[piece & _pbits] = i | orien;
	}

	return ret;

inverse_inconsistent:
	fprintf(stderr, "inverse error, inconsistent cube\n");
	return errorcube;
}

cube_t
compose(cube_t c1, cube_t c2)
{
	uint8_t i, piece, orien, aux, auy;
	cube_t ret = {0};

#ifdef DEBUG
	if (!isconsistent(c1) || !isconsistent(c2))
		goto compose_inconsistent;
#endif

	for (i = 0; i < 12; i++) {
		piece = c2.e[i] & _pbits;
		orien = (c2.e[i] ^ c1.e[piece]) & _eobit;
		ret.e[i] = (c1.e[piece] & _pbits) | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = c2.c[i] & _pbits;
		aux = (c2.c[i] & _cobits) + (c1.c[piece] & _cobits);
		auy = (aux + _ctwist_cw) >> 2U;
		orien = (aux + auy) & _cobits2;
		ret.c[i] = (c1.c[piece] & _pbits) | orien;
	}

	return ret;

compose_inconsistent:
	fprintf(stderr, "compose error, inconsistent cube\n");
	return errorcube;
}

cube_t
transform(cube_t c, trans_t t)
{
	return errorcube;
}
