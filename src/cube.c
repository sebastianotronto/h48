#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "cube.h"

#define U  0U
#define U2 1U
#define U3 2U
#define D  3U
#define D2 4U
#define D3 5U
#define R  6U
#define R2 7U
#define R3 8U
#define L  9U
#define L2 10U
#define L3 11U
#define F  12U
#define F2 13U
#define F3 14U
#define B  15U
#define B2 16U
#define B3 17U

#define UFr 0
#define ULr 1
#define UBr 2
#define URr 3
#define DFr 4
#define DLr 5
#define DBr 6
#define DRr 7
#define RUr 8
#define RFr 9
#define RDr 10
#define RBr 11
#define LUr 12
#define LFr 13
#define LDr 14
#define LBr 15
#define FUr 16
#define FRr 17
#define FDr 18
#define FLr 19
#define BUr 20
#define BRr 21
#define BDr 22
#define BLr 23

#define UFm 24
#define ULm 25
#define UBm 26
#define URm 27
#define DFm 28
#define DLm 29
#define DBm 30
#define DRm 31
#define RUm 32
#define RFm 33
#define RDm 34
#define RBm 35
#define LUm 36
#define LFm 37
#define LDm 38
#define LBm 39
#define FUm 40
#define FRm 41
#define FDm 42
#define FLm 43
#define BUm 44
#define BRm 45
#define BDm 46
#define BLm 47

#define errormove 99U
#define errortrans 99U

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

#define get_edge(cube, i)      (cube).e[(i)]
#define get_corner(cube, i)    (cube).c[(i)]
#define set_edge(cube, i, p)   (cube).e[(i)] = (p)
#define set_corner(cube, i, p) (cube).c[(i)] = (p)

cube_arr_t solvedcube_arr = {
	.c = {0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0},
	.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}
};
cube_arr_t zerocube_arr = { .e = {0}, .c = {0} };

#ifdef CUBE_AVX2

#define _co_avx2 _mm256_set_epi8( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, \
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0) 
#define _co2_avx2 _mm256_set_epi8( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, \
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60) 
#define _eo_avx2 _mm256_set_epi8( \
    0, 0, 0, 0, 0x10, 0x10, 0x10, 0x10, \
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define setsolved(cube) cube = _mm256_loadu_si256((__m256i_u *)&solvedcube_arr)
#define setzero(cube)   cube = _mm256_setzero_si256()

#include "_moves_avx2.c"
#include "_trans_avx2.c"

static cube_t
arrtocube(cube_arr_t a)
{
	return _mm256_loadu_si256((__m256i_u *)&a);
}

static void
cubetoarr(cube_t c, cube_arr_t *a)
{
	_mm256_storeu_si256((__m256i_u *)a, c);
}

#else

#define setsolved(cube) cube = solvedcube_arr
#define setzero(cube)   cube = zerocube_arr

static cube_t
arrtocube(cube_arr_t a)
{
	return a;
}

static void
cubetoarr(cube_t c, cube_arr_t *a)
{
	memcpy(a, &c, sizeof(cube_t));
}

#include "_moves_arr.c"
#include "_trans_arr.c"

#endif

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

static char *transstr[] = {
	[UFr] = "rotation UF",
	[UFm] = "mirrored UF",
	[ULr] = "rotation UL",
	[ULm] = "mirrored UL",
	[UBr] = "rotation UB",
	[UBm] = "mirrored UB",
	[URr] = "rotation UR",
	[URm] = "mirrored UR",
	[DFr] = "rotation DF",
	[DFm] = "mirrored DF",
	[DLr] = "rotation DL",
	[DLm] = "mirrored DL",
	[DBr] = "rotation DB",
	[DBm] = "mirrored DB",
	[DRr] = "rotation DR",
	[DRm] = "mirrored DR",
	[RUr] = "rotation RU",
	[RUm] = "mirrored RU",
	[RFr] = "rotation RF",
	[RFm] = "mirrored RF",
	[RDr] = "rotation RD",
	[RDm] = "mirrored RD",
	[RBr] = "rotation RB",
	[RBm] = "mirrored RB",
	[LUr] = "rotation LU",
	[LUm] = "mirrored LU",
	[LFr] = "rotation LF",
	[LFm] = "mirrored LF",
	[LDr] = "rotation LD",
	[LDm] = "mirrored LD",
	[LBr] = "rotation LB",
	[LBm] = "mirrored LB",
	[FUr] = "rotation FU",
	[FUm] = "mirrored FU",
	[FRr] = "rotation FR",
	[FRm] = "mirrored FR",
	[FDr] = "rotation FD",
	[FDm] = "mirrored FD",
	[FLr] = "rotation FL",
	[FLm] = "mirrored FL",
	[BUr] = "rotation BU",
	[BUm] = "mirrored BU",
	[BRr] = "rotation BR",
	[BRm] = "mirrored BR",
	[BDr] = "rotation BD",
	[BDm] = "mirrored BD",
	[BLr] = "rotation BL",
	[BLm] = "mirrored BL",
};

