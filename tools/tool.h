#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/nissy.h"
#include "nissy_extra.h"

static void log_stderr(const char *, void *);
static double timerun(void (*)(void));
static void writetable(const char *, int64_t, const char *);
static long long int generatetable(const char *, char **,
    char [static NISSY_SIZE_DATAID]);
static long long int derivetable(
    const char *, const char *, const char *, char **);
static int getdata(const char *, char **, const char *);
static void gendata_run(const char *, uint64_t[static 21]);
static void derivedata_run(
    const char *, const char *, const char *, const char *);

static void
log_stderr(const char *str, void *unused)
{
	fprintf(stderr, "%s", str);
}

static double
timerun(void (*run)(void))
{
	struct timespec start, end;
	double tdiff, tdsec, tdnano;

	fflush(stdout);

	if (run == NULL) {
		printf("nothing to run!\n");
		fflush(stdout);
		return -1.0;
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	run();
	clock_gettime(CLOCK_MONOTONIC, &end);

	tdsec = end.tv_sec - start.tv_sec;
	tdnano = end.tv_nsec - start.tv_nsec;
	tdiff = tdsec + 1e-9 * tdnano;

	printf("---------\n");
	printf("\nTotal time: %.4fs\n", tdiff);
	fflush(stdout);

	return tdiff;
}

static void
writetable(const char *buf, int64_t size, const char *filename)
{
	FILE *f;

	if ((f = fopen(filename, "wb")) == NULL) {
		printf("Could not write tables to file %s"
		    ", will be regenerated next time.\n", filename);
	} else {
		fwrite(buf, size, 1, f);
		fclose(f);
		printf("Table written to %s.\n", filename);
	}
}

static long long int
generatetable(
	const char *solver,
	char **buf,
	char dataid[static NISSY_SIZE_DATAID]
)
{
	long long int size, gensize;

	size = nissy_solverinfo(solver, dataid);
	if (size < 0) {
		printf("Error getting table size.\n");
		return -1;
	}

	*buf = malloc(size);
	gensize = nissy_gendata(solver, size, *buf);

	if (gensize != size) {
		printf("Error generating table");
		if (gensize == NISSY_OK)
			printf(" (got %lld bytes)", gensize);
		printf("\n");
		return -2;
	}

	return gensize;
}

static long long int
derivetable(
	const char *solver_large,
	const char *solver_small,
	const char *filename_large,
	char **buf
)
{
	uint8_t h, k;
	long long int size, gensize;
	char *fulltable, dataid[NISSY_SIZE_DATAID];

	if (getdata(solver_large, &fulltable, filename_large) != 0) {
		printf("Error reading full table.\n");
		gensize = -1;
		goto derivetable_error_nofree;
	}

	size = nissy_solverinfo(solver_small, dataid);
	if (size == -1) {
		printf("Error getting table size.\n");
		gensize = -2;
		goto derivetable_error;
	}

	if (parse_h48_solver(solver_small, &h, &k) != 0) {
		gensize = -3;
		goto derivetable_error;
	}

	*buf = malloc(size);
	gensize = gendata_h48_derive(h, fulltable, *buf);

	if (gensize != size) {
		printf("Error deriving table\n");
		gensize = -4;
		goto derivetable_error;
	}

derivetable_error:
	free(fulltable);

derivetable_error_nofree:
	return gensize;
}

static int
getdata(
	const char *solver,
	char **buf,
	const char *filename
) {
	long long int size, sizeread;
	FILE *f;
	char dataid[NISSY_SIZE_DATAID];

	if ((f = fopen(filename, "rb")) == NULL) {
		printf("Table file not found, generating it.\n");
		size = generatetable(solver, buf, dataid);
		switch (size) {
		case -1:
			goto getdata_error_nofree;
		case -2:
			goto getdata_error;
		default:
			writetable(*buf, size, filename);
			break;
		}
	} else {
		printf("Reading tables from file %s\n", filename);
		size = nissy_solverinfo(solver, dataid);
		*buf = malloc(size);
		sizeread = fread(*buf, size, 1, f);
		fclose(f);
		if (sizeread != 1) {
			printf("Error reading table, stopping\n");
			goto getdata_error;
		}
	}

	return 0;

getdata_error:
	free(*buf);
getdata_error_nofree:
	return 1;
}

static void
gendata_run(
	const char *solver,
	uint64_t expected[static 21]
) {
	long long int size;
	char *buf, filename[1024], dataid[NISSY_SIZE_DATAID];

	size = generatetable(solver, &buf, dataid);
	sprintf(filename, "tables/%s", dataid);
	switch (size) {
	case -1:
		return;
	case -2:
		goto gendata_run_finish;
	default:
		nissy_datainfo(size, buf);
		printf("\n");
		printf("Succesfully generated %lld bytes. "
		       "See above for details on the tables.\n", size);

		/* TODO: check that the table is correct */
		writetable(buf, size, filename);
		break;
	}

gendata_run_finish:
	free(buf);
}

static void
derivedata_run(
	const char *solver_large,
	const char *solver_small,
	const char *filename_large,
	const char *filename_small
)
{
	long long int size;
	char *buf;

	buf = NULL;
	size = derivetable(solver_large, solver_small, filename_large, &buf);
	switch (size) {
	case -1:
		return;
	case -2:
		goto derivedata_run_finish;
	default:
		nissy_datainfo(size, buf);
		printf("\n");
		printf("Succesfully generated %lld bytes. "
		       "See above for details on the tables.\n", size);

		writetable(buf, size, filename_small);
		break;
	}

derivedata_run_finish:
	free(buf);
}
