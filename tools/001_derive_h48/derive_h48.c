#include "../tool.h"

char *opts_large, *opts_small, *filename_large, *filename_small;

void run(void) {
	derivedata_run(opts_large, opts_small, filename_large, filename_small);
}

int main(int argc, char **argv) {
	if (argc < 5) {
		fprintf(stderr,
		    "Error: not enough arguments. Required:\n"
		    "1. Options for large table\n"
		    "2. Options for derived table\n"
		    "3. Filename containing large table\n"
		    "4. Filename for saving derived table\n");
		return 1;
	}

	opts_large = argv[1];
	opts_small = argv[2];
	filename_large = argv[3];
	filename_small = argv[4];

	nissy_setlogger(log_stderr);

	timerun(run);

	return 0;
}
