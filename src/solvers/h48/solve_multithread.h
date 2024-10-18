#define MAX_QUEUE_SIZE 244
#define BFS_DEPTH 2

typedef struct {
	dfsarg_solveh48_t tasks[MAX_QUEUE_SIZE];
	int front;
	int rear;
	int tasks_count;
	int active;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_cond_t active_cond;
	atomic_bool terminate;
} task_queue_t;

STATIC void solve_h48_appendsolution_thread(dfsarg_solveh48_t *, task_queue_t *);
STATIC void init_queue(task_queue_t *);
STATIC void submit_task(task_queue_t *, dfsarg_solveh48_t);
STATIC void copy_queue(task_queue_t *, task_queue_t *, int, _Atomic int64_t *);
STATIC void *start_thread(void *);
STATIC int64_t solve_h48_bfs(dfsarg_solveh48_t *, task_queue_t *, int8_t);
STATIC int64_t solve_h48_single(dfsarg_solveh48_t *, task_queue_t *);
STATIC int64_t solve_h48_multithread(cube_t, int8_t, int8_t, int8_t, uint64_t,
    const void *, uint64_t, char *, long long [static NISSY_SIZE_SOLVE_STATS]);

STATIC void
solve_h48_appendsolution_thread(dfsarg_solveh48_t *arg, task_queue_t *tq)
{
	pthread_mutex_lock(&tq->mutex);
	int64_t strl = 0;
	uint8_t invertedpremoves[MAXLEN];
	char *solution = *arg->nextsol;

	strl = writemoves(
	    arg->moves, arg->nmoves, arg->solutions_size, *arg->nextsol);

	if (strl < 0)
		goto solve_h48_appendsolution_thread_error;
	*arg->nextsol += strl-1;
	arg->solutions_size -= strl-1;

	if (arg->npremoves)
	{
		**arg->nextsol = ' ';
		(*arg->nextsol)++;
		arg->solutions_size--;

		invertmoves(arg->premoves, arg->npremoves, invertedpremoves);
		strl = writemoves(invertedpremoves,
		    arg->npremoves, arg->solutions_size, *arg->nextsol);

		if (strl < 0)
			goto solve_h48_appendsolution_thread_error;
		*arg->nextsol += strl-1;
		arg->solutions_size -= strl-1;
	}
	LOG("Solution found: %s\n", solution);

	**arg->nextsol = '\n';
	(*arg->nextsol)++;
	arg->solutions_size--;
	(*arg->nsols)++;

solve_h48_appendsolution_thread_error:
	/* We could add some logging, but writemoves() already does */
	pthread_mutex_unlock(&tq->mutex);
}

STATIC void
init_queue(task_queue_t *queue)
{
	queue->front = 0;
	queue->rear = 0;
	queue->tasks_count = 0;
	queue->active = 0;
	queue->terminate = ATOMIC_VAR_INIT(false);
	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);
	pthread_cond_init(&queue->active_cond, NULL);
}

STATIC void
submit_task(task_queue_t *queue, dfsarg_solveh48_t task)
{
	pthread_mutex_lock(&queue->mutex);
	queue->tasks[queue->rear] = task;
	queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
	queue->tasks_count++;
	pthread_cond_broadcast(&queue->cond);
	pthread_mutex_unlock(&queue->mutex);
}

STATIC void
copy_queue(task_queue_t *src, task_queue_t *dest, int depth, _Atomic int64_t *nsols)
{
	pthread_mutex_lock(&dest->mutex);
	for (int i = src->front; i != src->rear; i = (i + 1) % MAX_QUEUE_SIZE)
	{
			dest->tasks[i] = src->tasks[i];
			dest->tasks[i].depth = depth;
	}
	dest->front = src->front;
	dest->rear = src->rear;
	dest->tasks_count = src->tasks_count;
	pthread_cond_broadcast(&dest->cond);
	pthread_mutex_unlock(&dest->mutex);
}

