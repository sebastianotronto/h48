STATIC uint8_t readco(const char *);
STATIC uint8_t readcp(const char *);
STATIC uint8_t readeo(const char *);
STATIC uint8_t readep(const char *);
STATIC cube_t readcube_B32(const char *);
STATIC cube_t readcube_H48(const char *);
STATIC uint8_t readpiece_LST(const char **);
STATIC cube_t readcube_LST(const char *);

STATIC int writepiece_LST(uint8_t, char *);
STATIC void writecube_B32(cube_t, char *);
STATIC void writecube_H48(cube_t, char *);
STATIC void writecube_LST(cube_t, char *);

STATIC uint8_t b32toedge(char);
STATIC uint8_t b32tocorner(char);
STATIC char edgetob32(uint8_t);
STATIC char cornertob32(uint8_t);

STATIC struct {
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

cube_t
readcube(const char *format, const char *buf)
{
	int i;

	for (i = 0; ioformat[i].read != NULL; i++)
		if (!strcmp(format, ioformat[i].name))
			return ioformat[i].read(buf);

	LOG("Cannot read cube in the given format\n");
	return ZERO_CUBE;
}

void
writecube(const char *format, cube_t cube, char *buf)
{
	char *errormsg;
	size_t len;

	if (!isconsistent(cube)) {
		errormsg = "ERROR: inconsistent";
		goto writecube_error;
	}

	int i;

	for (i = 0; ioformat[i].write != NULL; i++) {
		if (!strcmp(format, ioformat[i].name)) {
			ioformat[i].write(cube, buf);
			return;
		}
	}

	errormsg = "ERROR: format";

writecube_error:
	LOG("writecube error, see stdout for details\n");
	len = strlen(errormsg);
	memcpy(buf, errormsg, len);
	buf[len] = '\n';
	buf[len+1] = '\0';
}

STATIC uint8_t
readco(const char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return CTWIST_CW;
	if (*str == '2')
		return CTWIST_CCW;

	LOG("Error reading CO\n");
	return UINT8_ERROR;
}

STATIC uint8_t
readcp(const char *str)
{
	uint8_t c;

	for (c = 0; c < 8; c++)
		if (!strncmp(str, cornerstr[c], 3) ||
		    !strncmp(str, cornerstralt[c], 3))
			return c;

	LOG("Error reading CP\n");
	return UINT8_ERROR;
}

STATIC uint8_t
readeo(const char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return EFLIP;

	LOG("Error reading EO\n");
	return UINT8_ERROR;
}

STATIC uint8_t
readep(const char *str)
{
	uint8_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

	LOG("Error reading EP\n");
	return UINT8_ERROR;
}

STATIC cube_t
readcube_B32(const char *buf)
{
	int i;
	uint8_t c[8], e[12];

	for (i = 0; i < 8; i++) {
		c[i] = b32tocorner(buf[i]);
		DBG_ASSERT(c[i] < 255, ZERO_CUBE,
		    "Error reading B32 corner %d (char %d)\n", i, i);
	}

	for (i = 0; i < 12; i++) {
		e[i] = b32toedge(buf[i+9]);
		DBG_ASSERT(e[i] < 255, ZERO_CUBE,
		    "Error reading B32 edge %d (char %d)\n", i, i+9);
	}

	return cubefromarray_ce(c, e);
}

STATIC cube_t
readcube_H48(const char *buf)
{
	int i;
	uint8_t piece, orient, c[8], e[12];
	const char *b;
	
	b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == UINT8_ERROR)
			return ZERO_CUBE;
		b += 2;
		if ((orient = readeo(b)) == UINT8_ERROR)
			return ZERO_CUBE;
		b++;
		e[i] = piece | orient;
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == UINT8_ERROR)
			return ZERO_CUBE;
		b += 3;
		if ((orient = readco(b)) == UINT8_ERROR)
			return ZERO_CUBE;
		b++;
		c[i] = piece | orient;
	}

	return cubefromarray_ce(c, e);
}

STATIC uint8_t
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

	return read ? ret : UINT8_ERROR;
}

STATIC cube_t
readcube_LST(const char *buf)
{
	int i;
	uint8_t c[8], e[12];

	for (i = 0; i < 8; i++)
		c[i] = readpiece_LST(&buf);

	for (i = 0; i < 12; i++)
		e[i] = readpiece_LST(&buf);

	return cubefromarray_ce(c, e);
}

STATIC int
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

STATIC void
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

STATIC void
writecube_H48(cube_t cube, char *buf)
{
	uint8_t piece, perm, orient, corner[8], edge[12];
	int i;

	pieces(&cube, corner, edge);

	for (i = 0; i < 12; i++) {
		piece = edge[i];
		perm = piece & PBITS;
		orient = (piece & EOBIT) >> EOSHIFT;
		buf[4*i    ] = edgestr[perm][0];
		buf[4*i + 1] = edgestr[perm][1];
		buf[4*i + 2] = orient + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = corner[i];
		perm = piece & PBITS;
		orient = (piece & COBITS) >> COSHIFT;
		buf[48 + 5*i    ] = cornerstr[perm][0];
		buf[48 + 5*i + 1] = cornerstr[perm][1];
		buf[48 + 5*i + 2] = cornerstr[perm][2];
		buf[48 + 5*i + 3] = orient + '0';
		buf[48 + 5*i + 4] = ' ';
	}

	buf[48+39] = '\0';
}

STATIC void
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

STATIC uint8_t
b32toedge(char c)
{
	if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'f')))
		return 255;

	return c <= 'Z' ? (uint8_t)(c - 'A') : (uint8_t)(c - 'a') + 26;
}

STATIC uint8_t
b32tocorner(char c) {
	uint8_t val;

	if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'f')))
		return 255;

	val = c <= 'Z' ? (uint8_t)(c - 'A') : (uint8_t)(c - 'a') + 26;

	return (val & 7) | ((val & 24) << 2);
}

STATIC char
edgetob32(uint8_t edge)
{
	return edge < 26 ? 'A' + (char)edge : 'a' + (char)(edge - 26);
}

STATIC char
cornertob32(uint8_t corner)
{
	uint8_t val;

	val = (corner & 7) | ((corner & 96) >> 2);

	return val < 26 ? 'A' + (char)val : 'a' + (char)(val - 26);
}
