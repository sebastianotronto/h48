#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/nissy.h"

static void log_stderr(const char *, ...);
static void log_stdout(const char *, ...);
static double timerun(void (*)(void), const char *);
static void writetable(const char *, int64_t, const char *);
static int64_t generatetable(const char *, const char *, char **);
static int getdata(const char *, const char *, char **, const char *);
static void gendata_run(const char *, const char *, const char *, uint64_t[static 21]);

static void
log_stderr(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
}

static void
write_stdout(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stdout, str, args);
	va_end(args);
}

static double
timerun(void (*run)(void), const char *name)
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

static void
writetable(const char *buf, int64_t size, const char *filename)
{
	FILE *f;

	if ((f = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "Could not write tables to file %s"
		    ", will be regenerated next time.\n", filename);
	} else {
		fwrite(buf, size, 1, f);
		fclose(f);
		fprintf(stderr, "Table written to %s.\n", filename);
	}
}

static int64_t
generatetable(const char *solver, const char *options, char **buf)
{
	int64_t size, gensize;

	size = nissy_datasize(solver, options);
	if (size == -1) {
		printf("Error getting table size.\n");
		return -1;
	}

	*buf = malloc(size);
	gensize = nissy_gendata(solver, options, *buf);

	if (gensize != size) {
		fprintf(stderr, "Error generating table");
		if (gensize != -1)
			fprintf(stderr, " (got %" PRId64 " bytes)", gensize);
		fprintf(stderr, "\n");
		return -2;
	}

	return gensize;
}

static int
getdata(
	const char *solver,
	const char *options,
	char **buf,
	const char *filename
) {
	int64_t size, sizeread;
	FILE *f;

	if ((f = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Table file not found, generating it.\n");
		size = generatetable(solver, options, buf);
		switch (size) {
		case -1:
			goto getdata_error_nofree;
		case -2:
			goto getdata_error;
		default:
			writetable(filename, size, *buf);
			break;
		}
	} else {
		fprintf(stderr, "Reading tables from file %s\n", filename);
		size = nissy_datasize(solver, options);
		*buf = malloc(size);
		sizeread = fread(*buf, size, 1, f);
		fclose(f);
		if (sizeread != 1) {
			fprintf(stderr, "Error reading table, stopping\n");
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
	const char *options,
	const char *filename, /* TODO: remove filename, use solver name */
	uint64_t expected[static 21]
) {
	int64_t size;
	char *buf;

	
	size = generatetable(solver, options, &buf);
	switch (size) {
	case -1:
		return;
	case -2:
		goto gendata_run_finish;
	default:
		nissy_datainfo(buf, write_stdout);
		printf("\n");
		printf("Succesfully generated %" PRId64 " bytes. "
		       "See above for details on the tables.\n", size);

		/* TODO: check that the table is correct */
		writetable(buf, size, filename);
		break;
	}

gendata_run_finish:
	free(buf);
}
