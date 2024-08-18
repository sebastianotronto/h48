#define _co2_neon vdupq_n_u8(0x60)
#define _cocw_neon vdupq_n_u8(0x20)
#define _cp_neon vdupq_n_u8(0x07)
#define _ep_neon vcombine_u8(vdupq_n_u8(0x0F), vdupq_n_u8(0x0F))
#define _eo_neon vcombine_u8(vdupq_n_u8(0x10), vdupq_n_u8(0x10))

// static cube
#define static_cube(c_ufr, c_ubl, c_dfl, c_dbr, c_ufl, c_ubr, c_dfr, c_dbl, \
					e_uf, e_ub, e_db, e_df, e_ur, e_ul, e_dl, e_dr, e_fr, e_fl, e_bl, e_br) \
	((cube_t){ \
		.corner = {c_ufr, c_ubl, c_dfl, c_dbr, c_ufl, c_ubr, c_dfr, c_dbl, 0, 0, 0, 0, 0, 0, 0, 0}, \
		.edge = {e_uf, e_ub, e_db, e_df, e_ur, e_ul, e_dl, e_dr, e_fr, e_fl, e_bl, e_br, 0, 0, 0, 0}})

// zero cube
#define zero \
	(cube_t) \
	{ \
		.corner = vdupq_n_u8(0), \
		.edge = vdupq_n_u8(0) \
	}

// solved cube
#define solved static_cube(	\
	0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)

_static void
pieces(cube_t *cube, uint8_t c[static 8], uint8_t e[static 12])
{
	// First 8 bytes of the corner vector are copied from the c array
	vst1_u8(c, vget_low_u8(cube->corner));

	// 12 bytes of the edge vector are copied from the e array
	// First 8 bytes
	vst1_u8(e, vget_low_u8(cube->edge));
	// Next 4 bytes
	vst1_lane_u32((uint32_t *)(e + 8), vreinterpret_u32_u8(vget_high_u8(cube->edge)), 0);
}

_static_inline bool
equal(cube_t c1, cube_t c2)
{
	uint8x16_t cmp_corner, cmp_edge;
	uint64x2_t cmp_corner_u64, cmp_edge_u64;
	uint64x2_t cmp_result;

	// compare the corner vectors
	cmp_corner = vceqq_u8(c1.corner, c2.corner);
	// compare the edge vectors
	cmp_edge = vceqq_u8(c1.edge, c2.edge);

	// convert the comparison vectors to 64-bit vectors
	cmp_corner_u64 = vreinterpretq_u64_u8(cmp_corner);
	cmp_edge_u64 = vreinterpretq_u64_u8(cmp_edge);

	// combine the comparison vectors
	cmp_result = vandq_u64(cmp_corner_u64, cmp_edge_u64);

	// check if all the bits are set
	return vgetq_lane_u64(cmp_result, 0) == ~0ULL && vgetq_lane_u64(cmp_result, 1) == ~0ULL;
}

_static_inline cube_t
invertco(cube_t c)
{
	cube_t ret;
	uint8x16_t co, shleft, shright, summed, newco, cleanco;

	co = vandq_u8(c.corner, _co2_neon);
	shleft = vshlq_n_u8(co, 1);
	shright = vshrq_n_u8(co, 1);
	summed = vorrq_u8(shleft, shright);
	newco = vandq_u8(summed, _co2_neon);
	cleanco = veorq_u8(c.corner, co);
	ret.corner = vorrq_u8(cleanco, newco);
	ret.edge = c.edge;
	
	return ret;
}

_static_inline cube_t
compose_edges(cube_t c1, cube_t c2)
{
	cube_t ret = {0};
	ret.edge = compose_edges_slim(c1.edge, c2.edge);
	return ret;
}

_static_inline cube_t 
compose_corners(cube_t c1, cube_t c2)
{
	cube_t ret = {0};
	ret.corner = compose_corners_slim(c1.corner, c2.corner);
	return ret;
}

_static_inline uint8x16_t 
compose_edges_slim(uint8x16_t edge1, uint8x16_t edge2)
{
	// Masks
	uint8x16_t p_bits = vdupq_n_u8(_pbits);
	uint8x16_t eo_bit = vdupq_n_u8(_eobit);

	// Find the index and permutation
	uint8x16_t p = vandq_u8(edge2, p_bits);
	uint8x16_t piece1 = vqtbl1q_u8(edge1, p);

	// Calculate the orientation through XOR
	uint8x16_t orien = vandq_u8(veorq_u8(edge2, piece1), eo_bit);

	// Combine the results
	uint8x16_t ret = vorrq_u8(vandq_u8(piece1, p_bits), orien);

	// Mask to clear the last 32 bits of the result
	uint8x16_t mask_last_32 = vsetq_lane_u32(0, vreinterpretq_u32_u8(ret), 3);
	ret = vreinterpretq_u8_u32(mask_last_32);

	return ret;
}

