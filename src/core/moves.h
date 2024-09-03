#define _move(M, c) compose(c, _move_cube_ ## M)
#define _premove(M, c) compose(_move_cube_ ## M, c)

_static_inline bool allowednextmove(uint8_t *, uint8_t);

_static_inline uint8_t inverse_trans(uint8_t);
_static_inline uint8_t movebase(uint8_t);
_static_inline uint8_t moveaxis(uint8_t);
_static_inline uint32_t disable_moves(uint32_t, uint8_t);

_static cube_t move(cube_t, uint8_t);
_static cube_t premove(cube_t, uint8_t);
_static uint8_t inverse_move(uint8_t);
_static void invertmoves(uint8_t *, uint8_t, uint8_t *);

_static cube_t applymoves(cube_t, const char *);
_static cube_t frommoves(const char *);

_static bool
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

_static_inline uint32_t 
disable_moves(uint32_t current_result, uint8_t base_index)
{
	return current_result & ~(7 << base_index);
}

_static_inline uint8_t
inverse_trans(uint8_t t)
{
	return inverse_trans_table[t];
}

_static_inline uint8_t
movebase(uint8_t move)
{
	return move / 3;
}

_static_inline uint8_t
moveaxis(uint8_t move)
{
	return move / 6;
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
		LOG("move error, unknown move\n");
		return zero;
	}
}

_static cube_t
premove(cube_t c, uint8_t m)
{
	switch (m) {
	case _move_U:
		return _premove(U3, c);
	case _move_U2:
		return _premove(U2, c);
	case _move_U3:
		return _premove(U, c);
	case _move_D:
		return _premove(D3, c);
	case _move_D2:
		return _premove(D2, c);
	case _move_D3:
		return _premove(D, c);
	case _move_R:
		return _premove(R3, c);
	case _move_R2:
		return _premove(R2, c);
	case _move_R3:
		return _premove(R, c);
	case _move_L:
		return _premove(L3, c);
	case _move_L2:
		return _premove(L2, c);
	case _move_L3:
		return _premove(L, c);
	case _move_F:
		return _premove(F3, c);
	case _move_F2:
		return _premove(F2, c);
	case _move_F3:
		return _premove(F, c);
	case _move_B:
		return _premove(B3, c);
	case _move_B2:
		return _premove(B2, c);
	case _move_B3:
		return _premove(B, c);
	default:
		LOG("move error, unknown move\n");
		return zero;
	}
}

_static uint8_t
inverse_move(uint8_t m)
{
	return m - 2 * (m % 3) + 2;	
}

_static void
invertmoves(uint8_t *moves, uint8_t nmoves, uint8_t *ret)
{
	uint8_t i;

	for (i = 0; i < nmoves; i++)
		ret[i] = inverse_move(moves[nmoves - i - 1]);
}

_static cube_t
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
	LOG("applymoves error\n");
	return zero;
}

_static cube_t
frommoves(const char *buf)
{
	return applymoves(solved, buf);
}
