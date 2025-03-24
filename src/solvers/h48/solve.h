#define STARTING_MOVES 3
#define STARTING_CUBES 3240 /* Number of 3-move sequences */

typedef struct {
	cube_t cube;
	uint8_t moves[STARTING_MOVES];
} solve_h48_task_t;

typedef struct {
	cube_t start_cube;
	cube_t cube;
	cube_t inverse;
	int8_t target_depth;
	solution_moves_t *solution_moves;
	solution_settings_t *solution_settings;
	solution_list_t *solution_list;
	int8_t lb_normal;
	int8_t lb_inverse;
	bool use_lb_normal;
	bool use_lb_inverse;
	uint8_t h;
	uint8_t k;
	uint8_t base;
	const uint32_t *cocsepdata;
	const uint8_t *h48data;
	const uint8_t *h48data_fallback_h0k4;
	const void *h48data_fallback_eoesep;
	uint32_t movemask_normal;
	uint32_t movemask_inverse;
	int64_t nodes_visited;
	int64_t table_fallbacks;
	int64_t table_lookups;
	int8_t threads;
	int ntasks;
	solve_h48_task_t *tasks;
	int thread_id;
	pthread_mutex_t *solutions_mutex;
} dfsarg_solve_h48_t;

typedef struct {
	cube_t cube;
	int8_t nmoves;
	uint8_t moves[STARTING_MOVES];
	int8_t minmoves;
	int8_t maxmoves;
	int8_t *shortest_sol;
} dfsarg_solve_h48_maketasks_t;

STATIC_INLINE bool solve_h48_stop(dfsarg_solve_h48_t *);
STATIC int64_t solve_h48_maketasks(
    dfsarg_solve_h48_t *, dfsarg_solve_h48_maketasks_t *,
    solve_h48_task_t [static STARTING_CUBES], int *);
STATIC void *solve_h48_runthread(void *);
STATIC int64_t solve_h48_dfs(dfsarg_solve_h48_t *);
STATIC int64_t solve_h48(cube_t, int8_t, int8_t, uint64_t, int8_t, int8_t,
    uint64_t, const void *, size_t n, char [n],
    long long [static NISSY_SIZE_SOLVE_STATS]);

STATIC_INLINE bool
solve_h48_stop(dfsarg_solve_h48_t *arg)
{
	uint32_t data, data_inv;
	int64_t coord;
	int8_t target, nh, n;
	uint8_t pval_cocsep, pval_eoesep;

	n = arg->solution_moves->nmoves + arg->solution_moves->npremoves;
	target = arg->target_depth - n;
	if (target <= 0 ||
	    arg->solution_list->nsols == arg->solution_settings->maxsolutions)
		return true;

	arg->movemask_normal = arg->movemask_inverse = MM_ALLMOVES;
	arg->nodes_visited++;

	/* Preliminary probing using last computed bound, if possible */

	if ((arg->use_lb_normal && arg->lb_normal > target) ||
	    (arg->use_lb_inverse && arg->lb_inverse > target))
		return true;

	/* Preliminary corner probing */

	if (get_h48_cdata(arg->cube, arg->cocsepdata, &data) > target ||
	    get_h48_cdata(arg->inverse, arg->cocsepdata, &data_inv) > target)
		return true;

	/* Inverse probing */

	if (!arg->use_lb_inverse) {
		coord = coord_h48_edges(
		    arg->inverse, COCLASS(data_inv), TTREP(data_inv), arg->h);
		arg->lb_inverse = get_h48_pval(arg->h48data, coord, arg->k);
		arg->table_lookups++;

		if (arg->k == 2 && arg->lb_inverse == 0) {
			arg->table_fallbacks++;

			pval_cocsep = get_h48_pval(
			    arg->h48data_fallback_h0k4, coord >> arg->h, 4);
			pval_eoesep = get_eoesep_pval_cube(
			    arg->h48data_fallback_eoesep, arg->inverse);
			arg->lb_inverse = MAX(pval_cocsep, pval_eoesep);
		} else {
			arg->lb_inverse += arg->base;
		}

		arg->use_lb_inverse = true;
	}

	if (arg->lb_inverse > target)
		return true;
	nh = arg->lb_inverse == target;
	arg->movemask_normal = nh * MM_NOHALFTURNS + (1-nh) * MM_ALLMOVES;

	/* Normal probing */

	if (!arg->use_lb_normal) {
		coord = coord_h48_edges(
		    arg->cube, COCLASS(data), TTREP(data), arg->h);
		arg->lb_normal = get_h48_pval(arg->h48data, coord, arg->k);
		arg->table_lookups++;

		if (arg->k == 2 && arg->lb_normal == 0) {
			arg->table_fallbacks++;

			pval_cocsep = get_h48_pval(
			    arg->h48data_fallback_h0k4, coord >> arg->h, 4);
			pval_eoesep = get_eoesep_pval_cube(
			    arg->h48data_fallback_eoesep, arg->cube);
			arg->lb_normal = MAX(pval_cocsep, pval_eoesep);
		} else {
			arg->lb_normal += arg->base;
		}

		arg->use_lb_normal = true;
	}

	if (arg->lb_normal > target)
		return true;
	nh = arg->lb_normal == target;
	arg->movemask_inverse = nh * MM_NOHALFTURNS + (1-nh) * MM_ALLMOVES;

	return false;
}

