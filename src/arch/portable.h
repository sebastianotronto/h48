#define STATIC_CUBE(c_ufr, c_ubl, c_dfl, c_dbr, c_ufl, c_ubr, c_dfr, c_dbl, \
    e_uf, e_ub, e_db, e_df, e_ur, e_ul, e_dl, e_dr, e_fr, e_fl, e_bl, e_br) \
    ((cube_t) { \
        .corner = { c_ufr, c_ubl, c_dfl, c_dbr, c_ufl, c_ubr, c_dfr, c_dbl }, \
        .edge = { e_uf, e_ub, e_db, e_df, e_ur, e_ul, \
                  e_dl, e_dr, e_fr, e_fl, e_bl, e_br } })
#define ZERO_CUBE STATIC_CUBE( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define SOLVED_CUBE STATIC_CUBE( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)

/* TODO: optimize this (use bit tricks?) */
STATIC_INLINE int
popcount_u32(uint32_t x)
{
	int ret;

	for (ret = 0; x != 0; x >>= 1)
		ret += x & 1;

	return ret;
}

STATIC void
pieces(cube_t cube[static 1], uint8_t c[static 8], uint8_t e[static 12])
{
	memcpy(c, cube->corner, 8);
	memcpy(e, cube->edge, 12);
}

STATIC_INLINE bool
equal(cube_t c1, cube_t c2)
{
	uint8_t i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.corner[i] == c2.corner[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.edge[i] == c2.edge[i];

	return ret;
}

STATIC_INLINE cube_t
invertco(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = c.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & COBITS_2;
		ret.corner[i] = (piece & PBITS) | orien;
	}

	return ret;
}

STATIC_INLINE void
compose_edges_inplace(cube_t c1, cube_t c2, cube_t *ret)
{
	uint8_t i, piece1, piece2, p, orien;

	for (i = 0; i < 12; i++) {
		piece2 = c2.edge[i];
		p = piece2 & PBITS;
		piece1 = c1.edge[p];
		orien = (piece2 ^ piece1) & EOBIT;
		ret->edge[i] = (piece1 & PBITS) | orien;
	}
}

STATIC_INLINE void
compose_corners_inplace(cube_t c1, cube_t c2, cube_t *ret)
{
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	for (i = 0; i < 8; i++) {
		piece2 = c2.corner[i];
		p = piece2 & PBITS;
		piece1 = c1.corner[p];
		aux = (piece2 & COBITS) + (piece1 & COBITS);
		auy = (aux + CTWIST_CW) >> 2;
		orien = (aux + auy) & COBITS_2;
		ret->corner[i] = (piece1 & PBITS) | orien;
	}
}

STATIC_INLINE cube_t
compose_edges(cube_t c1, cube_t c2)
{
	cube_t ret = ZERO_CUBE;

	compose_edges_inplace(c1, c2, &ret);

	return ret;
}

STATIC_INLINE cube_t
compose_corners(cube_t c1, cube_t c2)
{
	cube_t ret = ZERO_CUBE;

	compose_corners_inplace(c1, c2, &ret);

	return ret;
}

STATIC_INLINE cube_t
compose(cube_t c1, cube_t c2)
{
	cube_t ret = ZERO_CUBE;

	compose_edges_inplace(c1, c2, &ret);
	compose_corners_inplace(c1, c2, &ret);

	return ret;
}

cube_t
inverse(cube_t cube)
{
	uint8_t i, piece, orien;
	cube_t ret;

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		orien = piece & EOBIT;
		ret.edge[piece & PBITS] = i | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & COBITS_2;
		ret.corner[piece & PBITS] = i | orien;
	}

	return ret;
}

STATIC_INLINE int64_t
coord_co(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 3)
		ret += p * (c.corner[i] >> COSHIFT);

	return ret;
}

/*
For corner separation, we consider the axis (a.k.a. tetrad) each
corner belongs to as 0 or 1 and we translate this sequence into binary.
Ignoring the last bit, we have a value up to 2^7, but not all values are
possible. Encoding this as a number from 0 to C(8,4) would save about 40%
of space, but we are not going to use this coordinate in large tables.
*/
STATIC_INLINE int64_t
coord_csep(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 2)
		ret += p * ((c.corner[i] & CSEPBIT) >> 2);

	return ret;
}

STATIC_INLINE int64_t
coord_cocsep(cube_t c)
{
	return (coord_co(c) << 7) + coord_csep(c);
}

STATIC_INLINE int64_t
coord_eo(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 1, p = 1; i < 12; i++, p *= 2)
		ret += p * (c.edge[i] >> EOSHIFT);

	return ret;
}

/*
We encode the edge separation as a number from 0 to C(12,4)*C(8,4).
It can be seen as the composition of two "subset index" coordinates.
*/
STATIC_INLINE int64_t
coord_esep(cube_t c)
{
	int64_t i, j, jj, k, l, ret1, ret2, bit1, bit2, is1;

	for (i = 0, j = 0, k = 4, l = 4, ret1 = 0, ret2 = 0; i < 12; i++) {
		/* Simple version:
		if (c.edge[i] & ESEPBIT_2) {
			ret1 += binomial[11-i][k--];
		} else {
			if (c.edge[i] & ESEPBIT_1)
				ret2 += binomial[7-j][l--];
			j++;
		}
		*/

		bit1 = (c.edge[i] & ESEPBIT_1) >> 2;
		bit2 = (c.edge[i] & ESEPBIT_2) >> 3;
		is1 = (1 - bit2) * bit1;

		ret1 += bit2 * binomial[11-i][k];
		k -= bit2;

		jj = j < 8;
		ret2 += jj * is1 * binomial[7-(j*jj)][l];
		l -= is1;
		j += (1-bit2);
	}

	return ret1 * 70 + ret2;
}

STATIC_INLINE void
copy_corners(cube_t dest[static 1], cube_t src)
{
	memcpy(&dest->corner, src.corner, sizeof(src.corner));
}

STATIC_INLINE void
copy_edges(cube_t dest[static 1], cube_t src)
{
	memcpy(&dest->edge, src.edge, sizeof(src.edge));
}

STATIC_INLINE void
set_eo(cube_t cube[static 1], int64_t eo)
{
	uint8_t i, sum, flip;

	for (sum = 0, i = 1; i < 12; i++, eo >>= 1) {
		flip = eo % 2;
		sum += flip;
		cube->edge[i] = (cube->edge[i] & ~EOBIT) | (EOBIT * flip);
	}
	cube->edge[0] = (cube->edge[0] & ~EOBIT) | (EOBIT * (sum % 2));
}

STATIC_INLINE cube_t
invcoord_esep(int64_t esep)
{
	cube_t ret;

	ret = SOLVED_CUBE;
	invcoord_esep_array(esep % 70, esep / 70, ret.edge);

	return ret;
}
