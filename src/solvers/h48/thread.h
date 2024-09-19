#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 500

typedef struct
{
	dfsarg_solveh48_t *tasks[MAX_QUEUE_SIZE];
	int front;
	int rear;
	int tasks_count;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	atomic_int active_tasks;
	pthread_mutex_t active_tasks_mutex;
	pthread_cond_t active_tasks_cond;
} task_queue_t;

STATIC void solve_h48_appendsolution_thread(dfsarg_solveh48_t *, task_queue_t *);
STATIC void init_queue(task_queue_t *);
STATIC void submit_task(task_queue_t *, dfsarg_solveh48_t *);
STATIC void copy_queue(task_queue_t *, task_queue_t *, int);
STATIC dfsarg_solveh48_t *get_task(task_queue_t *);
STATIC void *start_thread(void *);
STATIC int64_t solve_h48_bfs(dfsarg_solveh48_t *, task_queue_t *);
STATIC int64_t solve_h48_single(dfsarg_solveh48_t *, task_queue_t *);
STATIC int64_t solve_h48_parent(cube_t, int8_t, int8_t, int8_t, const void *, char *);

STATIC void
solve_h48_appendsolution_thread(dfsarg_solveh48_t *arg, task_queue_t *tq){
	pthread_mutex_lock(&tq->mutex);
	int strl;
	uint8_t invertedpremoves[MAXLEN];
	char *solution = *arg->nextsol;

	strl = writemoves(arg->moves, arg->nmoves, *arg->nextsol);
	*arg->nextsol += strl;

	if (arg->npremoves)
	{
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
	pthread_mutex_unlock(&tq->mutex);
}

STATIC void
init_queue(task_queue_t *queue)
{
	queue->front = 0;
	queue->rear = 0;
	queue->tasks_count = 0;
	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);
}

STATIC void
submit_task(task_queue_t *queue, dfsarg_solveh48_t *task)
{
	pthread_mutex_lock(&queue->mutex);
	queue->tasks[queue->rear] = task;
	queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
	queue->tasks_count++;
	pthread_cond_signal(&queue->cond);
	pthread_mutex_unlock(&queue->mutex);
}

STATIC void
copy_queue(task_queue_t *src, task_queue_t *dest, int depth)
{
	pthread_mutex_lock(&src->mutex);
	pthread_mutex_lock(&dest->mutex);
	for (int i = src->front; i != src->rear; i = (i + 1) % MAX_QUEUE_SIZE)
	{
		if (src->tasks[i] != NULL)
		{
			dest->tasks[i] = src->tasks[i];
			dest->tasks[i]->depth = depth;
		}
	}
	dest->front = src->front;
	dest->rear = src->rear;
	dest->tasks_count = src->tasks_count;
	pthread_mutex_unlock(&src->mutex);
	pthread_cond_signal(&dest->cond);
	pthread_mutex_unlock(&dest->mutex);
}
STATIC dfsarg_solveh48_t *
get_task(task_queue_t *queue)
{
	pthread_mutex_lock(&queue->mutex);
	while (queue->tasks_count == 0)
	{
		pthread_cond_wait(&queue->cond, &queue->mutex);
	}
	dfsarg_solveh48_t *task = queue->tasks[queue->front];
	queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
	queue->tasks_count--;
	pthread_mutex_unlock(&queue->mutex);
	return task;
}

STATIC void *
start_thread(void *arg)
{
	task_queue_t *queue = (task_queue_t *)arg;
	while (true)
	{
		dfsarg_solveh48_t *task = get_task(queue);
		if (task == NULL)
		{
			break;
		}
		solve_h48_single(task, queue);
		free(task);

		pthread_mutex_lock(&queue->active_tasks_mutex);
		atomic_fetch_sub(&queue->active_tasks, 1);
		if (atomic_load(&queue->active_tasks) == 0)
		{
			pthread_cond_signal(&queue->active_tasks_cond);
		}
		pthread_mutex_unlock(&queue->active_tasks_mutex);
	}
	return NULL;
}