STATIC int64_t
solve_h48_dfs(dfsarg_solve_h48_t *arg)
{
	int64_t ret, n;
	uint8_t m, nm, lbn, lbi;
	uint32_t mm_normal, mm_inverse;
	bool ulbi, ulbn;
	cube_t backup_cube, backup_inverse;

	if (issolved(arg->cube)) {
		nm = arg->solution_moves->nmoves
		    + arg->solution_moves->npremoves;
		if (arg->target_depth != nm)
			return 0;
		pthread_mutex_lock(arg->solutions_mutex);
		ret = appendsolution(arg->solution_moves,
		    arg->solution_settings, arg->solution_list);
		pthread_mutex_unlock(arg->solutions_mutex);
		return ret;
	}

	if (solve_h48_stop(arg))
		return 0;

	backup_cube = arg->cube;
	backup_inverse = arg->inverse;
	lbn = arg->lb_normal;
	lbi = arg->lb_inverse;
	ulbn = arg->use_lb_normal;
	ulbi = arg->use_lb_inverse;

	ret = 0;
	mm_normal = allowednextmove_mask(arg->solution_moves->nmoves,
	    arg->solution_moves->moves) & arg->movemask_normal;
	mm_inverse = allowednextmove_mask(arg->solution_moves->npremoves,
	    arg->solution_moves->premoves) & arg->movemask_inverse;
	if (popcount_u32(mm_normal) <= popcount_u32(mm_inverse)) {
		arg->solution_moves->nmoves++;
		for (m = 0; m < 18; m++) {
			if (!(mm_normal & (UINT32_C(1) << (uint32_t)m)))
				continue;
			arg->solution_moves->moves[
			    arg->solution_moves->nmoves-1] = m;
			arg->cube = move(backup_cube, m);
			arg->inverse = premove(backup_inverse, m);
			arg->lb_inverse = lbi;
			arg->use_lb_normal = false;
			arg->use_lb_inverse = ulbi && m % 3 == 1;
			n = solve_h48_dfs(arg);
			if (n < 0)
				return n;
			ret += n;
		}
		arg->solution_moves->nmoves--;
	} else {
		arg->solution_moves->npremoves++;
		for (m = 0; m < 18; m++) {
			if(!(mm_inverse & (UINT32_C(1) << (uint32_t)m)))
				continue;
			arg->solution_moves->premoves[
			    arg->solution_moves->npremoves-1] = m;
			arg->inverse = move(backup_inverse, m);
			arg->cube = premove(backup_cube, m);
			arg->lb_normal = lbn;
			arg->use_lb_inverse = false;
			arg->use_lb_normal = ulbn && m % 3 == 1;
			n = solve_h48_dfs(arg);
			if (n < 0)
				return n;
			ret += n;
		}
		arg->solution_moves->npremoves--;
	}

	arg->cube = backup_cube;
	arg->inverse = backup_inverse;

	return ret;
}

STATIC void *
solve_h48_runthread(void *arg)
{
	int i, j;
	solve_h48_task_t task;
	dfsarg_solve_h48_t *dfsarg;

	dfsarg = (dfsarg_solve_h48_t *)arg;

	for (i = dfsarg->thread_id; i < dfsarg->ntasks; i += dfsarg->threads) {
		task = dfsarg->tasks[i];

		solution_moves_reset(dfsarg->solution_moves);
		memcpy(
		    dfsarg->solution_moves->moves, task.moves, STARTING_MOVES);
		dfsarg->solution_moves->nmoves = STARTING_MOVES;

		dfsarg->cube = dfsarg->start_cube;
		for (j = 0; j < STARTING_MOVES; j++)
			dfsarg->cube = move(dfsarg->cube, task.moves[j]);
		dfsarg->inverse = inverse(dfsarg->cube);

		dfsarg->lb_normal = 0;
		dfsarg->lb_inverse = 0;
		dfsarg->use_lb_normal = false;
		dfsarg->use_lb_inverse = false;
		dfsarg->movemask_normal = MM_ALLMOVES;
		dfsarg->movemask_inverse = MM_ALLMOVES;

		solve_h48_dfs(dfsarg);
	}

	return NULL;
}

