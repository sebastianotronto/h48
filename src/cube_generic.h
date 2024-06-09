#define _move(M, c) compose(c, _move_cube_ ## M)
#define _premove(M, c) compose(_move_cube_ ## M, c)

_static cube_t cubefromarray(uint8_t [static 8], uint8_t [static 12]);
_static int permsign(uint8_t *, int);
_static uint8_t readco(const char *);
_static uint8_t readcp(const char *);
_static uint8_t readeo(const char *);
_static uint8_t readep(const char *);
_static cube_t readcube_B32(const char *);
_static cube_t readcube_H48(const char *);
_static uint8_t readpiece_LST(const char **);
_static cube_t readcube_LST(const char *);
_static int writepiece_LST(uint8_t, char *);
_static void writecube_B32(cube_t, char *);
_static void writecube_H48(cube_t, char *);
_static void writecube_LST(cube_t, char *);
_static uint8_t b32toedge(char);
_static uint8_t b32tocorner(char);
_static char edgetob32(uint8_t);
_static char cornertob32(uint8_t);
_static uint8_t readmove(char);
_static uint8_t readmodifier(char);
_static uint8_t readtrans(const char *);
_static int writemoves(uint8_t *, int, char *);
_static void writetrans(uint8_t, char *);
_static cube_t move(cube_t, uint8_t);
_static cube_t transform_edges(cube_t, uint8_t);
_static cube_t transform_corners(cube_t, uint8_t);
_static cube_t transform(cube_t, uint8_t);

_static struct {
	const char *name;
	cube_t (*read)(const char *);
	void (*write)(cube_t, char *);
} ioformat[] =
{
	{ .name = "B32", .read = readcube_B32, .write = writecube_B32 },
	{ .name = "LST", .read = readcube_LST, .write = writecube_LST },
	{ .name = "H48", .read = readcube_H48, .write = writecube_H48 },
	{ .name = "NONE", .read = NULL, .write = NULL },
};

_static_inline cube_t
cubefromarray(uint8_t c[static 8], uint8_t e[static 12])
{
	return static_cube(
	    c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
	    e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7],
	    e[8], e[9], e[10], e[11]);
}

cube_t
solvedcube(void)
{
	return solved;
}

bool
isconsistent(cube_t cube)
{
	uint8_t i, p, e, piece, corner[8], edge[12];
	bool found[12];

	pieces(&cube, corner, edge);

	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		piece = edge[i];
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
		piece = corner[i];
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
	DBG_LOG("Inconsistent EP\n");
	return false;
inconsistent_cp:
	DBG_LOG("Inconsistent CP\n");
	return false;
inconsistent_eo:
	DBG_LOG("Inconsistent EO\n");
	return false;
inconsistent_co:
	DBG_LOG("Inconsistent CO\n");
	return false;
}

bool
issolvable(cube_t cube)
{
	uint8_t i, eo, co, piece, edge[12], corner[8], ep[12], cp[8];

	DBG_ASSERT(isconsistent(cube), false,
	    "issolvable: cube is inconsistent\n");

	pieces(&cube, corner, edge);
	for (i = 0; i < 12; i++)
		ep[i] = edge[i] & _pbits;
	for (i = 0; i < 8; i++)
		cp[i] = corner[i] & _pbits;

	if (permsign(ep, 12) != permsign(cp, 8))
		goto issolvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++) {
		piece = edge[i];
		eo += (piece & _eobit) >> _eoshift;
	}
	if (eo % 2 != 0)
		goto issolvable_eo;

	co = 0;
	for (i = 0; i < 8; i++) {
		piece = corner[i];
		co += (piece & _cobits) >> _coshift;
	}
	if (co % 3 != 0)
		goto issolvable_co;

	return true;

issolvable_parity:
	DBG_LOG("EP and CP parities are different\n");
	return false;
issolvable_eo:
	DBG_LOG("Odd number of flipped edges\n");
	return false;
issolvable_co:
	DBG_LOG("Sum of corner orientation is not multiple of 3\n");
	return false;
}

bool
issolved(cube_t cube)
{
	return equal(cube, solved);
}

bool
iserror(cube_t cube)
{
	return equal(cube, zero);
}

cube_t
inverse(cube_t cube)
{
	uint8_t i, piece, orien, e[12], c[8], edge[12], corner[8];

	DBG_ASSERT(isconsistent(cube), zero,
	    "inverse error: inconsistent cube\n");

	pieces(&cube, corner, edge);
	
	for (i = 0; i < 12; i++) {
		piece = edge[i];
		orien = piece & _eobit;
		e[piece & _pbits] = i | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		c[piece & _pbits] = i | orien;
	}

	return cubefromarray(c, e);
}

