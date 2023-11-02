static inline cube_t
flipallcorners(cube_t c)
{
	cube_t shleft, shright, summed, newco, cleanco, ret;

	shleft = _mm256_slli_si256(c, 1);
	shright = _mm256_srli_si256(c, 1);
	summed = _mm256_or_si256(shleft, shright);
	newco = _mm256_and_si256(summed, _co_avx2);
	cleanco = _mm256_andnot_si256_(c, _co_avx2);
	ret = _mm256_and_si256(cleanco, newco);

	return ret;
}

static inline cube_t
inline_trans_UFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_ULr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 3, 2, 1, 0, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 1, 6, 7, 5, 4
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 2, 3, 0, 1, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 7, 6, 4, 5
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_UBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 6, 7, 4, 5, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 4, 5, 2, 3, 0, 1
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 6, 7, 4, 5, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 4, 5, 2, 3, 0, 1
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_URr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 2, 3, 0, 1, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 7, 6, 4, 5
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 3, 2, 1, 0, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 1, 6, 7, 5, 4
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 5, 4, 7, 6, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 7, 6, 1, 0, 3, 2
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 5, 4, 7, 6, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 7, 6, 1, 0, 3, 2
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DLr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 1, 0, 3, 2, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 5, 4, 6, 7
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 1, 0, 3, 2, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 5, 4, 6, 7
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 4, 5, 6, 7, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 0, 1, 2, 3
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 4, 5, 6, 7, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 0, 1, 2, 3
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_DRr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 0, 1, 2, 3, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 4, 5, 7, 6
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 0, 1, 2, 3, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 3, 4, 5, 7, 6
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 25, 26, 27, 24, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 38, 37, 66, 65, 67, 64
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 17, 18, 19, 16, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 68, 70, 33, 35, 34, 32
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 22, 21, 20, 23, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 64, 39, 36, 37, 38
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 20, 23, 22, 21, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 65, 66, 37, 38, 39, 36
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 26, 25, 24, 27, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 37, 38, 65, 66, 64, 67
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 16, 19, 18, 17, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 69, 71, 32, 34, 35, 33
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_RBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 21, 22, 23, 20, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 64, 67, 36, 39, 38, 37
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 21, 22, 23, 20, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 64, 67, 36, 39, 38, 37
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 27, 24, 25, 26, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 39, 36, 67, 64, 66, 65
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 18, 17, 16, 19, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 70, 68, 35, 33, 32, 34
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LFr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 20, 23, 22, 21, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 65, 66, 37, 38, 39, 36
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 22, 21, 20, 23, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 64, 39, 36, 37, 38
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 24, 27, 26, 25, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 36, 39, 64, 67, 65, 66
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 19, 16, 17, 18, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 71, 69, 34, 32, 33, 35
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_LBr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 66, 65, 38, 37, 36, 39
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 23, 20, 21, 22, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 66, 65, 38, 37, 36, 39
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 10, 11, 8, 9, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 34, 32, 71, 69, 70, 68
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 10, 11, 8, 9, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 34, 32, 71, 69, 70, 68
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FRr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 17, 18, 19, 16, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 68, 70, 33, 35, 34, 32
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 25, 26, 27, 24, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 38, 37, 66, 65, 67, 64
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 11, 10, 9, 8, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 32, 34, 69, 71, 68, 70
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 8, 9, 10, 11, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 35, 33, 70, 68, 71, 69
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_FLr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 18, 17, 16, 19, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 70, 68, 35, 33, 32, 34
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 27, 24, 25, 26, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 39, 36, 67, 64, 66, 65
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BUr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 8, 9, 10, 11, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 35, 33, 70, 68, 71, 69
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 11, 10, 9, 8, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 32, 34, 69, 71, 68, 70
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BRr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 19, 16, 17, 18, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 71, 69, 34, 32, 33, 35
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 24, 27, 26, 25, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 36, 39, 64, 67, 65, 66
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BDr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 9, 8, 11, 10, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 33, 35, 68, 70, 69, 71
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 9, 8, 11, 10, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 33, 35, 68, 70, 69, 71
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_BLr(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 16, 19, 18, 17, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 69, 71, 32, 34, 35, 33
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 26, 25, 24, 27, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 37, 38, 65, 66, 64, 67
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);

	return ret;
}