STATIC int64_t
solve_h48_maketasks(
	dfsarg_solve_h48_t *solve_arg,
	dfsarg_solve_h48_maketasks_t *maketasks_arg,
	solve_h48_task_t tasks[static STARTING_CUBES],
	int *ntasks
)
{
	int r;
	int64_t appret;
	uint8_t m, t;
	uint32_t mm;
	cube_t backup_cube;
	solution_moves_t moves;

	if (issolved(maketasks_arg->cube)) {
		if (maketasks_arg->nmoves > maketasks_arg->maxmoves ||
		    maketasks_arg->nmoves < maketasks_arg->minmoves ||
		    solve_arg->solution_list->nsols >=
		        solve_arg->solution_settings->maxsolutions)
			return NISSY_OK;

		solution_moves_reset(&moves);
		moves.nmoves = maketasks_arg->nmoves;
		memcpy(moves.moves,
		    maketasks_arg->moves, maketasks_arg->nmoves);

		appret = appendsolution(&moves,
		    solve_arg->solution_settings, solve_arg->solution_list);
		return appret < 0 ? appret : NISSY_OK;
	}

	if (maketasks_arg->nmoves == STARTING_MOVES) {
		tasks[*ntasks].cube = maketasks_arg->cube;
		memcpy(tasks[*ntasks].moves,
		    maketasks_arg->moves, STARTING_MOVES);
		(*ntasks)++;
		return NISSY_OK;
	}

	mm = allowednextmove_mask(maketasks_arg->nmoves, maketasks_arg->moves);

	maketasks_arg->nmoves++;
	backup_cube = maketasks_arg->cube;
	for (m = 0; m < 18; m++) {
		if (!(mm & (1 << m)))
			continue;
		maketasks_arg->moves[maketasks_arg->nmoves-1] = m;
		maketasks_arg->cube = move(backup_cube, m);
		r = solve_h48_maketasks(
		    solve_arg, maketasks_arg, tasks, ntasks);
		if (r < 0)
			return r;

		/* Avoid symmetry-equivalent moves from the starting cube */
		if (maketasks_arg->nmoves == 1)
			for (t = 0; t < NTRANS; t++)
				if (solve_arg->solution_settings->tmask &
				    (UINT64_C(1) << (uint64_t)t))
					mm &= ~(UINT32_C(1) <<
					    (uint32_t)transform_move(m, t));
	}
	maketasks_arg->nmoves--;
	maketasks_arg->cube = backup_cube;

	return NISSY_OK;
}

