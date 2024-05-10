_static_inline bool allowednextmove(uint8_t *, uint8_t);
_static_inline uint8_t inverse_trans(uint8_t);
_static_inline uint8_t movebase(uint8_t);
_static_inline uint8_t moveaxis(uint8_t);

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