static inline cube_t inline_compose(cube_t, cube_t);
static bool isconsistent(cube_t);
static cube_t flipallcorners(cube_t);
static uint8_t readco(char *);
static uint8_t readcp(char *);
static uint8_t readeo(char *);
static uint8_t readep(char *);
static uint8_t readmove(char);
static uint8_t readmodifier(char);
static cube_t readcube_H48(char *);
static void writecube_AVX(cube_arr_t, char *);
static void writecube_H48(cube_arr_t, char *);
static int writepiece_SRC(uint8_t, char *);
static void writecube_SRC(cube_arr_t, char *);
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
	cube_arr_t ret = {0};
	cube_t err;
	char *b;
	
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

	return arrtocube(ret);
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
writecube_AVX(cube_arr_t cube, char *buf)
{
	int i, ptr;
	uint8_t piece;

	memcpy(buf, "_mm256_set_epi8(\n\t0, 0, 0, 0, ", 30);
	ptr = 30;

	for (i = 11; i >= 0; i--) {
		piece = get_edge(cube, i);
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, ",\n\t0, 0, 0, 0, 0, 0, 0, 0, ", 27);
	ptr += 25;

	for (i = 7; i >= 0; i--) {
		piece = get_corner(cube, i);
		ptr += writepiece_SRC(piece, buf + ptr);
	}

	memcpy(buf+ptr-2, "\n)\0", 3);
}

static void
writecube_H48(cube_arr_t cube, char *buf)
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
writecube_SRC(cube_arr_t cube, char *buf)
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
	cube_arr_t a;
	char *errormsg;
	size_t len;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	cubetoarr(cube, &a);

	switch (format) {
	case AVX:
		writecube_AVX(a, buf);
		break;
	case H48:
		writecube_H48(a, buf);
		break;
	case SRC:
		writecube_SRC(a, buf);
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
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

static bool
isconsistent(cube_t cube)
{
	cube_arr_t c;
	uint8_t i, p, e, piece;
	bool found[12];

	cubetoarr(cube, &c);

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
	cube_arr_t c;
	uint8_t i, eo, co, piece, edges[12], corners[8];

#ifdef DEBUG
	if (!isconsistent(cube)) {
		fprintf(stderr, "issolvable: cube is inconsistent\n");
		return false;
	}
#endif

	cubetoarr(cube, &c);

	for (i = 0; i < 12; i++)
		edges[i] = get_edge(c, i) & _pbits;
	for (i = 0; i < 8; i++)
		corners[i] = get_corner(c, i) & _pbits;

	if (permsign(edges, 12) != permsign(corners, 8))
		goto issolvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++) {
		piece = get_edge(c, i);
		eo += (piece & _eobit) >> _eoshift;
	}
	if (eo % 2 != 0)
		goto issolvable_eo;

	co = 0;
	for (i = 0; i < 8; i++) {
		piece = get_corner(c, i);
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

	switch (m) {
	case U:
		return inline_move_U(c);
	case U2:
		return inline_move_U2(c);
	case U3:
		return inline_move_U3(c);
	case D:
		return inline_move_D(c);
	case D2:
		return inline_move_D2(c);
	case D3:
		return inline_move_D3(c);
	case R:
		return inline_move_R(c);
	case R2:
		return inline_move_R2(c);
	case R3:
		return inline_move_R3(c);
	case L:
		return inline_move_L(c);
	case L2:
		return inline_move_L2(c);
	case L3:
		return inline_move_L3(c);
	case F:
		return inline_move_F(c);
	case F2:
		return inline_move_F2(c);
	case F3:
		return inline_move_F3(c);
	case B:
		return inline_move_B(c);
	case B2:
		return inline_move_B2(c);
	case B3:
		return inline_move_B3(c);
	default:
#ifdef DEBUG
		fprintf(stderr, "mover error, unknown move\n");
#endif
		goto move_error;
	}

move_error:
	setzero(err);
	return err;
}

cube_t
inverse(cube_t c)
{
	cube_t ret;

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "inverse error, inconsistent cube\n");
		setzero(ret);
		return ret;
	}
#endif

#ifdef CUBE_AVX2
	/* Method taken from Andrew Skalski's vcube[1]. The addition sequence
	 * was generated using [2].
	 * [1] https://github.com/Voltara/vcube
	 * [2] http://wwwhomes.uni-bielefeld.de/achim/addition_chain.html
	 */
	cube_t v3, vi, vo, vp;

	v3 = _mm256_shuffle_epi8(c, c);
	v3 = _mm256_shuffle_epi8(v3, c);
	vi = _mm256_shuffle_epi8(v3, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, c);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, c);

	vo = _mm256_and_si256(c, _mm256_or_si256(_eo_avx2, _co2_avx2));
	vo = _mm256_shuffle_epi8(vo, vi);
	vp = _mm256_andnot_si256(_mm256_or_si256(_eo_avx2, _co2_avx2), vi);
	ret = _mm256_or_si256(vp, vo);
	
	return flipallcorners(ret);
