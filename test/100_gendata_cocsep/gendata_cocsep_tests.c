#include "../test.h"

#define COCSEP_CLASSES 3393

size_t gendata_cocsep(void *, uint64_t *, cube_t *);

void run(void) {
	uint32_t buf[300000], i;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t rep[COCSEP_CLASSES];
	size_t result;

	result = gendata_cocsep(buf, selfsim, rep);

	printf("%zu\n", result);
	printf("Classes: %" PRIu32 "\n", buf[result/4-12]);
	printf("Max value: %" PRIu32 "\n", buf[result/4-11]);
	for (i = 0; i < 10; i++)
		printf("%" PRIu32 ": %" PRIu32 "\n", i, buf[result/4-10+i]);
}
