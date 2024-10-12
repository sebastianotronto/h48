typedef struct {
	cube_t cube;
	cube_t inverse;
	int8_t nmoves;
	int8_t depth;
	uint8_t moves[MAXLEN];
	_Atomic int64_t *nsols;
	int64_t maxsolutions;
	uint8_t h;
	uint8_t k;
	const uint32_t *cocsepdata;
	const uint8_t *h48data;
	uint64_t solutions_size;
	char **nextsol;
	uint8_t nissbranch;
	int8_t npremoves;
	uint8_t premoves[MAXLEN];
} dfsarg_solveh48_t;

STATIC uint32_t allowednextmove_h48(uint8_t *, uint8_t, uint32_t);

STATIC void solve_h48_appendsolution(dfsarg_solveh48_t *);
STATIC_INLINE bool solve_h48_stop(dfsarg_solveh48_t *);
STATIC int64_t solve_h48_dfs(dfsarg_solveh48_t *);
STATIC int64_t solve_h48(cube_t, int8_t, int8_t,
    int8_t, uint64_t, const void *, uint64_t, char *);

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
	int64_t strl;
	uint8_t invertedpremoves[MAXLEN];
	char *solution = *arg->nextsol; 

	strl = writemoves(
	    arg->moves, arg->nmoves, arg->solutions_size, *arg->nextsol);

	if (strl < 0)
		goto solve_h48_appendsolution_error;
	*arg->nextsol += strl; 
	arg->solutions_size -= strl;

	if (arg->npremoves) {
		**arg->nextsol = ' ';
		(*arg->nextsol)++;

		invertmoves(arg->premoves, arg->npremoves, invertedpremoves);
		strl = writemoves(invertedpremoves,
		    arg->npremoves, arg->solutions_size, *arg->nextsol);

		if (strl < 0)
			goto solve_h48_appendsolution_error;
		*arg->nextsol += strl;
		arg->solutions_size -= strl;
	}
	LOG("Solution found: %s\n", solution);

	**arg->nextsol = '\n';
	(*arg->nextsol)++;
	(*arg->nsols)++;

solve_h48_appendsolution_error:
	/* We could add some logging, but writemoves() already does */
	return;
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
	uint64_t data_size,
	const void *data,
	uint64_t solutions_size,
	char *solutions
)
{
	_Atomic int64_t nsols;
	dfsarg_solveh48_t arg;
	tableinfo_t info;

	if(readtableinfo_n(data_size, data, 2, &info) != NISSY_OK) {
		LOG("solve_h48: error reading table\n");
		return NISSY_ERROR_DATA;
	}

	arg = (dfsarg_solveh48_t) {
		.cube = cube,
		.inverse = inverse(cube),
		.nsols = &nsols,
		.maxsolutions = maxsolutions,
		.h = info.h48h,
		.k = info.bits,
		.cocsepdata = get_cocsepdata_constptr(data),
		.h48data = get_h48data_constptr(data),
		.solutions_size = solutions_size,
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
	**arg.nextsol = '\0';
	(*arg.nextsol)++;
	return nsols;
}