static inline cube_t
inline_trans_UFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 1, 0, 7, 6, 5, 4
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 1, 0, 7, 6, 5, 4
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_ULm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 3, 2, 1, 0, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 2, 3, 1, 0
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 25, 26, 27, 24, 3, 2, 1, 0, 7, 6, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 2, 3, 1, 0
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_UBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 7, 6, 5, 4, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 1, 6, 7, 4, 5
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 8, 9, 10, 11, 7, 6, 5, 4, 2, 3, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 1, 6, 7, 4, 5
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_URm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 2, 3, 0, 1, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 3, 2, 0, 1
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 27, 24, 25, 26, 2, 3, 0, 1, 6, 7, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 3, 2, 0, 1
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 4, 5, 6, 7, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 2, 5, 4, 7, 6
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 4, 5, 6, 7, 0, 1, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 2, 5, 4, 7, 6
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DLm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 1, 0, 3, 2, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 7, 6, 1, 0, 2, 3
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 0, 1, 2, 3, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 7, 0, 1, 3, 2
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_DRm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 24, 27, 26, 25, 0, 1, 2, 3, 5, 4, 7, 6,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 7, 0, 1, 3, 2
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 26, 25, 24, 27, 1, 0, 3, 2, 4, 5, 6, 7,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 7, 6, 1, 0, 2, 3
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 24, 27, 26, 25, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 32, 34, 33, 70, 69, 71, 68
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 18, 17, 16, 19, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 35, 34, 32, 71, 69, 68, 70
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 23, 20, 21, 22, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 70, 71, 68, 35, 32, 33, 34
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 16, 19, 23, 20, 21, 22, 24, 27, 26, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 38, 39, 36, 67, 64, 65, 66
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 27, 24, 25, 26, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 35, 33, 34, 69, 70, 68, 71
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 19, 16, 17, 18, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 32, 34, 35, 33, 70, 68, 69, 71
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_RBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 20, 23, 22, 21, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 69, 68, 71, 32, 35, 34, 33
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 22, 21, 20, 23, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 38, 37, 66, 65, 64, 67
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 26, 25, 24, 27, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 33, 35, 32, 71, 68, 70, 69
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 17, 18, 19, 16, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 35, 33, 32, 34, 69, 71, 70, 68
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LFm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 68, 69, 70, 33, 34, 35, 32
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 19, 16, 21, 22, 23, 20, 25, 26, 27, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 36, 37, 38, 65, 66, 67, 64
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 25, 26, 27, 24, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 34, 32, 35, 68, 71, 69, 70
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 16, 19, 18, 17, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 32, 33, 35, 68, 70, 71, 69
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_LBm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 19, 16, 17, 18, 22, 21, 20, 23, 26, 25, 24, 27,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 71, 70, 69, 34, 33, 32, 35
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 16, 19, 18, 17, 20, 23, 22, 21, 27, 24, 25, 26,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 37, 36, 39, 64, 67, 66, 65
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 11, 10, 9, 8, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 39, 37, 38, 36, 67, 65, 66, 64
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 7, 6, 5, 4, 11, 10, 9, 8, 17, 18, 19, 16,
		0, 0, 0, 0, 0, 0, 0, 0, 71, 69, 70, 68, 35, 33, 34, 32
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FRm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 20, 23, 22, 21, 17, 18, 19, 16, 10, 11, 8, 9,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 65, 64, 66, 37, 39, 38, 36
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 2, 3, 0, 1, 26, 25, 24, 27, 21, 22, 23, 20,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 65, 67, 64, 39, 36, 38, 37
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 10, 11, 8, 9, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 37, 39, 36, 38, 65, 67, 64, 66
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 9, 8, 11, 10, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 70, 68, 71, 69, 34, 32, 35, 33
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_FLm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 22, 21, 20, 23, 18, 17, 16, 19, 11, 10, 9, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 67, 66, 64, 39, 37, 36, 38
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 3, 2, 1, 0, 24, 27, 26, 25, 20, 23, 22, 21,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 64, 66, 65, 38, 37, 39, 36
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BUm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 6, 7, 4, 5, 9, 8, 11, 10, 16, 19, 18, 17,
		0, 0, 0, 0, 0, 0, 0, 0, 38, 36, 39, 37, 66, 64, 67, 65
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 5, 4, 7, 6, 10, 11, 8, 9, 18, 17, 16, 19,
		0, 0, 0, 0, 0, 0, 0, 0, 69, 71, 68, 70, 33, 35, 32, 34
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BRm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 23, 20, 21, 22, 19, 16, 17, 18, 9, 8, 11, 10,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 66, 67, 65, 38, 36, 37, 39
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 1, 0, 3, 2, 27, 24, 25, 26, 23, 20, 21, 22,
		0, 0, 0, 0, 0, 0, 0, 0, 64, 67, 65, 66, 37, 38, 36, 39
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BDm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 36, 38, 37, 39, 64, 66, 65, 67
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 19, 16, 17, 18,
		0, 0, 0, 0, 0, 0, 0, 0, 68, 70, 69, 71, 32, 34, 33, 35
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

static inline cube_t
inline_trans_BLm(cube_t c)
{
	cube_t ret;

	cube_t tn = _mm256_set_epi8(
		0, 0, 0, 0, 21, 22, 23, 20, 16, 19, 18, 17, 8, 9, 10, 11,
		0, 0, 0, 0, 0, 0, 0, 0, 66, 64, 65, 67, 36, 38, 39, 37
	);
	cube_t ti = _mm256_set_epi8(
		0, 0, 0, 0, 0, 1, 2, 3, 25, 26, 27, 24, 22, 21, 20, 23,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 64, 67, 36, 39, 37, 38
	);

	ret = compose(tn, c);
	ret = compose(ret, ti);
	ret = flipallcorners(ret);

	return ret;
}

