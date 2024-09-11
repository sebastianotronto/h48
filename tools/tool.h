#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/nissy.h"

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

static int
getdata(
	const char *solver,
	const char *options,
	char **buf,
	const char *filename
) {
	int64_t s, size, sizeread;
	FILE *f;

	if ((size = nissy_datasize(solver, options)) == -1) {
		printf("Error in datasize\n");
		goto getdata_error_nofree;
	}

	*buf = malloc(size);

	if ((f = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Table file not found, generating them."
		    " This can take a while.\n");
		s = nissy_gendata(solver, options, *buf);
		if (s != size) {
			fprintf(stderr, "Error generating table");
			if (s != -1)
				fprintf(stderr, " (got %" PRId64 " bytes)", s);
			fprintf(stderr, "\n");
			goto getdata_error;
		}
		if ((f = fopen(filename, "wb")) == NULL) {
			fprintf(stderr, "Could not write tables to file %s"
			    ", will be regenerated next time.\n", filename);
		} else {
			fwrite(*buf, size, 1, f);
			fclose(f);
		}
	} else {
		fprintf(stderr, "Reading tables from file %s\n", filename);
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
