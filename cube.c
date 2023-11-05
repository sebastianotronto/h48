#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "cube.h"

/******************************************************************************
Section: constants and strings
******************************************************************************/

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

/******************************************************************************
Section: cube_array

This section contains non-optimized functions that operate on the cube in
array format. These utilities are not used in performance-critical parts;
for example, all I/O related stuff is here, as well as some checks on the
state of the cube that are used in debugging.
******************************************************************************/

typedef struct {
	uint8_t c[16];
	uint8_t e[16];
} cube_array_t;

#define get_edge(cube, i)      (cube).e[(i)]
#define get_corner(cube, i)    (cube).c[(i)]
#define set_edge(cube, i, p)   (cube).e[(i)] = (p)
#define set_corner(cube, i, p) (cube).c[(i)] = (p)

static void setzero_array(cube_array_t *);
static bool equal_array(cube_array_t, cube_array_t);
static bool iserror_array(cube_array_t);
static bool isconsistent_array(cube_array_t);
static bool issolvable_array(cube_array_t);
static uint8_t readco(char *);
static uint8_t readcp(char *);
static uint8_t readeo(char *);
static uint8_t readep(char *);
static cube_array_t readcube_array(format_t, char *);
static int permsign(uint8_t *, int);
static cube_array_t readcube_array_H48(char *);
static void writecube_array_AVX(cube_array_t, char *);
static void writecube_array_H48(cube_array_t, char *);
static int writepiece_SRC(uint8_t, char *);
static void writecube_array_SRC(cube_array_t, char *);
static uint8_t readmove(char);
static uint8_t readmodifier(char);

cube_array_t solvedcube_array = {
	.c = {0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0},
	.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0}
};
cube_array_t zerocube_array = { .e = {0}, .c = {0} };

static void
setzero_array(cube_array_t *arr)
{
	memset(arr, 0, 32);
}

static bool
equal_array(cube_array_t c1, cube_array_t c2)
{
	int i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.c[i] == c2.c[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.e[i] == c2.e[i];

	return ret;
}

static bool
iserror_array(cube_array_t arr)
{
	return equal_array(arr, zerocube_array);
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

static cube_array_t
readcube_array_H48(char *buf)
{
	int i;
	uint8_t piece, orient;
	cube_array_t ret = {0};
	char *b;
	
	b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			return zerocube_array;
		b += 2;
		if ((orient = readeo(b)) == _error)
			return zerocube_array;
		b++;
		set_edge(ret, i, piece | orient);
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			return zerocube_array;
		b += 3;
		if ((orient = readco(b)) == _error)
			return zerocube_array;
		b++;
		set_corner(ret, i, piece | orient);
	}

	return ret;
}

cube_array_t
readcube_array(format_t format, char *buf)
{
	cube_array_t arr;

	switch (format) {
	case H48:
		arr = readcube_array_H48(buf);
		break;
	default:
#ifdef DEBUG
		fprintf(stderr, "Cannot read cube in the given format\n");
#endif
		setzero_array(&arr);
	}

#ifdef DEBUG
	if (iserror_array(arr))
		fprintf(stderr, "readcube error\n");
#endif
	return arr;
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
writecube_array_AVX(cube_array_t cube, char *buf)
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
writecube_array_H48(cube_array_t cube, char *buf)
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

static void
writecube_array_SRC(cube_array_t cube, char *buf)
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
writecube_array(format_t format, cube_array_t a, char *buf)
{
	char *errormsg;
	size_t len;

	if (!isconsistent_array(a)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	switch (format) {
	case AVX:
		writecube_array_AVX(a, buf);
		break;
	case H48:
		writecube_array_H48(a, buf);
		break;
	case SRC:
		writecube_array_SRC(a, buf);
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
isconsistent_array(cube_array_t c)
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
issolvable_array(cube_array_t c)
{
	uint8_t i, eo, co, piece, edges[12], corners[8];

#ifdef DEBUG
	if (!isconsistent_array(c)) {
		fprintf(stderr, "issolvable: cube is inconsistent\n");
		return false;
	}
#endif

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

/******************************************************************************
Section: AVX2 fast methods

This section contains performance-critical methods that rely on AVX2
intructions such as routines for moving or transforming the cube.

Note: the #ifdef below is closed in the next section.
******************************************************************************/

#ifdef CUBE_AVX2

#define _co_avx2 _mm256_set_epi64x(0, 0, 0, 0xF0F0F0F0F0F0F0F0)
#define _co2_avx2 _mm256_set_epi64x(0, 0, 0, 0x6060606060606060)
#define _cocw_avx2 _mm256_set_epi64x(0, 0, 0, 0x2020202020202020)
#define _eo_avx2 _mm256_set_epi64x(0x10101010, 0x1010101010101010, 0, 0)

static cube_t _arraytocube(cube_array_t);
static void _cubetoarray(cube_t, cube_array_t *);
static inline bool _equal(cube_t, cube_t);
static inline cube_t _invertco(cube_t);
static inline cube_t _inverse(cube_t);
static inline cube_t _compose(cube_t, cube_t);

static inline cube_t
_move_U(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 0, 1, 3, 2, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 1, 0, 3, 2, 4, 5
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_U2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 4, 5, 3, 2, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 3, 2, 0, 1
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_U3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 1, 0, 3, 2, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 0, 1, 3, 2, 5, 4
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_D(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 3, 2, 5, 4, 6, 7, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 5, 4, 6, 7, 1, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_D2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 6, 7, 5, 4, 2, 3, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 2, 3, 1, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_D3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 2, 3, 5, 4, 7, 6, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 5, 4, 7, 6, 1, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_R(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 4, 10, 9, 7, 11, 6, 5, 8, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 35, 32, 4, 69, 2, 1, 70
	);

	return _compose(c, m);
}

static inline cube_t
_move_R2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 8, 10, 9, 11, 4, 6, 5, 7, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 5, 6, 4, 0, 2, 1, 3
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_R3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 7, 10, 9, 4, 8, 6, 5, 11, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 32, 35, 4, 70, 2, 1, 69
	);

	return _compose(c, m);
}

static inline cube_t
_move_L(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 6, 5, 8, 7, 9, 10, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 6, 5, 33, 3, 68, 71, 0
	);

	return _compose(c, m);
}

static inline cube_t
_move_L2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 9, 10, 8, 7, 5, 6, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 5, 7, 3, 1, 2, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_L3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 5, 6, 8, 7, 10, 9, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 6, 5, 34, 3, 71, 68, 0
	);

	return _compose(c, m);
}

static inline cube_t
_move_F(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 19, 16, 7, 6, 5, 4, 24, 2, 1, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 64, 5, 66, 3, 38, 1, 36
	);

	return _compose(c, m);
}

static inline cube_t
_move_F2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 8, 9, 7, 6, 5, 4, 0, 2, 1, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 4, 5, 6, 3, 0, 1, 2
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_F3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 16, 19, 7, 6, 5, 4, 25, 2, 1, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 66, 5, 64, 3, 36, 1, 38
	);

	return _compose(c, m);
}