STATIC void *
start_thread(void *arg)
{
	task_queue_t *queue = (task_queue_t *)arg;
	while (1) {
		pthread_mutex_lock(&queue->mutex);
		while (queue->tasks_count == 0 && !queue->terminate) {
			pthread_cond_wait(&queue->cond, &queue->mutex);
		}
		if (queue->tasks_count == 0 && queue->terminate) {
			pthread_mutex_unlock(&queue->mutex);
			break;
		}

		if (queue->tasks_count > 0) {
			dfsarg_solveh48_t task = queue->tasks[queue->front];
			queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
			queue->tasks_count--;
			queue->active++;
			pthread_mutex_unlock(&queue->mutex);
			
			solve_h48_single(&task, queue);

			pthread_mutex_lock(&queue->mutex);
			queue->active--;

			if(queue->tasks_count == 0 && queue->active == 0)
				pthread_cond_signal(&queue->active_cond);
		}
		pthread_mutex_unlock(&queue->mutex);
	}
	return NULL;
}

STATIC int64_t
solve_h48_bfs(dfsarg_solveh48_t *arg_zero, task_queue_t *tq, int8_t maxmoves)
{
	dfsarg_solveh48_t queue[MAX_QUEUE_SIZE];
	int front = 0, rear = 0;
	dfsarg_solveh48_t nextarg;
	int depth = 0;
	int nodes_at_current_depth = 1;
	int nodes_at_next_depth = 0;
	queue[rear++] = *arg_zero;

	dfsarg_solveh48_t task_pool[MAX_QUEUE_SIZE];

	while (front < rear){
		dfsarg_solveh48_t arg = queue[front++];
		nodes_at_current_depth--;

		if (*arg.nsols == arg.maxsolutions)
			return 1;

		if (issolved(arg.cube)){
			if (arg.nmoves + arg.npremoves >= arg.depth && arg.nmoves + arg.npremoves <= maxmoves)
			solve_h48_appendsolution(&arg);
			continue;
		}

		arg.nissbranch = MM_NORMAL;
		uint32_t allowed = allowednextmove_h48(arg.moves, arg.nmoves, arg.nissbranch);

		for (uint8_t m = 0; m < 18; m++){
			if (allowed & (1 << m)){
				nextarg = arg;
				nextarg.nmoves = arg.nmoves + 1;
				nextarg.moves[arg.nmoves] = m;
				nextarg.cube = move(arg.cube, m);
				nextarg.inverse = premove(arg.inverse, m);

				if (nextarg.nmoves == BFS_DEPTH){
					dfsarg_solveh48_t *task = &task_pool[rear % MAX_QUEUE_SIZE];
					*task = nextarg;
					submit_task(tq, *task);
				} else {
					queue[rear++] = nextarg;
					nodes_at_next_depth++;
				}
			}
		}
		if (nodes_at_current_depth == 0){
			nodes_at_current_depth = nodes_at_next_depth;
			nodes_at_next_depth = 0;
			LOG("Found %" PRId64 " solutions, searching at depth %" PRId8 "\n", *nextarg.nsols, depth++);
		}
		if (depth == BFS_DEPTH) return 0;
	}
	return 1;
}

