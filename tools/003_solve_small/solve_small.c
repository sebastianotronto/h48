#include <pthread.h>
#include <time.h>
#include "../timerun.h"
#include "../../src/nissy.h"

#define OPTIONS "0;4;20"

const char *filename = "tables/h48h0k4";
char *buf;
char *scrambles[] = {
	"R D' R2 D R U2 R' D' R U2 R D R'", /* 12 optimal */
	"RLUD RLUD RLUD", /* 12 optimal */
	NULL
};

void run(void) {
	int i;
	int64_t n;
	char sol[100], cube[22];

	printf("Solved the following scrambles:\n\n");
	for (i = 0; scrambles[i] != NULL; i++) {
		printf("%s\n", scrambles[i]);
		fprintf(stderr, "Solving scramble %s\n", scrambles[i]);
		if (nissy_frommoves(scrambles[i], cube) == -1) {
			fprintf(stderr, "Invalid scramble, "
			    "continuing with next scramble\n");
			continue;
		}
		n = nissy_solve(
		    cube, "h48", OPTIONS, "", 0, 20, 1, -1, buf, sol);
		if (n == 0)
			fprintf(stderr, "No solution found, "
			    "continuing with next scramble\n");
	}
	printf("\n");
}

int getdata(int64_t size) {
	int64_t s;
	FILE *f;

	buf = malloc(size);

	if ((f = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Table file not found, generating them."
		    " This can take a while.\n");
		s = nissy_gendata("h48", OPTIONS, buf);
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
	size = nissy_datasize("h48", OPTIONS);
	if (size == -1) {
		printf("h48 stats: error in datasize\n");
		return 1;
	}

	if (getdata(size) != 0) {
		printf("Error getting table, stopping\n");
		free(buf);
		return 1;
	}

	timerun(run, "small solver benchmark");

	free(buf);
	return 0;
}
