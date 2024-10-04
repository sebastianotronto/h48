#include "../test.h"

size_t gendata_cocsep(void *, uint64_t *, cube_t *);
bool readtableinfo(const void *, tableinfo_t *);

void run(void) {
	char buf[2000000];
	uint32_t i;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t rep[COCSEP_CLASSES];
	size_t result;
	tableinfo_t info;

	result = gendata_cocsep(buf, selfsim, rep);
	if (!readtableinfo(buf, &info)) {
		printf("Error reading info from table\n");
		return;
	}

	printf("%zu\n", result);
	printf("Classes: %" PRIu64 "\n", info.classes);
	printf("Max value: %" PRIu8 "\n", info.maxvalue);
	for (i = 0; i < 10; i++)
		printf("%" PRIu32 ": %" PRIu64 "\n", i, info.distribution[i]);
}
