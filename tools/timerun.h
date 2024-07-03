#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double
timerun(void (*run)(void), char *name)
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

	printf("Running tool: %s\n", name);
	printf("==========\n");
	fflush(stdout);

	clock_gettime(CLOCK_MONOTONIC, &start);
	run();
	clock_gettime(CLOCK_MONOTONIC, &end);

	tdsec = end.tv_sec - start.tv_sec;
	tdnano = end.tv_nsec - start.tv_nsec;
	tdiff = tdsec + 1e-9 * tdnano;

	printf("==========\n");
	printf("\nTotal time: %.4fs\n", tdiff);
	fflush(stdout);

	return tdiff;
}
