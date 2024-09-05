typedef struct {
	cube_t cube;
	uint8_t depth;
	int64_t maxsols;
	char **nextsol;
	int64_t *nsols;
	uint8_t nmoves;
	uint8_t moves[20];
	uint8_t (*estimate)(cube_t);
} dfsarg_generic_t;

STATIC void solve_generic_appendsolution(dfsarg_generic_t *);
STATIC int solve_generic_dfs(dfsarg_generic_t *);
STATIC int64_t solve_generic(cube_t, const char *, int8_t, int8_t, int64_t,
    int8_t, char *, uint8_t (*)(cube_t));
STATIC uint8_t estimate_simple(cube_t);
STATIC int64_t solve_simple(cube_t, int8_t, int8_t, int64_t, int8_t, char *);

STATIC void
solve_generic_appendsolution(dfsarg_generic_t *arg)
{
	int strl;

	strl = writemoves(arg->moves, arg->nmoves, *arg->nextsol);
	LOG("Solution found: %s\n", *arg->nextsol);
	*arg->nextsol += strl;
	**arg->nextsol = '\n';
	(*arg->nextsol)++;
	(*arg->nsols)++;
}

STATIC int
solve_generic_dfs(dfsarg_generic_t *arg)
{
	dfsarg_generic_t nextarg;
	uint8_t m, bound;
	int64_t ret;

	if (!allowednextmove(arg->moves, arg->nmoves))
		return 0;

	if (arg->nmoves > 0)
		arg->cube = move(arg->cube, arg->moves[arg->nmoves-1]);

	bound = arg->estimate(arg->cube);
	if (*arg->nsols == arg->maxsols || bound + arg->nmoves > arg->depth)
		return 0;

	if (bound == 0) {
		if (arg->nmoves != arg->depth)
			return 0;
		solve_generic_appendsolution(arg);
		return 1;
	}

	nextarg = *arg;
	nextarg.nmoves = arg->nmoves + 1;
	for (m = 0, ret = 0; m < 18; m++) {
		nextarg.cube = arg->cube;
		nextarg.moves[arg->nmoves] = m;
		ret += solve_generic_dfs(&nextarg);
	}

	return ret;
}

STATIC int64_t
solve_generic(
	cube_t cube,
	const char *nisstype,
	/* TODO: handle NISS */
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	char *sols,
	uint8_t (*estimate)(cube_t)
	/* TODO: add validator */
	/* TODO: maybe add data for estimate */
	/* TODO: add moveset (and allowednext?) */
)
{
	dfsarg_generic_t arg;
	int64_t ret, tmp, first;

	if (issolved(cube)) {
		LOG("solve: cube is already solved\n");
		sols[0] = '\n';
		sols[1] = 0;
		return 1;
	}

	if (estimate == NULL) {
		LOG("solve: 'estimate' is NULL\n");
		return -1;
	}

	arg = (dfsarg_generic_t) {
		.cube = cube,
		.maxsols = maxsols,
		.nextsol = &sols,
		.nsols = &ret,
		.nmoves = 0,
		.moves = {0},
		.estimate = estimate,
	};

	ret = 0;
	first = -1;
	for (arg.depth = minmoves; arg.depth <= maxmoves; arg.depth++) {
		tmp = solve_generic_dfs(&arg);
		if (tmp != 0)
			first = arg.depth;

		LOG("Found %" PRId64 " solution%s at depth %" PRIu8 "\n",
		        tmp, tmp == 1 ? "" : "s", arg.depth);

		if (ret >= maxsols)
			break;

		if (optimal >= 0 && first >= 0 && arg.depth - first == optimal)
			break;
	}

	DBG_ASSERT(ret <= maxsols, ret,
	    "solve: found more than 'maxsols' solutions\n");

	return ret;
}

STATIC uint8_t
estimate_simple(cube_t cube)
{
	return issolved(cube) ? 0 : 1;
}

STATIC int64_t
solve_simple(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	char *solutions
)
{
	return solve_generic(
		cube,
		"",
		minmoves,
		maxmoves,
		maxsols,
		optimal,
		solutions,
		&estimate_simple
	);
}