_static_inline uint8x16_t 
compose_corners_slim(uint8x16_t corner1, uint8x16_t corner2)
{
	// Masks
	uint8x16_t p_bits = vdupq_n_u8(_pbits);
	uint8x16_t cobits = vdupq_n_u8(_cobits);
	uint8x16_t cobits2 = vdupq_n_u8(_cobits2);
	uint8x16_t twist_cw = vdupq_n_u8(_ctwist_cw);

	// Find the index and permutation
	uint8x16_t p = vandq_u8(corner2, p_bits);
	uint8x16_t piece1 = vqtbl1q_u8(corner1, p);

	// Calculate the orientation
	uint8x16_t aux = vaddq_u8(vandq_u8(corner2, cobits), vandq_u8(piece1, cobits));
	uint8x16_t auy = vshrq_n_u8(vaddq_u8(aux, twist_cw), 2);
	uint8x16_t orien = vandq_u8(vaddq_u8(aux, auy), cobits2);

	// Combine the results
	uint8x16_t ret = vorrq_u8(vandq_u8(piece1, p_bits), orien);

	// Mask to clear the last 64 bits of the result
	uint8x16_t mask_last_64 = vsetq_lane_u64(0, vreinterpretq_u64_u8(ret), 1);
	ret = vreinterpretq_u8_u64(mask_last_64);

	return ret;
}

_static_inline cube_t 
compose(cube_t c1, cube_t c2)
{
	cube_t ret = {0};

	ret.edge = compose_edges_slim(c1.edge, c2.edge);
	ret.corner = compose_corners_slim(c1.corner, c2.corner);

	return ret;
}

_static_inline cube_t 
inverse(cube_t cube)
{
	uint8_t i, piece, orien;
	cube_t ret;

	// Temp arrays to store the NEON vectors
	uint8_t edges[16];
	uint8_t corners[16];

	// Copy the NEON vectors to the arrays
	vst1q_u8(edges, cube.edge);
	vst1q_u8(corners, cube.corner);

	uint8_t edge_result[16] = {0};
	uint8_t corner_result[16] = {0};

	// Process the edges
	for (i = 0; i < 12; i++)
	{
		piece = edges[i];
		orien = piece & _eobit;
		edge_result[piece & _pbits] = i | orien;
	}

	// Process the corners
	for (i = 0; i < 8; i++)
	{
		piece = corners[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		corner_result[piece & _pbits] = i | orien;
	}

	// Copy the results back to the NEON vectors
	ret.edge = vld1q_u8(edge_result);
	ret.corner = vld1q_u8(corner_result);

	return ret;
}

_static_inline int64_t 
coord_co(cube_t c)
{
	// Temp array to store the NEON vector
	uint8_t mem[16];
	vst1q_u8(mem, c.corner);

	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 3)
		ret += p * (mem[i] >> _coshift);

	return ret;
}

_static_inline int64_t
coord_csep(cube_t c)
{
	// Temp array to store the NEON vector
	uint8_t mem[16];
	vst1q_u8(mem, c.corner);

	int64_t ret = 0;
	int i, p;
	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 2)
		ret += p * ((mem[i] & _csepbit) >> 2);

	return ret;
	return 0;
}

_static_inline int64_t
coord_cocsep(cube_t c)
{
	return (coord_co(c) << 7) + coord_csep(c);
}

_static_inline int64_t 
coord_eo(cube_t c)
{
	int64_t ret = 0;
	int64_t p = 1;

	// Temp array to store the NEON vector
	uint8_t mem[16];
	vst1q_u8(mem, c.edge);

	for (int i = 1; i < 12; i++, p *= 2)
	{
		ret += p * (mem[i] >> _eoshift);
	}

	return ret;
}

_static_inline int64_t 
coord_esep(cube_t c)
{
	int64_t i, j, jj, k, l, ret1, ret2, bit1, bit2, is1;

	// Temp array to store the NEON vector
	uint8_t mem[16];
	vst1q_u8(mem, c.edge);

	for (i = 0, j = 0, k = 4, l = 4, ret1 = 0, ret2 = 0; i < 12; i++)
	{
		bit1 = (mem[i] & _esepbit1) >> 2;
		bit2 = (mem[i] & _esepbit2) >> 3;
		is1 = (1 - bit2) * bit1;

		ret1 += bit2 * binomial[11 - i][k];
		k -= bit2;

		jj = j < 8;
		ret2 += jj * is1 * binomial[7 - (j * jj)][l];
		l -= is1;
		j += (1 - bit2);
	}

	return ret1 * 70 + ret2;
}

_static_inline void
copy_corners(cube_t *dst, cube_t src)
{
	dst->corner = src.corner;
}

_static_inline void
copy_edges(cube_t *dst, cube_t src)
{
	dst->edge = src.edge;
}

_static_inline void
set_eo(cube_t *cube, int64_t eo)
{
	// Temp array to store the NEON vector
	uint8_t mem[16];
	vst1q_u8(mem, cube->edge);
	uint8_t i, sum, flip;

	for (sum = 0, i = 1; i < 12; i++, eo >>= 1)
	{
		flip = eo % 2;
		sum += flip;
		mem[i] = (mem[i] & ~_eobit) | (_eobit * flip);
	}
	mem[0] = (mem[0] & ~_eobit) | (_eobit * (sum % 2));

	// Copy the results back to the NEON vector
	cube->edge = vld1q_u8(mem);
	return;
}

_static_inline cube_t
invcoord_esep(int64_t esep)
{
	cube_t ret;
	uint8_t mem[16] = {0};

	invcoord_esep_array(esep % 70, esep / 70, mem);

	ret = solved;
	ret.edge = vld1q_u8(mem);

	return ret;
}