STATIC int64_t
solve_h48_single(dfsarg_solveh48_t *arg, task_queue_t *tq)
{
	dfsarg_solveh48_t nextarg;
	int64_t ret;
	uint8_t m;

	if (*arg->nsols == arg->maxsolutions)
		return 0;

	if (solve_h48_stop(arg))
		return 0;

	if (issolved(arg->cube)){
		if (arg->nmoves + arg->npremoves != arg->depth)
			return 0;
		solve_h48_appendsolution_thread(arg, tq);
		return 1;
	}

	nextarg = *arg;
	ret = 0;
	uint32_t allowed;
	if (arg->nissbranch & MM_INVERSE){
		allowed = allowednextmove_h48(arg->premoves, arg->npremoves, arg->nissbranch);
		for (m = 0; m < 18; m++){
			if (allowed & (1 << m)){
				nextarg.npremoves = arg->npremoves + 1;
				nextarg.premoves[arg->npremoves] = m;
				nextarg.inverse = move(arg->inverse, m);
				nextarg.cube = premove(arg->cube, m);
				ret += solve_h48_single(&nextarg, tq);
			}
		}
	} else {
		allowed = allowednextmove_h48(arg->moves, arg->nmoves, arg->nissbranch);
		for (m = 0; m < 18; m++){
			if (allowed & (1 << m)){
				nextarg.nmoves = arg->nmoves + 1;
				nextarg.moves[arg->nmoves] = m;
				nextarg.cube = move(arg->cube, m);
				nextarg.inverse = premove(arg->inverse, m);
				ret += solve_h48_single(&nextarg, tq);
			}
		}
	}
	return ret;
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
	_Atomic int64_t nsols = 0;
	_Atomic long long nodes, fallbacks;
	int p_depth = 0;
	dfsarg_solveh48_t arg;
	tableinfo_t info, fbinfo;
	pthread_t threads[THREADS];

	if (readtableinfo_n(data_size, data, 2, &info) != NISSY_OK)
		goto solve_h48_multithread_error_data;

	nodes = fallbacks = 0;
	arg = (dfsarg_solveh48_t){
		.cube = cube,
		.inverse = inverse(cube),
		.nsols = &nsols,
		.depth = minmoves,
		.maxsolutions = maxsolutions,
		.h = info.h48h,
		.k = info.bits,
		.base = info.base,
		.cocsepdata = (uint32_t *)((char *)data + INFOSIZE),
		.h48data = (uint8_t *)data + COCSEP_FULLSIZE + INFOSIZE,
		.solutions_size = solutions_size,
		.nextsol = &solutions,
		.nodes_visited = &nodes,
		.table_fallbacks = &fallbacks
	};

	if (info.bits == 2) {
		if (readtableinfo_n(data_size, data, 3, &fbinfo) != NISSY_OK)
			goto solve_h48_multithread_error_data;
		/* We only support h0k4 as fallback table */
		if (fbinfo.h48h != 0 || fbinfo.bits != 4)
			goto solve_h48_multithread_error_data;
		arg.h48data_fallback = arg.h48data + info.next;
	} else {
		arg.h48data_fallback = NULL;
	}

	task_queue_t q;
	init_queue(&q);
	if (solve_h48_bfs(&arg, &q, maxmoves))
		return nsols;

	task_queue_t nq;
	init_queue(&nq);

	for (int i = 0; i < THREADS; i++) {
		pthread_create(&threads[i], NULL, &start_thread, &nq);
	}

	nsols = 0;
	for (p_depth = minmoves > BFS_DEPTH ? minmoves : BFS_DEPTH;
		 p_depth <= maxmoves && nsols < maxsolutions;
		 p_depth++)
	{
		LOG("Found %" PRId64 " solutions, "
		    "searching at depth %" PRId8 "\n", nsols, p_depth);
		copy_queue(&q, &nq, p_depth, &nsols);
		
		pthread_mutex_lock(&nq.mutex);
		while (nq.active > 0 || nq.tasks_count > 0)
			pthread_cond_wait(&nq.active_cond, &nq.mutex);
		pthread_mutex_unlock(&nq.mutex);
	}

	atomic_store(&nq.terminate, true);
	pthread_cond_broadcast(&nq.cond);

	for (int i = 0; i < THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	**arg.nextsol = '\0';

	stats[0] = nodes;
	stats[1] = fallbacks;
	LOG("Nodes visited: %lld\nTable fallbacks: %lld\n", nodes, fallbacks);

	return nsols;

solve_h48_multithread_error_data:
	LOG("solve_h48: error reading table\n");
	return NISSY_ERROR_DATA;
}
