STATIC uint8_t readmove(char);
STATIC int64_t readmoves(const char *, size_t n, uint8_t [n]);
STATIC int64_t countmoves(const char *);
STATIC uint8_t readmodifier(char);
STATIC int64_t writemoves(size_t n, const uint8_t [n], size_t m, char [m]);

#define FOREACH_READMOVE(ARG_BUF, ARG_MOVE, ARG_C, ARG_MAX, \
	RET_ERROR, ARG_ACTION) \
	const char *VAR_B; \
	uint8_t VAR_MOVE_NOMOD, VAR_MOD; \
	for (VAR_B = ARG_BUF, ARG_C = 0; *VAR_B != '\0'; VAR_B++, ARG_C++) { \
		while (*VAR_B == ' ' || *VAR_B == '\t' || *VAR_B == '\n') \
			VAR_B++; \
		if (*VAR_B == '\0' || ARG_C == ARG_MAX) \
			break; \
		if ((VAR_MOVE_NOMOD = readmove(*VAR_B)) == UINT8_ERROR) { \
			LOG("Unknown move: %c\n", *VAR_B); \
			return RET_ERROR; \
		} \
		if ((VAR_MOD = readmodifier(*(VAR_B+1))) != 0) \
			VAR_B++; \
		ARG_MOVE = VAR_MOVE_NOMOD + VAR_MOD; \
		ARG_ACTION \
	}

STATIC uint8_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return MOVE_U;
	case 'D':
		return MOVE_D;
	case 'R':
		return MOVE_R;
	case 'L':
		return MOVE_L;
	case 'F':
		return MOVE_F;
	case 'B':
		return MOVE_B;
	default:
		return UINT8_ERROR;
	}
}

STATIC uint8_t
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

STATIC int64_t
readmoves(const char *buf, size_t n, uint8_t ret[n])
{
	uint8_t m;
	uint64_t c;

	FOREACH_READMOVE(buf, m, c, n, NISSY_ERROR_INVALID_MOVES,
		ret[c] = m;
	)

	return (int64_t)c;
}

STATIC int64_t
countmoves(const char *buf)
{
	uint8_t m;
	uint64_t c;

	FOREACH_READMOVE(buf, m, c, INT_MAX, NISSY_ERROR_INVALID_MOVES,
		{}
	)

	(void)m; /* Ignore "variable set but not used" warning */

	return (int64_t)c;
}

STATIC int64_t
writemoves(
	size_t nmoves,
	const uint8_t m[nmoves],
	size_t buf_size,
	char buf[buf_size]
)
{
	size_t i, len, w;
	const char *s;

	if (buf_size == 0) {
		LOG("Error: cannot write moves to buffer of size 0.\n");
		return NISSY_ERROR_BUFFER_SIZE;
	}

	for (i = 0, w = 0; i < nmoves; i++, w++) {
		s = movestr[m[i]];
		len = strlen(s);
		if (len + w >= buf_size) {
			LOG("Error: the given buffer is too small for "
			     "writing the given moves.\n");
			goto writemoves_error;
		}
		memcpy(buf+w, s, len);
		w += len;
		buf[w] = ' ';
	}

	if (w > 0) w--; /* Remove last space */
	buf[w] = '\0';

	return (int64_t)w;

writemoves_error:
	*buf = '\0';
	return NISSY_ERROR_BUFFER_SIZE;
}
