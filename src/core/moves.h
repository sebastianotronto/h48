#define MOVE(M, c) compose(c, MOVE_CUBE_ ## M)
#define PREMOVE(M, c) compose(MOVE_CUBE_ ## M, c)

STATIC_INLINE bool allowednextmove(uint8_t *, uint8_t);

STATIC_INLINE uint8_t inverse_trans(uint8_t);
STATIC_INLINE uint8_t movebase(uint8_t);
STATIC_INLINE uint8_t moveaxis(uint8_t);
STATIC_INLINE uint32_t disable_moves(uint32_t, uint8_t);

STATIC cube_t move(cube_t, uint8_t);
STATIC cube_t premove(cube_t, uint8_t);
STATIC uint8_t inverse_move(uint8_t);
STATIC void invertmoves(uint8_t *, uint8_t, uint8_t *);

STATIC cube_t applymoves(cube_t, const char *);
STATIC cube_t frommoves(const char *);

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
		LOG("move error, unknown move\n");
		return ZERO_CUBE;
	}
}

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

STATIC void
invertmoves(uint8_t *moves, uint8_t nmoves, uint8_t *ret)
{
	uint8_t i;

	for (i = 0; i < nmoves; i++)
		ret[i] = inverse_move(moves[nmoves - i - 1]);
}

STATIC cube_t
applymoves(cube_t cube, const char *buf)
{
	uint8_t r, m;
	const char *b;

	DBG_ASSERT(isconsistent(cube), ZERO_CUBE,
	    "move error: inconsistent cube\n");

	for (b = buf; *b != '\0'; b++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if (*b == '\0')
			goto applymoves_finish;
		if ((r = readmove(*b)) == UINT8_ERROR)
			goto applymoves_error;
		if ((m = readmodifier(*(b+1))) != 0)
			b++;
		cube = move(cube, r + m);
	}

applymoves_finish:
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
