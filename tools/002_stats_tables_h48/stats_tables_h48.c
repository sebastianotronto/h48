#include <pthread.h>
#include <time.h>
#include "../timerun.h"
#include "../../src/nissy.h"

#define MAXMOVES 20
#define NTHREADS 32
#define NCUBES_PER_THREAD 10000
#define LOG_EVERY (NCUBES_PER_THREAD / 10)

typedef struct {
	int n;
	int thread_id;
	int64_t v[12][100];
} thread_arg_t;

const char *filename = "tables/h48h0k4";
char *buf;

uint64_t rand64(void) {
	uint64_t i, ret;

	for (i = 0, ret = 0; i < 64; i++)
		ret |= (uint64_t)(rand() % 2) << i;

	return ret;
}

static void *
run_thread(void *arg)
{
	char sols[12], cube[22];
	int64_t ep, eo, cp, co;
	int i, j;

	thread_arg_t *a = (thread_arg_t *)arg;

	for (i = 0; i < a->n; i++) {
		ep = rand64();
		eo = rand64();
		cp = rand64();
		co = rand64();
		nissy_getcube(ep, eo, cp, co, "fix", cube);
		nissy_solve(cube, "h48stats", "", "",
		    0, MAXMOVES, 1, -1, buf, sols);
		for (j = 0; j < 12; j++)
			a->v[j][(int)sols[j]]++;
		if ((i+1) % LOG_EVERY == 0)
			fprintf(stderr, "[thread %d] %d cubes solved...\n",
			    a->thread_id, i+1);
	}

	return NULL;
}

void run(void) {
	int64_t i, j, k, tot;
	double avg;
	pthread_t thread[NTHREADS];
	thread_arg_t arg[NTHREADS];

	for (i = 0; i < NTHREADS; i++) {
		arg[i] = (thread_arg_t) {
			.thread_id = i,
			.n = NCUBES_PER_THREAD,
			.v = {{0}}
		};
		pthread_create(&thread[i], NULL, run_thread, &arg[i]);
	}

	for (i = 0; i < NTHREADS; i++)
		pthread_join(thread[i], NULL);

	for (j = 0; j < 12; j++) {
		printf("Data for h=%" PRId64 "\n", j);
		for (k = 0, avg = 0.0; k <= 16; k++) {
			for (i = 0, tot = 0; i < NTHREADS; i++)
				tot += arg[i].v[j][k];
			printf("%" PRId64 "\t%" PRId64 "\n", k, tot);
			avg += tot * k;
		}
		avg /= (double)(NCUBES_PER_THREAD * NTHREADS);
		printf("Average: %.4lf\n", avg);
		printf("\n");
	}
}

int getdata(int64_t size) {
	int64_t s;
	FILE *f;

	buf = malloc(size);

	if ((f = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Table file not found, generating them."
		    " This can take a while.\n");
		s = nissy_gendata("h48stats", "", buf);
		if (s != size) {
			fprintf(stderr, "Error generating table");
			if (s != -1)
				fprintf(stderr, " (got %" PRId64 " bytes)", s);
			fprintf(stderr, "\n");
			return 1;
		}
		if ((f = fopen(filename, "wb")) == NULL) {
			fprintf(stderr, "Could not write tables to file %s"
			    ", will be regenerated next time.\n", filename);
		} else {
			fwrite(buf, size, 1, f);
			fclose(f);
		}
	} else {
		fprintf(stderr, "Reading tables from file %s\n", filename);
		fread(buf, size, 1, f);
		fclose(f);
	}

	return 0;
}

int main(void) {
	int64_t size;

	srand(time(NULL));

	nissy_setlogger(log_stderr);
	size = nissy_datasize("h48stats", "");
	if (size == -1) {
		printf("h48 stats: error in datasize\n");
		return 1;
	}

	if (getdata(size) != 0) {
		printf("Error getting table, stopping\n");
		free(buf);
		return 1;
	}

	timerun(run, "h48 table stats");

	free(buf);
	return 0;
}
