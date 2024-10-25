typedef struct {
	cube_t startcube;
	cube_t cube;
	cube_t inverse;
	int8_t depth;
	int8_t nmoves;
	uint8_t moves[MAXLEN];
	int8_t npremoves;
	uint8_t premoves[MAXLEN];
	int8_t lastbound_normal;
	int8_t lastbound_inverse;
	bool last_double;
	bool last_double_pre;
	_Atomic int64_t *nsols;
	int64_t maxsolutions;
	uint8_t h;
	uint8_t k;
	uint8_t base;
	const uint32_t *cocsepdata;
	const uint8_t *h48data;
	const uint8_t *h48data_fallback;
	uint64_t solutions_size;
	uint64_t *solutions_used;
	char **solutions;
	uint32_t movemask_normal;
	uint32_t movemask_inverse;
	_Atomic int64_t nodes_visited;
	_Atomic int64_t table_fallbacks;
	_Atomic int64_t table_lookups;
	pthread_mutex_t *solutions_mutex;
	pthread_mutex_t *task_mutex;
	uint16_t *task_moves;
} dfsarg_solveh48_t;

STATIC void solve_h48_appendsolution(dfsarg_solveh48_t *);
STATIC bool solve_h48_appendmoves(dfsarg_solveh48_t *, int8_t, uint8_t *);
STATIC bool solve_h48_appendchar(dfsarg_solveh48_t *, char);
STATIC_INLINE bool solve_h48_stop(dfsarg_solveh48_t *);
STATIC bool solve_h48_checkonemove(dfsarg_solveh48_t *);
STATIC bool solve_h48_gettask(dfsarg_solveh48_t *);
STATIC void *solve_h48_runthread(void *);
STATIC int64_t solve_h48_dfs(dfsarg_solveh48_t *);
STATIC int64_t solve_h48(cube_t, int8_t, int8_t, int8_t, uint64_t,
    const void *, uint64_t, char *, long long [static NISSY_SIZE_SOLVE_STATS]);

STATIC void
solve_h48_appendsolution(dfsarg_solveh48_t *arg)
{
	uint8_t invprem[MAXLEN];
	uint64_t solstart;

	if (*arg->nsols >= arg->maxsolutions)
		return;

	solstart = *arg->solutions_used;

	if (!solve_h48_appendmoves(arg, arg->nmoves, arg->moves))
		goto solve_h48_appendsolution_error;

	if (arg->npremoves > 0) {
		if (!solve_h48_appendchar(arg, ' '))
			goto solve_h48_appendsolution_error;

		invertmoves(arg->premoves, arg->npremoves, invprem);

		if (!solve_h48_appendmoves(arg, arg->npremoves, invprem))
			goto solve_h48_appendsolution_error;
	}
	LOG("Solution found: %s\n", *arg->solutions + solstart);

	if (!solve_h48_appendchar(arg, '\n'))
		goto solve_h48_appendsolution_error;
	(*arg->nsols)++;

	return;

solve_h48_appendsolution_error:
	LOG("Could not append solution to buffer: size too small\n");
	return;
}

STATIC bool
solve_h48_appendmoves(dfsarg_solveh48_t *arg, int8_t n, uint8_t *moves)
{
	int64_t strl;

	strl = writemoves(moves, n, arg->solutions_size - *arg->solutions_used,
	    *arg->solutions + *arg->solutions_used);

	if (strl < 0)
		return false;

	*arg->solutions_used += MAX(0, strl-1);
	return true;
}

STATIC bool
solve_h48_appendchar(dfsarg_solveh48_t *arg, char c)
{
	if (arg->solutions_size <= *arg->solutions_used)
		return false;

	*(*arg->solutions + *arg->solutions_used) = c;
	(*arg->solutions_used)++;

	return true;
}

