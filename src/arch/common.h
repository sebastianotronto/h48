STATIC void pieces(cube_t *, uint8_t [static 8], uint8_t [static 12]);
STATIC_INLINE bool equal(cube_t, cube_t);
STATIC_INLINE cube_t invertco(cube_t);
STATIC_INLINE cube_t compose_edges(cube_t, cube_t);
STATIC_INLINE cube_t compose_corners(cube_t, cube_t);
STATIC_INLINE cube_t compose(cube_t, cube_t);
STATIC_INLINE cube_t inverse(cube_t);

STATIC_INLINE int64_t coord_co(cube_t);
STATIC_INLINE int64_t coord_csep(cube_t);
STATIC_INLINE int64_t coord_cocsep(cube_t);
STATIC_INLINE int64_t coord_eo(cube_t);
STATIC_INLINE int64_t coord_esep(cube_t);

STATIC_INLINE void copy_corners(cube_t *, cube_t);
STATIC_INLINE void copy_edges(cube_t *, cube_t);
STATIC_INLINE void set_eo(cube_t *, int64_t);
STATIC_INLINE cube_t invcoord_esep(int64_t);

STATIC_INLINE void invcoord_esep_array(int64_t, int64_t, uint8_t[static 12]);

STATIC_INLINE void
invcoord_esep_array(int64_t set1, int64_t set2, uint8_t mem[static 12])
{
	int64_t bit1, bit2, i, j, jj, k, l, s, v, w, is1;
	uint8_t slice[3] = {0};

	for (i = 0, j = 0, k = 4, l = 4; i < 12; i++)
	{
		v = binomial[11 - i][k];
		jj = j < 8;
		w = jj * binomial[7 - (j * jj)][l];
		bit2 = set2 >= v;
		bit1 = set1 >= w;
		is1 = (1 - bit2) * bit1;

		set2 -= bit2 * v;
		k -= bit2;
		set1 -= is1 * w;
		l -= is1;
		j += (1 - bit2);
		s = 2 * bit2 + (1 - bit2) * bit1;

		mem[i] = (slice[s]++) | (uint8_t)(s << 2);
	}
}
