#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef CUBE_AVX2
#include <immintrin.h>
#endif

#ifdef DEBUG
#include <stdio.h>
#endif

#include "cube.h"

#include "_constants.c"

cube_arr_t solvedcube_arr = {
	.c = {0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0},
	.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}
};
cube_arr_t zerocube_arr = { .e = {0}, .c = {0} };

#ifdef CUBE_AVX2

#define setsolved(cube) cube = _mm256_loadu_si256((__m256i_u *)&solvedcube_arr)
#define setzero(cube)   cube = _mm256_setzero_si256()
#define _c _mm256_set_epi64x(0, 0, 0, 0xFF)
#define _e _mm256_set_epi64x(0, 0xFF, 0, 0)
/* TODO: in the next 4 macros, all 0 should be i, but must be constant! */
#define get_edge(cube, i)    _mm256_extract_epi8(cube, 0+16)
#define get_corner(cube, i)  _mm256_extract_epi8(cube, 0)
#define set_edge(cube, i, p) _mm256_or_si256( \
    _mm256_andnot_si256(cube, _mm256_slli_si256(_e, 0)), \
    _mm256_and_si256(_mm256_set1_epi8(p), _mm256_slli_si256(_e, 0)))
#define set_corner(cube, i, p) _mm256_or_si256( \
    _mm256_andnot_si256(cube, _mm256_slli_si256(_c, 0)), \
    _mm256_and_si256(_mm256_set1_epi8(p), _mm256_slli_si256(_c, 0)))

#else

#define setsolved(cube) cube = solvedcube_arr
#define setzero(cube)   cube = zerocube_arr
#define _c _mm256_set_epi64x(0, 0, 0, 0xFF)
#define get_edge(cube, i)      (cube).e[(i)]
#define get_corner(cube, i)    (cube).c[(i)]
#define set_edge(cube, i, p)   (cube).e[(i)] = (p)
#define set_corner(cube, i, p) (cube).c[(i)] = (p)

#endif

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

cube_t
solvedcube(void)
{
	cube_t solved;
	setsolved(solved);
	return solved;
}

cube_t
zerocube(void)
{
	cube_t solved;
	setzero(solved);
	return solved;
}

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
	cube_t ret, err;
	char *b;
	
	setzero(ret);
	setzero(err);
	b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			return err;
		b += 2;
		if ((orient = readeo(b)) == _error)
			return err;
		b++;
		set_edge(ret, i, piece | orient);
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			return err;
		b += 3;
		if ((orient = readco(b)) == _error)
			return err;
		b++;
		set_corner(ret, i, piece | orient);
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
		setzero(ret);
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
	uint8_t piece, perm, orient;
	int i;

	for (i = 0; i < 12; i++) {
		piece = get_edge(cube, i);
		perm = piece & _pbits;
		orient = (piece & _eobit) >> _eoshift;
		buf[4*i    ] = edgestr[perm][0];
		buf[4*i + 1] = edgestr[perm][1];
		buf[4*i + 2] = orient + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = get_corner(cube, i);
		perm = piece & _pbits;
		orient = (piece & _cobits) >> _coshift;
		buf[48 + 5*i    ] = cornerstr[perm][0];
		buf[48 + 5*i + 1] = cornerstr[perm][1];
		buf[48 + 5*i + 2] = cornerstr[perm][2];
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
	uint8_t piece;

	memcpy(buf, "{\n\t.c = {", 9);
	ptr = 9;

	for (i = 0; i < 8; i++) {
		piece = get_corner(cube, i);
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, "},\n\t.e = {", 10);
	ptr += 8;

	for (i = 0; i < 12; i++) {
		piece = get_edge(cube, i);
		ptr += writepiece_SRC(piece, buf + ptr);
	}

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

move_t
inverse_move(move_t m)
{
	return inverse_move_arr[m];
}

trans_t
inverse_trans(trans_t t)
{
	return inverse_trans_arr[t];
}

static int
permsign(uint8_t *a, int n)
{
	int i, j;
	uint8_t ret = 0;

	for (i = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

static bool
isconsistent(cube_t c)
{
	uint8_t i, p, e, piece;
	bool found[12];

	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		piece = get_edge(c, i);
		p = piece & _pbits;
		e = piece & _eobit;
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
		piece = get_corner(c, i);
		p = piece & _pbits;
		e = piece & _cobits;
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
	uint8_t i, eo, co, piece, e[12], c[8];

#ifdef DEBUG
	if (!isconsistent(cube)) {
		fprintf(stderr, "issolvable: cube is inconsistent\n");
		return false;
	}
#endif

	for (i = 0; i < 12; i++)
		e[i] = get_edge(cube, i) & _pbits;
	for (i = 0; i < 8; i++)
		c[i] = get_corner(cube, i) & _pbits;

	if (permsign(e, 12) != permsign(c, 8))
		goto issolvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++) {
		piece = get_edge(cube, i);
		eo += (piece & _eobit) >> _eoshift;
	}
	if (eo % 2 != 0)
		goto issolvable_eo;

	co = 0;
	for (i = 0; i < 8; i++) {
		piece = get_corner(cube, i);
		co += (piece & _cobits) >> _coshift;
	}
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
#ifdef CUBE_AVX2
	uint32_t mask;
	__m256i cmp;

	cmp = _mm256_cmpeq_epi8(cube1, cube2);
	mask = _mm256_movemask_epi8(cmp);

	return mask == 0xffffffffU;
#else
	uint8_t i;

	for (i = 0; i < 12; i++)
		if (cube1.e[i] != cube2.e[i])
			return false;

	for (i = 0; i < 8; i++)
		if (cube1.c[i] != cube2.c[i])
			return false;

	return true;
#endif
}

bool
issolved(cube_t cube)
{
	cube_t solved;
	setsolved(solved);
	return equal(cube, solved);
}

bool
iserror(cube_t cube)
{
	cube_t err;
	setzero(err);
	return equal(cube, err);
}

cube_t
move(cube_t c, move_t m)
{
	cube_t err;

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "move error, inconsistent cube\n");
		goto move_error;
	}
#endif

#ifdef CUBE_AVX2
#include "_move_logic_avx2.c"
#else
#include "_move_logic_arr.c"
#endif

move_error:
	setzero(err);
	return err;
}

