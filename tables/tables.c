#define _POSIX_C_SOURCE 200809L /* Required to use clock_gettime */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/cube.h"

#define SIZE_H1 118687270

char buf[SIZE_H1];

int main(int argc, char *argv[]) {
	FILE *f;
	struct timespec t0, t1;
	double s;

	clock_gettime(CLOCK_MONOTONIC, &t0);
	gendata("H48", argc > 1 ? argv[1] : "4", buf);
	clock_gettime(CLOCK_MONOTONIC, &t1);

	if ((f = fopen("tables/data/h48_test", "wb")) == NULL) {
		printf("Could not open file!\n");
		return 1;
	}
	fwrite(buf, SIZE_H1, 1, f);

	s = t1.tv_sec - t0.tv_sec + (t1.tv_nsec - t0.tv_nsec) / 1000000000.0;
	printf("Data generated in %lfs\n", s);

	return 0;
}