cube_t
applymoves(cube_t cube, const char *buf)
{
	uint8_t r, m;
	const char *b;

	DBG_ASSERT(isconsistent(cube), zero,
	    "move error: inconsistent cube\n");

	for (b = buf; *b != '\0'; b++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if (*b == '\0')
			goto applymoves_finish;
		if ((r = readmove(*b)) == _error)
			goto applymoves_error;
		if ((m = readmodifier(*(b+1))) != 0)
			b++;
		cube = move(cube, r + m);
	}

applymoves_finish:
	return cube;

applymoves_error:
	DBG_LOG("applymoves error\n");
	return zero;
}

cube_t
applytrans(cube_t cube, const char *buf)
{
	uint8_t t;

	DBG_ASSERT(isconsistent(cube), zero,
	    "transformation error: inconsistent cube\n");

	t = readtrans(buf);

	return transform(cube, t);
}

cube_t
readcube(const char *format, const char *buf)
{
	int i;

	for (i = 0; ioformat[i].read != NULL; i++)
		if (!strcmp(format, ioformat[i].name))
			return ioformat[i].read(buf);

	DBG_LOG("Cannot read cube in the given format\n");
	return zero;
}

void
writecube(const char *format, cube_t cube, char *buf)
{
	char *errormsg;
	size_t len;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	int i;

	for (i = 0; ioformat[i].write != NULL; i++) {
		if (!strcmp(format, ioformat[i].name)) {
			ioformat[i].write(cube, buf);
			return;
		}
	}

	errormsg = "ERROR: cannot write cube in the given format";

writecube_error:
	DBG_LOG("writecube error, see stdout for details\n");
	len = strlen(errormsg);
	memcpy(buf, errormsg, len);
	buf[len] = '\n';
	buf[len+1] = '\0';
}

_static int
permsign(uint8_t *a, int n)
{
	int i, j;
	uint8_t ret = 0;

	for (i = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

_static uint8_t
readco(const char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _ctwist_cw;
	if (*str == '2')
		return _ctwist_ccw;

	DBG_LOG("Error reading CO\n");
	return _error;
}

_static uint8_t
readcp(const char *str)
{
	uint8_t c;

	for (c = 0; c < 8; c++)
		if (!strncmp(str, cornerstr[c], 3) ||
		    !strncmp(str, cornerstralt[c], 3))
			return c;

	DBG_LOG("Error reading CP\n");
	return _error;
}

_static uint8_t
readeo(const char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _eflip;

	DBG_LOG("Error reading EO\n");
	return _error;
}

_static uint8_t
readep(const char *str)
{
	uint8_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

	DBG_LOG("Error reading EP\n");
	return _error;
}

_static cube_t
readcube_B32(const char *buf)
{
	int i;
	uint8_t c[8], e[12];

	for (i = 0; i < 8; i++) {
		c[i] = b32tocorner(buf[i]);
		DBG_ASSERT(c[i] < 255, zero,
		    "Error reading B32 corner %d (char %d)\n", i, i);
	}

	for (i = 0; i < 12; i++) {
		e[i] = b32toedge(buf[i+9]);
		DBG_ASSERT(e[i] < 255, zero,
		    "Error reading B32 edge %d (char %d)\n", i, i+9);
	}

	return cubefromarray(c, e);
}

_static cube_t
readcube_H48(const char *buf)
{
	int i;
	uint8_t piece, orient, c[8], e[12];
	const char *b;
	
	b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			return zero;
		b += 2;
		if ((orient = readeo(b)) == _error)
			return zero;
		b++;
		e[i] = piece | orient;
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			return zero;
		b += 3;
		if ((orient = readco(b)) == _error)
			return zero;
		b++;
		c[i] = piece | orient;
	}

	return cubefromarray(c, e);
}

_static uint8_t
readpiece_LST(const char **b)
{
	uint8_t ret;
	bool read;

	while (**b == ',' || **b == ' ' || **b == '\t' || **b == '\n')
		(*b)++;

	for (ret = 0, read = false; **b >= '0' && **b <= '9'; (*b)++) {
		read = true;
		ret = ret * 10 + (**b) - '0';
	}

	return read ? ret : _error;
}

_static cube_t
readcube_LST(const char *buf)
{
	int i;
	uint8_t c[8], e[12];

	for (i = 0; i < 8; i++)
		c[i] = readpiece_LST(&buf);

	for (i = 0; i < 12; i++)
		e[i] = readpiece_LST(&buf);

	return cubefromarray(c, e);
}

_static int
writepiece_LST(uint8_t piece, char *buf)
{
	char digits[3];
	int i, len;

	len = 0;
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

_static void
writecube_B32(cube_t cube, char *buf)
{
	int i;
	uint8_t corner[8], edge[12];

	pieces(&cube, corner, edge);

	for (i = 0; i < 8; i++)
		buf[i] = cornertob32(corner[i]);

	buf[8] = '=';

	for (i = 0; i < 12; i++)
		buf[i+9] = edgetob32(edge[i]);

	buf[21] = '\0';
}

_static void
writecube_H48(cube_t cube, char *buf)
{
	uint8_t piece, perm, orient, corner[8], edge[12];
	int i;

	pieces(&cube, corner, edge);

	for (i = 0; i < 12; i++) {
		piece = edge[i];
		perm = piece & _pbits;
		orient = (piece & _eobit) >> _eoshift;
		buf[4*i    ] = edgestr[perm][0];
		buf[4*i + 1] = edgestr[perm][1];
		buf[4*i + 2] = orient + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = corner[i];
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

_static void
writecube_LST(cube_t cube, char *buf)
{
	int i;
	size_t ptr;
	uint8_t piece, corner[8], edge[12];

	ptr = 0;
	pieces(&cube, corner, edge);

	for (i = 0; i < 8; i++) {
		piece = corner[i];
		ptr += writepiece_LST(piece, buf + ptr);
	}

	for (i = 0; i < 12; i++) {
		piece = edge[i];
		ptr += writepiece_LST(piece, buf + ptr);
	}

	*(buf+ptr-2) = 0;
}

_static uint8_t
b32toedge(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'g'))
		return 255;

	return c <= 'Z' ? (uint8_t)(c - 'A') : (uint8_t)(c - 'a');
}

_static uint8_t
b32tocorner(char c) {
	uint8_t val;

	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'g'))
		return 255;

	val = c <= 'Z' ? (uint8_t)(c - 'A') : (uint8_t)(c - 'a') + 26;

	return (val & 7) | ((val & 24) << 2);
}