STATIC int64_t
solve_h48_bfs(dfsarg_solveh48_t *arg_zero, task_queue_t *tq)
{
	dfsarg_solveh48_t queue[MAX_QUEUE_SIZE];
	int front = 0, rear = 0;
	dfsarg_solveh48_t nextarg;
	int depth = 0;
	int nodes_at_current_depth = 1;
	int nodes_at_next_depth = 0;

	queue[rear++] = *arg_zero;

	while (front < rear)
	{
		dfsarg_solveh48_t arg = queue[front++];
		nodes_at_current_depth--;

		if (*arg.nsols == arg.maxsolutions)
			return 1;

		if (issolved(arg.cube))
		{
			if (arg.nmoves + arg.npremoves != arg.depth)
				continue;
			solve_h48_appendsolution(&arg);
			continue;
		}

		arg.nissbranch = MM_NORMAL;
		uint32_t allowed = allowednextmove_h48(arg.moves, arg.nmoves, arg.nissbranch);

		for (uint8_t m = 0; m < 18; m++)
		{
			if (allowed & (1 << m))
			{
				nextarg = arg;
				nextarg.nmoves = arg.nmoves + 1;
				nextarg.moves[arg.nmoves] = m;
				nextarg.cube = move(arg.cube, m);
				nextarg.inverse = premove(arg.inverse, m);

				if (nextarg.nmoves == 2)
				{
					dfsarg_solveh48_t *task = malloc(sizeof(dfsarg_solveh48_t));
					*task = nextarg;
					submit_task(tq, task);
				}
				else
				{
					queue[rear++] = nextarg;
					nodes_at_next_depth++;
				}
			}
		}

		if (nodes_at_current_depth == 0)
		{
			depth++;
			nodes_at_current_depth = nodes_at_next_depth;
			nodes_at_next_depth = 0;
		}
		if (depth == 2)
		{
			return 0;
		}
	}
	return 0;
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

	if (issolved(arg->cube))
	{
		if (arg->nmoves + arg->npremoves != arg->depth)
			return 0;
		solve_h48_appendsolution_thread(arg, tq);
		return 1;
	}

	nextarg = *arg;
	ret = 0;
	uint32_t allowed;
	if (arg->nissbranch & MM_INVERSE)
	{
		allowed = allowednextmove_h48(arg->premoves, arg->npremoves, arg->nissbranch);
		for (m = 0; m < 18; m++)
		{
			if (allowed & (1 << m))
			{
				nextarg.npremoves = arg->npremoves + 1;
				nextarg.premoves[arg->npremoves] = m;
				nextarg.inverse = move(arg->inverse, m);
				nextarg.cube = premove(arg->cube, m);
				ret += solve_h48_single(&nextarg, tq);
			}
		}
	}
	else
	{
		allowed = allowednextmove_h48(arg->moves, arg->nmoves, arg->nissbranch);
		for (m = 0; m < 18; m++)
		{
			if (allowed & (1 << m))
			{
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
solve_h48_parent(
	cube_t cube,
	int8_t minmoves,
	int8_t maxmoves,
	int8_t maxsolutions,
	const void *data,
	char *solutions)
{
	int64_t nsols;
	int p_depth = 0;
	dfsarg_solveh48_t bfs_arg;
	tableinfo_t info;
	pthread_t threads[THREADS];

	if (!readtableinfo_n(data, 2, &info))
	{
		LOG("solve_h48: error reading table\n");
		return 0;
	}

	bfs_arg = (dfsarg_solveh48_t){
		.cube = cube,
		.inverse = inverse(cube),
		.nsols = &nsols,
		.maxsolutions = maxsolutions,
		.h = info.h48h,
		.k = info.bits,
		.cocsepdata = get_cocsepdata_ptr(data),
		.h48data = get_h48data_ptr(data),
		.nextsol = &solutions};

	task_queue_t q;
	init_queue(&q);
	if (solve_h48_bfs(&bfs_arg, &q))
		return nsols;

	task_queue_t nq;
	init_queue(&nq);

	atomic_store(&nq.active_tasks, 0);
	pthread_mutex_init(&nq.active_tasks_mutex, NULL);
	pthread_cond_init(&nq.active_tasks_cond, NULL);

	for (int i = 0; i < THREADS; i++)
	{
		pthread_create(&threads[i], NULL, &start_thread, &nq);
	}

	nsols = 0;
	for (p_depth = minmoves > 2 ? minmoves : 2;
		 p_depth <= maxmoves && nsols < maxsolutions;
		 p_depth++)
	{
		LOG("Found %" PRId64 " solutions, searching at depth %" PRId8 "\n", nsols, p_depth);
		copy_queue(&q, &nq, p_depth);
		pthread_mutex_lock(&nq.active_tasks_mutex);
		while (nq.active_tasks > 0)
		{
			pthread_cond_wait(&nq.active_tasks_cond, &nq.active_tasks_mutex);
		}
		pthread_mutex_unlock(&nq.active_tasks_mutex);
	}
	for (int i = 0; i < THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	return nsols;
}