static inline cube_t
_move_B(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 9, 8, 7, 6, 5, 4, 3, 26, 27, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 6, 67, 4, 39, 2, 37, 0
	);

	return _compose(c, m);
}

static inline cube_t
_move_B2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 9, 8, 7, 6, 5, 4, 3, 1, 2, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 4, 1, 2, 3, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
_move_B3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 9, 8, 7, 6, 5, 4, 3, 27, 26, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 6, 65, 4, 37, 2, 39, 0
	);

	return _compose(c, m);
}

static inline cube_t
_trans_UFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_ULr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 3, 2, 1, 0, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 1, 6, 7, 5, 4
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 2, 3, 0, 1, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 7, 6, 4, 5
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_UBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 6, 7, 4, 5, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 4, 5, 2, 3, 0, 1
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 6, 7, 4, 5, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 4, 5, 2, 3, 0, 1
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_URr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 2, 3, 0, 1, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 7, 6, 4, 5
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 3, 2, 1, 0, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 1, 6, 7, 5, 4
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 5, 4, 7, 6, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 7, 6, 1, 0, 3, 2
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 5, 4, 7, 6, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 7, 6, 1, 0, 3, 2
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DLr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 1, 0, 3, 2, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 5, 4, 6, 7
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 1, 0, 3, 2, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 5, 4, 6, 7
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 4, 5, 6, 7, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 0, 1, 2, 3
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 4, 5, 6, 7, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 0, 1, 2, 3
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DRr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 0, 1, 2, 3, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 4, 5, 7, 6
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 0, 1, 2, 3, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 4, 5, 7, 6
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 25, 26, 27, 24, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 38, 37, 66, 65, 67, 64
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 17, 18, 19, 16, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 68, 70, 33, 35, 34, 32
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 22, 21, 20, 23, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 64, 39, 36, 37, 38
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 20, 23, 22, 21, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 65, 66, 37, 38, 39, 36
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 26, 25, 24, 27, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 37, 38, 65, 66, 64, 67
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 16, 19, 18, 17, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 69, 71, 32, 34, 35, 33
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 21, 22, 23, 20, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 64, 67, 36, 39, 38, 37
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 21, 22, 23, 20, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 64, 67, 36, 39, 38, 37
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 27, 24, 25, 26, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 39, 36, 67, 64, 66, 65
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 18, 17, 16, 19, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 70, 68, 35, 33, 32, 34
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 20, 23, 22, 21, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 65, 66, 37, 38, 39, 36
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 22, 21, 20, 23, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 64, 39, 36, 37, 38
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 24, 27, 26, 25, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 36, 39, 64, 67, 65, 66
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 19, 16, 17, 18, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 71, 69, 34, 32, 33, 35
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 66, 65, 38, 37, 36, 39
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 66, 65, 38, 37, 36, 39
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 10, 11, 8, 9, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 34, 32, 71, 69, 70, 68
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 10, 11, 8, 9, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 34, 32, 71, 69, 70, 68
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FRr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 17, 18, 19, 16, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 68, 70, 33, 35, 34, 32
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 25, 26, 27, 24, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 38, 37, 66, 65, 67, 64
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 11, 10, 9, 8, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 32, 34, 69, 71, 68, 70
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 8, 9, 10, 11, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 35, 33, 70, 68, 71, 69
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FLr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 18, 17, 16, 19, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 70, 68, 35, 33, 32, 34
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 27, 24, 25, 26, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 39, 36, 67, 64, 66, 65
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 8, 9, 10, 11, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 35, 33, 70, 68, 71, 69
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 11, 10, 9, 8, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 32, 34, 69, 71, 68, 70
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BRr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 19, 16, 17, 18, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 71, 69, 34, 32, 33, 35
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 24, 27, 26, 25, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 36, 39, 64, 67, 65, 66
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 9, 8, 11, 10, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 33, 35, 68, 70, 69, 71
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 9, 8, 11, 10, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 33, 35, 68, 70, 69, 71
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BLr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 16, 19, 18, 17, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 69, 71, 32, 34, 35, 33
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 26, 25, 24, 27, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 37, 38, 65, 66, 64, 67
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_UFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 1, 0, 7, 6, 5, 4
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 1, 0, 7, 6, 5, 4
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_ULm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 3, 2, 1, 0, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 2, 3, 1, 0
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 3, 2, 1, 0, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 2, 3, 1, 0
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_UBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 7, 6, 5, 4, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 1, 6, 7, 4, 5
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 7, 6, 5, 4, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 1, 6, 7, 4, 5
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_URm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 2, 3, 0, 1, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 3, 2, 0, 1
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 2, 3, 0, 1, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 3, 2, 0, 1
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 4, 5, 6, 7, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 2, 5, 4, 7, 6
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 4, 5, 6, 7, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 2, 5, 4, 7, 6
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DLm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 1, 0, 3, 2, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 7, 6, 1, 0, 2, 3
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 0, 1, 2, 3, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 7, 0, 1, 3, 2
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DRm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 0, 1, 2, 3, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 7, 0, 1, 3, 2
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 1, 0, 3, 2, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 7, 6, 1, 0, 2, 3
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 24, 27, 26, 25, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 32, 34, 33, 70, 69, 71, 68
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 18, 17, 16, 19, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 34, 32, 71, 69, 68, 70
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 23, 20, 21, 22, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 70, 71, 68, 35, 32, 33, 34
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 23, 20, 21, 22, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 39, 36, 67, 64, 65, 66
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 27, 24, 25, 26, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 35, 33, 34, 69, 70, 68, 71
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 19, 16, 17, 18, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 35, 33, 70, 68, 69, 71
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 20, 23, 22, 21, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 69, 68, 71, 32, 35, 34, 33
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 22, 21, 20, 23, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 38, 37, 66, 65, 64, 67
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 26, 25, 24, 27, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 33, 35, 32, 71, 68, 70, 69
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 17, 18, 19, 16, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 32, 34, 69, 71, 70, 68
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 68, 69, 70, 33, 34, 35, 32
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 37, 38, 65, 66, 67, 64
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 25, 26, 27, 24, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 34, 32, 35, 68, 71, 69, 70
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 16, 19, 18, 17, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 33, 35, 68, 70, 71, 69
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 22, 21, 20, 23, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 71, 70, 69, 34, 33, 32, 35
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 20, 23, 22, 21, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 36, 39, 64, 67, 66, 65
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 11, 10, 9, 8, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 37, 38, 36, 67, 65, 66, 64
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 11, 10, 9, 8, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 70, 68, 35, 33, 34, 32
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FRm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 17, 18, 19, 16, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 65, 64, 66, 37, 39, 38, 36
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 26, 25, 24, 27, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 67, 64, 39, 36, 38, 37
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 10, 11, 8, 9, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 39, 36, 38, 65, 67, 64, 66
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 9, 8, 11, 10, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 71, 69, 34, 32, 35, 33
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FLm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 18, 17, 16, 19, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 67, 66, 64, 39, 37, 36, 38
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 24, 27, 26, 25, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 66, 65, 38, 37, 39, 36
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 9, 8, 11, 10, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 36, 39, 37, 66, 64, 67, 65
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 10, 11, 8, 9, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 68, 70, 33, 35, 32, 34
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BRm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 19, 16, 17, 18, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 66, 67, 65, 38, 36, 37, 39
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 27, 24, 25, 26, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 65, 66, 37, 38, 36, 39
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 38, 37, 39, 64, 66, 65, 67
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 69, 71, 32, 34, 33, 35
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BLm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 16, 19, 18, 17, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 64, 65, 67, 36, 38, 39, 37
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 25, 26, 27, 24, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 64, 67, 36, 39, 37, 38
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static cube_t
_arraytocube(cube_array_t a)
{
	return _mm256_loadu_si256((__m256i_u *)&a);
}

