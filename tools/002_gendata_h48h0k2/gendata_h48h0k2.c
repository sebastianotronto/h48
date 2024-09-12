#include "../tool.h"

uint64_t expected[21] = {
	/* Base value is 8 */
	[0] = 5473562,
	[1] = 34776317,
	[2] = 68566704,
	[3] = 8750867,
};

void run(void) {
	gendata_run("h48", "0;2;20", "tables/h48h0k2", expected);
}

int main(void) {
	nissy_setlogger(log_stderr);

	timerun(run, "benchmark gendata_h48 h = 0, k = 2");

	return 0;
}