_static char
edgetob32(uint8_t edge)
{
	return edge <= 26 ? 'A' + (char)edge : 'a' + (char)(edge - 26);
}

_static char
cornertob32(uint8_t corner)
{
	uint8_t val;

	val = (corner & 7) | ((corner & 96) >> 2);

	return val <= 26 ? 'A' + (char)val : 'a' + (char)(val - 26);
}

_static uint8_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return _move_U;
	case 'D':
		return _move_D;
	case 'R':
		return _move_R;
	case 'L':
		return _move_L;
	case 'F':
		return _move_F;
	case 'B':
		return _move_B;
	default:
		return _error;
	}
}

_static uint8_t
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

_static uint8_t
readtrans(const char *buf)
{
	uint8_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	DBG_LOG("readtrans error\n");
	return _error;
}

_static int
writemoves(uint8_t *m, int n, char *buf)
{
	int i;
	size_t len;
	const char *s;
	char *b;

	for (i = 0, b = buf; i < n; i++, b++) {
		s = movestr[m[i]];
		len = strlen(s);
		memcpy(b, s, len);
		b += len;	
		*b = ' ';
	}

	if (b != buf)
		b--; /* Remove last space */
	*b = '\0';

	return b - buf;
}

_static void
writetrans(uint8_t t, char *buf)
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
}

_static cube_t
move(cube_t c, uint8_t m)
{
	switch (m) {
	case _move_U:
		return _move(U, c);
	case _move_U2:
		return _move(U2, c);
	case _move_U3:
		return _move(U3, c);
	case _move_D:
		return _move(D, c);
	case _move_D2:
		return _move(D2, c);
	case _move_D3:
		return _move(D3, c);
	case _move_R:
		return _move(R, c);
	case _move_R2:
		return _move(R2, c);
	case _move_R3:
		return _move(R3, c);
	case _move_L:
		return _move(L, c);
	case _move_L2:
		return _move(L2, c);
	case _move_L3:
		return _move(L3, c);
	case _move_F:
		return _move(F, c);
	case _move_F2:
		return _move(F2, c);
	case _move_F3:
		return _move(F3, c);
	case _move_B:
		return _move(B, c);
	case _move_B2:
		return _move(B2, c);
	case _move_B3:
		return _move(B3, c);
	default:
		DBG_LOG("move error, unknown move\n");
		return zero;
	}
}

/*
TODO transform is now relegated to a separated file because it is too long.
It would be nice to make it shorter without loosing performance.
*/
