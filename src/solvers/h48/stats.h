/*
The h48stats solver computes how many moves it takes to solve to
each of the 12 h48 coordinates, one for each value of h from 0 to 11.
The solutions array is filled with the length of the solutions. The
solutions array is therefore not a printable string.
*/

typedef struct {
	cube_t cube;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAXLEN];
	const uint32_t *cocsepdata;
	const uint8_t *h48data;
	char *s;
} dfsarg_solveh48stats_t;

STATIC int64_t solve_h48stats_dfs(dfsarg_solveh48stats_t *);
STATIC int64_t solve_h48stats(cube_t, int8_t, const void *, char [static 12]);

STATIC int64_t
solve_h48stats_dfs(dfsarg_solveh48stats_t *arg)
{
	const int64_t limit = 11;

	int8_t bound, u;
	uint8_t m;
	uint32_t d;
	int64_t coord, h;
	dfsarg_solveh48stats_t nextarg;

	/* Check cocsep lower bound (corners only) */
	bound = get_h48_cdata(arg->cube, arg->cocsepdata, &d);
	if (bound + arg->nmoves > arg->depth)
		return 0;

	/* Check h48 lower bound for h=0 (esep, but no eo) */
	coord = coord_h48_edges(arg->cube, COCLASS(d), TTREP(d), 0);
	bound = get_h48_bound(arg->cube, d, 0, 4, arg->h48data);
	if (bound + arg->nmoves > arg->depth)
		return 0;
	
	/* Update all other values, if solved */
	coord = coord_h48_edges(arg->cube, COCLASS(d), TTREP(d), 11);
	for (h = 0; h <= limit; h++) {
		u = coord >> (11-h) == 0 && arg->s[h] == 99;
		arg->s[h] = u * arg->nmoves + (1-u) * arg->s[h];
	}

	if (arg->s[limit] != 99)
		return 0;

	nextarg = *arg;
	nextarg.nmoves = arg->nmoves + 1;
	for (m = 0; m < 18; m++) {
		nextarg.moves[arg->nmoves] = m;
		if (!allowednextmove(nextarg.moves, nextarg.nmoves)) {
			/* If a move is not allowed, neither are its 180
			 * and 270 degree variations */
			m += 2;
			continue;
		}
		nextarg.cube = move(arg->cube, m);
		solve_h48stats_dfs(&nextarg);
	}

	return 0;
}

STATIC int64_t
solve_h48stats(
	cube_t cube,
	int8_t maxmoves,
	const void *data,
	char solutions[static 12]
)
{
	int i;
	dfsarg_solveh48stats_t arg;

	arg = (dfsarg_solveh48stats_t) {
		.cube = cube,
		.cocsepdata = get_cocsepdata_constptr(data),
		.h48data = get_h48data_constptr(data),
		.s = solutions
	};

	for (i = 0; i < 12; i++)
		solutions[i] = (char)99;

	for (arg.depth = 0;
	     arg.depth <= maxmoves && solutions[11] == 99;
	     arg.depth++)
	{
		arg.nmoves = 0;
		solve_h48stats_dfs(&arg);
	}

	return 0;
}
