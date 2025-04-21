#define MOVE(M, c) compose(c, MOVE_CUBE_ ## M)
#define PREMOVE(M, c) compose(MOVE_CUBE_ ## M, c)

STATIC uint8_t readmove(char);
STATIC int64_t readmoves(const char *, size_t n, uint8_t [n]);
STATIC int64_t countmoves(const char *);
STATIC uint8_t readmodifier(char);
STATIC int64_t writemoves(size_t n, const uint8_t [n], size_t m, char [m]);

STATIC_INLINE bool allowednextmove(uint8_t, uint8_t);
STATIC bool allowedmoves(size_t n, const uint8_t [n]);

STATIC_INLINE uint8_t movebase(uint8_t);
STATIC_INLINE uint8_t moveaxis(uint8_t);
STATIC_INLINE bool isbase(uint8_t);
STATIC_INLINE bool parallel(uint8_t, uint8_t);

STATIC cube_t move(cube_t, uint8_t);
STATIC cube_t premove(cube_t, uint8_t);
STATIC uint8_t inverse_move(uint8_t);
STATIC void sortparallel_moves(size_t n, uint8_t [n]);
STATIC bool are_lastmoves_singlecw(size_t n, const uint8_t [n]);

STATIC cube_t applymoves(cube_t, const char *);

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

STATIC_INLINE bool
allowednextmove(uint8_t m1, uint8_t m2)
{
	return allowedmask[movebase(m1)] & (UINT32_C(1) << m2);
}

STATIC bool
allowedmoves(size_t n, const uint8_t m[n])
{
	uint8_t j;

	for (j = 1; j < n; j++)
		if (!allowednextmove(m[j-1], m[j]))
			return false;

	return true;
}

STATIC_INLINE uint8_t
movebase(uint8_t move)
{
	return move / 3;
}

STATIC_INLINE uint8_t
moveaxis(uint8_t move)
{
	return move / 6;
}

STATIC_INLINE bool
isbase(uint8_t move)
{
	return move == 3 * movebase(move);
}

STATIC_INLINE bool
parallel(uint8_t m1, uint8_t m2)
{
	return moveaxis(m1) == moveaxis(m2);
}

STATIC_INLINE uint8_t
moveopposite(uint8_t move)
{
	return movebase(move) == 2 * moveaxis(move) ? move + 3 : move - 3;
}

STATIC cube_t
move(cube_t c, uint8_t m)
{
	switch (m) {
	case MOVE_U:
		return MOVE(U, c);
	case MOVE_U2:
		return MOVE(U2, c);
	case MOVE_U3:
		return MOVE(U3, c);
	case MOVE_D:
		return MOVE(D, c);
	case MOVE_D2:
		return MOVE(D2, c);
	case MOVE_D3:
		return MOVE(D3, c);
	case MOVE_R:
		return MOVE(R, c);
	case MOVE_R2:
		return MOVE(R2, c);
	case MOVE_R3:
		return MOVE(R3, c);
	case MOVE_L:
		return MOVE(L, c);
	case MOVE_L2:
		return MOVE(L2, c);
	case MOVE_L3:
		return MOVE(L3, c);
	case MOVE_F:
		return MOVE(F, c);
	case MOVE_F2:
		return MOVE(F2, c);
	case MOVE_F3:
		return MOVE(F3, c);
	case MOVE_B:
		return MOVE(B, c);
	case MOVE_B2:
		return MOVE(B2, c);
	case MOVE_B3:
		return MOVE(B3, c);
	default:
		LOG("move error: unknown move %" PRIu8 "\n", m);
		return ZERO_CUBE;
	}
}

/* Applies the INVERSE of m BEFORE the scramble corresponding to c */
STATIC cube_t
premove(cube_t c, uint8_t m)
{
	switch (m) {
	case MOVE_U:
		return PREMOVE(U3, c);
	case MOVE_U2:
		return PREMOVE(U2, c);
	case MOVE_U3:
		return PREMOVE(U, c);
	case MOVE_D:
		return PREMOVE(D3, c);
	case MOVE_D2:
		return PREMOVE(D2, c);
	case MOVE_D3:
		return PREMOVE(D, c);
	case MOVE_R:
		return PREMOVE(R3, c);
	case MOVE_R2:
		return PREMOVE(R2, c);
	case MOVE_R3:
		return PREMOVE(R, c);
	case MOVE_L:
		return PREMOVE(L3, c);
	case MOVE_L2:
		return PREMOVE(L2, c);
	case MOVE_L3:
		return PREMOVE(L, c);
	case MOVE_F:
		return PREMOVE(F3, c);
	case MOVE_F2:
		return PREMOVE(F2, c);
	case MOVE_F3:
		return PREMOVE(F, c);
	case MOVE_B:
		return PREMOVE(B3, c);
	case MOVE_B2:
		return PREMOVE(B2, c);
	case MOVE_B3:
		return PREMOVE(B, c);
	default:
		LOG("premove error: unknown move %" PRIu8 "\n", m);
		return ZERO_CUBE;
	}
}

STATIC uint8_t
inverse_move(uint8_t m)
{
	return m - 2 * (m % 3) + 2;	
}

STATIC void
sortparallel_moves(size_t n, uint8_t moves[n])
{
	uint8_t i;

	if (n < 2)
		return;

	for (i = 0; i < n-1; i++)
		if (moveaxis(moves[i]) == moveaxis(moves[i+1]) &&
		    movebase(moves[i]) == movebase(moves[i+1]) + 1)
			SWAP(moves[i], moves[i+1]);
}

STATIC bool
are_lastmoves_singlecw(size_t n, const uint8_t moves[n])
{
	bool two;

	if (n == 0)
		return true;

	two = n > 1 && parallel(moves[n-1], moves[n-2]);

	return isbase(moves[n-1]) && (!two || isbase(moves[n-2]));
}

STATIC cube_t
applymoves(cube_t cube, const char *buf)
{
	int c;
	uint8_t m;

	DBG_ASSERT(isconsistent(cube), ZERO_CUBE,
	    "move error: inconsistent cube\n");

	FOREACH_READMOVE(buf, m, c, -1, ZERO_CUBE,
		cube = move(cube, m);
	)

	return cube;
}
