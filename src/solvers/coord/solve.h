#define MAXLEN_COORDSOL 20

typedef struct {
	cube_t cube;
	uint8_t depth;
	uint8_t nmoves;
	uint8_t moves[MAXLEN_COORDSOL];
	coord_t *coord;
	const void *coord_data;
	const uint8_t *ptable;
	uint8_t trans;
	int64_t *nsols;
	int64_t maxsolutions;
	int optimal;
	uint8_t *shortest_sol;
	uint64_t solutions_size;
	uint64_t *solutions_used;
	char **solutions;
} dfsarg_solve_coord_t;

STATIC int64_t solve_coord(cube_t, coord_t *, uint8_t, uint8_t, uint8_t,
    uint8_t, uint64_t, int, int, uint64_t, const void *, uint64_t, char *);
STATIC int64_t solve_coord_dispatch(cube_t, const char *, uint8_t, uint8_t,
    uint8_t, uint64_t, int, int, uint64_t, const void *, uint64_t, char *);
STATIC bool solve_coord_appendchar(char *, uint64_t, uint64_t *, char);
STATIC int64_t solve_coord_appendsolution(dfsarg_solve_coord_t *);
STATIC int64_t solve_coord_dfs(dfsarg_solve_coord_t *);

STATIC int64_t
solve_coord_appendsolution(dfsarg_solve_coord_t *arg)
{
	uint8_t i, t, tmoves[MAXLEN_COORDSOL];

	if (*arg->nsols >= arg->maxsolutions ||
	    arg->nmoves > *arg->shortest_sol + arg->optimal)
		return 0;

	sortparallel(arg->moves, arg->nmoves);

	t = inverse_trans(arg->trans);
	for (i = 0; i < arg->nmoves; i++)
		tmoves[i] = transform_move(arg->moves[i], t);

	/* TODO append tmoves[] */

	return 1;
}

STATIC bool
solve_coord_appendchar(char *s, uint64_t s_size, uint64_t *s_used, char c)
{
	if (s_size == *s_used)
		return false;

	s[*s_used] = c;
	(*s_used)++;

	return true;
}

STATIC int64_t
solve_coord_dfs(dfsarg_solve_coord_t *arg)
{
	uint64_t coord;
	uint8_t pval;

	coord = arg->coord->coord(arg->cube, arg->coord_data);

	if (coord == 0) {
		if (arg->nmoves != arg->depth)
			return 0;
		return solve_coord_appendsolution(arg);
	}

	pval = get_coord_pval(arg->coord, arg->ptable, coord);
	if (arg->nmoves + pval > arg->depth)
		return 0;

	/* TODO recursive call */
	/* Is allowednextmove available here? */

	return 0;
}

STATIC int64_t
solve_coord_dispatch(
	cube_t cube,
	const char *coord_axis,
	uint8_t nissflag,
	uint8_t minmoves,
	uint8_t maxmoves,
	uint64_t maxsolutions,
	int optimal,
	int threads,
	uint64_t data_size,
	const void *data,
	uint64_t sols_size,
	char *sols
)
{
	int i, n;
	coord_t *coord;
	uint8_t axis;

	n = strlen(coord_axis);
	for (i = 0; coord_axis[i] != ' ' && coord_axis[i] != '\0'; i++) ;

	coord = parse_coord(coord_axis, i);
	axis = parse_axis(coord_axis + i + 1, n - i - 1);

	if (coord == NULL) {
		LOG("Could not parse coordinate '%s'\n", coord_axis);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	if (axis == UINT8_ERROR) {
		LOG("Could not parse axis from '%s'\n", coord_axis);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return solve_coord(cube, coord, axis, nissflag, minmoves, maxmoves,
	    maxsolutions, optimal, threads, data_size, data, sols_size, sols);
}

STATIC int64_t
solve_coord(
	cube_t cube,
	coord_t *coord,
	uint8_t axis,
	uint8_t nissflag,
	uint8_t minmoves,
	uint8_t maxmoves,
	uint64_t maxsolutions,
	int optimal,
	int threads,
	uint64_t data_size,
	const void *data,
	uint64_t sols_size,
	char *sols
)
{
	int8_t d;
	uint8_t t, shortest_sol;
	int64_t nsols;
	uint64_t sols_used;
	cube_t c;
	const void *coord_data;
	const uint8_t *ptable;
	dfsarg_solve_coord_t arg;
	tableinfo_t info;

	if (readtableinfo(data_size, data, &info) != NISSY_OK)
		goto solve_coord_error_data;

	if (info.type == TABLETYPE_PRUNING) {
		/* Only the pruning table */
		coord_data = NULL;
		ptable = (uint8_t *)data + INFOSIZE;
	} else {
		/* Coordinate has extra data */
		coord_data = (uint8_t *)data + INFOSIZE;
		ptable = (uint8_t *)data + info.next + INFOSIZE;
	}

	nsols = 0;
	sols_used = 0;
	shortest_sol = MAXLEN_COORDSOL + 1;
	c = transform(cube, t);
	t = coord->axistrans[axis];

	arg = (dfsarg_solve_coord_t) {
		.cube = c,
		.coord = coord,
		.coord_data = coord_data,
		.ptable = ptable,
		.trans = t,
		.nsols = &nsols,
		.maxsolutions = (int64_t)maxsolutions,
		.optimal = optimal,
		.shortest_sol = &shortest_sol,
		.solutions_size = sols_size,
		.solutions_used = &sols_used,
		.solutions = &sols,
	};

	if (coord->coord(c, coord_data) == 0) {
		if (minmoves == 0) {
			nsols = 1;
			if (!solve_coord_appendchar(
			    sols, sols_size, &sols_used, '\n'))
				goto solve_coord_error_buffer;
		}
		goto solve_coord_done;
	}

	for (
	    d = MAX(minmoves, 1);
	    d <= maxmoves && nsols < (int64_t)maxsolutions
	        && !(nsols != 0 && d > shortest_sol + optimal);
	    d++
	) {
		if (d >= 10)
			LOG("Found %" PRId64 " solutions, searching at depth %"
			    PRId8 "\n", nsols, d);

		arg.depth = d;
		arg.nmoves = 0;
		nsols += solve_coord_dfs(&arg);
	}

solve_coord_done:
	if (!solve_coord_appendchar(sols, sols_size, &sols_used, '\0'))
		goto solve_coord_error_buffer;

	return nsols;

solve_coord_error_data:
	LOG("solve_coord: error reading table\n");
	return NISSY_ERROR_DATA;

solve_coord_error_buffer:
	LOG("Could not append solution to buffer: size too small\n");
	return NISSY_ERROR_BUFFER_SIZE;
}
