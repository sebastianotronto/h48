#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "cube.h"

#include "_constants.c"

static bool isconsistent(cube_t);
static cube_t flipallcorners(cube_t);
static uint8_t readco(char *);
static uint8_t readcp(char *);
static uint8_t readeo(char *);
static uint8_t readep(char *);
static uint8_t readmove(char);
static uint8_t readmodifier(char);
static cube_t readcube_H48(char *);
static void writecube_H48(cube_t, char *);
static int writepiece_SRC(uint8_t, char *);
static void writecube_SRC(cube_t, char *);
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

static cube_t
readcube_H48(char *buf)
{
	int i;
	uint8_t piece, orient;
	cube_t ret = {0};
	char *b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			return errorcube;
		b += 2;
		if ((orient = readeo(b)) == _error)
			return errorcube;
		b++;
		ret.e[i] = piece | orient;
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			return errorcube;
		b += 3;
		if ((orient = readco(b)) == _error)
			return errorcube;
		b++;
		ret.c[i] = piece | orient;
	}

	return ret;
}

cube_t
readcube(format_t format, char *buf)
{
	cube_t ret;

	switch (format) {
	case H48:
		ret = readcube_H48(buf);
		break;
	default:
	#ifdef DEBUG
		fprintf(stderr, "Cannot read cube in the given format\n");
	#endif
		ret = errorcube;
	}

#ifdef DEBUG
	if (iserror(ret))
		fprintf(stderr, "readcube error\n");
#endif
	return ret;
}

static void
writecube_H48(cube_t cube, char *buf)
{
	uint8_t piece, orient;
	int i;

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
}

static int
writepiece_SRC(uint8_t piece, char *buf)
{
	char digits[3];
	int i, len = 0;

	while (piece != 0) {
		digits[len++] = (piece % 10) + '0';
		piece /= 10;
	}

	if (len == 0)
		digits[len++] = '0';

	for (i = 0; i < len; i++)
		buf[i] = digits[len-i-1];

	buf[len] = ',';
	buf[len+1] = ' ';

	return len+2;
}

static void
writecube_SRC(cube_t cube, char *buf)
{
	int i, ptr;

	memcpy(buf, "{\n\t.c = {", 9);
	ptr = 9;

	for (i = 0; i < 8; i++)
		ptr += writepiece_SRC(cube.c[i], buf + ptr);

	memcpy(buf+ptr-2, "},\n\t.e = {", 10);
	ptr += 8;

	for (i = 0; i < 12; i++)
		ptr += writepiece_SRC(cube.e[i], buf + ptr);

	memcpy(buf+ptr-2, "}\n}\0", 4);
}

void
writecube(format_t format, cube_t cube, char *buf)
{
	char *errormsg;
	size_t len;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	switch (format) {
	case H48:
		writecube_H48(cube, buf);
		break;
	case SRC:
		writecube_SRC(cube, buf);
		break;
	default:
		errormsg = "ERROR: cannot write cube in the given format";
		goto writecube_error;
	}

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

trans_t
readtrans(char *buf)
{
	uint8_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

#ifdef DEBUG
	fprintf(stderr, "readtrans error\n");
#endif
	return errortrans;
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

void
writetrans(trans_t t, char *buf)
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
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
	if (!isconsistent(cube)) {
		fprintf(stderr, "issolvable: cube is inconsistent\n");
		return false;
	}
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
	if (!isconsistent(c)) {
		fprintf(stderr, "move error, inconsistent cube\n");
		goto move_error;
	}
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
#ifdef DEBUG
		fprintf(stderr, "mover error, unknown move\n");
#endif
		goto move_error;
	}

move_error:
	return errorcube;
}

cube_t
inverse(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret = {0};

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "inverse error, inconsistent cube\n");
		return errorcube;
	}
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
}

cube_t
compose(cube_t c1, cube_t c2)
{
	uint8_t i, piece, orien, aux, auy;
	cube_t ret = {0};

#ifdef DEBUG
	if (!isconsistent(c1) || !isconsistent(c2)) {
		fprintf(stderr, "compose error, inconsistent cube\n");
		return errorcube;
	}
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
}

static cube_t
flipallcorners(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = ret.c[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.c[i] = (piece & _pbits) | orien;
	}

	return ret;
}

cube_t
transform(cube_t c, trans_t t)
{
#include "_trans_move_arr.c"

	cube_t ret;

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "transform error, inconsistent cube\n");
		return errorcube;
	}
	if (t >= 48) {
		fprintf(stderr, "transform error, unknown transformation\n");
		return errorcube;
	}
#endif

	ret = compose(solvedcube, trans_move_cube[t]);
	ret = compose(ret, c);
	ret = compose(ret, trans_move_cube_inverse[t]);

	/* TODO: work out a better way to do this */
	if (t >= 24)
		ret = flipallcorners(ret);

	return ret;
}
