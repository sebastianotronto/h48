#include "../tool.h"

#define MAXDEPTH 20
#define HVALUE 0
#define OPTIONS "0;2;20"
#define LONGOPTIONS "h = 0, k = 2, max depth = 20"

uint32_t expected[21] = {
	/* Base value is 8 */
	[0] = 5473562,
	[1] = 34776317,
	[2] = 68566704,
	[3] = 8750867,
};

char *buf;

void run(void) {
	int64_t s;

	s = nissy_gendata("h48", OPTIONS, buf);

	if (s == -1) {
		printf("Error generating table\n");
	} else {
		nissy_datainfo(buf, write_stdout);
		printf("\n");
		printf("Succesfully generated %" PRId64 " bytes. "
		       "See above for details on the tables.\n", s);

		/* TODO: check that the table is correct */
	}
}

int main(void) {
	int64_t size;

	nissy_setlogger(log_stderr);

	size = nissy_datasize("h48", OPTIONS);
	if (size == -1) {
		printf("gendata_h48 benchmark: error in datasize\n");
		return 1;
	}

	buf = malloc(size);

	timerun(run, "benchmark gendata_h48 " LONGOPTIONS);

	free(buf);

	return 0;
}
