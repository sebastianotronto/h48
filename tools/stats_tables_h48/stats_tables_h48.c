#include <stdarg.h>
#include <time.h>
#include "../timerun.h"
#include "../../src/cube.h"

#define MAXMOVES 20
#define NCUBES 10000
#define LOG_EVERY (NCUBES / 20)

const char *filename = "tables/h48h0k4";
char *buf;

uint64_t rand64(void) {
	uint64_t i, ret;

	for (i = 0, ret = 0; i < 64; i++)
		ret |= (uint64_t)(rand() % 2) << i;

	return ret;
}

void log_stderr(const char *str, ...) {
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
}

void run(void) {
	int i, j, k;
	char sols[12], cube[22];
	int64_t ep, eo, cp, co, v[12][100] = {0};

	for (i = 0; i < NCUBES; i++) {
		ep = rand64();
		eo = rand64();
		cp = rand64();
		co = rand64();
		nissy_getcube(ep, eo, cp, co, "fix", cube);
		nissy_solve(cube, "h48stats", "", "",
		    0, MAXMOVES, 1, -1, buf, sols);
		for (j = 0; j < 12; j++)
			v[j][(int)sols[j]]++;
		if ((i+1) % LOG_EVERY == 0)
			fprintf(stderr, "%d cubes solved...\n", i+1);
	}

	for (j = 0; j < 12; j++) {
		printf("Data for h=%d\n", j);
		for (k = 0; k <= 16; k++)
			printf("%d\t%" PRId64 "\n", k, v[j][k]);
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

int main() {
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
