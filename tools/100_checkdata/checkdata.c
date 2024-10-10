#include "../tool.h"
#include "../expected_distributions.h"

char *solver, *filename;

static void
run(void) {
	int64_t size, result;
	char *buf;
	FILE *f;
	
	size = nissy_datasize(solver);

	if (size <= 0) {
		fprintf(stderr, "Error in datasize\n");
		return;
	}

	if ((f = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Error reading file %s\n", filename);
		return;
	}

	buf = malloc(size);
	fread(buf, size, 1, f);
	fclose(f);
	result = nissy_checkdata(solver, buf);
	free(buf);

	printf("checkdata %s\n", result == 0 ? "succeeded" : "failed");

	/* TODO: cross-check with expected distributions? */
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Error: not enough arguments. "
		    "A solver and a file name must be given.\n");
		return 1;
	}

	solver = argv[1];
	filename = argv[2];
	nissy_setlogger(log_stderr);

	timerun(run);

	return 0;
}