STATIC_INLINE bool
solve_h48_stop(dfsarg_solveh48_t *arg)
{
	uint32_t data, data_inv, nh;
	int64_t coord, coord_inv;
	uint8_t target;

	if (*arg->nsols == arg->maxsolutions)
		return true;

	target = arg->depth - arg->nmoves - arg->npremoves;
	arg->movemask_normal = arg->movemask_inverse = MM_ALLMOVES;
	arg->nodes_visited++;

	/* Inverse probing */

	if (arg->last_double)
		goto solve_h48_stop_inverseprobe_check;

	arg->lastbound_inverse =
	    get_h48_cdata(arg->inverse, arg->cocsepdata, &data_inv);
	if (arg->lastbound_inverse + arg->nmoves + arg->npremoves > arg->depth)
		return true;

	coord_inv = coord_h48_edges(
	    arg->inverse, COCLASS(data_inv), TTREP(data_inv), arg->h);
	arg->lastbound_inverse = get_h48_pval(arg->h48data, coord_inv, arg->k);
	arg->table_lookups++;

	if (arg->k == 2) {
		if (arg->lastbound_inverse == 0) {
			arg->table_fallbacks++;
			arg->lastbound_inverse = get_h48_pval(
			    arg->h48data_fallback, coord_inv >> arg->h, 4);
		} else {
			arg->lastbound_inverse += arg->base;
		}
	}

solve_h48_stop_inverseprobe_check:
	if (arg->lastbound_inverse > target)
		return true;
	nh = arg->lastbound_inverse == target;
	arg->movemask_normal = nh * MM_NOHALFTURNS + (1-nh) * MM_ALLMOVES;

	/* Normal probing */

	if (arg->last_double_pre)
		goto solve_h48_stop_normalprobe_check;

	arg->lastbound_normal =
	    get_h48_cdata(arg->cube, arg->cocsepdata, &data);
	if (arg->lastbound_normal + arg->nmoves + arg->npremoves > arg->depth)
		return true;

	coord = coord_h48_edges(arg->cube, COCLASS(data), TTREP(data), arg->h);
	arg->lastbound_normal = get_h48_pval(arg->h48data, coord, arg->k);
	arg->table_lookups++;

	if (arg->k == 2) {
		if (arg->lastbound_normal == 0) {
			arg->table_fallbacks++;
			arg->lastbound_normal = get_h48_pval(
			    arg->h48data_fallback, coord >> arg->h, 4);
		} else {
			arg->lastbound_normal += arg->base;
		}
	}

solve_h48_stop_normalprobe_check:
	if (arg->lastbound_normal > target)
		return true;
	nh = arg->lastbound_normal == target;
	arg->movemask_inverse = nh * MM_NOHALFTURNS + (1-nh) * MM_ALLMOVES;

	return false;
}

STATIC int64_t
solve_h48_dfs(dfsarg_solveh48_t *arg)
{
	int64_t ret;
	bool backup_last_double, backup_last_double_pre;
	uint8_t m, backup_lastbound_normal, backup_lastbound_inverse;
	uint32_t mm_normal, mm_inverse;
	cube_t backup_cube, backup_inverse;

	if (solve_h48_stop(arg))
		return 0;

	if (issolved(arg->cube)) {
		if (arg->nmoves + arg->npremoves != arg->depth)
			return 0;
		pthread_mutex_lock(arg->solutions_mutex);
		solve_h48_appendsolution(arg);
		pthread_mutex_unlock(arg->solutions_mutex);
		return 1;
	}

	backup_last_double = arg->last_double;
	backup_last_double_pre = arg->last_double_pre;
	backup_cube = arg->cube;
	backup_inverse = arg->inverse;
	backup_lastbound_normal = arg->lastbound_normal;
	backup_lastbound_inverse = arg->lastbound_inverse;

	ret = 0;
	mm_normal = allowednextmove_mask(arg->moves, arg->nmoves) &
	    arg->movemask_normal;
	mm_inverse = allowednextmove_mask(arg->premoves, arg->npremoves) &
	    arg->movemask_inverse;
	if (popcount_u32(mm_normal) <= popcount_u32(mm_inverse)) {
		arg->last_double_pre = false;
		arg->nmoves++;
		for (m = 0; m < 18; m++) {
			if (mm_normal & (1 << m)) {
				arg->moves[arg->nmoves-1] = m;
				arg->cube = move(backup_cube, m);
				arg->inverse = premove(backup_inverse, m);
				arg->lastbound_inverse =
				    backup_lastbound_inverse;
				arg->last_double = m % 3 == 1;
				ret += solve_h48_dfs(arg);
			}
		}
		arg->nmoves--;
	} else {
		arg->last_double = false;
		arg->npremoves++;
		for (m = 0; m < 18; m++) {
			if(mm_inverse & (1 << m)) {
				arg->premoves[arg->npremoves-1] = m;
				arg->inverse = move(backup_inverse, m);
				arg->cube = premove(backup_cube, m);
				arg->lastbound_normal =
				    backup_lastbound_normal;
				arg->last_double_pre = m % 3 == 1;
				ret += solve_h48_dfs(arg);
			}
		}
		arg->npremoves--;
	}

	arg->last_double = backup_last_double;
	arg->last_double_pre = backup_last_double_pre;
	arg->cube = backup_cube;
	arg->inverse = backup_inverse;
	arg->lastbound_normal = backup_lastbound_normal;
	arg->lastbound_inverse = backup_lastbound_inverse;

	return ret;
}

