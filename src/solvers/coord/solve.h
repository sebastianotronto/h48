typedef struct {
	cube_t cube;
	uint8_t target_depth;
	solution_moves_t *solution_moves;
	solution_settings_t *solution_settings;
	coord_t *coord;
	const void *coord_data;
	const uint8_t *ptable;
	solution_list_t *solution_list;
} dfsarg_solve_coord_t;

STATIC int64_t solve_coord(cube_t, coord_t *, uint8_t, uint8_t, uint8_t,
    uint8_t, uint64_t, int8_t, int, uint64_t, const void *, size_t, char *);
STATIC int64_t solve_coord_dispatch(cube_t, const char *, uint8_t, uint8_t,
    uint8_t, uint64_t, int8_t, int, uint64_t, const void *, size_t, char *);
STATIC int64_t solve_coord_dfs(dfsarg_solve_coord_t *);

STATIC int64_t
solve_coord_dfs(dfsarg_solve_coord_t *arg)
{
	uint8_t m, pval;
	uint32_t mm;
	uint64_t coord;
	int64_t n, ret;
	cube_t backup_cube;

	coord = arg->coord->coord(arg->cube, arg->coord_data);

	if (coord == 0) {
		if (arg->solution_moves->nmoves != arg->target_depth ||
		    (arg->coord->is_admissible != NULL &&
		        !arg->coord->is_admissible(arg->solution_moves->nmoves,
		            arg->solution_moves->moves)))
			return 0;
		return appendsolution(arg->solution_moves,
		    arg->solution_settings, arg->solution_list);
	}

	pval = get_coord_pval(arg->coord, arg->ptable, coord);
	if (arg->solution_moves->nmoves + pval > arg->target_depth)
		return 0;

	backup_cube = arg->cube;

	ret = 0;
	mm = allowednextmove_mask(
	    arg->solution_moves->nmoves, arg->solution_moves->moves);
	arg->solution_moves->nmoves++;
	for (m = 0; m < NMOVES; m++) {
		if (!(mm & (1 << m)))
			continue;

		arg->solution_moves->moves[arg->solution_moves->nmoves-1] = m;
		arg->cube = move(backup_cube, m);
		n = solve_coord_dfs(arg);
		if (n < 0)
			return n;
		ret += n;
	}
	arg->cube = backup_cube;
	arg->solution_moves->nmoves--;

	return 0;
}

STATIC int64_t
solve_coord_dispatch(
	cube_t cube,
	const char *coord_and_axis,
	uint8_t nissflag,
	uint8_t minmoves,
	uint8_t maxmoves,
	uint64_t maxsolutions,
	int8_t optimal,
	int threads,
	uint64_t data_size,
	const void *data,
	size_t solutions_size,
	char *sols
)
{
	coord_t *coord;
	uint8_t axis;

	parse_coord_and_axis(
	    strlen(coord_and_axis), coord_and_axis, &coord, &axis);

	if (coord == NULL) {
		LOG("Could not parse coordinate from '%s'\n", coord_and_axis);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	if (axis == UINT8_ERROR) {
		LOG("Could not parse axis from '%s'\n", coord_and_axis);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return solve_coord(cube, coord, axis, nissflag, minmoves, maxmoves,
	    maxsolutions, optimal, threads, data_size, data,
	    solutions_size, sols);
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
	int8_t optimal,
	int threads,
	uint64_t data_size,
	const void *data,
	size_t solutions_size,
	char *sols
)
{
	int8_t d;
	uint8_t t;
	int64_t ndepth;
	cube_t c;
	const void *coord_data;
	const uint8_t *ptable;
	dfsarg_solve_coord_t arg;
	tableinfo_t info;
	solution_moves_t solution_moves;
	solution_settings_t solution_settings;
	solution_list_t solution_list;

	if (!solution_list_init(&solution_list, solutions_size, sols))
		goto solve_coord_error_buffer;

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

	t = coord->axistrans[axis];
	c = transform(cube, t);

	solution_moves_reset(&solution_moves);

	solution_settings = (solution_settings_t) {
		.tmask = TM_SINGLE(inverse_trans(t)),
		.unniss = false,
		.maxmoves = maxmoves,
		.maxsolutions = maxsolutions,
		.optimal = optimal,
	};

	arg = (dfsarg_solve_coord_t) {
		.cube = c,
		.coord = coord,
		.coord_data = coord_data,
		.ptable = ptable,
		.solution_moves = &solution_moves,
		.solution_settings = &solution_settings,
		.solution_list = &solution_list,
	};

	if (coord->coord(c, coord_data) == 0) {
		if (minmoves == 0 && !appendsolution(
		    &solution_moves, &solution_settings, &solution_list))
				goto solve_coord_error_buffer;
		goto solve_coord_done;
	}

	for (
	    d = MAX(minmoves, 1);
	    !solutions_done(&solution_list, &solution_settings, d);
	    d++
	) {
		if (d >= 10)
			LOG("Found %" PRIu64 " solutions, searching at depth %"
			    PRId8 "\n", solution_list.nsols, d);

		arg.target_depth = d;
		solution_moves_reset(arg.solution_moves);
		ndepth = solve_coord_dfs(&arg);

		if (ndepth < 0) {
			LOG("Error %" PRId64 "\n", ndepth);
			return ndepth;
		}

		solution_list.nsols += (uint64_t)ndepth;
	}

solve_coord_done:
	return (int64_t)solution_list.nsols;

solve_coord_error_data:
	LOG("solve_coord: error reading table\n");
	return NISSY_ERROR_DATA;

solve_coord_error_buffer:
	LOG("Could not append solution to buffer: size too small\n");
	return NISSY_ERROR_BUFFER_SIZE;
}
