#include "../tool.h"
#include "../expected_distributions.h"

char *solver, *options;
uint64_t *expected;

static void
run(void) {
	int64_t size;
	char *buf, filename[1024];
	
	getfilename(solver, options, filename);
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

		writetable(buf, size, filename);
		break;
	}

gendata_run_finish:
	free(buf);
}

int main(int argc, char **argv) {
	uint8_t h, k;

	if (argc < 3) {
		fprintf(stderr, "Error: not enough arguments. "
		    "A solver and its options must be given.\n");
		return 1;
	}

	solver = argv[1];
	options = argv[2];
	parse_h48_options(options, &h, &k, NULL);
	expected = expected_h48[h][k];

	nissy_setlogger(log_stderr);

	timerun(run);

	return 0;
}
