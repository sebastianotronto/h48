/*
This tool is specific to the H48 solver. It can be used to derive small
intermediate tables from larger tables, including the full h11 table.

When using k=2, the base values for the tables may differ. If you want
to change this value, for example for generating a table with base = 9
from the h11 table with base = 10, you must re-generate the large table
with the correct base value. The easiest way to do so is to manually
edit the base value in the source code and recompile.
*/

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