STATIC bool
solve_h48_gettask(dfsarg_solveh48_t *arg)
{
	uint8_t task_move[2];

	pthread_mutex_lock(arg->task_mutex);

	for ( ; *arg->task_moves < NMOVES * NMOVES; (*arg->task_moves)++) {
		task_move[0] = (uint8_t)(*arg->task_moves / NMOVES);
		task_move[1] = (uint8_t)(*arg->task_moves % NMOVES);
		if (allowednextmove(task_move, 2))
			goto solve_h48_gettask_found;
	}

	pthread_mutex_unlock(arg->task_mutex);
	return false;

solve_h48_gettask_found:
	(*arg->task_moves)++;
	arg->moves[0] = task_move[0];
	arg->moves[1] = task_move[1];

	pthread_mutex_unlock(arg->task_mutex);

	arg->cube = move(arg->startcube, arg->moves[0]);
	arg->cube = move(arg->cube, arg->moves[1]);
	arg->inverse = inverse(arg->cube);
	arg->nmoves = 2;
	arg->npremoves = 0;
	arg->lastbound_normal = 0;
	arg->lastbound_inverse = 0;
	arg->last_double = false;
	arg->last_double_pre = false;
	arg->movemask_normal = MM_ALLMOVES;
	arg->movemask_inverse = MM_ALLMOVES;

	return true;
}

STATIC void *
solve_h48_runthread(void *arg)
{
	while (solve_h48_gettask((dfsarg_solveh48_t *)arg))
		solve_h48_dfs((dfsarg_solveh48_t *)arg);

	return NULL;
}

