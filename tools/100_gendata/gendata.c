#include "../tool.h"

char *solver, *options;
uint64_t *expected;

uint64_t expected_h48[12][9][21] = {
	[0] = {
		[2] = {
			[0] = 5473562,
			[1] = 34776317,
			[2] = 68566704,
			[3] = 8750867,
		},
		[4] = {
			[0] = 1,
			[1] = 1,
			[2] = 4,
			[3] = 34,
			[4] = 331,
			[5] = 3612,
			[6] = 41605,
			[7] = 474128,
			[8] = 4953846,
			[9] = 34776317,
			[10] = 68566704,
			[11] = 8749194,
			[12] = 1673,
		},
	},
};

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
	char description[256];

	if (argc < 3) {
		fprintf(stderr, "Error: not enough arguments."
		    "A solver and its options must be given.\n");
		return 1;
	}

	solver = argv[1];
	options = argv[2];
	parse_h48_options(options, &h, &k, NULL);
	expected = expected_h48[h][k];
	sprintf(description, "benchmark gendata_h48 h = %" PRIu8
	    ", k = %" PRIu8 "", h, k);

	nissy_setlogger(log_stderr);

	timerun(run, description);

	return 0;
}
