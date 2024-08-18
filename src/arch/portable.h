#define static_cube(c_ufr, c_ubl, c_dfl, c_dbr, c_ufl, c_ubr, c_dfr, c_dbl, \
    e_uf, e_ub, e_db, e_df, e_ur, e_ul, e_dl, e_dr, e_fr, e_fl, e_bl, e_br) \
    ((cube_t) { \
        .corner = { c_ufr, c_ubl, c_dfl, c_dbr, c_ufl, c_ubr, c_dfr, c_dbl }, \
        .edge = { e_uf, e_ub, e_db, e_df, e_ur, e_ul, \
                  e_dl, e_dr, e_fr, e_fl, e_bl, e_br } })
#define zero static_cube( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define solved static_cube( \
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)

_static void
pieces(cube_t *cube, uint8_t c[static 8], uint8_t e[static 12])
{
	memcpy(c, cube->corner, 8);
	memcpy(e, cube->edge, 12);
}

_static_inline bool
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

_static_inline cube_t
invertco(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = c.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[i] = (piece & _pbits) | orien;
	}

	return ret;
}

_static_inline void
compose_edges_inplace(cube_t c1, cube_t c2, cube_t *ret)
{
	uint8_t i, piece1, piece2, p, orien;

	for (i = 0; i < 12; i++) {
		piece2 = c2.edge[i];
		p = piece2 & _pbits;
		piece1 = c1.edge[p];
		orien = (piece2 ^ piece1) & _eobit;
		ret->edge[i] = (piece1 & _pbits) | orien;
	}
}

_static_inline void
compose_corners_inplace(cube_t c1, cube_t c2, cube_t *ret)
{
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	for (i = 0; i < 8; i++) {
		piece2 = c2.corner[i];
		p = piece2 & _pbits;
		piece1 = c1.corner[p];
		aux = (piece2 & _cobits) + (piece1 & _cobits);
		auy = (aux + _ctwist_cw) >> 2;
		orien = (aux + auy) & _cobits2;
		ret->corner[i] = (piece1 & _pbits) | orien;
	}
}

_static_inline cube_t
compose_edges(cube_t c1, cube_t c2)
{
	cube_t ret = zero;

	compose_edges_inplace(c1, c2, &ret);

	return ret;
}

_static_inline cube_t
compose_corners(cube_t c1, cube_t c2)
{
	cube_t ret = zero;

	compose_corners_inplace(c1, c2, &ret);

	return ret;
}

_static_inline cube_t
compose(cube_t c1, cube_t c2)
{
	cube_t ret = zero;

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
		orien = piece & _eobit;
		ret.edge[piece & _pbits] = i | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[piece & _pbits] = i | orien;
	}

	return ret;
}

_static_inline int64_t
coord_co(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 3)
		ret += p * (c.corner[i] >> _coshift);

	return ret;
}

/*
For corner separation, we consider the axis (a.k.a. tetrad) each
corner belongs to as 0 or 1 and we translate this sequence into binary.
Ignoring the last bit, we have a value up to 2^7, but not all values are
possible. Encoding this as a number from 0 to C(8,4) would save about 40%
of space, but we are not going to use this coordinate in large tables.
*/
_static_inline int64_t
coord_csep(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 2)
		ret += p * ((c.corner[i] & _csepbit) >> 2);

	return ret;
}

_static_inline int64_t
coord_cocsep(cube_t c)
{
	return (coord_co(c) << 7) + coord_csep(c);
}

_static_inline int64_t
coord_eo(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 1, p = 1; i < 12; i++, p *= 2)
		ret += p * (c.edge[i] >> _eoshift);

	return ret;
}

/*
We encode the edge separation as a number from 0 to C(12,4)*C(8,4).
It can be seen as the composition of two "subset index" coordinates.
*/
_static_inline int64_t
coord_esep(cube_t c)
{
	int64_t i, j, jj, k, l, ret1, ret2, bit1, bit2, is1;

	for (i = 0, j = 0, k = 4, l = 4, ret1 = 0, ret2 = 0; i < 12; i++) {
		/* Simple version:
		if (c.edge[i] & _esepbit2) {
			ret1 += binomial[11-i][k--];
		} else {
			if (c.edge[i] & _esepbit1)
				ret2 += binomial[7-j][l--];
			j++;
		}
		*/

		bit1 = (c.edge[i] & _esepbit1) >> 2;
		bit2 = (c.edge[i] & _esepbit2) >> 3;
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

_static_inline void
copy_corners(cube_t *dest, cube_t src)
{
	memcpy(&dest->corner, src.corner, sizeof(src.corner));
}

_static_inline void
copy_edges(cube_t *dest, cube_t src)
{
	memcpy(&dest->edge, src.edge, sizeof(src.edge));
}

_static_inline void
set_eo(cube_t *cube, int64_t eo)
{
	uint8_t i, sum, flip;

	for (sum = 0, i = 1; i < 12; i++, eo >>= 1) {
		flip = eo % 2;
		sum += flip;
		cube->edge[i] = (cube->edge[i] & ~_eobit) | (_eobit * flip);
	}
	cube->edge[0] = (cube->edge[0] & ~_eobit) | (_eobit * (sum % 2));
}

_static_inline cube_t
invcoord_esep(int64_t esep)
{
	cube_t ret;
	int64_t bit1, bit2, i, j, jj, k, l, s, v, w, is1, set1, set2;
	uint8_t slice[3] = {0};

	ret = solved;
	set1 = esep % 70;
	set2 = esep / 70;

	for (i = 0, j = 0, k = 4, l = 4; i < 12; i++) {
		v = binomial[11-i][k];
		jj = j < 8;
		w = jj * binomial[7-(j*jj)][l];
		bit2 = set2 >= v;
		bit1 = set1 >= w;
		is1 = (1 - bit2) * bit1;

		set2 -= bit2 * v;
		k -= bit2;
		set1 -= is1 * w;
		l -= is1;
		j += (1-bit2);
		s = 2*bit2 + (1-bit2)*bit1;

		ret.edge[i] = (slice[s]++) | (uint8_t)(s << 2);
	}

	return ret;
}
