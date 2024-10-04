#include "../tool.h"
#include "../expected_distributions.h"

char *solver, *options, *filename;

static void
run(void) {
	int64_t size;
	char *buf;
	FILE *f;
	
	size = nissy_datasize(solver, options);

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
	nissy_checkdata(solver, options, buf);
	free(buf);

	/* TODO: cross-check with expected distributions? */
}

int main(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "Error: not enough arguments. "
		    "A solver, its options and a file name must be given.\n");
		return 1;
	}

	solver = argv[1];
	options = argv[2];
	filename = argv[3];
	nissy_setlogger(log_stderr);

	timerun(run);

	return 0;
}
