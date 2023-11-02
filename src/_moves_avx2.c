static inline cube_t
inline_move_U(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 0, 1, 3, 2, 5, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 1, 0, 3, 2, 4, 5
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_U2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 4, 5, 3, 2, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 4, 5, 3, 2, 0, 1
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_U3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 1, 0, 3, 2, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 0, 1, 3, 2, 5, 4
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_D(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 3, 2, 5, 4, 6, 7, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 5, 4, 6, 7, 1, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_D2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 6, 7, 5, 4, 2, 3, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 5, 4, 2, 3, 1, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_D3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 9, 8, 2, 3, 5, 4, 7, 6, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 5, 4, 7, 6, 1, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_R(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 4, 10, 9, 7, 11, 6, 5, 8, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 35, 32, 4, 69, 2, 1, 70
	);

	return compose(c, m);
}

static inline cube_t
inline_move_R2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 8, 10, 9, 11, 4, 6, 5, 7, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 5, 6, 4, 0, 2, 1, 3
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_R3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 7, 10, 9, 4, 8, 6, 5, 11, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 32, 35, 4, 70, 2, 1, 69
	);

	return compose(c, m);
}

static inline cube_t
inline_move_L(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 6, 5, 8, 7, 9, 10, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 34, 6, 5, 33, 3, 68, 71, 0
	);

	return compose(c, m);
}

static inline cube_t
inline_move_L2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 9, 10, 8, 7, 5, 6, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 5, 7, 3, 1, 2, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_L3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 5, 6, 8, 7, 10, 9, 4, 3, 2, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 33, 6, 5, 34, 3, 71, 68, 0
	);

	return compose(c, m);
}

static inline cube_t
inline_move_F(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 19, 16, 7, 6, 5, 4, 24, 2, 1, 25,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 64, 5, 66, 3, 38, 1, 36
	);

	return compose(c, m);
}

static inline cube_t
inline_move_F2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 8, 9, 7, 6, 5, 4, 0, 2, 1, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 4, 5, 6, 3, 0, 1, 2
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_F3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 11, 10, 16, 19, 7, 6, 5, 4, 25, 2, 1, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 7, 66, 5, 64, 3, 36, 1, 38
	);

	return compose(c, m);
}

static inline cube_t
inline_move_B(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 18, 17, 9, 8, 7, 6, 5, 4, 3, 26, 27, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 65, 6, 67, 4, 39, 2, 37, 0
	);

	return compose(c, m);
}

static inline cube_t
inline_move_B2(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 10, 11, 9, 8, 7, 6, 5, 4, 3, 1, 2, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 4, 1, 2, 3, 0
	);

	return _mm256_shuffle_epi8(c, m);
}

static inline cube_t
inline_move_B3(cube_t c)
{
	cube_t m = _mm256_set_epi8(
		0, 0, 0, 0, 17, 18, 9, 8, 7, 6, 5, 4, 3, 27, 26, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 67, 6, 65, 4, 37, 2, 39, 0
	);

	return compose(c, m);
}
