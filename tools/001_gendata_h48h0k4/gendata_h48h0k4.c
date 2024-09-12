#include "../tool.h"

uint64_t expected[21] = {
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
};

void run(void) {
	gendata_run("h48", "0;4;20", "tables/h48h0k4", expected);
}

int main(void) {
	nissy_setlogger(log_stderr);

	timerun(run, "benchmark gendata_h48 h = 0, k = 4");

	return 0;
}