static void
_cubetoarray(cube_t c, cube_array_t *a)
{
	_mm256_storeu_si256((__m256i_u *)a, c);
}

static inline bool
_equal(cube_t c1, cube_t c2)
{
	uint32_t mask;
	__m256i cmp;

	cmp = _mm256_cmpeq_epi8(c1, c2);
	mask = _mm256_movemask_epi8(cmp);

	return mask == 0xffffffffU;
}

static inline cube_t
_invertco(cube_t c)
{
        cube_t co, shleft, shright, summed, newco, cleanco, ret;

        co = _mm256_and_si256(c, _co2_avx2);
        shleft = _mm256_slli_epi32(co, 1);
        shright = _mm256_srli_epi32(co, 1);
        summed = _mm256_or_si256(shleft, shright);
        newco = _mm256_and_si256(summed, _co2_avx2);
        cleanco = _mm256_xor_si256(c, co);
        ret = _mm256_or_si256(cleanco, newco);

        return ret;
}

static inline cube_t
_inverse(cube_t c)
{
	/* Method taken from Andrew Skalski's vcube[1]. The addition sequence
	 * was generated using [2].
	 * [1] https://github.com/Voltara/vcube
	 * [2] http://wwwhomes.uni-bielefeld.de/achim/addition_chain.html
	 */
	cube_t v3, vi, vo, vp, ret;

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
	
	return _invertco(ret);
}

