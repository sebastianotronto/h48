cube_t solvedcube = {
	.c = {0, 1, 2, 3, 4, 5, 6, 7},
	.e = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
};
static cube_t errorcube = { .e = {0}, .c = {0} };
static cube_t zerocube = { .e = {0}, .c = {0} };

static uint8_t
get_edge(cube_t c, uint8_t i)
{
	return c.e[i];
}

static uint8_t
get_corner(cube_t c, uint8_t i)
{
	return c.c[i];
}

static void
set_edge(cube_t *c, uint8_t i, uint8_t p)
{
	c->e[i] = p;
}

static void
set_corner(cube_t *c, uint8_t i, uint8_t p)
{
	c->c[i] = p;
}
