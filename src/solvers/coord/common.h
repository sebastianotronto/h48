STATIC void append_coord_name(const coord_t *, char *);
STATIC bool solution_lastqt_cw(const solution_moves_t [static 1]);
STATIC bool coord_can_switch(
    const coord_t [static 1], const void *, size_t n, const uint8_t [n]);

STATIC void
append_coord_name(const coord_t *coord, char *str)
{
	int i, j;

	i = 0;
	j = strlen(str);
	while (coord->name[i]) str[j++] = coord->name[i++];

	str[j] = '\0';
}

STATIC bool
solution_lastqt_cw(const solution_moves_t s[static 1])
{
	return are_lastmoves_singlecw(s->nmoves, s->moves) &&
	    are_lastmoves_singlecw(s->npremoves, s->premoves);
}

STATIC bool
coord_can_switch(
	const coord_t coord[static 1],
	const void *data,
	size_t n,
	const uint8_t moves[n]
)
{
	/*
	This function checks that the last move (or two moves, if parallel)
	have a non-trivial effect on the coordinate of the solved cube. This
	works in general for all coordinates that have been used so far, but
	in more general cases that have not been considered yet it may fail.
	*/

	uint64_t i;

	if (n == 0)
		return true;

	i = coord->coord(move(SOLVED_CUBE, moves[n-1]), data);
	if (i == 0)
		return false;

	if (n == 1 || !parallel(moves[n-1], moves[n-2]))
		return true;

	i = coord->coord(move(SOLVED_CUBE, moves[n-1]), data);
	return i != 0;
}