STATIC bool
solve_h48_checkonemove(dfsarg_solveh48_t *arg)
{
	uint8_t i;
	cube_t c;

	for (i = 0; i <= MOVE_B3; i++) {
		c = move(arg->startcube, i);
		if (*arg->nsols < arg->maxsolutions && issolved(c)) {
			if (!solve_h48_appendmoves(arg, 1, &i) ||
			    !solve_h48_appendchar(arg, '\n'))
				return false;
			(*arg->nsols)++;
		}
	}

	return true;
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
	char *solutions,
	long long stats[static NISSY_SIZE_SOLVE_STATS]
)
{
	int i;
	int8_t d;
	_Atomic int64_t nsols;
	dfsarg_solveh48_t arg[THREADS];
	long double fallback_rate, lookups_per_node;
	uint16_t task_moves;
	uint64_t solutions_used;
	int64_t nodes_visited, table_lookups, table_fallbacks;
	tableinfo_t info, fbinfo;
	const uint32_t *cocsepdata;
	const uint8_t *fallback, *h48data;
	pthread_t thread[THREADS];
	pthread_mutex_t solutions_mutex, task_mutex;

	if(readtableinfo_n(data_size, data, 2, &info) != NISSY_OK)
		goto solve_h48_error_data;

	cocsepdata = (uint32_t *)((char *)data + INFOSIZE);
	h48data = (uint8_t *)data + COCSEP_FULLSIZE + INFOSIZE;
	if (info.bits == 2) {
		if (readtableinfo_n(data_size, data, 3, &fbinfo) != NISSY_OK)
			goto solve_h48_error_data;

		/* We only support h0k4 as fallback table */
		if (fbinfo.h48h != 0 || fbinfo.bits != 4)
			goto solve_h48_error_data;
		fallback = h48data + info.next;
	} else {
		fallback = NULL;
	}

	for (i = 0; i < THREADS; i++) {
		arg[i] = (dfsarg_solveh48_t) {
			.startcube = cube,
			.nsols = &nsols,
			.maxsolutions = maxsolutions,
			.h = info.h48h,
			.k = info.bits,
			.base = info.base,
			.cocsepdata = cocsepdata,
			.h48data = h48data,
			.h48data_fallback = fallback,
			.solutions_size = solutions_size,
			.solutions_used = &solutions_used,
			.solutions = &solutions,
			.nodes_visited = 0,
			.table_fallbacks = 0,
			.table_lookups = 0,
			.solutions_mutex = &solutions_mutex,
			.task_mutex = &task_mutex,
			.task_moves = &task_moves,
		};

	}

	nsols = 0;
	solutions_used = 0;

	pthread_mutex_init(&solutions_mutex, NULL);
	pthread_mutex_init(&task_mutex, NULL);

	if (minmoves <= 1 && maxmoves >= 1)
		if (!solve_h48_checkonemove(&arg[0]))
			goto solve_h48_error_solutions_buffer;

	for (d = MAX(minmoves, 2); d <= maxmoves && nsols < maxsolutions; d++)
	{
		if (d >= 10)
			LOG("Found %" PRId64 " solutions, searching at depth %"
			    PRId8 "\n", nsols, d);
		task_moves = 0;
		for (i = 0; i < THREADS; i++) {
			arg[i].depth = d;
			pthread_create(
			    &thread[i], NULL, solve_h48_runthread, &arg[i]);
		}
		for (i = 0; i < THREADS; i++)
			pthread_join(thread[i], NULL);
	}

	if (!solve_h48_appendchar(&arg[0], '\0'))
		goto solve_h48_error_solutions_buffer;

	nodes_visited = table_lookups = table_fallbacks = 0;
	for (i = 0; i < THREADS; i++) {
		nodes_visited += arg[i].nodes_visited;
		table_fallbacks += arg[i].table_fallbacks;
		table_lookups += arg[i].table_lookups;
	}

	stats[0] = nodes_visited;
	stats[1] = table_lookups;
	lookups_per_node = table_lookups / (long double)nodes_visited;
	stats[2] = table_fallbacks;
	fallback_rate = nodes_visited == 0 ? 0.0 :
	    (table_fallbacks * 100) / (long double)table_lookups;
	LOG("Nodes visited: %" PRId64 "\n", nodes_visited);
	LOG("Lookups: %" PRId64 " (%.3Lf per node)\n",
	    table_lookups, lookups_per_node);
	LOG("Table fallbacks: %" PRId64 " (%.3Lf%%)\n",
	    table_fallbacks, fallback_rate);

	return nsols;

solve_h48_error_data:
	LOG("solve_h48: error reading table\n");
	return NISSY_ERROR_DATA;

solve_h48_error_solutions_buffer:
	return NISSY_ERROR_BUFFER_SIZE;
}

STATIC int64_t
solve_h48_multithread(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	int8_t maxsolutions,
	uint64_t data_size,
	const void *data,
	uint64_t solutions_size,
	char *solutions,
	long long stats[static NISSY_SIZE_SOLVE_STATS]
)
{
	return solve_h48(cube, minmoves, maxmoves, maxsolutions, data_size,
	    data, solutions_size, solutions, stats);
}
