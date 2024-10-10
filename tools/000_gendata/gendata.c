#include "../tool.h"
#include "../expected_distributions.h"

char *solver;
uint64_t *expected;

static void
run(void) {
	int64_t size;
	bool consistent, expected;
	char *buf, filename[1024];
	
	size = generatetable(solver, &buf);
	switch (size) {
	case -1:
		return;
	case -2:
		goto gendata_run_finish;
	default:
		nissy_datainfo(buf, write_stdout);
		consistent = nissy_checkdata(solver, buf) == 0;
		expected = check_distribution(solver, buf);
		if (consistent && expected) {
			printf("\n");
			printf("Generated %" PRId64 " bytes.\n", size);
			sprintf(filename, "tables/%s", solver);
			writetable(buf, size, filename);
		}
		if (!consistent)
			printf("Error: table is not consistent with info"
			    " (nissy_checkdata() failed)\n");
		if (!expected)
			printf("Error: distribution is not as expected\n");
		break;
	}

gendata_run_finish:
	free(buf);
}

int main(int argc, char **argv) {
	uint8_t h, k;

	if (argc < 2) {
		fprintf(stderr, "Error: not enough arguments. "
		    "A solver must be given.\n");
		return 1;
	}

	solver = argv[1];
	parse_h48_solver(solver, &h, &k);
	expected = expected_h48[h][k];

	nissy_setlogger(log_stderr);

	timerun(run);

	return 0;
}