#else
	uint8_t i, piece, orien;

	setzero(ret);

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
#endif

	return ret;
}

static inline cube_t
inline_compose(cube_t c1, cube_t c2)
{
	cube_t ret;

#ifdef DEBUG
	if (!isconsistent(c1) || !isconsistent(c2)) {
		fprintf(stderr, "compose error, inconsistent cube\n");
		setzero(ret);
		return ret;
	}
#endif

#ifdef CUBE_AVX2
	cube_t s, eo2, ed, co1, co2, aux, auy1, auy2, cw, cwccw, auz1, auz2,
	    coclean;

	eo2 = _mm256_and_si256(c2, _eo_avx2);
	s = _mm256_shuffle_epi8(c1, c2);
	ed = _mm256_xor_si256(s, eo2);
	co1 = _mm256_and_si256(s, _co2_avx2);
	co2 = _mm256_and_si256(c2, _co2_avx2);
	aux = _mm256_add_epi8(co1, co2);
	cw = _mm256_set_epi8(
	    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0, 0, 0, 0, 0, 0, 0, 0,
	    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
	);
	auy1 = _mm256_add_epi8(aux, cw);
	auy2 = _mm256_srli_epi32(auy1, 2);
	auz1 = _mm256_add_epi8(aux, auy2);
	cwccw = _mm256_set_epi8(
	    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0, 0, 0, 0, 0, 0, 0, 0,
	    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60
	);
	auz2 = _mm256_and_si256(auz1, cwccw);
	coclean = _mm256_andnot_si256(_co2_avx2, ed);
	ret = _mm256_or_si256(coclean, auz2);
#else
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	setzero(ret);

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
#endif

	return ret;
}

cube_t
compose(cube_t c1, cube_t c2)
{
	return inline_compose(c1, c2);
}

cube_t
transform(cube_t c, trans_t t)
{
	cube_t ret;

#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "transform error, inconsistent cube\n");
		goto trans_error;
	}
#endif

	switch (t) {
	case UFr:
		return inline_trans_UFr(c);
	case ULr:
		return inline_trans_ULr(c);
	case UBr:
		return inline_trans_UBr(c);
	case URr:
		return inline_trans_URr(c);
	case DFr:
		return inline_trans_DFr(c);
	case DLr:
		return inline_trans_DLr(c);
	case DBr:
		return inline_trans_DBr(c);
	case DRr:
		return inline_trans_DRr(c);
	case RUr:
		return inline_trans_RUr(c);
	case RFr:
		return inline_trans_RFr(c);
	case RDr:
		return inline_trans_RDr(c);
	case RBr:
		return inline_trans_RBr(c);
	case LUr:
		return inline_trans_LUr(c);
	case LFr:
		return inline_trans_LFr(c);
	case LDr:
		return inline_trans_LDr(c);
	case LBr:
		return inline_trans_LBr(c);
	case FUr:
		return inline_trans_FUr(c);
	case FRr:
		return inline_trans_FRr(c);
	case FDr:
		return inline_trans_FDr(c);
	case FLr:
		return inline_trans_FLr(c);
	case BUr:
		return inline_trans_BUr(c);
	case BRr:
		return inline_trans_BRr(c);
	case BDr:
		return inline_trans_BDr(c);
	case BLr:
		return inline_trans_BLr(c);
	case UFm:
		return inline_trans_UFm(c);
	case ULm:
		return inline_trans_ULm(c);
	case UBm:
		return inline_trans_UBm(c);
	case URm:
		return inline_trans_URm(c);
	case DFm:
		return inline_trans_DFm(c);
	case DLm:
		return inline_trans_DLm(c);
	case DBm:
		return inline_trans_DBm(c);
	case DRm:
		return inline_trans_DRm(c);
	case RUm:
		return inline_trans_RUm(c);
	case RFm:
		return inline_trans_RFm(c);
	case RDm:
		return inline_trans_RDm(c);
	case RBm:
		return inline_trans_RBm(c);
	case LUm:
		return inline_trans_LUm(c);
	case LFm:
		return inline_trans_LFm(c);
	case LDm:
		return inline_trans_LDm(c);
	case LBm:
		return inline_trans_LBm(c);
	case FUm:
		return inline_trans_FUm(c);
	case FRm:
		return inline_trans_FRm(c);
	case FDm:
		return inline_trans_FDm(c);
	case FLm:
		return inline_trans_FLm(c);
	case BUm:
		return inline_trans_BUm(c);
	case BRm:
		return inline_trans_BRm(c);
	case BDm:
		return inline_trans_BDm(c);
	case BLm:
		return inline_trans_BLm(c);
	default:
#ifdef DEBUG
		fprintf(stderr, "transform error, unknown transformation\n");
#endif
		goto trans_error;
	}

trans_error:
	setzero(ret);
	return ret;
}
