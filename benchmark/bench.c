#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../cube.h"

#define MOVES 100000000
#define TRANS 100000000
#define COMP  100000000
#define INV   100000000

#define GEN_MOVES 10000
#define GEN_TRANS 10000
#define GEN_CUBES 10000

static move_t gen_moves[GEN_MOVES];
static trans_t gen_trans[GEN_TRANS];
static cube_t gen_cubes[GEN_CUBES];

void setup_moves(void);
void setup_trans(void);
void setup_cube(void);
void run_moves(void);
void run_trans(void);
void run_comp(void);
void run_inv(void);
double bench(void (*)(void), void (*)(void), char *);

void
setup_moves(void)
{
	int i;

	for (i = 0; i < GEN_MOVES; i++)
		gen_moves[i] = (move_t)rand() % 18;
}

void
setup_trans(void)
{
	int i;

	for (i = 0; i < GEN_TRANS; i++)
		gen_trans[i] = (trans_t)rand() % 48;
}

void
setup_cubes(void)
{
	int i, j;
	move_t m;

	for (i = 0; i < GEN_CUBES; i++) {
		gen_cubes[i] = solvedcube();
		for (j = 0; j < 30; j++) {
			m = (move_t)rand() % 18;
			gen_cubes[i] = move(gen_cubes[i], m);
		}
	}
}

void
run_moves(void)
{
	int i;
	cube_t c;
	char str[1000];

	c = solvedcube();
	for (i = 0; i < MOVES; i++)
		c = move(c, gen_moves[i % GEN_MOVES]);

	writecube(H48, c, str);
	str[3] = 0;
	printf("> moves: resulting cube, first piece: %s\n", str);
	fflush(stdout);
}

void
run_trans(void)
{
	int i;
	cube_t c;
	char str[1000];

	c = solvedcube();
	for (i = 0; i < TRANS; i++)
		c = transform(c, gen_trans[i % GEN_TRANS]);

	writecube(H48, c, str);
	str[3] = 0;
	printf("> trans: resulting cube, first piece: %s\n", str);
	fflush(stdout);
}

void
run_comp(void)
{
	int i;
	cube_t c;
	char str[1000];

	c = solvedcube();
	for (i = 0; i < COMP; i++)
		c = compose(c, gen_cubes[i % GEN_CUBES]);

	writecube(H48, c, str);
	str[3] = 0;
	printf("> comp: resulting cube, first piece: %s\n", str);
	fflush(stdout);
}

void
run_inv(void)
{
	int i, j;
	char str[1000];

	for (i = 0; i < COMP; i++) {
		j = i % (GEN_CUBES-1);
		gen_cubes[j] = inverse(gen_cubes[j+1]);
	}

	writecube(H48, gen_cubes[0], str);
	str[3] = 0;
	printf("> comp: resulting cube, first piece: %s\n", str);
	fflush(stdout);
}

double
bench(void (*run)(void), void (*setup)(void), char *name)
{
	struct timespec start, end;
	double tdiff, tdsec, tdnano;

	printf("\n");
	fflush(stdout);

	if (run == NULL) {
		printf("> %s: nothing to run!\n", name);
		fflush(stdout);
		return -1.0;
	}

	printf("> %s: setting up benchmark...\n", name);
	fflush(stdout);
	if (setup != NULL)
		setup();
	printf("> %s: running benchmark...\n", name);
	fflush(stdout);
	clock_gettime(CLOCK_MONOTONIC, &start);
	run();
	clock_gettime(CLOCK_MONOTONIC, &end);
	tdsec = end.tv_sec - start.tv_sec;
	tdnano = end.tv_nsec - start.tv_nsec;
	tdiff = tdsec + 1e-9 * tdnano;
	printf("> %s: %.4fs\n", name, tdiff);
	fflush(stdout);

	return tdiff;
}

int main() {
	double tmoves, ttrans, tcomp, tinv;

	printf(
	    "Benchmarks settings:\n"
	    "MOVES:\t%d\nTRANS:\t%d\nCOMP:\t%d\nINV:\t%d\n",
	    MOVES, TRANS, COMP, INV
	);
	fflush(stdout);

	srand(time(NULL));

	tmoves = bench(run_moves, setup_moves, "moves");
	ttrans = bench(run_trans, setup_trans, "trans");
	tcomp = bench(run_comp, setup_cubes, "comp");
	tinv = bench(run_inv, setup_cubes, "inv");

	printf(
	    "\nBenchmark summary:\n"
	    "moves: %d moves in %.4fs (%.4f MTPS)\n"
	    "trans: %d trans in %.4fs (%.4f MTPS)\n"
	    "comp:  %d comps in %.4fs (%.4f MCPS)\n"
	    "inv:   %d invs in  %.4fs (%.4f MIPS)\n"
	    "Total time: %.4f\n",
	    MOVES, tmoves, MOVES / (1e6 * tmoves),
	    TRANS, ttrans, TRANS / (1e6 * ttrans),
	    COMP, tcomp, COMP / (1e6 * tcomp),
	    INV, tinv, INV / (1e6 * tinv),
	    tmoves + ttrans + tcomp + tinv
	);

	return 0;
}