static inline cube_t
_compose(cube_t c1, cube_t c2)
{
	cube_t ret;

	cube_t s, eo2, ed, co1, co2, aux, auy1, auy2, auz1, auz2, coclean;

	eo2 = _mm256_and_si256(c2, _eo_avx2);
	s = _mm256_shuffle_epi8(c1, c2);
	ed = _mm256_xor_si256(s, eo2);
	co1 = _mm256_and_si256(s, _co2_avx2);
	co2 = _mm256_and_si256(c2, _co2_avx2);
	aux = _mm256_add_epi8(co1, co2);
	auy1 = _mm256_add_epi8(aux, _cocw_avx2);
	auy2 = _mm256_srli_epi32(auy1, 2);
	auz1 = _mm256_add_epi8(aux, auy2);
	auz2 = _mm256_and_si256(auz1, _co2_avx2);
	coclean = _mm256_andnot_si256(_co2_avx2, ed);
	ret = _mm256_or_si256(coclean, auz2);

	return ret;
}

static inline int16_t
_coord_eo(cube_t c)
{
	cube_t eo, shifted;
	int mask;

	eo = _mm256_and_si256(c, _eo_avx2);
	shifted = _mm256_slli_epi32(eo, 3);
	mask = _mm256_movemask_epi8(shifted);

	return (int16_t)(mask >> 17);
}


/******************************************************************************
Section: portable fast methods

This section contains performance-critical methods that do not use
advanced CPU instructions. They are used as an alternative to the ones
in the previous section(s) for unsupported architectures.
******************************************************************************/

#else

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

static cube_t _arraytocube(cube_array_t);
static void _cubetoarray(cube_t, cube_array_t *);
static inline bool _equal(cube_t, cube_t);
static inline cube_t _invertco(cube_t);
static inline cube_t _inverse(cube_t);
static inline cube_t _compose(cube_t, cube_t);

static inline cube_t
_move_U(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM4(ret.e, _e_uf, _e_ul, _e_ub, _e_ur)
	PERM4(ret.c, _c_ufr, _c_ufl, _c_ubl, _c_ubr)

	return ret;
}

static inline cube_t
_move_U2(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM22(ret.e, _e_uf, _e_ub, _e_ul, _e_ur)
	PERM22(ret.c, _c_ufr, _c_ubl, _c_ufl, _c_ubr)

	return ret;
}

static inline cube_t
_move_U3(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM4(ret.e, _e_uf, _e_ur, _e_ub, _e_ul)
	PERM4(ret.c, _c_ufr, _c_ubr, _c_ubl, _c_ufl)

	return ret;
}

static inline cube_t
_move_D(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM4(ret.e, _e_df, _e_dr, _e_db, _e_dl)
	PERM4(ret.c, _c_dfr, _c_dbr, _c_dbl, _c_dfl)

	return ret;
}

static inline cube_t
_move_D2(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM22(ret.e, _e_df, _e_db, _e_dr, _e_dl)
	PERM22(ret.c, _c_dfr, _c_dbl, _c_dbr, _c_dfl)

	return ret;
}

static inline cube_t
_move_D3(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM4(ret.e, _e_df, _e_dl, _e_db, _e_dr)
	PERM4(ret.c, _c_dfr, _c_dfl, _c_dbl, _c_dbr)

	return ret;
}

static inline cube_t
_move_R(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_ur, _e_br, _e_dr, _e_fr)
	PERM4(ret.c, _c_ufr, _c_ubr, _c_dbr, _c_dfr)

	CO4(ret.c, _c_ubr, _c_dfr, _c_ufr, _c_dbr)

	return ret;
}

static inline cube_t
_move_R2(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM22(ret.e, _e_ur, _e_dr, _e_fr, _e_br)
	PERM22(ret.c, _c_ufr, _c_dbr, _c_ubr, _c_dfr)

	return ret;
}

static inline cube_t
_move_R3(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_ur, _e_fr, _e_dr, _e_br)
	PERM4(ret.c, _c_ufr, _c_dfr, _c_dbr, _c_ubr)

	CO4(ret.c, _c_ubr, _c_dfr, _c_ufr, _c_dbr)

	return ret;
}

static inline cube_t
_move_L(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_ul, _e_fl, _e_dl, _e_bl)
	PERM4(ret.c, _c_ufl, _c_dfl, _c_dbl, _c_ubl)

	CO4(ret.c, _c_ufl, _c_dbl, _c_dfl, _c_ubl)

	return ret;
}

static inline cube_t
_move_L2(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM22(ret.e, _e_ul, _e_dl, _e_fl, _e_bl)
	PERM22(ret.c, _c_ufl, _c_dbl, _c_ubl, _c_dfl)

	return ret;
}

static inline cube_t
_move_L3(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_ul, _e_bl, _e_dl, _e_fl)
	PERM4(ret.c, _c_ufl, _c_ubl, _c_dbl, _c_dfl)

	CO4(ret.c, _c_ufl, _c_dbl, _c_dfl, _c_ubl)

	return ret;
}

static inline cube_t
_move_F(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
	PERM4(ret.c, _c_ufr, _c_dfr, _c_dfl, _c_ufl)

	EO4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
	CO4(ret.c, _c_ufr, _c_dfl, _c_dfr, _c_ufl)

	return ret;
}

static inline cube_t
_move_F2(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM22(ret.e, _e_uf, _e_df, _e_fr, _e_fl)
	PERM22(ret.c, _c_ufr, _c_dfl, _c_ufl, _c_dfr)

	return ret;
}

static inline cube_t
_move_F3(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_uf, _e_fl, _e_df, _e_fr)
	PERM4(ret.c, _c_ufr, _c_ufl, _c_dfl, _c_dfr)

	EO4(ret.e, _e_uf, _e_fr, _e_df, _e_fl)
	CO4(ret.c, _c_ufr, _c_dfl, _c_dfr, _c_ufl)

	return ret;
}

