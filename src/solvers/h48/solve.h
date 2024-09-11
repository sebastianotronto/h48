typedef struct {
	cube_t cube;
	cube_t inverse;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAXLEN];
	int64_t *nsols;
	int64_t maxsolutions;
	uint8_t h;
	uint8_t k;
	uint32_t *cocsepdata;
	uint8_t *h48data;
	char **nextsol;
	uint8_t nissbranch;
	int8_t npremoves;
	uint8_t premoves[MAXLEN];
} dfsarg_solveh48_t;

typedef struct {
	cube_t cube;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAXLEN];
	uint32_t *cocsepdata;
	uint8_t *h48data;
	char *s;
} dfsarg_solveh48stats_t;

STATIC uint32_t allowednextmove_h48(uint8_t *, uint8_t, uint32_t);

STATIC void solve_h48_appendsolution(dfsarg_solveh48_t *);
STATIC_INLINE bool solve_h48_stop(dfsarg_solveh48_t *);
STATIC int64_t solve_h48_dfs(dfsarg_solveh48_t *);
STATIC int64_t solve_h48(cube_t, int8_t, int8_t, int8_t, const void *, char *);

STATIC int64_t solve_h48stats_dfs(dfsarg_solveh48stats_t *);
STATIC int64_t solve_h48stats(cube_t, int8_t, const void *, char [static 12]);

STATIC uint32_t
allowednextmove_h48(uint8_t *moves, uint8_t n, uint32_t h48branch)
{
	uint32_t result = MM_ALLMOVES;
	if (h48branch & MM_NORMALBRANCH)
		result &= MM_NOHALFTURNS;
	if (n < 1)
		return result;

	uint8_t base1 = movebase(moves[n-1]);
	uint8_t axis1 = moveaxis(moves[n-1]);

	result = disable_moves(result, base1 * 3);
	if (base1 % 2)
		result = disable_moves(result, (base1 - 1) * 3);

	if (n == 1)
		return result;

	uint8_t base2 = movebase(moves[n-2]);
	uint8_t axis2 = moveaxis(moves[n-2]);

	if(axis1 == axis2)
		result = disable_moves(result, base2 * 3);

	return result;
}

STATIC void
solve_h48_appendsolution(dfsarg_solveh48_t *arg)
{
	int strl;
	uint8_t invertedpremoves[MAXLEN];
	char *solution = *arg->nextsol; 

	strl = writemoves(arg->moves, arg->nmoves, *arg->nextsol);
	*arg->nextsol += strl; 

	if (arg->npremoves) {
		**arg->nextsol = ' ';
		(*arg->nextsol)++;

		invertmoves(arg->premoves, arg->npremoves, invertedpremoves);
		strl = writemoves(invertedpremoves, arg->npremoves, *arg->nextsol);
		*arg->nextsol += strl;
	}
	LOG("Solution found: %s\n", solution);

	**arg->nextsol = '\n';
	(*arg->nextsol)++;
	(*arg->nsols)++;
}

STATIC_INLINE bool
solve_h48_stop(dfsarg_solveh48_t *arg)
{
	uint32_t data, data_inv;
	int8_t bound;

	arg->nissbranch = MM_NORMAL;
	bound = get_h48_cdata(arg->cube, arg->cocsepdata, &data);
	if (bound + arg->nmoves + arg->npremoves > arg->depth)
		return true;

	bound = get_h48_cdata(arg->inverse, arg->cocsepdata, &data_inv);
	if (bound + arg->nmoves + arg->npremoves > arg->depth)
		return true;

	bound = get_h48_bound(arg->cube, data, arg->h, arg->k, arg->h48data);
	if (bound + arg->nmoves + arg->npremoves > arg->depth)
		return true;
	if (bound + arg->nmoves + arg->npremoves == arg->depth)
		arg->nissbranch = MM_INVERSEBRANCH;

	bound = get_h48_bound(arg->inverse, data_inv, arg->h, arg->k, arg->h48data);
	if (bound + arg->nmoves + arg->npremoves > arg->depth)
		return true;
	if (bound + arg->nmoves + arg->npremoves == arg->depth)
		arg->nissbranch = MM_NORMALBRANCH;

	return false;
}

STATIC int64_t
solve_h48_dfs(dfsarg_solveh48_t *arg)
{
	dfsarg_solveh48_t nextarg;
	int64_t ret;
	uint8_t m;

	if (*arg->nsols == arg->maxsolutions)
		return 0;

	if (solve_h48_stop(arg))
		return 0;

	if (issolved(arg->cube)) {
		if (arg->nmoves + arg->npremoves != arg->depth)
			return 0;
		solve_h48_appendsolution(arg);
		return 1;
	}

	/* TODO: avoid copy, change arg and undo changes after recursion */
	nextarg = *arg;
	ret = 0;
	uint32_t allowed;
	if(arg->nissbranch & MM_INVERSE) {
		allowed = allowednextmove_h48(arg->premoves, arg->npremoves, arg->nissbranch);
		for (m = 0; m < 18; m++) {
			if(allowed & (1 << m)) {
				nextarg.npremoves = arg->npremoves + 1;
				nextarg.premoves[arg->npremoves] = m;
				nextarg.inverse = move(arg->inverse, m);
				nextarg.cube = premove(arg->cube, m);
				ret += solve_h48_dfs(&nextarg);
			}
		}
	} else {
		allowed = allowednextmove_h48(arg->moves, arg->nmoves, arg->nissbranch);
		for (m = 0; m < 18; m++) {
			if (allowed & (1 << m)) {
				nextarg.nmoves = arg->nmoves + 1;
				nextarg.moves[arg->nmoves] = m;
				nextarg.cube = move(arg->cube, m);
				nextarg.inverse = premove(arg->inverse, m);
				ret += solve_h48_dfs(&nextarg);
			}
		}
	}

	return ret;
}

STATIC int64_t
solve_h48(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	int8_t maxsolutions,
	const void *data,
	char *solutions
)
{
	int64_t nsols;
	dfsarg_solveh48_t arg;
	tableinfo_t info;

	if(!readtableinfo_n(data, 2, &info)) {
		LOG("solve_h48: error reading table\n");
		return 0;
	}

	arg = (dfsarg_solveh48_t) {
		.cube = cube,
		.inverse = inverse(cube),
		.nsols = &nsols,
		.maxsolutions = maxsolutions,
		.h = info.h48h,
		.k = info.bits,
		.cocsepdata = get_cocsepdata_ptr(data),
		.h48data = get_h48data_ptr(data),
		.nextsol = &solutions
	};

	nsols = 0;
	for (arg.depth = minmoves;
	     arg.depth <= maxmoves && nsols < maxsolutions;
	     arg.depth++)
	{
		LOG("Found %" PRId64 " solutions, searching at depth %"
		    PRId8 "\n", nsols, arg.depth);
		arg.nmoves = 0;
		arg.npremoves = 0;
		solve_h48_dfs(&arg);
	}

	return nsols;
}

/*
The h48stats solver computes how many moves it takes to solve to
each of the 12 h48 coordinates, one for each value of h from 0 to 11.
The solutions array is filled with the length of the solutions. The
solution array is therefore not a printable string.
*/
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
		.cocsepdata = get_cocsepdata_ptr(data),
		.h48data = get_h48data_ptr(data),
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
