#define solvedcube _mm256_set_epi8( \
	0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 5, 4, 3, 2, 1, 0, /* Corners */ \
	0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 /* Edges */ \
)
#define errorcube _mm256_setzero_si256()
#define zerocube _mm256_setzero_si256()

static uint8_t
get_edge(cube_t c, uint8_t i)
{
	return 0;
}

static uint8_t
get_corner(cube_t c, uint8_t i)
{
	return 0;
}

static void
set_edge(cube_t *c, uint8_t i, uint8_t p)
{
}

static void
set_corner(cube_t *c, uint8_t i, uint8_t p)
{
}