static inline cube_t
_move_B(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_ub, _e_bl, _e_db, _e_br)
	PERM4(ret.c, _c_ubr, _c_ubl, _c_dbl, _c_dbr)

	EO4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
	CO4(ret.c, _c_ubl, _c_dbr, _c_dbl, _c_ubr)

	return ret;
}

static inline cube_t
_move_B2(cube_t c)
{
	uint8_t aux;
	cube_t ret = c;

	PERM22(ret.e, _e_ub, _e_db, _e_br, _e_bl)
	PERM22(ret.c, _c_ubr, _c_dbl, _c_ubl, _c_dbr)

	return ret;
}

static inline cube_t
_move_B3(cube_t c)
{
	uint8_t aux, auy, auz;
	cube_t ret = c;

	PERM4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
	PERM4(ret.c, _c_ubr, _c_dbr, _c_dbl, _c_ubl)

	EO4(ret.e, _e_ub, _e_br, _e_db, _e_bl)
	CO4(ret.c, _c_ubl, _c_dbr, _c_dbl, _c_ubr)

	return ret;
}

static inline cube_t
_invertco(cube_t c)
{
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

static inline cube_t
_trans_UFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {0, 1, 2, 3, 4, 5, 6, 7},
		.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
	};
	cube_t ti = {
		.c = {0, 1, 2, 3, 4, 5, 6, 7},
		.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_ULr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {4, 5, 7, 6, 1, 0, 2, 3},
		.e = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24}
	};
	cube_t ti = {
		.c = {5, 4, 6, 7, 0, 1, 3, 2},
		.e = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_UBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {1, 0, 3, 2, 5, 4, 7, 6},
		.e = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9}
	};
	cube_t ti = {
		.c = {1, 0, 3, 2, 5, 4, 7, 6},
		.e = {1, 0, 3, 2, 5, 4, 7, 6, 10, 11, 8, 9}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_URr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {5, 4, 6, 7, 0, 1, 3, 2},
		.e = {4, 5, 6, 7, 1, 0, 3, 2, 27, 24, 25, 26}
	};
	cube_t ti = {
		.c = {4, 5, 7, 6, 1, 0, 2, 3},
		.e = {5, 4, 7, 6, 0, 1, 2, 3, 25, 26, 27, 24}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {2, 3, 0, 1, 6, 7, 4, 5},
		.e = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10}
	};
	cube_t ti = {
		.c = {2, 3, 0, 1, 6, 7, 4, 5},
		.e = {3, 2, 1, 0, 6, 7, 4, 5, 9, 8, 11, 10}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DLr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {7, 6, 4, 5, 2, 3, 1, 0},
		.e = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27}
	};
	cube_t ti = {
		.c = {7, 6, 4, 5, 2, 3, 1, 0},
		.e = {6, 7, 4, 5, 2, 3, 0, 1, 26, 25, 24, 27}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {3, 2, 1, 0, 7, 6, 5, 4},
		.e = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8}
	};
	cube_t ti = {
		.c = {3, 2, 1, 0, 7, 6, 5, 4},
		.e = {2, 3, 0, 1, 7, 6, 5, 4, 11, 10, 9, 8}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_DRr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {6, 7, 5, 4, 3, 2, 0, 1},
		.e = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25}
	};
	cube_t ti = {
		.c = {6, 7, 5, 4, 3, 2, 0, 1},
		.e = {7, 6, 5, 4, 3, 2, 1, 0, 24, 27, 26, 25}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {64, 67, 65, 66, 37, 38, 36, 39},
		.e = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3}
	};
	cube_t ti = {
		.c = {32, 34, 35, 33, 70, 68, 69, 71},
		.e = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {38, 37, 36, 39, 64, 67, 66, 65},
		.e = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18}
	};
	cube_t ti = {
		.c = {36, 39, 38, 37, 66, 65, 64, 67},
		.e = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {67, 64, 66, 65, 38, 37, 39, 36},
		.e = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1}
	};
	cube_t ti = {
		.c = {33, 35, 34, 32, 71, 69, 68, 70},
		.e = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_RBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {37, 38, 39, 36, 67, 64, 65, 66},
		.e = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16}
	};
	cube_t ti = {
		.c = {37, 38, 39, 36, 67, 64, 65, 66},
		.e = {27, 24, 25, 26, 20, 23, 22, 21, 17, 18, 19, 16}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {65, 66, 64, 67, 36, 39, 37, 38},
		.e = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2}
	};
	cube_t ti = {
		.c = {34, 32, 33, 35, 68, 70, 71, 69},
		.e = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LFr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {36, 39, 38, 37, 66, 65, 64, 67},
		.e = {25, 26, 27, 24, 21, 22, 23, 20, 16, 19, 18, 17}
	};
	cube_t ti = {
		.c = {38, 37, 36, 39, 64, 67, 66, 65},
		.e = {24, 27, 26, 25, 23, 20, 21, 22, 19, 16, 17, 18}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {66, 65, 67, 64, 39, 36, 38, 37},
		.e = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0}
	};
	cube_t ti = {
		.c = {35, 33, 32, 34, 69, 71, 70, 68},
		.e = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_LBr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {39, 36, 37, 38, 65, 66, 67, 64},
		.e = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19}
	};
	cube_t ti = {
		.c = {39, 36, 37, 38, 65, 66, 67, 64},
		.e = {26, 25, 24, 27, 22, 21, 20, 23, 18, 17, 16, 19}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {68, 70, 69, 71, 32, 34, 33, 35},
		.e = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6}
	};
	cube_t ti = {
		.c = {68, 70, 69, 71, 32, 34, 33, 35},
		.e = {16, 19, 18, 17, 9, 8, 11, 10, 5, 4, 7, 6}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FRr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {32, 34, 35, 33, 70, 68, 69, 71},
		.e = {8, 9, 10, 11, 16, 19, 18, 17, 20, 23, 22, 21}
	};
	cube_t ti = {
		.c = {64, 67, 65, 66, 37, 38, 36, 39},
		.e = {20, 23, 22, 21, 24, 27, 26, 25, 0, 1, 2, 3}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {70, 68, 71, 69, 34, 32, 35, 33},
		.e = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4}
	};
	cube_t ti = {
		.c = {69, 71, 68, 70, 33, 35, 32, 34},
		.e = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_FLr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {34, 32, 33, 35, 68, 70, 71, 69},
		.e = {9, 8, 11, 10, 19, 16, 17, 18, 22, 21, 20, 23}
	};
	cube_t ti = {
		.c = {65, 66, 64, 67, 36, 39, 37, 38},
		.e = {21, 22, 23, 20, 26, 25, 24, 27, 1, 0, 3, 2}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BUr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {69, 71, 68, 70, 33, 35, 32, 34},
		.e = {17, 18, 19, 16, 11, 10, 9, 8, 4, 5, 6, 7}
	};
	cube_t ti = {
		.c = {70, 68, 71, 69, 34, 32, 35, 33},
		.e = {19, 16, 17, 18, 8, 9, 10, 11, 7, 6, 5, 4}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BRr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {35, 33, 32, 34, 69, 71, 70, 68},
		.e = {11, 10, 9, 8, 18, 17, 16, 19, 23, 20, 21, 22}
	};
	cube_t ti = {
		.c = {66, 65, 67, 64, 39, 36, 38, 37},
		.e = {22, 21, 20, 23, 25, 26, 27, 24, 3, 2, 1, 0}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BDr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {71, 69, 70, 68, 35, 33, 34, 32},
		.e = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5}
	};
	cube_t ti = {
		.c = {71, 69, 70, 68, 35, 33, 34, 32},
		.e = {18, 17, 16, 19, 10, 11, 8, 9, 6, 7, 4, 5}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_BLr(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {33, 35, 34, 32, 71, 69, 68, 70},
		.e = {10, 11, 8, 9, 17, 18, 19, 16, 21, 22, 23, 20}
	};
	cube_t ti = {
		.c = {67, 64, 66, 65, 38, 37, 39, 36},
		.e = {23, 20, 21, 22, 27, 24, 25, 26, 2, 3, 0, 1}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
_trans_UFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {4, 5, 6, 7, 0, 1, 2, 3},
		.e = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10}
	};
	cube_t ti = {
		.c = {4, 5, 6, 7, 0, 1, 2, 3},
		.e = {0, 1, 2, 3, 5, 4, 7, 6, 9, 8, 11, 10}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_ULm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {0, 1, 3, 2, 5, 4, 6, 7},
		.e = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25}
	};
	cube_t ti = {
		.c = {0, 1, 3, 2, 5, 4, 6, 7},
		.e = {4, 5, 6, 7, 0, 1, 2, 3, 24, 27, 26, 25}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_UBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {5, 4, 7, 6, 1, 0, 3, 2},
		.e = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8}
	};
	cube_t ti = {
		.c = {5, 4, 7, 6, 1, 0, 3, 2},
		.e = {1, 0, 3, 2, 4, 5, 6, 7, 11, 10, 9, 8}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_URm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {1, 0, 2, 3, 4, 5, 7, 6},
		.e = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27}
	};
	cube_t ti = {
		.c = {1, 0, 2, 3, 4, 5, 7, 6},
		.e = {5, 4, 7, 6, 1, 0, 3, 2, 26, 25, 24, 27}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {6, 7, 4, 5, 2, 3, 0, 1},
		.e = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11}
	};
	cube_t ti = {
		.c = {6, 7, 4, 5, 2, 3, 0, 1},
		.e = {3, 2, 1, 0, 7, 6, 5, 4, 8, 9, 10, 11}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DLm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {3, 2, 0, 1, 6, 7, 5, 4},
		.e = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26}
	};
	cube_t ti = {
		.c = {2, 3, 1, 0, 7, 6, 4, 5},
		.e = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {7, 6, 5, 4, 3, 2, 1, 0},
		.e = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9}
	};
	cube_t ti = {
		.c = {7, 6, 5, 4, 3, 2, 1, 0},
		.e = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_DRm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {2, 3, 1, 0, 7, 6, 4, 5},
		.e = {6, 7, 4, 5, 3, 2, 1, 0, 25, 26, 27, 24}
	};
	cube_t ti = {
		.c = {3, 2, 0, 1, 6, 7, 5, 4},
		.e = {7, 6, 5, 4, 2, 3, 0, 1, 27, 24, 25, 26}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {68, 71, 69, 70, 33, 34, 32, 35},
		.e = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3}
	};
	cube_t ti = {
		.c = {70, 68, 69, 71, 32, 34, 35, 33},
		.e = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {34, 33, 32, 35, 68, 71, 70, 69},
		.e = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18}
	};
	cube_t ti = {
		.c = {66, 65, 64, 67, 36, 39, 38, 37},
		.e = {25, 26, 27, 24, 22, 21, 20, 23, 19, 16, 17, 18}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {71, 68, 70, 69, 34, 33, 35, 32},
		.e = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1}
	};
	cube_t ti = {
		.c = {71, 69, 68, 70, 33, 35, 34, 32},
		.e = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_RBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {33, 34, 35, 32, 71, 68, 69, 70},
		.e = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16}
	};
	cube_t ti = {
		.c = {67, 64, 65, 66, 37, 38, 39, 36},
		.e = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {69, 70, 68, 71, 32, 35, 33, 34},
		.e = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2}
	};
	cube_t ti = {
		.c = {68, 70, 71, 69, 34, 32, 33, 35},
		.e = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LFm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {32, 35, 34, 33, 70, 69, 68, 71},
		.e = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17}
	};
	cube_t ti = {
		.c = {64, 67, 66, 65, 38, 37, 36, 39},
		.e = {24, 27, 26, 25, 20, 23, 22, 21, 16, 19, 18, 17}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {70, 69, 71, 68, 35, 32, 34, 33},
		.e = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0}
	};
	cube_t ti = {
		.c = {69, 71, 70, 68, 35, 33, 32, 34},
		.e = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_LBm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {35, 32, 33, 34, 69, 70, 71, 68},
		.e = {27, 24, 25, 26, 23, 20, 21, 22, 18, 17, 16, 19}
	};
	cube_t ti = {
		.c = {65, 66, 67, 64, 39, 36, 37, 38},
		.e = {26, 25, 24, 27, 21, 22, 23, 20, 17, 18, 19, 16}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {64, 66, 65, 67, 36, 38, 37, 39},
		.e = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7}
	};
	cube_t ti = {
		.c = {32, 34, 33, 35, 68, 70, 69, 71},
		.e = {16, 19, 18, 17, 8, 9, 10, 11, 4, 5, 6, 7}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FRm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {36, 38, 39, 37, 66, 64, 65, 67},
		.e = {9, 8, 11, 10, 16, 19, 18, 17, 21, 22, 23, 20}
	};
	cube_t ti = {
		.c = {37, 38, 36, 39, 64, 67, 65, 66},
		.e = {20, 23, 22, 21, 27, 24, 25, 26, 1, 0, 3, 2}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {66, 64, 67, 65, 38, 36, 39, 37},
		.e = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5}
	};
	cube_t ti = {
		.c = {33, 35, 32, 34, 69, 71, 68, 70},
		.e = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_FLm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {38, 36, 37, 39, 64, 66, 67, 65},
		.e = {8, 9, 10, 11, 19, 16, 17, 18, 23, 20, 21, 22}
	};
	cube_t ti = {
		.c = {36, 39, 37, 38, 65, 66, 64, 67},
		.e = {21, 22, 23, 20, 25, 26, 27, 24, 0, 1, 2, 3}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BUm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {65, 67, 64, 66, 37, 39, 36, 38},
		.e = {17, 18, 19, 16, 10, 11, 8, 9, 5, 4, 7, 6}
	};
	cube_t ti = {
		.c = {34, 32, 35, 33, 70, 68, 71, 69},
		.e = {19, 16, 17, 18, 9, 8, 11, 10, 6, 7, 4, 5}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BRm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {39, 37, 36, 38, 65, 67, 66, 64},
		.e = {10, 11, 8, 9, 18, 17, 16, 19, 22, 21, 20, 23}
	};
	cube_t ti = {
		.c = {39, 36, 38, 37, 66, 65, 67, 64},
		.e = {22, 21, 20, 23, 26, 25, 24, 27, 2, 3, 0, 1}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BDm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {67, 65, 66, 64, 39, 37, 38, 36},
		.e = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4}
	};
	cube_t ti = {
		.c = {35, 33, 34, 32, 71, 69, 70, 68},
		.e = {18, 17, 16, 19, 11, 10, 9, 8, 7, 6, 5, 4}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static inline cube_t
_trans_BLm(cube_t c)
{
	cube_t ret;
	cube_t tn = {
		.c = {37, 39, 38, 36, 67, 65, 64, 66},
		.e = {11, 10, 9, 8, 17, 18, 19, 16, 20, 23, 22, 21}
	};
	cube_t ti = {
		.c = {38, 37, 39, 36, 67, 64, 66, 65},
		.e = {23, 20, 21, 22, 24, 27, 26, 25, 3, 2, 1, 0}
	};

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = _invertco(ret);

	return ret;
}

static cube_t
_arraytocube(cube_array_t a)
{
	cube_t c;
	memcpy(&c, &a, sizeof(cube_t));
	return c;
}

static void
_cubetoarray(cube_t c, cube_array_t *a)
{
	memcpy(a, &c, sizeof(cube_t));
}

static inline bool
_equal(cube_t c1, cube_t c2)
{
	uint8_t i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.c[i] == c2.c[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.e[i] == c2.e[i];

	return ret;
}

static inline cube_t
_inverse(cube_t c)
{
	cube_t ret;
	uint8_t i, piece, orien;

	ret = _arraytocube(zerocube_array);

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

static inline cube_t
_compose(cube_t c1, cube_t c2)
{
	cube_t ret;
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	ret = _arraytocube(zerocube_array);

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

static inline int16_t
_coord_eo(cube_t c)
{
	int i, p;
	int16_t ret;

	ret = 0;
	for (i = 1, p = 1; i < 12; i++, p *= 2)
		ret += p * (c.e[i] >> 4);

	return ret;
}


#endif

/******************************************************************************
Section: generic methods.

This section contains functions that are based (directly or indirectly)
on the per-architecture functions defined in the previous sections. Many
of them are public functions from cube.h
******************************************************************************/

cube_t
solvedcube(void)
{
	cube_t solved;
	solved = _arraytocube(solvedcube_array);
	return solved;
}

cube_t
zerocube(void)
{
	cube_t solved;
	solved = _arraytocube(zerocube_array);
	return solved;
}

cube_t
readcube(format_t format, char *buf)
{
	cube_array_t arr;
	arr = readcube_array(format, buf);
	return _arraytocube(arr);
}

void
writecube(format_t format, cube_t cube, char *buf)
{
	cube_array_t arr;
	_cubetoarray(cube, &arr);
	writecube_array(format, arr, buf);
}

bool
isconsistent(cube_t c)
{
	cube_array_t arr;
	_cubetoarray(c, &arr);
	return isconsistent_array(arr);
}

bool
issolvable(cube_t c)
{
	cube_array_t arr;
	_cubetoarray(c, &arr);
	return issolvable_array(arr);
}

bool
iserror(cube_t c)
{
	cube_array_t arr;
	_cubetoarray(c, &arr);
	return iserror_array(arr);
}

bool
equal(cube_t c1, cube_t c2)
{
	return _equal(c1, c2);
}

bool
issolved(cube_t cube)
{
	cube_t solved;
	solved = _arraytocube(solvedcube_array);
	return equal(cube, solved);
}

cube_t
move(cube_t c, move_t m)
{
#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "move error, inconsistent cube\n");
		return _arraytocube(zerocube_array);
	}
#endif

	switch (m) {
	case U:
		return _move_U(c);
	case U2:
		return _move_U2(c);
	case U3:
		return _move_U3(c);
	case D:
		return _move_D(c);
	case D2:
		return _move_D2(c);
	case D3:
		return _move_D3(c);
	case R:
		return _move_R(c);
	case R2:
		return _move_R2(c);
	case R3:
		return _move_R3(c);
	case L:
		return _move_L(c);
	case L2:
		return _move_L2(c);
	case L3:
		return _move_L3(c);
	case F:
		return _move_F(c);
	case F2:
		return _move_F2(c);
	case F3:
		return _move_F3(c);
	case B:
		return _move_B(c);
	case B2:
		return _move_B2(c);
	case B3:
		return _move_B3(c);
	default:
#ifdef DEBUG
		fprintf(stderr, "mover error, unknown move\n");
#endif
		return _arraytocube(zerocube_array);
	}
}

cube_t
inverse(cube_t c)
{
#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "inverse error, inconsistent cube\n");
		return zerocube();
	}
#endif

	return _inverse(c);
}

cube_t
compose(cube_t c1, cube_t c2)
{
#ifdef DEBUG
	if (!isconsistent(c1) || !isconsistent(c2)) {
		fprintf(stderr, "compose error, inconsistent cube\n");
		return zerocube();
	}
#endif

	return _compose(c1, c2);
}

cube_t
transform(cube_t c, trans_t t)
{
#ifdef DEBUG
	if (!isconsistent(c)) {
		fprintf(stderr, "transform error, inconsistent cube\n");
		return zerocube();
	}
#endif

	switch (t) {
	case UFr:
		return _trans_UFr(c);
	case ULr:
		return _trans_ULr(c);
	case UBr:
		return _trans_UBr(c);
	case URr:
		return _trans_URr(c);
	case DFr:
		return _trans_DFr(c);
	case DLr:
		return _trans_DLr(c);
	case DBr:
		return _trans_DBr(c);
	case DRr:
		return _trans_DRr(c);
	case RUr:
		return _trans_RUr(c);
	case RFr:
		return _trans_RFr(c);
	case RDr:
		return _trans_RDr(c);
	case RBr:
		return _trans_RBr(c);
	case LUr:
		return _trans_LUr(c);
	case LFr:
		return _trans_LFr(c);
	case LDr:
		return _trans_LDr(c);
	case LBr:
		return _trans_LBr(c);
	case FUr:
		return _trans_FUr(c);
	case FRr:
		return _trans_FRr(c);
	case FDr:
		return _trans_FDr(c);
	case FLr:
		return _trans_FLr(c);
	case BUr:
		return _trans_BUr(c);
	case BRr:
		return _trans_BRr(c);
	case BDr:
		return _trans_BDr(c);
	case BLr:
		return _trans_BLr(c);
	case UFm:
		return _trans_UFm(c);
	case ULm:
		return _trans_ULm(c);
	case UBm:
		return _trans_UBm(c);
	case URm:
		return _trans_URm(c);
	case DFm:
		return _trans_DFm(c);
	case DLm:
		return _trans_DLm(c);
	case DBm:
		return _trans_DBm(c);
	case DRm:
		return _trans_DRm(c);
	case RUm:
		return _trans_RUm(c);
	case RFm:
		return _trans_RFm(c);
	case RDm:
		return _trans_RDm(c);
	case RBm:
		return _trans_RBm(c);
	case LUm:
		return _trans_LUm(c);
	case LFm:
		return _trans_LFm(c);
	case LDm:
		return _trans_LDm(c);
	case LBm:
		return _trans_LBm(c);
	case FUm:
		return _trans_FUm(c);
	case FRm:
		return _trans_FRm(c);
	case FDm:
		return _trans_FDm(c);
	case FLm:
		return _trans_FLm(c);
	case BUm:
		return _trans_BUm(c);
	case BRm:
		return _trans_BRm(c);
	case BDm:
		return _trans_BDm(c);
	case BLm:
		return _trans_BLm(c);
	default:
#ifdef DEBUG
		fprintf(stderr, "transform error, unknown transformation\n");
#endif
		return zerocube();
	}
}

int16_t
coord_eo(cube_t c)
{
	return _coord_eo(c);
}