STATIC int64_t
solve_h48(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	uint64_t maxsolutions,
	int8_t optimal,
	int8_t threads,
	uint64_t data_size,
	const void *data,
	size_t solutions_size,
	char solutions[solutions_size],
	long long stats[static NISSY_SIZE_SOLVE_STATS]
)
{
	int i, ntasks, eoesep_table_index;
	int8_t d;
	dfsarg_solve_h48_t arg[THREADS];
	solve_h48_task_t tasks[STARTING_CUBES];
	dfsarg_solve_h48_maketasks_t maketasks_arg;
	long double fallback_rate, lookups_per_node;
	uint64_t offset;
	int64_t nodes_visited, table_lookups, table_fallbacks;
	tableinfo_t info, fbinfo, fbinfo2;
	const uint32_t *cocsepdata;
	const uint8_t *fallback, *h48data;
	const void *fallback2;
	solution_moves_t solution_moves[THREADS];
	solution_settings_t settings;
	solution_list_t sollist;
	pthread_t thread[THREADS];
	pthread_mutex_t solutions_mutex;

	if (!solution_list_init(&sollist, solutions_size, solutions))
		goto solve_h48_error_solutions_buffer;

	if (readtableinfo_n(data_size, data, 2, &info) != NISSY_OK)
		goto solve_h48_error_data;

	cocsepdata = (uint32_t *)((char *)data + INFOSIZE);
	h48data = (uint8_t *)data + COCSEP_FULLSIZE + INFOSIZE;

	/* Read fallback table(s) */
	fallback = NULL;
	if (readtableinfo_n(data_size, data, 3, &fbinfo) != NISSY_OK)
		goto solve_h48_error_data;
	offset = info.next;
	eoesep_table_index = 3;
	if (info.bits == 2) {
		/* We only support h0k4 as fallback table */
		if (fbinfo.h48h != 0 || fbinfo.bits != 4)
			goto solve_h48_error_data;
		fallback = h48data + offset;
		offset += fbinfo.next;
		eoesep_table_index++;
	}

	if (readtableinfo_n(data_size, data, eoesep_table_index, &fbinfo2)
	    != NISSY_OK)
		goto solve_h48_error_data;

	/* Some heuristic check to see that it is eoesep */
	if (fbinfo2.bits != 4 || fbinfo2.type != TABLETYPE_SPECIAL)
		goto solve_h48_error_data;
	fallback2 = h48data + offset;

	settings = (solution_settings_t) {
		.tmask = symmetry_mask(cube),
		.unniss = true,
		.maxmoves = maxmoves,
		.maxsolutions = maxsolutions,
		.optimal = optimal,
	};

	for (i = 0; i < threads; i++) {
		arg[i] = (dfsarg_solve_h48_t) {
			.start_cube = cube,
			.cube = cube,
			.h = info.h48h,
			.k = info.bits,
			.base = info.base,
			.cocsepdata = cocsepdata,
			.h48data = h48data,
			.h48data_fallback_h0k4 = fallback,
			.h48data_fallback_eoesep = fallback2,
			.solution_moves = &solution_moves[i],
			.solution_settings = &settings,
			.solution_list = &sollist,
			.nodes_visited = 0,
			.table_fallbacks = 0,
			.table_lookups = 0,
			.threads = threads,
			.thread_id = i,
			.solutions_mutex = &solutions_mutex,
		};

	}

	pthread_mutex_init(&solutions_mutex, NULL);

	maketasks_arg = (dfsarg_solve_h48_maketasks_t) {
		.cube = cube,
		.nmoves = 0,
		.minmoves = minmoves,
		.maxmoves = maxmoves,
	};
	ntasks = 0;
	solve_h48_maketasks(&arg[0], &maketasks_arg, tasks, &ntasks);
	if (ntasks < 0)
		goto solve_h48_error_solutions_buffer;
	if (sollist.nsols >= maxsolutions)
		goto solve_h48_done;

	for (i = 0; i < threads; i++) {
		arg[i].ntasks = ntasks;
		arg[i].tasks = tasks;
	}

	LOG("Prepared %d tasks\n", ntasks);

	for (
	    d = MAX(minmoves, STARTING_MOVES + 1);
	    !solutions_done(&sollist, &settings, d);
	    d++
	) {
		if (d >= 10)
			LOG("Found %" PRId64 " solutions, searching at depth %"
			    PRId8 "\n", sollist.nsols, d);
		for (i = 0; i < threads; i++) {
			arg[i].target_depth = d;
			pthread_create(
			    &thread[i], NULL, solve_h48_runthread, &arg[i]);
		}
		for (i = 0; i < threads; i++)
			pthread_join(thread[i], NULL);
	}

solve_h48_done:
	nodes_visited = table_lookups = table_fallbacks = 0;
	for (i = 0; i < threads; i++) {
		nodes_visited += arg[i].nodes_visited;
		table_fallbacks += arg[i].table_fallbacks;
		table_lookups += arg[i].table_lookups;
	}

	stats[0] = nodes_visited;
	stats[1] = table_lookups;
	stats[2] = table_fallbacks;
	lookups_per_node = table_lookups / (long double)nodes_visited;
	fallback_rate = nodes_visited == 0 ? 0.0 :
	    (table_fallbacks * 100) / (long double)table_lookups;
	LOG("Nodes visited: %" PRId64 "\n", nodes_visited);
	LOG("Lookups: %" PRId64 " (%.3Lf per node)\n",
	    table_lookups, lookups_per_node);
	LOG("Table fallbacks: %" PRId64 " (%.3Lf%%)\n",
	    table_fallbacks, fallback_rate);

	return sollist.nsols;

solve_h48_error_data:
	LOG("solve_h48: error reading table\n");
	return NISSY_ERROR_DATA;

solve_h48_error_solutions_buffer:
	return NISSY_ERROR_BUFFER_SIZE;
}
