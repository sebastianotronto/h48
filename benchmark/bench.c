#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../cube.h"

#define MOVES     100000000
#define TRANS     100000000
#define COMPOSE   100000000
#define INVERSE   100000000

double
bench(cube_t (*run)(int64_t), int64_t n, char *name)
{
	char str[1000];
	cube_t cube;
	struct timespec start, end;
	double tdiff, tdsec, tdnano;

	printf("\n");
	fflush(stdout);

	if (run == NULL) {
		printf("> %s: nothing to run!\n", name);
		fflush(stdout);
		return -1.0;
	}

	printf("> %s: running benchmark...\n", name);
	fflush(stdout);
	clock_gettime(CLOCK_MONOTONIC, &start);

	cube = run(n);
	writecube("H48", cube, str);
	str[3] = 0;
	printf("> %s: resulting cube, first piece: %s\n", name, str);
	fflush(stdout);

	clock_gettime(CLOCK_MONOTONIC, &end);
	tdsec = end.tv_sec - start.tv_sec;
	tdnano = end.tv_nsec - start.tv_nsec;
	tdiff = tdsec + 1e-9 * tdnano;
	printf("> %s: %.4fs\n", name, tdiff);
	fflush(stdout);

	return tdiff;
}

int main() {
	double tmoves, ttrans, tcompose, tinverse;

	printf(
		"Benchmarks settings:\n"
		"MOVES:\t%d\nTRANS:\t%d\nCOMPOSE:\t%d\nINVERSE:\t%d\n",
		MOVES, TRANS, COMPOSE, INVERSE
	);
	fflush(stdout);

	srand(time(NULL));

	tmoves = bench(run_moves, MOVES, "moves");
	ttrans = bench(run_trans, TRANS, "trans");
	tcompose = bench(run_compose, COMPOSE, "compose");
	tinverse = bench(run_inverse, INVERSE, "inverse");

	printf(
		"\nBenchmark summary:\n"
		"moves:   %d moves            in %.4fs (%.4f MTPS)\n"
		"trans:   %d transformations  in %.4fs (%.4f MTPS)\n"
		"compose: %d compositions     in %.4fs (%.4f MCPS)\n"
		"inverse: %d inverses         in %.4fs (%.4f MIPS)\n"
		"Total time: %.4f\n",
		MOVES, tmoves, MOVES / (1e6 * tmoves),
		TRANS, ttrans, TRANS / (1e6 * ttrans),
		COMPOSE, tcompose, COMPOSE / (1e6 * tcompose),
		INVERSE, tinverse, INVERSE / (1e6 * tinverse),
		tmoves + ttrans + tcompose + tinverse
	);

	return 0;
}