cube_t
inverse(cube_t c)
{
	/* TODO: optimize for avx2 */
	uint8_t i, piece, orien;
	cube_t ret;

	setzero(ret);

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "inverse error, inconsistent cube\n");
		return ret;
	}
#endif

	for (i = 0; i < 12; i++) {
		piece = get_edge(c, i);
		orien = piece & _eobit;
		set_edge(ret, piece & _pbits, i | orien);
	}

	for (i = 0; i < 8; i++) {
		piece = get_corner(c, i);
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		set_corner(ret, piece & _pbits, i | orien);
	}

	return ret;
}

cube_t
compose(cube_t c1, cube_t c2)
{
	/* TODO: optimize for avx2 */
	uint8_t i, piece1, piece2, p, orien, aux, auy;
	cube_t ret;

	setzero(ret);

#ifdef DEBUG
	if (!isconsistent(c1) || !isconsistent(c2)) {
		fprintf(stderr, "compose error, inconsistent cube\n");
		return ret;
	}
#endif

	for (i = 0; i < 12; i++) {
		piece2 = get_edge(c2, i);
		p = piece2 & _pbits;
		piece1 = get_edge(c1, p);
		orien = (piece2 ^ piece1) & _eobit;
		set_edge(ret, i, (piece1 & _pbits) | orien);
	}

	for (i = 0; i < 8; i++) {
		piece2 = get_corner(c2, i);
		p = piece2 & _pbits;
		piece1 = get_corner(c1, p);
		aux = (piece2 & _cobits) + (piece1 & _cobits);
		auy = (aux + _ctwist_cw) >> 2U;
		orien = (aux + auy) & _cobits2;
		set_corner(ret, i, (piece1 & _pbits) | orien);
	}

	return ret;
}

static cube_t
flipallcorners(cube_t c)
{
/* TODO: optimize for avx2, can be a couple of instructions */

	uint8_t i, piece, orien;
	cube_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = get_corner(c, i);
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		set_corner(ret, i, (piece & _pbits) | orien);
	}

	return ret;
}

cube_t
transform(cube_t c, trans_t t)
{
	cube_t ret, solved;

#ifdef DEBUG
	setzero(ret);
	if (!isconsistent(c)) {
		fprintf(stderr, "transform error, inconsistent cube\n");
		return ret;
	}
	if (t >= 48) {
		fprintf(stderr, "transform error, unknown transformation\n");
		return ret;
	}
#endif

#ifdef CUBE_AVX2
#include "_trans_move_avx2.c"
#else
#include "_trans_move_arr.c"
#endif

	setsolved(solved);

	ret = compose(solved, trans_move_cube[t]);
	ret = compose(ret, c);
	ret = compose(ret, trans_move_cube_inverse[t]);

	/* TODO: work out a better way to do this */
	if (t >= 24)
		ret = flipallcorners(ret);

	return ret;
}
