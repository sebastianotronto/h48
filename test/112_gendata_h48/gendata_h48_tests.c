#include "../test.h"

#define COCSEP_CLASSES 3393
#define COCSEPSIZE 1119792

typedef struct {
	uint8_t h;
	uint8_t k;
	uint8_t maxdepth;
	void *buf;
	uint32_t *info;
	uint32_t *cocsepdata;
	uint32_t *h48data;
	uint64_t selfsim[COCSEP_CLASSES];
	cube_t crep[COCSEP_CLASSES];
} gendata_h48_arg_t;

int64_t gendata_h48(gendata_h48_arg_t *);

void run(void) {
	char str[STRLENMAX];
	uint8_t i;
	gendata_h48_arg_t arg;
	size_t result, sz;

	fgets(str, STRLENMAX, stdin);
	arg.maxdepth = atoi(str);
	fgets(str, STRLENMAX, stdin);
	arg.h = atoi(str);
	arg.k = 4;

	sz = gendata_h48(&arg); /* With buf = NULL returns data size */
	arg.buf = malloc(sz);
	result = gendata_h48(&arg);

	printf("%zu\n\n", result);

	printf("cocsepdata:\n");
	printf("Classes: %" PRIu32 "\n", arg.cocsepdata[COCSEPSIZE/4-12]);
	printf("Max value: %" PRIu32 "\n", arg.cocsepdata[COCSEPSIZE/4-11]);
	for (i = 0; i < 10; i++)
		printf("%" PRIu32 ": %" PRIu32 "\n",
		    i, arg.cocsepdata[COCSEPSIZE/4-10+i]);

	printf("\nh48:\n");
	for (i = 0; i < arg.maxdepth+1; i++)
		printf("%" PRIu32 ": %" PRIu32 "\n", i, arg.info[i+1]);

	free(arg.buf);
}
