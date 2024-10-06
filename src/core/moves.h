#define MOVE(M, c) compose(c, MOVE_CUBE_ ## M)
#define MOVE_NOCO(M, c) compose_epcpeo(c, MOVE_CUBE_ ## M)
#define PREMOVE(M, c) compose(MOVE_CUBE_ ## M, c)
#define PREMOVE_NOCO(M, c) compose_epcpeo(MOVE_CUBE_ ## M, c)

STATIC_INLINE bool allowednextmove(uint8_t *, uint8_t);

STATIC_INLINE uint8_t inverse_trans(uint8_t);
STATIC_INLINE uint8_t movebase(uint8_t);
STATIC_INLINE uint8_t moveaxis(uint8_t);
STATIC_INLINE uint32_t disable_moves(uint32_t, uint8_t);

STATIC cube_t move(cube_t, uint8_t);
STATIC cube_t premove(cube_t, uint8_t);
STATIC uint8_t inverse_move(uint8_t);
STATIC void invertmoves(uint8_t *, uint8_t, uint8_t *);

STATIC int readmoves(const char *, int, uint8_t *);
STATIC cube_t applymoves(cube_t, const char *);
STATIC cube_t frommoves(const char *);

#define FOREACH_READMOVE(ARG_BUF, ARG_MOVE, ARG_C, ARG_MAX, \
	LABEL_ERROR, ARG_ACTION) \
	const char *VAR_B; \
	uint8_t VAR_MOVE_NOMOD, VAR_MOD; \
	for (VAR_B = ARG_BUF, ARG_C = 0; *VAR_B != '\0'; VAR_B++, ARG_C++) { \
		while (*VAR_B == ' ' || *VAR_B == '\t' || *VAR_B == '\n') \
			VAR_B++; \
		if (*VAR_B == '\0' || ARG_C == ARG_MAX) \
			break; \
		if ((VAR_MOVE_NOMOD = readmove(*VAR_B)) == UINT8_ERROR) \
			goto LABEL_ERROR; \
		if ((VAR_MOD = readmodifier(*(VAR_B+1))) != 0) \
			VAR_B++; \
		ARG_MOVE = VAR_MOVE_NOMOD + VAR_MOD; \
		ARG_ACTION \
	}

STATIC bool
allowednextmove(uint8_t *moves, uint8_t n)
{
	uint8_t base[3], axis[3];

	if (n < 2)
		return true;

	base[0] = movebase(moves[n-1]);
	axis[0] = moveaxis(moves[n-1]);
	base[1] = movebase(moves[n-2]);
	axis[1] = moveaxis(moves[n-2]);

	if (base[0] == base[1] || (axis[0] == axis[1] && base[0] < base[1]))
		return false;

	if (n == 2)
		return true;

	base[2] = movebase(moves[n-3]);
	axis[2] = moveaxis(moves[n-3]);

	return axis[1] != axis[2] || base[0] != base[2];
}

STATIC_INLINE uint32_t 
disable_moves(uint32_t current_result, uint8_t base_index)
{
	return current_result & ~(7 << base_index);
}

STATIC_INLINE uint8_t
inverse_trans(uint8_t t)
{
	return inverse_trans_table[t];
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

STATIC cube_t
move(cube_t c, uint8_t m)
{
	switch (m) {
	case MOVE_U:
		return MOVE_NOCO(U, c);
	case MOVE_U2:
		return MOVE_NOCO(U2, c);
	case MOVE_U3:
		return MOVE_NOCO(U3, c);
	case MOVE_D:
		return MOVE_NOCO(D, c);
	case MOVE_D2:
		return MOVE_NOCO(D2, c);
	case MOVE_D3:
		return MOVE_NOCO(D3, c);
	case MOVE_R:
		return MOVE(R, c);
	case MOVE_R2:
		return MOVE_NOCO(R2, c);
	case MOVE_R3:
		return MOVE(R3, c);
	case MOVE_L:
		return MOVE(L, c);
	case MOVE_L2:
		return MOVE_NOCO(L2, c);
	case MOVE_L3:
		return MOVE(L3, c);
	case MOVE_F:
		return MOVE(F, c);
	case MOVE_F2:
		return MOVE_NOCO(F2, c);
	case MOVE_F3:
		return MOVE(F3, c);
	case MOVE_B:
		return MOVE(B, c);
	case MOVE_B2:
		return MOVE_NOCO(B2, c);
	case MOVE_B3:
		return MOVE(B3, c);
	default:
		LOG("move error, unknown move\n");
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
		LOG("move error, unknown move\n");
		return ZERO_CUBE;
	}
}

STATIC uint8_t
inverse_move(uint8_t m)
{
	return m - 2 * (m % 3) + 2;	
}

/*
GCC has issues when -Wstringop-overflow is used together with O3. It produces
warnings like the following:

In function 'invertmoves',
    inlined from 'solve_h48_appendsolution' at src/solvers/h48/solve.h:81:3,
    inlined from 'solve_h48_dfs.isra' at src/solvers/h48/solve.h:139:3:
warning: writing 32 bytes into a region of size 0 [-Wstringop-overflow=]
  197 |                 ret[i] = inverse_move(moves[nmoves - i - 1]);
      |                 ~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In function 'solve_h48_dfs.isra':
note: at offset 192 into destination object 'invertedpremoves' of size 20
   71 |         uint8_t invertedpremoves[MAXLEN];

Clang does not give any warning.
Someone else complained here: https://access.redhat.com/solutions/6755371

To solve this issue temporarily, we use a lower optimization setting for
this function only.

TODO check if the issue is resolved
*/
#pragma GCC push_options
#pragma GCC optimize ("O2")
STATIC void
invertmoves(uint8_t *moves, uint8_t nmoves, uint8_t *ret)
{
	uint8_t i;

	for (i = 0; i < nmoves; i++)
		ret[i] = inverse_move(moves[nmoves - i - 1]);
}
#pragma GCC pop_options

STATIC int
readmoves(const char *buf, int max, uint8_t *ret)
{
	uint8_t m;
	int c;

	FOREACH_READMOVE(buf, m, c, max, readmoves_error,
		ret[c] = m;
	)

	return c;

readmoves_error:
	LOG("readmoves error\n");
	return -1;
}

STATIC cube_t
applymoves(cube_t cube, const char *buf)
{
	int c;
	uint8_t m;

	DBG_ASSERT(isconsistent(cube), ZERO_CUBE,
	    "move error: inconsistent cube\n");

	FOREACH_READMOVE(buf, m, c, -1, applymoves_error,
		cube = move(cube, m);
	)

	return cube;

applymoves_error:
	LOG("applymoves error\n");
	return ZERO_CUBE;
}

STATIC cube_t
frommoves(const char *buf)
{
	return applymoves(SOLVED_CUBE, buf);
}
