#define MOVE(M, c) compose(c, MOVE_CUBE_ ## M)
#define PREMOVE(M, c) compose(MOVE_CUBE_ ## M, c)

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
