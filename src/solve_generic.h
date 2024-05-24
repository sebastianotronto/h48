typedef struct {
	cube_fast_t cube;
	uint8_t depth;
	int64_t maxsols;
	char **nextsol;
	int64_t *nsols;
	uint8_t nmoves;
	uint8_t moves[20];
	uint8_t (*estimate)(cube_fast_t);
} dfsarg_generic_t;

_static void solve_generic_appendsolution(dfsarg_generic_t *);
_static int solve_generic_dfs(dfsarg_generic_t *);
_static int64_t solve_generic(cube_t, const char *, int8_t, int8_t, int64_t,
    int8_t, char *, uint8_t (*)(cube_fast_t));
_static uint8_t estimate_simple(cube_fast_t);
_static int64_t solve_simple(cube_t, int8_t, int8_t, int64_t, int8_t, char *);

int64_t
solve(
	cube_t cube,
	const char *solver,
	const char *options,
	const char *nisstype,
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	const void *data,
	char *solutions
)
{
	DBG_WARN(!strcmp(options, ""),
	     "solve: 'options' not implemented yet, ignoring\n");

	DBG_WARN(!strcmp(nisstype, ""),
	     "solve: NISS not implemented yet, ignoring 'nisstype'\n");

	DBG_WARN(data == NULL,
	    "solve: 'data' not implemented yet, ignoring\n");

	if (!strcmp(solver, "optimal") || !strcmp(solver, "simple")) {
		return solve_simple(
			cube,
			minmoves,
			maxmoves,
			maxsols,
			optimal,
			solutions
		);
	} else {
		DBG_LOG("solve: unknown solver '%s'\n", solver);
		return -1;
	}

	DBG_LOG("solve: error\n");
	return -1;
}

void
multisolve(
	int n,
	cube_t *cube,
	const char *solver,
	const void *data,
	char *sols
)
{
	char *s;
	int i;

	s = sols;
	for (i = 0; i < n; i++) {
		solve(cube[i], solver, "", "normal", 0, -1, 1, 0, NULL, s);
		while (s++);
	}
}

_static void
solve_generic_appendsolution(dfsarg_generic_t *arg)
{
	int strl;

	strl = writemoves(arg->moves, arg->depth, *arg->nextsol);
	DBG_LOG("Solution found: %s\n", *arg->nextsol);
	*arg->nextsol += strl;
	**arg->nextsol = '\n';
	(*arg->nextsol)++;
	(*arg->nsols)++;
}

_static int
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

	/* memcpy(&nextarg, arg, sizeof(dfsarg_generic_t)); */
	nextarg = *arg;
	nextarg.nmoves = arg->nmoves + 1;
	for (m = 0, ret = 0; m < 18; m++) {
		nextarg.cube = arg->cube;
		nextarg.moves[arg->nmoves] = m;
		ret += solve_generic_dfs(&nextarg);
	}

	return ret;
}

_static int64_t
solve_generic(
	cube_t cube,
	const char *nisstype,
	/* TODO: handle NISS */
	int8_t minmoves,
	int8_t maxmoves,
	int64_t maxsols,
	int8_t optimal,
	char *sols,
	uint8_t (*estimate)(cube_fast_t)
	/* TODO: add validator */
	/* TODO: maybe add data for estimate */
	/* TODO: add moveset (and allowednext?) */
)
{
	dfsarg_generic_t arg;
	int64_t ret, tmp, first;

	if (!issolvable(cube)) {
		DBG_LOG("solve: cube is not solvable\n");
		return -1;
	}

	if (issolved(cube)) {
		DBG_LOG("solve: cube is already solved\n");
		sols[0] = '\n';
		sols[1] = 0;
		return 1;
	}

	DBG_WARN(!strcmp(nisstype, ""),
	    "solve: NISS not implemented yet, 'nisstype' ignored\n");

	if (minmoves < 0) {
		DBG_LOG("solve: 'minmoves' is negative, setting to 0\n");
		minmoves = 0;
	}

	if (maxmoves < 0) {
		DBG_LOG("solve: invalid 'maxmoves', setting to 20\n");
		maxmoves = 20;
	}

	if (maxsols < 0) {
		DBG_LOG("solve: 'maxsols' is negative\n");
		return -1;
	}

	if (maxsols == 0) {
		DBG_LOG("solve: 'maxsols' is 0\n");
		return 0;
	}

	if (sols == NULL) {
		DBG_LOG("solve: return parameter 'sols' is NULL\n");
		return -1;
	}

	if (estimate == NULL) {
		DBG_LOG("solve: 'estimate' is NULL\n");
		return -1;
	}

	arg = (dfsarg_generic_t) {
		.cube = cubetofast(cube),
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

		DBG_LOG("Found %" PRId64 " solution%s at depth %" PRIu8 "\n",
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

_static uint8_t
estimate_simple(cube_fast_t cube)
{
	return issolved_fast(cube) ? 0 : 1;
}

_static int64_t
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

int64_t
gendata(const char *solver, const char *options, void *data)
{
	int64_t ret;
	uint8_t maxdepth;

	if (!strcmp(solver, "H48")) {
		/* TODO: write a generic parser for options */
		maxdepth = atoi(options);
		ret = gendata_h48(data, 0, maxdepth);
	} else {
		DBG_LOG("gendata: implemented only for H48 solver\n");
		ret = -1;
	}

	return ret;
}
