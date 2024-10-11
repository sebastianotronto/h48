#include <pthread.h>

#include "../tool.h"

#define MAXMOVES 20
#define LOG_EVERY (NCUBES_PER_THREAD / 10)

int NCUBES_PER_THREAD;
int64_t size = 0;
const char *solver = "h48stats";
const char *filename = "tables/h48h0k4";
char *buf;

typedef struct {
	int n;
	int thread_id;
	int64_t v[12][100];
} thread_arg_t;

uint64_t rand64(void) {
	uint64_t i, ret;

	for (i = 0, ret = 0; i < 64; i++)
		ret |= (uint64_t)(rand() % 2) << i;

	return ret;
}

static void *
run_thread(void *arg)
{
	char s[12], cube[22];
	int64_t ep, eo, cp, co;
	int i, j;

	thread_arg_t *a = (thread_arg_t *)arg;

	for (i = 0; i < a->n; i++) {
		ep = rand64();
		eo = rand64();
		cp = rand64();
		co = rand64();
		nissy_getcube(ep, eo, cp, co, "fix", cube);
		nissy_solve(cube, "h48stats", NISSY_NISSFLAG_NORMAL,
		    0, MAXMOVES, 1, -1, size, buf, 12, s);
		for (j = 0; j < 12; j++)
			a->v[j][(int)s[j]]++;
		if ((i+1) % LOG_EVERY == 0)
			printf("[thread %d] %d cubes solved...\n",
			    a->thread_id, i+1);
	}

	return NULL;
}

void run(void) {
	int64_t i, j, k, tot;
	double avg;
	pthread_t thread[THREADS];
	thread_arg_t arg[THREADS];

	size = nissy_datasize(solver);

	for (i = 0; i < THREADS; i++) {
		arg[i] = (thread_arg_t) {
			.thread_id = i,
			.n = NCUBES_PER_THREAD,
			.v = {{0}}
		};
		pthread_create(&thread[i], NULL, run_thread, &arg[i]);
	}

	for (i = 0; i < THREADS; i++)
		pthread_join(thread[i], NULL);

	for (j = 0; j < 12; j++) {
		printf("Data for h=%" PRId64 "\n", j);
		for (k = 0, avg = 0.0; k < 16; k++) {
			for (i = 0, tot = 0; i < THREADS; i++)
				tot += arg[i].v[j][k];
			printf("%" PRId64 "\t%" PRId64 "\n", k, tot);
			avg += tot * k;
		}
		avg /= (double)(NCUBES_PER_THREAD * THREADS);
		printf("Average: %.4lf\n", avg);
		printf("\n");
	}
}

int main(int argc, char **argv) {
	srand(time(NULL));
	nissy_setlogger(log_stderr);

	if (argc < 2) {
		printf("Error: not enough arguments. "
		    "Number of cubes per thread must be provided.\n");
		return 1;
	}

	NCUBES_PER_THREAD = atoi(argv[1]);

	if (NCUBES_PER_THREAD < 1 || NCUBES_PER_THREAD > 1000000) {
		printf("Invalid number of cubes: must be > 0 and "
		    "< 1000000, but %d was given.\n", NCUBES_PER_THREAD);
		return 1;
	}

	printf("Using %d cubes per thread (%d total)\n",
	    NCUBES_PER_THREAD, NCUBES_PER_THREAD * THREADS);
	if (getdata(solver, &buf, filename) != 0)
		return 1;

	timerun(run);

	free(buf);
	return 0;
}
